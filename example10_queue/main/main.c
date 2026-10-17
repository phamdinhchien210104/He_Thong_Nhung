#include <stdio.h>
#include <stdint.h>

#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/task.h"

#include "esp_log.h"

static const char *TAG = "QUEUE_DEMO";

static QueueHandle_t xQueue = NULL;

static void vSenderTask(void *pvParameters) {
  long lValueToSend;
  BaseType_t xStatus;

  lValueToSend = (intptr_t)pvParameters;
  ESP_LOGI(TAG, "lValueToSend = %ld", lValueToSend);

  for (;;) {
    xStatus = xQueueSendToBack(xQueue, &lValueToSend, 0);

    if (xStatus != pdPASS) {
      ESP_LOGW(TAG, "Could not send to the queue.");
    }

    /* Cho phép task sender còn lại được chạy */
    // taskYIELD();
  }
}

static void vReceiverTask(void *pvParameters) {
  long lReceivedValue;
  BaseType_t xStatus;
  const TickType_t xTicksToWait = pdMS_TO_TICKS(100);

  for (;;) {
    if (uxQueueMessagesWaiting(xQueue) != 0) {
      ESP_LOGW(TAG, "Queue should have been empty!");
    }

    xStatus = xQueueReceive(xQueue, &lReceivedValue, xTicksToWait);

    if (xStatus == pdPASS) {
      ESP_LOGI(TAG, "Received = %ld", lReceivedValue);
    } else {
      ESP_LOGW(TAG, "Could not receive from the queue.");
    }
  }
}

void app_main(void) {
  /* Queue được tạo để chứa tối đa 5 giá trị kiểu long */
  xQueue = xQueueCreate(5, sizeof(long));

  if (xQueue != NULL) {
    xTaskCreate(vReceiverTask, "Receiver", 2048, NULL, 2, NULL);
    xTaskCreate(vSenderTask, "Sender1", 2048, (void *)(intptr_t)100, 1, NULL);
    xTaskCreate(vSenderTask, "Sender2", 2048, (void *)(intptr_t)200, 1, NULL);

    // /* Tạo task đọc từ queue, ưu tiên cao hơn 2 task sender ở trên */
    // /*xTaskCreate(vReceiverTask, "Receiver", 2048, NULL, 1, NULL); */

    //  Không cần gọi vTaskStartScheduler(): trong ESP-IDF scheduler
    //    đã được khởi động sẵn từ trước khi app_main() được gọi.
  } else {
    /* Queue không tạo được */
    ESP_LOGE(TAG, "The queue could not be created.");
  }
}
