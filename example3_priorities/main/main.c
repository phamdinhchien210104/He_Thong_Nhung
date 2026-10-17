/*
 * Example 3 - Experimenting with priorities
 * Giống hệt Example 2, chỉ khác: Task 2 được tạo ở priority 2 (cao hơn
 * Task 1 - priority 1). Scheduler luôn chọn task priority cao nhất có
 * thể chạy, nên Task 2 sẽ chiếm CPU hoàn toàn, "bỏ đói" (starve) Task 1.
 */
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#define mainDELAY_LOOP_COUNT 1000000

void vTaskFunction(void *pvParameters)
{
    char *pcTaskName;
    volatile unsigned long ul;

    pcTaskName = (char *) pvParameters;

    for (;;)
    {
        printf("%s", pcTaskName);

        for (ul = 0; ul < mainDELAY_LOOP_COUNT; ul++)
        {
        }
    }
}

void app_main(void)
{
    static const char *pcTextForTask1 = "Task 1 is running\r\n";
    static const char *pcTextForTask2 = "Task 2 is running\r\n";

    xTaskCreate(vTaskFunction, "Task 1", 2048, (void *) pcTextForTask1, 1, NULL);

    /* Task 2 priority 2 (cao hơn Task 1) -> Task 1 gần như không bao giờ
     * được chạy vì đây vẫn là vòng lặp busy-wait, không nhường CPU */
    xTaskCreate(vTaskFunction, "Task 2", 2048, (void *) pcTextForTask2, 2, NULL);
}
