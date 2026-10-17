#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"

#include "esp_log.h"

static const char *TAG = "queue_set_demo";

/* Hai task gửi. */
static void vSenderTask1( void * pvParameters );
static void vSenderTask2( void * pvParameters );

/* Task nhận — chờ trên queue set để nhận dữ liệu từ cả hai queue. */
static void vReceiverTask( void * pvParameters );

/*-----------------------------------------------------------*/

/* Hai queue, cùng được thêm vào một queue set. */
static QueueHandle_t xQueue1 = NULL, xQueue2 = NULL;

/* Queue set chứa cả hai queue trên. */
static QueueSetHandle_t xQueueSet = NULL;

/*-----------------------------------------------------------*/

void app_main( void )
{
    /* Mỗi queue gửi một con trỏ char*, dung lượng 1 phần tử. Priority của
    Receiver cao hơn Sender nên mỗi queue sẽ không bao giờ chứa quá 1 phần
    tử tại một thời điểm. */
    xQueue1 = xQueueCreate( 1, sizeof( char * ) );
    xQueue2 = xQueueCreate( 1, sizeof( char * ) );

    /* Queue set phải có dung lượng >= tổng dung lượng tối đa của các queue
    thành viên cộng lại: 1 + 1 = 2. */
    xQueueSet = xQueueCreateSet( 1 * 2 );

    xQueueAddToSet( xQueue1, xQueueSet );
    xQueueAddToSet( xQueue2, xQueueSet );

    xTaskCreate( vSenderTask1, "Sender1", 2048, NULL, 1, NULL );
    xTaskCreate( vSenderTask2, "Sender2", 2048, NULL, 1, NULL );

    /* Receiver có priority cao hơn hai Sender. */
    xTaskCreate( vReceiverTask, "Receiver", 2048, NULL, 2, NULL );
}

/*-----------------------------------------------------------*/

static void vSenderTask1( void * pvParameters )
{
    const TickType_t xBlockTime = pdMS_TO_TICKS( 100 );
    const char * const pcMessage = "Message from vSenderTask1";

    ( void ) pvParameters;

    for( ; ; )
    {
        vTaskDelay( xBlockTime );

        /* Không cần block time dù queue chỉ chứa được 1 phần tử: vì
        Receiver có priority cao hơn, ngay khi task này ghi vào queue nó sẽ
        bị preempt bởi Receiver, nên queue luôn kịp rỗng trước lần gửi kế
        tiếp. */
        xQueueSend( xQueue1, &pcMessage, 0 );
    }
}

/*-----------------------------------------------------------*/

static void vSenderTask2( void * pvParameters )
{
    const TickType_t xBlockTime = pdMS_TO_TICKS( 200 );
    const char * const pcMessage = "Message from vSenderTask2";

    ( void ) pvParameters;

    for( ; ; )
    {
        vTaskDelay( xBlockTime );
        xQueueSend( xQueue2, &pcMessage, 0 );
    }
}

/*-----------------------------------------------------------*/

static void vReceiverTask( void * pvParameters )
{
    QueueHandle_t xQueueThatContainsData;
    char * pcReceivedString;

    ( void ) pvParameters;

    for( ; ; )
    {
        /* Block vô hạn trên queue set cho tới khi MỘT trong hai queue
        thành viên có dữ liệu. Trả về handle của chính queue đó (ép kiểu
        sang QueueHandle_t vì biết chắc thành viên của set này chỉ toàn là
        queue, không có semaphore). */
        xQueueThatContainsData =
            ( QueueHandle_t ) xQueueSelectFromSet( xQueueSet, portMAX_DELAY );

        /* Đã biết chắc queue này có dữ liệu nên không cần block time. */
        xQueueReceive( xQueueThatContainsData, &pcReceivedString, 0 );

        ESP_LOGI( TAG, "%s", pcReceivedString );
    }
}