#include <stdio.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/timers.h"

#include "esp_log.h"

static const char *TAG = "timer_demo";

#define mainONE_SHOT_TIMER_PERIOD       ( pdMS_TO_TICKS( 3333UL ) )
#define mainAUTO_RELOAD_TIMER_PERIOD    ( pdMS_TO_TICKS( 500UL ) )

/*-----------------------------------------------------------*/

static void prvTimerCallback( TimerHandle_t xTimer );

/*-----------------------------------------------------------*/

static TimerHandle_t xAutoReloadTimer, xOneShotTimer;

void app_main( void )
{
    BaseType_t xTimer1Started, xTimer2Started;

    xOneShotTimer = xTimerCreate( "OneShot",
                                   mainONE_SHOT_TIMER_PERIOD,
                                   pdFALSE,
                                   0,
                                   prvTimerCallback );

    xAutoReloadTimer = xTimerCreate( "AutoReload",
                                      mainAUTO_RELOAD_TIMER_PERIOD,
                                      pdTRUE,
                                      0,
                                      prvTimerCallback );

    if( ( xOneShotTimer != NULL ) && ( xAutoReloadTimer != NULL ) )
    {
        xTimer1Started = xTimerStart( xOneShotTimer, 0 );
        xTimer2Started = xTimerStart( xAutoReloadTimer, 0 );

        if( ( xTimer1Started == pdPASS ) && ( xTimer2Started == pdPASS ) )
        {
            ESP_LOGI( TAG, "Timers started successfully" );
        }
        else
        {
            ESP_LOGE( TAG, "Failed to start one or both timers" );
        }
    }
    else
    {
        ESP_LOGE( TAG, "Failed to create one or both timers" );
    }
}
/*-----------------------------------------------------------*/

static void prvTimerCallback( TimerHandle_t xTimer )
{
    TickType_t xTimeNow;
    uint32_t ulExecutionCount;

    ulExecutionCount = ( uint32_t ) pvTimerGetTimerID( xTimer );
    ulExecutionCount++;
    vTimerSetTimerID( xTimer, ( void * ) ulExecutionCount );

    xTimeNow = xTaskGetTickCount();

    if( xTimer == xOneShotTimer )
    {
        ESP_LOGI( TAG, "One-shot timer callback executing %lu", (unsigned long) xTimeNow );
    }
    else
    {
        ESP_LOGI( TAG, "Auto-reload timer callback executing %lu", (unsigned long) xTimeNow );

        if( ulExecutionCount == 5 )
        {
            xTimerStop( xTimer, 0 );
        }
    }
}
/*-----------------------------------------------------------*/