/*
 * Example 9 - Deleting a task
 *
 * Task 1 (priority 1) chạy, tự tạo ra Task 2 với priority cao hơn (2).
 * Task 2 chiếm quyền chạy ngay lập tức, in ra thông báo rồi tự xóa
 * chính mình bằng vTaskDelete(). Task 1 sau đó tiếp tục chạy, gọi
 * vTaskDelay() để nhường CPU cho Idle task - Idle task chịu trách
 * nhiệm dọn dẹp bộ nhớ của Task 2 đã bị xóa.
 *
 * Lưu ý: dòng "__malloc_heap_end = (char*)RAMEND;" trong slide chỉ dành
 * riêng cho AVR (Arduino Uno...) để cấp phát heap thủ công - trên ESP32
 * heap được cấp phát tự động nên bỏ dòng này.
 */
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

/* Handle của Task 2, dùng để Task 2 tự gọi vTaskDelete() lên chính mình */
static TaskHandle_t xTask2Handle = NULL;

/* Khai báo trước vì vTask1 gọi tới vTask2 */
void vTask2(void *pvParameters);

void vTask1(void *pvParameters)
{
    const TickType_t xDelay100ms = pdMS_TO_TICKS(100);

    for (;;)
    {
        printf("Task1 is running\r\n");

        /* Tạo Task 2 ở priority cao hơn (2), lấy handle để Task 2 tự xóa mình */
        xTaskCreate(vTask2, "Task 2", 2048, NULL, 2, &xTask2Handle);

        /* Đến đây Task 2 (priority cao hơn) chắc chắn đã chạy và tự xóa
         * xong mới quay lại được đây. Nhường CPU 100ms cho Idle task dọn
         * dẹp bộ nhớ của Task 2 vừa bị xóa. */
        vTaskDelay(xDelay100ms);
    }
}

void vTask2(void *pvParameters)
{
    /* Task 2 không làm gì ngoài việc tự xóa chính mình */
    printf("Task2 is running and about to delete itself\r\n");
    vTaskDelete(xTask2Handle);
}

void app_main(void)
{
    xTaskCreate(vTask1, "Task 1", 2048, NULL, 1, NULL);
}
