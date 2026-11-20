/*
 * Port của FreeRTOS Book - Example018 (Windows simulator) sang ESP-IDF.
 *
 * Ý tưởng: thay vì dùng semaphore + một task riêng như Example017, ví dụ này
 * "hoãn" việc xử lý ngắt cho DAEMON TASK (timer service task) bằng
 * xTimerPendFunctionCallFromISR(). ISR chỉ gửi yêu cầu "hãy gọi hàm X với
 * tham số Y" vào timer command queue; daemon task sẽ lấy ra và thực thi hàm
 * đó trong ngữ cảnh task.
 *
 * Ngắt được mô phỏng bằng esp_timer với dispatch_method = ESP_TIMER_ISR
 * (giống Example017).
 *
 * Lưu ý cấu hình (menuconfig):
 *   - Component config -> High resolution timer (esp_timer)
 *       -> Support ISR dispatch method
 *   - Priority của daemon task do configTIMER_TASK_PRIORITY quyết định:
 *       Component config -> FreeRTOS -> Kernel -> configTIMER_TASK_PRIORITY
 *     Mặc định ESP-IDF là 1, khi đó Periodic task (priority = 1 - 1 = 0) sẽ
 *     ngang priority với Idle task. Vẫn chạy được, nhưng nên tăng lên 3 để
 *     giống ý đồ của sách (Periodic thấp hơn daemon task rõ ràng).
 */

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/timers.h"
#include "esp_timer.h"
#include "esp_log.h"
#include "esp_attr.h"

static const char *TAG = "EXAMPLE018";

/* Task sẽ được tạo. */
static void vPeriodicTask( void * pvParameters );

/* Hàm thực hiện phần xử lý ngắt bị hoãn lại. Hàm này chạy trong ngữ cảnh
 * của daemon task (không phải ISR) nên được phép dùng ESP_LOG. */
static void vDeferredHandlingFunction( void * pvParameter1,
                                       uint32_t ulParameter2 );

/* Hàm phục vụ ngắt (mô phỏng). IRAM_ATTR để callback nằm trong IRAM. */
static void vExampleInterruptHandler( void * arg );

/*-----------------------------------------------------------*/

/* Handle của esp_timer đóng vai trò ngắt mô phỏng. Để global vì
 * vPeriodicTask cần truy cập để "kích hoạt" ngắt. */
static esp_timer_handle_t xInterruptTimer;

void app_main( void )
{
    /* Task tạo ngắt phần mềm được tạo với priority THẤP HƠN daemon task.
     * Priority của daemon task do configTIMER_TASK_PRIORITY quyết định. */
    const UBaseType_t ulPeriodicTaskPriority = configTIMER_TASK_PRIORITY - 1;

    /* Tạo task định kỳ tạo ngắt phần mềm. */
    xTaskCreate( vPeriodicTask, "Periodic", 4096, NULL, ulPeriodicTaskPriority, NULL );

    /* Thay cho vPortSetInterruptHandler() của bản Windows port: tạo (chưa
     * start) một esp_timer với dispatch_method = ESP_TIMER_ISR. Callback
     * chạy trực tiếp trong ngữ cảnh ngắt mỗi khi timer kích hoạt. */
    const esp_timer_create_args_t xTimerArgs = {
        .callback = &vExampleInterruptHandler,
        .arg = NULL,
        .dispatch_method = ESP_TIMER_ISR,
        .name = "sim_interrupt"
    };
    ESP_ERROR_CHECK( esp_timer_create( &xTimerArgs, &xInterruptTimer ) );

    /* Không gọi vTaskStartScheduler(): trên ESP-IDF scheduler đã chạy sẵn
     * trước khi app_main() được gọi. */
}
/*-----------------------------------------------------------*/

static void vPeriodicTask( void * pvParameters )
{
    const TickType_t xDelay500ms = pdMS_TO_TICKS( 500UL );

    /* Như hầu hết các task, task này nằm trong vòng lặp vô hạn. */
    for( ; ; )
    {
        /* Task này chỉ dùng để "mô phỏng" ngắt: block cho tới khi tới lúc
         * tạo ngắt tiếp theo. */
        vTaskDelay( xDelay500ms );

        /* In thông báo trước và sau khi tạo ngắt để thấy rõ thứ tự thực thi
         * trong log. */
        ESP_LOGI( TAG, "Periodic task - About to generate an interrupt." );

        /* Thay cho vPortGenerateSimulatedInterrupt(): kích hoạt timer với
         * timeout = 0 để callback chạy gần như ngay lập tức. */
        esp_err_t err = esp_timer_start_once( xInterruptTimer, 0 );
        if( err != ESP_OK )
        {
            /* Trường hợp hiếm: lần kích hoạt trước chưa hoàn tất (timer còn
             * active) thì hàm trả lỗi thay vì crash. */
            ESP_LOGW( TAG, "Khong the kich hoat ngat: %s", esp_err_to_name( err ) );
        }

        ESP_LOGI( TAG, "Periodic task - Interrupt generated." );
    }
}
/*-----------------------------------------------------------*/

static void IRAM_ATTR vExampleInterruptHandler( void * arg )
{
    static uint32_t ulParameterValue = 0;
    BaseType_t xHigherPriorityTaskWoken;

    /* Phải khởi tạo xHigherPriorityTaskWoken = pdFALSE; nó sẽ được đặt
     * thành pdTRUE bên trong hàm FromISR nếu cần chuyển ngữ cảnh. */
    xHigherPriorityTaskWoken = pdFALSE;

    /* Gửi con trỏ tới hàm xử lý bị hoãn sang daemon task. pvParameter1 không
     * dùng nên để NULL. ulParameter2 dùng để truyền một con số tăng dần mỗi
     * lần ngắt xảy ra. */
    xTimerPendFunctionCallFromISR( vDeferredHandlingFunction, NULL, ulParameterValue, &xHigherPriorityTaskWoken );
    ulParameterValue++;

    /* Nếu xHigherPriorityTaskWoken = pdTRUE (daemon task có priority cao hơn
     * task đang chạy) thì yêu cầu chuyển ngữ cảnh ngay khi thoát ISR. */
    portYIELD_FROM_ISR( xHigherPriorityTaskWoken );
}
/*-----------------------------------------------------------*/

static void vDeferredHandlingFunction( void * pvParameter1,
                                       uint32_t ulParameter2 )
{
    /* Bỏ cảnh báo compiler vì pvParameter1 không dùng trong ví dụ này. */
    ( void ) pvParameter1;

    /* Xử lý sự kiện - ở đây chỉ in một thông báo và giá trị ulParameter2. */
    ESP_LOGI( TAG, "Handler function - Processing event %lu", (unsigned long)ulParameter2 );
}