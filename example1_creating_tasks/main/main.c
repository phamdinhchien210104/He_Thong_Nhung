/*
 * Example 1 - Creating 2 tasks
 * Hai task giống hệt nhau (chỉ khác chuỗi in ra), cùng priority 1.
 * Độ trễ được tạo bằng vòng lặp null (busy-wait) - cách làm "thô",
 * các ví dụ sau (Example 4 trở đi) sẽ thay bằng vTaskDelay().
 */
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

/* ESP32 chạy nhanh hơn nhiều so với AVR (Arduino Uno) trong sách gốc,
 * nên cần tăng số vòng lặp lên đáng kể mới thấy rõ độ trễ */
#define mainDELAY_LOOP_COUNT 1000000

void vTask1(void *pvParameters)
{
    const char *pcTaskName = "Task 1 is running\r\n";
    volatile unsigned long ul;

    /* Như hầu hết các task, task này được cài đặt dưới dạng vòng lặp vô hạn */
    for (;;)
    {
        /* In tên task ra */
        printf("%s", pcTaskName);

        /* Trễ 1 khoảng thời gian */
        for (ul = 0; ul < mainDELAY_LOOP_COUNT; ul++)
        {
            /* Vòng lặp trễ thô sơ, không làm gì cả */
        }
    }
}

void vTask2(void *pvParameters)
{
    const char *pcTaskName = "Task 2 is running\r\n";
    volatile unsigned long ul;

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
    /* Tạo 2 task, cùng priority 1 */
    xTaskCreate(vTask1, "Task 1", 2048, NULL, 1, NULL);
    xTaskCreate(vTask2, "Task 2", 2048, NULL, 1, NULL);

    /* Không cần gọi vTaskStartScheduler() - ESP-IDF đã chạy scheduler rồi */
}
