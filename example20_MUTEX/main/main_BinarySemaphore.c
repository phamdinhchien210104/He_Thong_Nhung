/*
 * Phiên bản dùng BINARY SEMAPHORE thay cho mutex để bảo vệ truy cập tài
 * nguyên dùng chung (standard out) trên ESP-IDF v6.0.2.
 *
 * Ý tưởng: hai instance của cùng một task (priority khác nhau) cùng in ra
 * console. Binary semaphore đảm bảo tại một thời điểm chỉ có một task được
 * in, nên các chuỗi không bị chen lẫn vào nhau.
 *
 * Khác biệt so với mutex:
 *   - xSemaphoreCreateBinary() tạo semaphore ở trạng thái RỖNG (chưa
 *     available), nên phải gọi xSemaphoreGive() một lần ngay sau khi tạo.
 *     Nếu quên, cả hai task sẽ bị block mãi ở xSemaphoreTake() (deadlock).
 *   - Binary semaphore KHÔNG có priority inheritance. Nếu task priority thấp
 *     đang giữ semaphore mà bị task priority trung bình preempt, task
 *     priority cao đang chờ semaphore có thể bị trễ (priority inversion).
 *   - Binary semaphore không có "chủ sở hữu": task nào cũng có thể Give.
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

static const char *TAG = "BINSEM_DEMO";

/* Task sẽ được tạo. Hai instance của task này được tạo. */
static void prvPrintTask( void * pvParameters );

/* Hàm dùng binary semaphore để kiểm soát truy cập vào standard out. */
static void prvNewPrintString( const char * pcString );

/*-----------------------------------------------------------*/

/* Biến kiểu SemaphoreHandle_t, dùng để tham chiếu tới binary semaphore đảm
 * bảo loại trừ tương hỗ khi truy cập stdout. */
SemaphoreHandle_t xBinarySemaphore;

/* Các task block một khoảng thời gian ngẫu nhiên trong khoảng
 * 0 .. xMaxBlockTimeTicks - 1 tick. */
const TickType_t xMaxBlockTimeTicks = 0x20;

void app_main( void )
{
    /* Phải tạo semaphore trước khi dùng. Ở đây tạo một binary semaphore. */
    xBinarySemaphore = xSemaphoreCreateBinary();

    /* Kiểm tra semaphore đã được tạo thành công chưa. */
    if( xBinarySemaphore != NULL )
    {
        /* Binary semaphore mới tạo ở trạng thái RỖNG. Phải "give" một lần để
         * đưa nó về trạng thái available, nếu không lần take đầu tiên của
         * hai task sẽ block mãi. */
        xSemaphoreGive( xBinarySemaphore );

        /* Tạo hai instance của task cùng ghi ra stdout. Chuỗi mỗi task in
         * được truyền vào qua tham số của task. Hai task có priority khác
         * nhau nên sẽ có preemption. */
        xTaskCreate( prvPrintTask, "Print1", 4096, "Task 1 ******************************************", 1, NULL );
        xTaskCreate( prvPrintTask, "Print2", 4096, "Task 2 ------------------------------------------", 2, NULL );
    }
    else
    {
        ESP_LOGE( TAG, "Khong tao duoc binary semaphore." );
    }
}
/*-----------------------------------------------------------*/

static void prvNewPrintString( const char * pcString )
{
    /* Semaphore được tạo và give trước khi task chạy nên đã sẵn sàng khi
     * task thực thi.
     *
     * Cố gắng take semaphore, block vô thời hạn nếu chưa available. Hàm
     * xSemaphoreTake() chỉ trả về khi lấy được semaphore nên không cần kiểm
     * tra giá trị trả về. Nếu dùng thời gian chờ khác thì phải kiểm tra
     * xSemaphoreTake() trả về pdTRUE trước khi truy cập tài nguyên. */
    xSemaphoreTake( xBinarySemaphore, portMAX_DELAY );
    {
        /* Dòng dưới chỉ chạy khi đã lấy được semaphore, nên có thể truy cập
         * standard out một cách an toàn. */
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
    /* Trả semaphore lại để task kia có thể take. */
    xSemaphoreGive( xBinarySemaphore );
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
        /* In chuỗi bằng hàm đã định nghĩa. */
        prvNewPrintString( pcStringToPrint );

        /* Chờ một khoảng thời gian ngẫu nhiên. Dùng esp_random() (bộ sinh số
         * ngẫu nhiên phần cứng) thay cho rand(). */
        vTaskDelay( ( TickType_t ) ( esp_random() % xMaxBlockTimeTicks ) + xSlowDownDelay);
    }
}