#include "main.h"
#include "cmsis_os.h"
#include "timers.h"
#include "application14.h"

/* Used as a loop counter to create a very crude delay. */
#define mainDELAY_LOOP_COUNT		( 0xffffff )
#define GREEN_LED LD4_Pin
#define BLUE_LED LD6_Pin
#define RED_LED LD5_Pin
#define ORANGE_LED LD3_Pin

/* The periods assigned to the one-shot and auto-reload timers respectively. */
#define mainONE_SHOT_TIMER_PERIOD		( pdMS_TO_TICKS( 333UL ) )
#define mainAUTO_RELOAD_TIMER_PERIOD	( pdMS_TO_TICKS( 50UL ) )

/* The timer handles are used inside the callback function so this time are
given file scope. */
static TimerHandle_t xAutoReloadTimer, xOneShotTimer;

static void prvTimerCallback( TimerHandle_t xTimer )
{
//TickType_t xTimeNow;
uint32_t ulExecutionCount;

	/* The count of the number of times this software timer has expired is
	stored in the timer's ID.  Obtain the ID, increment it, then save it as the
	new ID value.  The ID is a void pointer, so is cast to a uint32_t. */
	ulExecutionCount = ( uint32_t ) pvTimerGetTimerID( xTimer );
	ulExecutionCount++;
	vTimerSetTimerID( xTimer, ( void * ) ulExecutionCount );

	/* Obtain the current tick count. */
	//xTimeNow = xTaskGetTickCount();

    /* The handle of the one-shot timer was stored in xOneShotTimer when the
	timer was created.  Compare the handle passed into this function with
	xOneShotTimer to determine if it was the one-shot or auto-reload timer that
	expired, then output a string to show the time at which the callback was
	executed. */
	if( xTimer == xOneShotTimer )
	{
		//toggle led
		HAL_GPIO_TogglePin(GPIOD, RED_LED);
	}
	else
	{
        /* xTimer did not equal xOneShotTimer, so it must be the auto-reload
		timer that expired. */
		//toggle led
		HAL_GPIO_TogglePin(GPIOD, GREEN_LED);

		if( ulExecutionCount == 100 )
		{
			/* Stop the auto-reload timer after it has executed 5 times.  This
			callback function executes in the context of the RTOS daemon task so
			must not call any functions that might place the daemon task into
			the Blocked state.  Therefore a block time of 0 is used. */
			xTimerStop( xTimer, 0 );
		}
	}
}
/*-----------------------------------------------------------*/






inline void application14(void)
{

	BaseType_t xTimer1Started, xTimer2Started;

		/* Create the one shot timer, storing the handle to the created timer in
		xOneShotTimer. */
		xOneShotTimer = xTimerCreate( "OneShot",					/* Text name for the timer - not used by FreeRTOS. */
									  mainONE_SHOT_TIMER_PERIOD,	/* The timer's period in ticks. */
									  pdFALSE,						/* Set uxAutoRealod to pdFALSE to create a one-shot timer. */
									  0,							/* The timer ID is initialised to 0. */
									  prvTimerCallback );			/* The callback function to be used by the timer being created. */

		/* Create the auto-reload, storing the handle to the created timer in
		xAutoReloadTimer. */
		xAutoReloadTimer = xTimerCreate( "AutoReload",					/* Text name for the timer - not used by FreeRTOS. */
										 mainAUTO_RELOAD_TIMER_PERIOD,	/* The timer's period in ticks. */
										 pdTRUE,						/* Set uxAutoRealod to pdTRUE to create an auto-reload timer. */
										 0,								/* The timer ID is initialised to 0. */
										 prvTimerCallback );			/* The callback function to be used by the timer being created. */

		/* Check the timers were created. */
		if( ( xOneShotTimer != NULL ) && ( xAutoReloadTimer != NULL ) )
		{
			/* Start the timers, using a block time of 0 (no block time).  The
			scheduler has not been started yet so any block time specified here
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
		if((xTimer1Started==pdPASS)&&(xTimer2Started==pdPASS))
					vTaskStartScheduler();

}
