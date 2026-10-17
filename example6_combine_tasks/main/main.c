/*
 * Example 6 - Combining blocking and non-blocking tasks
 * - 2 task priority 1: continuous processing task (không gọi API nào
 *   khiến vào Blocked state)
 * - 1 task priority 2: periodic task, dùng vTaskDelayUntil()
 *
 * Lưu ý: bản gốc trên slide có gọi delay(100) ngay trong task "continuous"
 * (dù về lý thuyết task continuous không nên tự block) - mình giữ nguyên
 * ý slide (vTaskDelay(100)) chỉ để log không bị in tràn màn hình quá nhanh.
 */
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

void vContinuousProcessingTask(void *pvParameters)
{
    char *pcTaskName = (char *) pvParameters;

    for (;;)
    {
        printf("%s", pcTaskName);
        vTaskDelay(pdMS_TO_TICKS(100));
    }
}

void vPeriodicTask(void *pvParameters)
{
    char *pcTaskName = (char *) pvParameters;
    TickType_t xLastWakeTime = xTaskGetTickCount();

    for (;;)
    {
        printf("%s", pcTaskName);
        vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(500));
    }
}

void app_main(void)
{
    static const char *pcTextForTask1 = "Continuous task 1 is running\r\n";
    static const char *pcTextForTask2 = "Continuous task 2 is running\r\n";
    static const char *pcTextForTask3 = "Periodic task is running\r\n";

    /* Hai task continuous - priority 1 */
    xTaskCreate(vContinuousProcessingTask, "Task 1", 2048, (void *) pcTextForTask1, 1, NULL);
    xTaskCreate(vContinuousProcessingTask, "Task 2", 2048, (void *) pcTextForTask2, 1, NULL);

    /* Task periodic - priority 2 (cao hơn) */
    xTaskCreate(vPeriodicTask, "Task 3", 2048, (void *) pcTextForTask3, 2, NULL);
}
