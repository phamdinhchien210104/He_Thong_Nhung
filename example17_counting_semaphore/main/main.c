/*
 * Chuyển đổi từ ví dụ FreeRTOS (sách Mastering the FreeRTOS Real Time Kernel)
 * sang ESP-IDF (ESP32 / ESP32-C3) — Ví dụ 17: Đồng bộ hoá một task với ngắt
 * bằng Counting Semaphore (đếm và bảo lưu sự kiện ngắt).
 */

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "esp_timer.h"
#include "esp_log.h"

static const char *TAG = "COUNTING_SEMAPHORE_DEMO";

/* Khai báo các task */
static void vHandlerTask( void * pvParameters );
static void vPeriodicTask( void * pvParameters );

/* Hàm callback đóng vai trò ISR */
static void vExampleInterruptHandler( void * arg );

/* Định nghĩa giá trị đếm tối đa của counting semaphore.
 * uxMaxCount = 10: Cho phép tích lũy tối đa 10 sự kiện chưa kịp xử lý.
 * uxInitialCount = 0: Khởi tạo với 0 sự kiện (chưa có sự kiện nào xảy ra). */
#define mainSEMAPHORE_MAX_COUNT    10
#define mainSEMAPHORE_INIT_COUNT   0

/* Handle của counting semaphore */
SemaphoreHandle_t xCountingSemaphore;

/* Handle của esp_timer dùng làm ngắt mô phỏng */
static esp_timer_handle_t xInterruptTimer;

void app_main( void )
{
    /* Tạo counting semaphore với max count = 10 và initial count = 0 */
    xCountingSemaphore = xSemaphoreCreateCounting( mainSEMAPHORE_MAX_COUNT, mainSEMAPHORE_INIT_COUNT );

    if( xCountingSemaphore != NULL )
    {
        /* Tạo task Handler với độ ưu tiên cao (3) để xử lý sự kiện ngắt được deferred */
        xTaskCreate( vHandlerTask, "Handler", 4096, NULL, 3, NULL );

        /* Tạo task Periodic với độ ưu tiên thấp (1) để định kỳ mô phỏng ngắt */
        xTaskCreate( vPeriodicTask, "Periodic", 4096, NULL, 1, NULL );

        /* Tạo esp_timer chạy ở chế độ ISR để mô phỏng ngắt phần cứng */
        const esp_timer_create_args_t xTimerArgs = {
            .callback = &vExampleInterruptHandler,
            .dispatch_method = ESP_TIMER_ISR,
            .name = "sim_interrupt"
        };
        ESP_ERROR_CHECK( esp_timer_create( &xTimerArgs, &xInterruptTimer ) );
    }
    else
    {
        ESP_LOGE( TAG, "Khong the tao counting semaphore!" );
    }
}
/*-----------------------------------------------------------*/

static void vHandlerTask( void * pvParameters )
{
    for( ; ; )
    {
        /* Chờ semaphore. Mỗi lần Take thành công, giá trị đếm của semaphore giảm đi 1.
         * Nếu count > 0 (do ISR đã give nhiều lần), hàm trả về ngay mà không bị block.
         * Khi count = 0, task sẽ block chờ lần ngắt tiếp theo. */
        xSemaphoreTake( xCountingSemaphore, portMAX_DELAY );

        /* Xử lý sự kiện */
        ESP_LOGI( TAG, "Handler task - Processing event." );
    }
}
/*-----------------------------------------------------------*/

static void vPeriodicTask( void * pvParameters )
{
    const TickType_t xDelay500ms = pdMS_TO_TICKS( 500UL );

    for( ; ; )
    {
        /* Chờ 500ms trước khi tạo ngắt tiếp theo */
        vTaskDelay( xDelay500ms );

        ESP_LOGI( TAG, "Periodic task - About to generate an interrupt." );

        /* Kích hoạt timer ngay lập tức để mô phỏng ngắt */
        esp_err_t err = esp_timer_start_once( xInterruptTimer, 0 );
        if( err != ESP_OK )
        {
            ESP_LOGW( TAG, "Khong the kich hoat ngat: %s", esp_err_to_name( err ) );
        }

        ESP_LOGI( TAG, "Periodic task - Interrupt generated." );
    }
}
/*-----------------------------------------------------------*/

static void vExampleInterruptHandler( void * arg )
{
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;

    /* 'Give' semaphore 3 lần liên tiếp trong cùng một ngắt.
     * Điều này mô phỏng tình huống nhiều ngắt/sự kiện dồn dập xuất hiện.
     * - Với Binary Semaphore: Lần give thứ 1 đưa count lên 1, lần 2 và 3 bị mất (bị bỏ qua).
     * - Với Counting Semaphore: Giá trị đếm tăng từ 0 lên 3, toàn bộ 3 sự kiện đều được
     *   lưu lại (latched) để task Handler xử lý tuần tự đủ 3 lần! */
    xSemaphoreGiveFromISR( xCountingSemaphore, &xHigherPriorityTaskWoken );
    xSemaphoreGiveFromISR( xCountingSemaphore, &xHigherPriorityTaskWoken );
    xSemaphoreGiveFromISR( xCountingSemaphore, &xHigherPriorityTaskWoken );

    /* Yêu cầu chuyển ngữ cảnh sang task Handler có ưu tiên cao hơn */
    if( xHigherPriorityTaskWoken == pdTRUE )
    {
        esp_timer_isr_dispatch_need_yield();
    }
    portYIELD_FROM_ISR( xHigherPriorityTaskWoken );
}
