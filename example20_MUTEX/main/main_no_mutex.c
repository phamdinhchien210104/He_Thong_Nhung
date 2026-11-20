/*
 * Phiên bản KHÔNG dùng mutex của ví dụ bảo vệ stdout (ESP-IDF v6.0.2).
 *
 * Ý tưởng: hai instance của cùng một task (priority khác nhau) cùng in ra
 * console mà không có cơ chế loại trừ tương hỗ. Để thấy rõ lỗi, hàm in chuỗi
 * in từng ký tự và nhường CPU (vTaskDelay) giữa các ký tự, nên task kia có
 * cơ hội chen vào giữa chuỗi. Kết quả: các ký tự '*' và '-' bị trộn lẫn
 * trong cùng một dòng.
 *
 * Lưu ý: không dùng ESP_LOGI ở đây, vì hệ thống log của ESP-IDF đã tự bảo vệ
 * từng lần ghi nên sẽ không thấy được hiện tượng trộn chuỗi.
 */

#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_random.h"

/* Task sẽ được tạo. Hai instance của task này được tạo. */
static void prvPrintTask( void * pvParameters );

/* Hàm in chuỗi, KHÔNG có cơ chế bảo vệ truy cập stdout. */
static void prvNewPrintString( const char * pcString );

/*-----------------------------------------------------------*/

/* Các task block một khoảng thời gian ngẫu nhiên trong khoảng
 * 0 .. xMaxBlockTimeTicks - 1 tick. */
const TickType_t xMaxBlockTimeTicks = 0x20;

void app_main( void )
{
    /* Tạo hai instance của task cùng ghi ra stdout. Chuỗi mỗi task in được
     * truyền vào qua tham số của task. Hai task có priority khác nhau nên sẽ
     * có preemption. */
    xTaskCreate( prvPrintTask, "Print1", 4096, "Task 1 ******************************************", 1, NULL );
    xTaskCreate( prvPrintTask, "Print2", 4096, "Task 2 ------------------------------------------", 2, NULL );

    /* Không gọi vTaskStartScheduler(): scheduler đã chạy sẵn. */
}
/*-----------------------------------------------------------*/

static void prvNewPrintString( const char * pcString )
{
    /* Không có mutex: bất kỳ lúc nào task này đang in dở, task khác (nếu
     * đến lượt chạy) có thể chen vào và in xen kẽ. */
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