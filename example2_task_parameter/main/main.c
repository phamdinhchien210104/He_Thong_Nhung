/*
 * Example 2 - Using the task parameter
 * 2 task trong Example 1 gần như giống hệt nhau, chỉ khác chuỗi in ra.
 * Gộp thành 1 hàm vTaskFunction() dùng chung, truyền chuỗi cần in qua
 * tham số pvParameters khi tạo từng instance của task.
 */
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#define mainDELAY_LOOP_COUNT 1000000

void vTaskFunction(void *pvParameters)
{
    char *pcTaskName;
    volatile unsigned long ul;

    /* Chuỗi cần in ra được truyền vào qua tham số, ép kiểu về con trỏ char */
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

    /* Tạo 2 instance độc lập của cùng 1 task, mỗi instance chạy dưới
     * sự quản lý riêng của scheduler */
    xTaskCreate(vTaskFunction, "Task 1", 2048, (void *) pcTextForTask1, 1, NULL);
    xTaskCreate(vTaskFunction, "Task 2", 2048, (void *) pcTextForTask2, 1, NULL);
}
