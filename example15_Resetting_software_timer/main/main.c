#include <stdio.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/timers.h"

#include "esp_log.h"

static const char *TAG = "backlight_demo";

#define mainBACKLIGHT_TIMER_PERIOD    ( pdMS_TO_TICKS( 5000UL ) )

/*-----------------------------------------------------------*/

static void prvBacklightTimerCallback( TimerHandle_t xTimer );
static void vKeyHitTask( void * pvParameters );

/*-----------------------------------------------------------*/

static BaseType_t xSimulatedBacklightOn = pdFALSE;
static TimerHandle_t xBacklightTimer = NULL;

/*-----------------------------------------------------------*/

void app_main( void )
{
    xSimulatedBacklightOn = pdFALSE;

    xBacklightTimer = xTimerCreate( "Backlight",
                                     mainBACKLIGHT_TIMER_PERIOD,
                                     pdFALSE,
                                     0,
                                     prvBacklightTimerCallback );

    xTaskCreate( vKeyHitTask, "Key poll", configMINIMAL_STACK_SIZE * 2, NULL, tskIDLE_PRIORITY + 1, NULL );

    xTimerStart( xBacklightTimer, 0 );
}
/*-----------------------------------------------------------*/

static void prvBacklightTimerCallback( TimerHandle_t xTimer )
{
    TickType_t xTimeNow = xTaskGetTickCount();

    xSimulatedBacklightOn = pdFALSE;

    ESP_LOGI( TAG, "Timer expired, turning backlight OFF at time %lu", (unsigned long) xTimeNow );
}
/*-----------------------------------------------------------*/

static void vKeyHitTask( void * pvParameters )
{
    const TickType_t xShortDelay = pdMS_TO_TICKS( 50 );
    TickType_t xTimeNow;
    int cKey;

    ESP_LOGI( TAG, "Press a key to turn the backlight on." );

    for( ; ; )
    {
        cKey = getchar();

        if( cKey != EOF )
        {
            xTimeNow = xTaskGetTickCount();

            if( xSimulatedBacklightOn == pdFALSE )
            {
                xSimulatedBacklightOn = pdTRUE;
                ESP_LOGI( TAG, "Key pressed, turning backlight ON at time %lu", (unsigned long) xTimeNow );
            }
            else
            {
                ESP_LOGI( TAG, "Key pressed, resetting software timer at time %lu", (unsigned long) xTimeNow );
            }

            xTimerReset( xBacklightTimer, xShortDelay );
        }

        vTaskDelay( xShortDelay );
    }
}
/*-----------------------------------------------------------*/