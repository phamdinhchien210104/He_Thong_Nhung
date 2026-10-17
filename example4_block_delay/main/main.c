/*
 * Example 4 - Using the Block state to create a delay
 * Thay vòng lặp null (busy-wait) bằng vTaskDelay() để task nhường CPU
 * trong lúc chờ, thay vì chiếm dụng CPU vô ích.
 */
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

void vTaskFunction(void *pvParameters)
{
    char *pcTaskName = (char *) pvParameters;

    for (;;)
    {
        /* In tên task ra */
        printf("%s", pcTaskName);

        /* Thay vì busy-wait, task chuyển sang trạng thái Blocked trong 250ms */
        vTaskDelay(pdMS_TO_TICKS(250));
    }
}

void app_main(void)
{
    static const char *pcTextForTask1 = "Task 1 is running\r\n";
    static const char *pcTextForTask2 = "Task 2 is running\r\n";

    /* Hai task khác priority (giống Example 3), nhưng giờ cả hai đều
     * được chạy vì không còn task nào chiếm CPU liên tục nữa */
    xTaskCreate(vTaskFunction, "Task 1", 2048, (void *) pcTextForTask1, 1, NULL);
    xTaskCreate(vTaskFunction, "Task 2", 2048, (void *) pcTextForTask2, 2, NULL);

    /* Không cần gọi vTaskStartScheduler() - ESP-IDF đã chạy scheduler rồi */
}
