/*
 * Example 8 - Changing task priorities
 *
 * Task 1 (priority 2) chạy trước, nâng priority của Task 2 lên cao hơn
 * mình -> Task 2 (priority 1 ban đầu) giành quyền chạy -> Task 2 tự hạ
 * priority của chính nó xuống thấp hơn Task 1 -> Task 1 giành lại quyền
 * chạy. Lặp lại chu trình.
 */
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

/* Handle của Task 2 - cần biết để Task 1 gọi vTaskPrioritySet() lên nó */
static TaskHandle_t xTask2Handle = NULL;

void vTask1(void *pvParameters)
{
    /* Đọc priority ban đầu của chính mình 1 lần khi task khởi tạo */
    UBaseType_t uxPriority = uxTaskPriorityGet(NULL);

    for (;;)
    {
        printf("Task 1 is running\r\n");
        printf("About to raise the Task 2 priority\r\n");

        /* Nâng priority Task 2 lên cao hơn priority hiện tại của Task 1 */
        vTaskPrioritySet(xTask2Handle, (uxPriority + 1));

        /* Đến đây Task 1 sẽ bị Task 2 (priority cao hơn) chiếm quyền chạy */
    }
}

void vTask2(void *pvParameters)
{
    UBaseType_t uxPriority = uxTaskPriorityGet(NULL);

    for (;;)
    {
        printf("Task 2 is running\r\n");
        printf("About to lower its own priority\r\n");

        /* Tự hạ priority của chính mình xuống thấp hơn Task 1 */
        vTaskPrioritySet(NULL, (uxPriority - 2));

        /* Đến đây Task 2 sẽ bị Task 1 (giờ priority cao hơn) chiếm quyền chạy */
    }
}

void app_main(void)
{
    /* Task 1: priority 2, Task 2: priority 1 (thấp hơn ban đầu) */
    xTaskCreate(vTask1, "Task 1", 2048, NULL, 2, NULL);
    xTaskCreate(vTask2, "Task 2", 2048, NULL, 1, &xTask2Handle);
}
