#include "main.h"
#include "cmsis_os.h"
#include "timers.h"
#include "application13.h"

/* Used as a loop counter to create a very crude delay. */
#define mainDELAY_LOOP_COUNT		( 0xffffff )
#define GREEN_LED LD4_Pin
#define BLUE_LED LD6_Pin
#define RED_LED LD5_Pin
#define ORANGE_LED LD3_Pin

/* The periods assigned to the one-shot and auto-reload timers respectively. */
#define mainONE_SHOT_TIMER_PERIOD		( pdMS_TO_TICKS( 3333UL ) )
#define mainAUTO_RELOAD_TIMER_PERIOD	( pdMS_TO_TICKS( 500UL ) )


static void prvOneShotTimerCallback( TimerHandle_t xTimer )
{
static TickType_t xTimeNow;

	/* Obtain the current tick count. */
	xTimeNow = xTaskGetTickCount();

	//toggle led
	HAL_GPIO_TogglePin(GPIOD, RED_LED);
}
/*-----------------------------------------------------------*/

static void prvAutoReloadTimerCallback( TimerHandle_t xTimer )
{
static TickType_t xTimeNow;

	/* Obtain the current tick count. */
	xTimeNow = xTaskGetTickCount();

	//toggle led
	HAL_GPIO_TogglePin(GPIOD, GREEN_LED);
}
/*-----------------------------------------------------------*/





inline void application13(void)
{

	TimerHandle_t xAutoReloadTimer, xOneShotTimer;
	BaseType_t xTimer1Started, xTimer2Started;

		/* Create the one shot software timer, storing the handle to the created
		software timer in xOneShotTimer. */
		xOneShotTimer = xTimerCreate( "OneShot",					/* Text name for the software timer - not used by FreeRTOS. */
									  mainONE_SHOT_TIMER_PERIOD,	/* The software timer's period in ticks. */
									  pdFALSE,						/* Setting uxAutoRealod to pdFALSE creates a one-shot software timer. */
									  0,							/* This example does not use the timer id. */
									  prvOneShotTimerCallback );	/* The callback function to be used by the software timer being created. */

		/* Create the auto-reload software timer, storing the handle to the created
		software timer in xAutoReloadTimer. */
		xAutoReloadTimer = xTimerCreate( "AutoReload",					/* Text name for the software timer - not used by FreeRTOS. */
										 mainAUTO_RELOAD_TIMER_PERIOD,	/* The software timer's period in ticks. */
										 pdTRUE,						/* Set uxAutoRealod to pdTRUE to create an auto-reload software timer. */
										 0,								/* This example does not use the timer id. */
										 prvAutoReloadTimerCallback );	/* The callback function to be used by the software timer being created. */

		/* Check the timers were created. */
		if( ( xOneShotTimer != NULL ) && ( xAutoReloadTimer != NULL ) )
		{
			/* Start the software timers, using a block time of 0 (no block time).
			The scheduler has not been started yet so any block time specified here
			would be ignored anyway. */
			xTimer1Started = xTimerStart( xOneShotTimer, 0 );
			xTimer2Started = xTimerStart( xAutoReloadTimer, 0 );

			/* The implementation of xTimerStart() uses the timer command queue, and
			xTimerStart() will fail if the timer command queue gets full.  The timer
			service task does not get created until the scheduler is started, so all
			commands sent to the command queue will stay in the queue until after
			the scheduler has been started.  Check both calls to xTimerStart()
			passed. */
		}


}
