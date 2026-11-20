/*
 * Chuyển đổi từ ví dụ FreeRTOS (sách Mastering the FreeRTOS Real Time Kernel)
 * sang ESP-IDF (ESP32 / ESP32-C3) — Ví dụ 19: Sử dụng hàng đợi (Queue)
 * bên trong hàm xử lý ngắt (Interrupt Service Routine - ISR).
 * 
 * Bổ sung: Chức năng ngắt phần cứng thông qua nút nhấn BOOT (GPIO 9 trên ESP32-C3).
 */

#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/gpio.h"
#include "esp_attr.h"
#include "esp_timer.h"
#include "esp_log.h"

static const char *TAG = "QUEUE_ISR_DEMO";

/* 
 * Định nghĩa chân GPIO cho nút BOOT:
 * - ESP32-C3: Nút BOOT nối với GPIO 9 (kéo lên mức cao mặc định, khi nhấn nối xuống GND).
 * - ESP32 tiêu chuẩn / ESP32-S3: Nút BOOT thường là GPIO 0.
 */
#if CONFIG_IDF_TARGET_ESP32C3 || CONFIG_IDF_TARGET_ESP32C2
#define BOOT_BUTTON_GPIO    GPIO_NUM_9
#else
#define BOOT_BUTTON_GPIO    GPIO_NUM_0
#endif

/* 
 * Chế độ mô phỏng ngắt bằng Timer:
 * - 0: Tắt timer mô phỏng, chỉ sử dụng ngắt phần cứng bằng nút BOOT.
 * - 1: Bật timer mô phỏng ngắt định kỳ kết hợp song song với nút BOOT.
 */
#define ENABLE_SIMULATED_TIMER    0

/* Khai báo các task */
static void vIntegerGenerator( void * pvParameters );
static void vStringPrinter( void * pvParameters );

/* Khai báo ISR ngắt phần cứng nút BOOT */
static void vButtonIsrHandler( void * arg );

/* Khai báo ISR ngắt mô phỏng qua esp_timer */
#if ENABLE_SIMULATED_TIMER
static void vExampleInterruptHandler( void * arg );
static esp_timer_handle_t xInterruptTimer;
#endif

/* Cấu hình ngắt phần cứng cho nút nhấn BOOT */
static void prvSetupBootButtonInterrupt( void );

/* Handle của 2 hàng đợi:
 * - xIntegerQueue: Chứa số nguyên (uint32_t) gửi từ task vIntegerGenerator đến ISR.
 * - xStringQueue: Chứa con trỏ chuỗi (char *) gửi từ ISR đến task vStringPrinter. */
static QueueHandle_t xIntegerQueue = NULL;
static QueueHandle_t xStringQueue = NULL;

void app_main( void )
{

    /* Tạo 2 hàng đợi:
     * - xIntegerQueue chứa tối đa 10 phần tử kiểu uint32_t.
     * - xStringQueue chứa tối đa 10 phần tử kiểu con trỏ chuỗi (char *). */
    xIntegerQueue = xQueueCreate( 10, sizeof( uint32_t ) );
    xStringQueue = xQueueCreate( 10, sizeof( char * ) );

    if( ( xIntegerQueue != NULL ) && ( xStringQueue != NULL ) )
    {
        /* Task phát số nguyên (vIntegerGenerator) có độ ưu tiên thấp (1) */
        xTaskCreate( vIntegerGenerator, "IntGen", 4096, NULL, 1, NULL );

        /* Task in chuỗi (vStringPrinter) có độ ưu tiên cao hơn (2)
         * để ngay khi ngắt kết thúc, task này được unblock và chạy ngay */
        xTaskCreate( vStringPrinter, "StringPrinter", 4096, NULL, 2, NULL );

        /* Cấu hình ngắt phần cứng cho nút nhấn BOOT */
        prvSetupBootButtonInterrupt();

#if ENABLE_SIMULATED_TIMER
        /* Tạo esp_timer chạy ở chế độ ISR để mô phỏng ngắt định kỳ */
        const esp_timer_create_args_t xTimerArgs = {
            .callback = &vExampleInterruptHandler,
            .dispatch_method = ESP_TIMER_ISR,
            .name = "sim_interrupt"
        };
        ESP_ERROR_CHECK( esp_timer_create( &xTimerArgs, &xInterruptTimer ) );
        ESP_LOGI( TAG, "Timer mo phong ngat da duoc khoi tao." );
#endif
    }
    else
    {
        ESP_LOGE( TAG, "Khong the tao hang doi!" );
    }
}
/*-----------------------------------------------------------*/

/*
 * Cấu hình chân GPIO của nút BOOT và đăng ký ngắt phần cứng (GPIO ISR).
 */
static void prvSetupBootButtonInterrupt( void )
{
    /* Cấu hình chân GPIO nút BOOT làm INPUT, kích hoạt điện trở kéo lên nội (Pull-up),
     * kích hoạt ngắt ở sườn âm (Falling Edge / NEGEDGE) khi nhấn nút nối xuống đất. */
    gpio_config_t io_conf = {
        .pin_bit_mask = ( 1ULL << BOOT_BUTTON_GPIO ),
        .mode = GPIO_MODE_INPUT,
        .pull_up_en = GPIO_PULLUP_ENABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_NEGEDGE
    };
    ESP_ERROR_CHECK( gpio_config( &io_conf ) );

    /* Cài đặt dịch vụ xử lý ngắt GPIO */
    esp_err_t err = gpio_install_isr_service( 0 );
    if( ( err != ESP_OK ) && ( err != ESP_ERR_INVALID_STATE ) )
    {
        ESP_LOGE( TAG, "Loi cai dat GPIO ISR service: %s", esp_err_to_name( err ) );
        return;
    }

    /* Gắn hàm xử lý ngắt vButtonIsrHandler cho chân BOOT_BUTTON_GPIO */
    ESP_ERROR_CHECK( gpio_isr_handler_add( BOOT_BUTTON_GPIO, vButtonIsrHandler, (void *)BOOT_BUTTON_GPIO ) );

    ESP_LOGI( TAG, "-> Da cai dat ngat phan cung nut BOOT tren GPIO %d thanh cong!", BOOT_BUTTON_GPIO );
    ESP_LOGI( TAG, "-> Nhan nut BOOT tren board de kich hoat ngat bat cu luc nao!" );
}
/*-----------------------------------------------------------*/

/*
 * Task sinh số nguyên định kỳ:
 * Cứ mỗi chu kỳ 2 giây, gửi 5 số nguyên vào xIntegerQueue.
 */
static void vIntegerGenerator( void * pvParameters )
{
    TickType_t xLastExecutionTime;
    uint32_t ulValueToSend = 0;
    int i;

    /* Khởi tạo mốc thời gian cho vTaskDelayUntil */
    xLastExecutionTime = xTaskGetTickCount();

    for( ; ; )
    {
        /* Chu kỳ lặp 2000ms (2 giây) để người dùng có thời gian nhấn nút quan sát */
        vTaskDelayUntil( &xLastExecutionTime, pdMS_TO_TICKS( 2000 ) );

        /* Gửi 5 số nguyên liên tiếp vào xIntegerQueue */
        int itemsSent = 0;
        for( i = 0; i < 5; i++ )
        {
            if( xQueueSendToBack( xIntegerQueue, &ulValueToSend, 0 ) == pdPASS )
            {
                ulValueToSend++;
                itemsSent++;
            }
            else
            {
                /* Hàng đợi đầy (đã chứa đủ 10 phần tử mà chưa có ngắt nào đọc bớt) */
                break;
            }
        }

        if( itemsSent > 0 )
        {
            ESP_LOGI( TAG, "IntGen: Da them %d so vao xIntegerQueue (gia tri ke tiep: %lu). [Hay bam nut BOOT de doc!]",
                      itemsSent, (unsigned long)ulValueToSend );
        }
        else
        {
            ESP_LOGW( TAG, "IntGen: xIntegerQueue dang DAY (10 phan tu)! Hay bam nut BOOT de giai phong." );
        }

#if ENABLE_SIMULATED_TIMER
        /* Kích hoạt timer mô phỏng nếu bật cờ */
        esp_timer_start_once( xInterruptTimer, 0 );
#endif
    }
}
/*-----------------------------------------------------------*/

/*
 * HÀM XỬ LÝ NGẮT PHẦN CỨNG (GPIO ISR) CHO NÚT BOOT:
 * Được gắn thuộc tính IRAM_ATTR để lưu trong IRAM nhằm phản hồi ngắt cực nhanh.
 * Chức năng:
 * 1. Chống rung phím (debounce 150ms).
 * 2. Đọc toàn bộ các số nguyên có trong xIntegerQueue bằng xQueueReceiveFromISR().
 * 3. Chuyển đổi mỗi số nguyên thành con trỏ chuỗi tương ứng.
 * 4. Đẩy con trỏ chuỗi vào xStringQueue bằng xQueueSendToBackFromISR().
 * 5. Yêu cầu chuyển ngữ cảnh (portYIELD_FROM_ISR) sang vStringPrinter nếu cần.
 */
static void IRAM_ATTR vButtonIsrHandler( void * arg )
{
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    uint32_t ulReceivedNumber;
    BaseType_t xReceivedAny = pdFALSE;

    /* Chống rung cơ khí (Debounce): Bỏ qua các tín hiệu nảy phím trong khoảng 150ms */
    static int64_t llLastInterruptTime = 0;
    int64_t llCurrentTime = esp_timer_get_time();
    if( ( llCurrentTime - llLastInterruptTime ) < 150000 ) /* 150,000 microseconds = 150ms */
    {
        return;
    }
    llLastInterruptTime = llCurrentTime;

    /* Mảng chuỗi tương ứng với số nguyên nhận được (2 bit cuối: 0, 1, 2, 3) */
    static const char *pcStrings[] =
    {
        "String 0 ",
        "String 1 ",
        "String 2 ",
        "String 3 "
    };

    /* Đọc hết tất cả số nguyên có trong xIntegerQueue */
    while( xQueueReceiveFromISR( xIntegerQueue,
                                 &ulReceivedNumber,
                                 &xHigherPriorityTaskWoken ) != errQUEUE_EMPTY )
    {
        xReceivedAny = pdTRUE;

        /* Lấy 2 bit cuối (giá trị từ 0 đến 3) */
        ulReceivedNumber &= 0x03;

        /* Gửi con trỏ chuỗi vào xStringQueue */
        xQueueSendToBackFromISR( xStringQueue,
                                 &pcStrings[ ulReceivedNumber ],
                                 &xHigherPriorityTaskWoken );
    }

    /* Nếu khi bấm nút BOOT mà xIntegerQueue đang rỗng, gửi thông báo để người dùng biết ngắt đã chạy */
    if( xReceivedAny == pdFALSE )
    {
        static const char *pcEmptyMsg = "[BOOT ISR]: Da kich hoat ngat phan cung! (xIntegerQueue dang rong)";
        xQueueSendToBackFromISR( xStringQueue,
                                 &pcEmptyMsg,
                                 &xHigherPriorityTaskWoken );
    }

    /* Nếu vStringPrinter (ưu tiên 2) được unblock và có độ ưu tiên cao hơn task hiện thời,
     * thực hiện chuyển ngữ cảnh ngay lập tức khi thoát khỏi ISR */
    portYIELD_FROM_ISR( xHigherPriorityTaskWoken );
}
/*-----------------------------------------------------------*/

#if ENABLE_SIMULATED_TIMER
/*
 * Hàm xử lý ngắt mô phỏng qua esp_timer (nếu bật ENABLE_SIMULATED_TIMER = 1)
 */
static void vExampleInterruptHandler( void * arg )
{
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    uint32_t ulReceivedNumber;

    static const char *pcStrings[] =
    {
        "String 0 [Tu Timer ISR]",
        "String 1 [Tu Timer ISR]",
        "String 2 [Tu Timer ISR]",
        "String 3 [Tu Timer ISR]"
    };

    while( xQueueReceiveFromISR( xIntegerQueue,
                                 &ulReceivedNumber,
                                 &xHigherPriorityTaskWoken ) != errQUEUE_EMPTY )
    {
        ulReceivedNumber &= 0x03;
        xQueueSendToBackFromISR( xStringQueue,
                                 &pcStrings[ ulReceivedNumber ],
                                 &xHigherPriorityTaskWoken );
    }

    if( xHigherPriorityTaskWoken == pdTRUE )
    {
        esp_timer_isr_dispatch_need_yield();
    }
    portYIELD_FROM_ISR( xHigherPriorityTaskWoken );
}
#endif
/*-----------------------------------------------------------*/

/*
 * Task in chuỗi:
 * Có độ ưu tiên cao hơn (2) so với vIntegerGenerator (1).
 * Luôn ở trạng thái Block chờ dữ liệu xuất hiện trong xStringQueue.
 * Ngay khi ISR gửi chuỗi vào xStringQueue, task này lập tức unblock và in chuỗi.
 */
static void vStringPrinter( void * pvParameters )
{
    char *pcString;

    for( ; ; )
    {
        /* Block chờ dữ liệu từ xStringQueue vô thời hạn */
        xQueueReceive( xStringQueue, &pcString, portMAX_DELAY );

        /* In chuỗi nhận được từ ISR */
        ESP_LOGI( TAG, "Printer received: %s", pcString );
    }
}
