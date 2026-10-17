/*
 * Example 5 - Converting the example tasks to use vTaskDelayUntil()
 * vTaskDelay() chỉ đảm bảo khoảng nghỉ TƯƠNG ĐỐI (tính từ lúc gọi),
 * còn vTaskDelayUntil() đảm bảo chu kỳ chạy CỐ ĐỊNH (tính theo mốc
 * tuyệt đối), phù hợp cho task định kỳ thật sự.
 */
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

void vTaskFunction(void *pvParameters)
{
    char *pcTaskName = (char *) pvParameters;

    /* Ghi lại thời điểm "thức dậy" gần nhất - chỉ khởi tạo 1 lần ở đây,
     * sau đó vTaskDelayUntil() tự động cập nhật */
    TickType_t xLastWakeTime = xTaskGetTickCount();

    for (;;)
    {
        printf("%s", pcTaskName);

        /* Task sẽ được đánh thức đúng 250ms kể từ xLastWakeTime,
         * bất kể phần thân vòng lặp phía trên chạy mất bao lâu */
        vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(250));
    }
}

void app_main(void)
{
    static const char *pcTextForTask1 = "Task 1 is running\r\n";
    static const char *pcTextForTask2 = "Task 2 is running\r\n";

    xTaskCreate(vTaskFunction, "Task 1", 2048, (void *) pcTextForTask1, 1, NULL);
    xTaskCreate(vTaskFunction, "Task 2", 2048, (void *) pcTextForTask2, 2, NULL);
}
