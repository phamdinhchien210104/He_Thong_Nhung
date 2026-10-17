/*
 * Example 7 - Defining an idle task hook function
 *
 * QUAN TRỌNG trên ESP-IDF: vApplicationIdleHook() chỉ được gọi nếu bạn
 * bật "legacy hook" qua sdkconfig.defaults (đã cấu hình sẵn):
 *   CONFIG_FREERTOS_LEGACY_HOOKS=y
 *   CONFIG_FREERTOS_LEGACY_IDLE_HOOK=y
 * (Thay thế hiện đại hơn, không cần menuconfig, là
 *  esp_register_freertos_idle_hook_for_cpu() - nhưng để bám sát đúng
 *  API vApplicationIdleHook() trong slide thì dùng cách legacy hook trên.)
 */
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

static volatile unsigned long ulIdleCycleCount = 0UL;

/* Tên hàm và prototype bắt buộc phải đúng như thế này */
void vApplicationIdleHook(void)
{
    ulIdleCycleCount++;
}

void vTaskFunction(void *pvParameters)
{
    char *pcTaskName = (char *) pvParameters;
    TickType_t xLastWakeTime = xTaskGetTickCount();

    for (;;)
    {
        printf("%s", pcTaskName);
        printf("Idle cycle count: %lu\r\n", ulIdleCycleCount);

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
