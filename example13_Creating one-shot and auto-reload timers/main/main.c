#include <stdio.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/timers.h"

#include "esp_log.h"

static const char *TAG = "timer_demo";

/* Chu kỳ của timer one-shot và auto-reload */
#define mainONE_SHOT_TIMER_PERIOD       ( pdMS_TO_TICKS( 3333UL ) )
#define mainAUTO_RELOAD_TIMER_PERIOD    ( pdMS_TO_TICKS( 500UL ) )

/*-----------------------------------------------------------*/

static void prvOneShotTimerCallback( TimerHandle_t xTimer );
static void prvAutoReloadTimerCallback( TimerHandle_t xTimer );

/*-----------------------------------------------------------*/

void app_main( void )
{
    TimerHandle_t xAutoReloadTimer, xOneShotTimer;
    BaseType_t xTimer1Started, xTimer2Started;

    /* Tạo timer one-shot. */
    xOneShotTimer = xTimerCreate( "OneShot",
                                   mainONE_SHOT_TIMER_PERIOD,
                                   pdFALSE,                  /* pdFALSE => one-shot. */
                                   0,
                                   prvOneShotTimerCallback );

    /* Tạo timer auto-reload. */
    xAutoReloadTimer = xTimerCreate( "AutoReload",
                                      mainAUTO_RELOAD_TIMER_PERIOD,
                                      pdTRUE,                /* pdTRUE => auto-reload. */
                                      0,
                                      prvAutoReloadTimerCallback );

    /* Kiểm tra tạo timer thành công. */
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

static void prvOneShotTimerCallback( TimerHandle_t xTimer )
{
    static TickType_t xTimeNow;

    xTimeNow = xTaskGetTickCount();
    ESP_LOGI( TAG, "One-shot timer callback executing %lu", (unsigned long) xTimeNow );
}
/*-----------------------------------------------------------*/

static void prvAutoReloadTimerCallback( TimerHandle_t xTimer )
{
    static TickType_t xTimeNow;

    xTimeNow = xTaskGetTickCount();
    ESP_LOGI( TAG, "Auto-reload timer callback executing %lu", (unsigned long) xTimeNow );
}
/*-----------------------------------------------------------*/
