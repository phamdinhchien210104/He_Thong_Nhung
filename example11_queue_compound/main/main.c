/*
 * FreeRTOS Example 11: Blocking When Sending to a Queue & Transferring Compound Types
 * 
 * Tài liệu tham khảo: FreeRTOS - Queue Management (Slide 31 đến 42)
 *
 * Mục tiêu ví dụ:
 * 1. Sử dụng Queue để truyền dữ liệu kiểu cấu trúc (struct xData) thay vì kiểu số nguyên đơn thuần.
 *    Cấu trúc chứa:
 *      - ucValue: giá trị dữ liệu (vd: 100, 200).
 *      - ucSource: mã định danh nguồn gửi (mainSENDER_1, mainSENDER_2).
 * 2. Phân tích sự ảnh hưởng của Task Priorities khi gửi và nhận từ Queue:
 *    - Sender 1 và Sender 2 có độ ưu tiên cao hơn (Priority 2).
 *    - Receiver có độ ưu tiên thấp hơn (Priority 1).
 *    - Hàng đợi (Queue) có độ dài là 3 phần tử.
 * 
 * Chu trình hoạt động:
 * - Sender 1 gửi phần tử đầu tiên (100, SENDER_1) -> Queue có 1 phần tử. Sender 1 gọi taskYIELD().
 * - Sender 2 gửi phần tử thứ hai (200, SENDER_2) -> Queue có 2 phần tử. Sender 2 gọi taskYIELD().
 * - Sender 1 gửi phần tử thứ ba (100, SENDER_1) -> Queue có 3 phần tử (ĐẦY). Sender 1 gọi taskYIELD().
 * - Sender 2 cố gắng gửi nhưng Queue đã đầy -> Sender 2 chuyển sang trạng thái Blocked (chờ tối đa 100ms).
 * - Sender 1 cố gắng gửi nhưng Queue vẫn đầy -> Sender 1 cũng chuyển sang trạng thái Blocked (chờ 100ms).
 * - Cả 2 task ưu tiên cao (Priority 2) đều bị Blocked -> CPU nhường cho Receiver (Priority 1).
 * - Receiver kiểm tra số phần tử trong Queue: luôn bằng 3 (Queue should have been full!).
 * - Receiver lấy ra 1 phần tử khỏi Queue -> Queue trống 1 vị trí.
 * - Ngay lập tức, task Sender bị block lâu nhất (Sender 2) unblock, chiếm lại quyền thực thi (preemption)
 *   từ Receiver để gửi phần tử mới vào, làm Queue đầy trở lại (3 phần tử).
 * - Chu trình này tiếp tục lặp lại.
 */

#include <stdio.h>
#include <stdint.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"

#include "esp_log.h"

static const char *TAG = "EXAMPLE_11";

/* Định nghĩa mã nguồn gửi dữ liệu (Slide 34) */
#define mainSENDER_1    ( 1 )
#define mainSENDER_2    ( 2 )

/* Định nghĩa kiểu cấu trúc dữ liệu truyền qua Queue (Slide 34) */
typedef struct
{
    uint8_t ucValue;   /* Giá trị dữ liệu */
    uint8_t ucSource;  /* Mã nguồn gửi dữ liệu */
} xData;

/* Khai báo 2 biến kiểu xData được sử dụng bởi Sender 1 và Sender 2 (Slide 34) */
static const xData xStructsToSend[ 2 ] =
{
    { 100, mainSENDER_1 }, /* Được sử dụng bởi Sender 1 */
    { 200, mainSENDER_2 }  /* Được sử dụng bởi Sender 2 */
};

/* Handle quản lý Queue */
static QueueHandle_t xQueue = NULL;

/*
 * Task gửi dữ liệu vào Queue (Slide 35, 40)
 * - Priority: 2 (cao hơn Receiver)
 * - Block time: 100ms khi queue bị đầy
 */
static void vSenderTask( void *pvParameters )
{
    BaseType_t xStatus;
    const TickType_t xTicksToWait = pdMS_TO_TICKS( 100 );

    for( ;; )
    {
        /*
         * Gửi vào cuối Queue (xQueueSendToBack):
         * - pvParameters trỏ đến cấu trúc dữ liệu xData cần gửi (&xStructsToSend[0] hoặc [1]).
         * - xTicksToWait là thời gian tối đa task ở trạng thái Blocked để chờ có chỗ trống nếu queue đầy.
         */
        xStatus = xQueueSendToBack( xQueue, pvParameters, xTicksToWait );

        if( xStatus != pdPASS )
        {
            /*
             * Nếu sau 100ms vẫn không thể ghi vào queue thì xuất hiện cảnh báo.
             * Thực tế sẽ không xảy ra vì Receiver sẽ đọc dữ liệu ngay khi cả 2 Sender bị block.
             */
            ESP_LOGW(TAG, "Could not send to the queue.");
        }

        /* Cho phép task sender khác có cùng độ ưu tiên được chạy */
        taskYIELD();
    }
}

/*
 * Task nhận dữ liệu từ Queue (Slide 36, 37, 41)
 * - Priority: 1 (thấp hơn Sender)
 * - Block time: 0 (không chờ vì chỉ chạy khi queue đã đầy)
 */
static void vReceiverTask( void *pvParameters )
{
    xData xReceivedStructure;
    BaseType_t xStatus;

    for( ;; )
    {
        /*
         * Task Receiver chỉ chạy khi cả 2 Sender task đang ở trạng thái Blocked.
         * Vì các Sender chỉ block khi Queue đầy, nên Receiver luôn thấy Queue có đủ 3 phần tử.
         */
        if( uxQueueMessagesWaiting( xQueue ) != 3 )
        {
            ESP_LOGW(TAG, "Queue should have been full!");
        }

        /*
         * Nhận phần tử từ đầu Queue:
         * Block time = 0 vì task chỉ thực thi khi queue đã có sẵn dữ liệu.
         */
        xStatus = xQueueReceive( xQueue, &xReceivedStructure, 0 );

        if( xStatus == pdPASS )
        {
            /* Phân biệt nguồn gửi để in dữ liệu tương ứng (Slide 37) */
            if( xReceivedStructure.ucSource == mainSENDER_1 )
            {
                ESP_LOGI(TAG, "From Sender 1 = %d", xReceivedStructure.ucValue);
            }
            else if( xReceivedStructure.ucSource == mainSENDER_2 )
            {
                ESP_LOGI(TAG, "From Sender 2 = %d", xReceivedStructure.ucValue);
            }
            else
            {
                ESP_LOGI(TAG, "Unknown Source = %d, Value = %d",
                         xReceivedStructure.ucSource, xReceivedStructure.ucValue);
            }
        }
        else
        {
            ESP_LOGW(TAG, "Could not receive from the queue.");
        }

        /*
         * Tùy chọn: Nếu muốn nhường CPU định kỳ cho Idle task nhằm tránh Task Watchdog cảnh báo,
         * có thể mở chú thích dòng sau:
         * vTaskDelay(pdMS_TO_TICKS(10));
         */
    }
}

void app_main( void )
{
    ESP_LOGI(TAG, "==========================================================");
    ESP_LOGI(TAG, "  FreeRTOS Example 11: Queue Compound Types & Blocking   ");
    ESP_LOGI(TAG, "==========================================================");

    /*
     * Khởi tạo Queue có thể chứa tối đa 3 phần tử kiểu xData (Slide 38)
     */
    xQueue = xQueueCreate( 3, sizeof( xData ) );

    if( xQueue != NULL )
    {
        /*
         * Tạo 2 task gửi (Sender 1 và Sender 2):
         * - Priority: 2 (cao hơn Receiver).
         * - Stack: 2048 bytes.
         * - Parameter: truyền con trỏ tới xStructsToSend[0] hoặc xStructsToSend[1].
         */
        xTaskCreate( vSenderTask, "Sender1", 2048, ( void * ) &( xStructsToSend[ 0 ] ), 2, NULL );
        xTaskCreate( vSenderTask, "Sender2", 2048, ( void * ) &( xStructsToSend[ 1 ] ), 2, NULL );

        /*
         * Tạo 1 task nhận (Receiver):
         * - Priority: 1 (thấp hơn Sender 1 và Sender 2).
         * - Stack: 2048 bytes.
         */
        xTaskCreate( vReceiverTask, "Receiver", 2048, NULL, 1, NULL );

        /*
         * Trong ESP-IDF, scheduler đã được khởi động tự động trước khi app_main() được gọi.
         * Do đó không cần gọi vTaskStartScheduler().
         */
    }
    else
    {
        ESP_LOGE(TAG, "The queue could not be created.");
    }
}
