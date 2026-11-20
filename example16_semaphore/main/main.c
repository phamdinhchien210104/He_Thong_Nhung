/*
 * Chuyển đổi từ ví dụ FreeRTOS (sách Mastering the FreeRTOS Real Time Kernel)
 * sang ESP-IDF (ESP32-C3) — Ví dụ 16: Đồng bộ hoá một task với ngắt bằng binary semaphore.
 */

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "esp_timer.h"
#include "esp_log.h"

static const char *TAG = "ISR_SYNC_DEMO";

/* Các task sẽ được tạo. */
static void vHandlerTask( void * pvParameters );
static void vPeriodicTask( void * pvParameters );

/* Hàm đóng vai trò ISR thật sự. Callback của esp_timer với dispatch_method = ESP_TIMER_ISR. */
static void vExampleInterruptHandler( void * arg );

/* Semaphore dùng để đồng bộ task với "ngắt". */
SemaphoreHandle_t xBinarySemaphore;

/* Handle của esp_timer đóng vai trò ngắt mô phỏng. */
static esp_timer_handle_t xInterruptTimer;

void app_main( void )
{
    /* Tạo binary semaphore trước khi dùng */
    xBinarySemaphore = xSemaphoreCreateBinary();

    if( xBinarySemaphore != NULL )
    {
        /* Task Handler có ưu tiên cao (3) để chạy ngay sau khi ngắt kết thúc */
        xTaskCreate( vHandlerTask, "Handler", 4096, NULL, 3, NULL );

        /* Task Periodic có ưu tiên thấp (1) để mô phỏng sự kiện định kỳ tạo ngắt */
        xTaskCreate( vPeriodicTask, "Periodic", 4096, NULL, 1, NULL );

        /* Tạo esp_timer với dispatch_method = ESP_TIMER_ISR để callback chạy trong ngữ cảnh ISR */
        const esp_timer_create_args_t xTimerArgs = {
            .callback = &vExampleInterruptHandler,
            .dispatch_method = ESP_TIMER_ISR,
            .name = "sim_interrupt"
        };
        ESP_ERROR_CHECK( esp_timer_create( &xTimerArgs, &xInterruptTimer ) );
    }
}
/*-----------------------------------------------------------*/

static void vHandlerTask( void * pvParameters )
{
    for( ; ; )
    {
        /* Chờ semaphore từ ISR (Block vô thời hạn) */
        xSemaphoreTake( xBinarySemaphore, portMAX_DELAY );

        /* Xử lý sự kiện sau khi nhận semaphore */
        ESP_LOGI( TAG, "Handler task - Processing event." );
    }
}
/*-----------------------------------------------------------*/

static void vPeriodicTask( void * pvParameters )
{
    const TickType_t xDelay500ms = pdMS_TO_TICKS( 500UL );

    for( ; ; )
    {
        vTaskDelay( xDelay500ms );

        ESP_LOGI( TAG, "Periodic task - About to generate an interrupt." );

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

    /* "Give" binary semaphore để unblock task Handler */
    xSemaphoreGiveFromISR( xBinarySemaphore, &xHigherPriorityTaskWoken );

    /* Yêu cầu chuyển ngữ cảnh ngay nếu task ưu tiên cao hơn được đánh thức */
    if( xHigherPriorityTaskWoken == pdTRUE )
    {
        esp_timer_isr_dispatch_need_yield();
    }
    portYIELD_FROM_ISR( xHigherPriorityTaskWoken );
}