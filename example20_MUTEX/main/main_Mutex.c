/*
 * Port của FreeRTOS Book - ví dụ dùng MUTEX để bảo vệ truy cập tài nguyên
 * dùng chung (standard out) sang ESP-IDF v6.0.2.
 *
 * Ý tưởng: hai instance của cùng một task (priority khác nhau) cùng in ra
 * console. Mutex đảm bảo tại một thời điểm chỉ có một task được in, nên các
 * chuỗi không bị chen lẫn vào nhau.
 *
 * Những thay đổi so với bản Windows:
 *   - Bỏ <conio.h>, _kbhit() và vTaskEndScheduler() (chỉ có trên Windows;
 *     trên ESP32 chương trình chạy mãi cho tới khi reset).
 *   - Bỏ vTaskStartScheduler(): scheduler đã chạy sẵn trước app_main().
 *   - printf + fflush được thay bằng ESP_LOGI, bỏ "\r\n" trong chuỗi.
 *   - Stack task tăng từ 1000 lên 4096.
 */

#include <stdlib.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "esp_log.h"
#include "esp_random.h"

static const char *TAG = "MUTEX_DEMO";

/* Task sẽ được tạo. Hai instance của task này được tạo. */
static void prvPrintTask( void * pvParameters );

/* Hàm dùng mutex để kiểm soát truy cập vào standard out. */
static void prvNewPrintString( const char * pcString );

/*-----------------------------------------------------------*/

/* Biến kiểu SemaphoreHandle_t, dùng để tham chiếu tới mutex đảm bảo loại trừ
 * tương hỗ khi truy cập stdout. */
SemaphoreHandle_t xMutex;

/* Các task block một khoảng thời gian giả ngẫu nhiên trong khoảng
 * 0 .. xMaxBlockTimeTicks tick. */
const TickType_t xMaxBlockTimeTicks = 0x20;

void app_main( void )
{
    /* Phải tạo semaphore trước khi dùng. Ở đây tạo một mutex. */
    xMutex = xSemaphoreCreateMutex();

    /* Kiểm tra mutex đã được tạo thành công chưa. */
    if( xMutex != NULL )
    {
        /* Tạo hai instance của task cùng ghi ra stdout. Chuỗi mỗi task in
         * được truyền vào qua tham số của task. Hai task có priority khác
         * nhau nên sẽ có preemption. */
        xTaskCreate( prvPrintTask, "Print1", 4096, "Task 1 ****************************************", 1, NULL );
        xTaskCreate( prvPrintTask, "Print2", 4096, "Task 2 ------------------------------------------", 2, NULL );

        
    }
    else
    {
        ESP_LOGE( TAG, "Khong tao duoc mutex." );
    }
}
/*-----------------------------------------------------------*/

static void prvNewPrintString( const char * pcString )
{
    /* Mutex được tạo trước khi task chạy nên đã tồn tại khi task thực thi.
     *
     * Cố gắng take mutex, block vô thời hạn nếu mutex chưa sẵn sàng. Hàm
     * xSemaphoreTake() chỉ trả về khi lấy được mutex nên không cần kiểm tra
     * giá trị trả về. Nếu dùng thời gian chờ khác thì phải kiểm tra
     * xSemaphoreTake() trả về pdTRUE trước khi truy cập tài nguyên. */
    xSemaphoreTake( xMutex, portMAX_DELAY );
    {
		for( const char * p = pcString; *p != '\0'; p++ )
		{
		    putchar( *p );
		    fflush( stdout );

		    /* Nhường CPU để task kia chen vào giữa chuỗi. */
		    vTaskDelay( 1 );
		}

		putchar( '\n' );
		fflush( stdout );
    }
    xSemaphoreGive( xMutex );
}
/*-----------------------------------------------------------*/

static void prvPrintTask( void * pvParameters )
{
    const char * pcStringToPrint;
    const TickType_t xSlowDownDelay = pdMS_TO_TICKS( 5UL );

    /* Hai instance của task này được tạo. Chuỗi mà task in ra được truyền
     * qua tham số của task, ép kiểu về đúng kiểu cần dùng. */
    pcStringToPrint = ( const char * ) pvParameters;

    for( ; ; )
    {
        /* In chuỗi bằng hàm mới định nghĩa. */
        prvNewPrintString( pcStringToPrint );

        /* Chờ một khoảng thời gian giả ngẫu nhiên. rand() không nhất thiết
         * re-entrant nhưng ở đây không quan trọng vì code không quan tâm
         * giá trị trả về. */
        vTaskDelay( ( TickType_t ) ( esp_random() % xMaxBlockTimeTicks ) + xSlowDownDelay);	

    }
}