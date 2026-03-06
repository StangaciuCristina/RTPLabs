#include "main.h"
#include "cmsis_os.h"
#include "timers.h"
#include "application15.h"

/* Used as a loop counter to create a very crude delay. */
#define mainDELAY_LOOP_COUNT		( 0xffffff )
#define GREEN_LED LD4_Pin
#define BLUE_LED LD6_Pin
#define RED_LED LD5_Pin
#define ORANGE_LED LD3_Pin

/* The periods assigned to the one-shot timer. */
#define mainBACKLIGHT_TIMER_PERIOD		( pdMS_TO_TICKS( 500UL ) )



/* The software timer used to turn the backlight off. */
static TimerHandle_t xBacklightTimer = NULL;

static void prvBacklightTimerCallback( TimerHandle_t xTimer )
{

	/* Turned off in this case the green led */
	HAL_GPIO_WritePin(GPIOD, GREEN_LED, GPIO_PIN_RESET);
}
/*-----------------------------------------------------------*/

static void vKeyHitTask( void *pvParameters )
{
const TickType_t xShortDelay = pdMS_TO_TICKS( 50 );


	GPIO_PinState state;


	/* The application reads the button pushes*/
	for( ;; )
	{
		/* Has the user button been pressed? */
		state = HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_0);
		if(  state != 0 )
		{

			/* The green LED was off so turn it on */
			HAL_GPIO_WritePin(GPIOD, GREEN_LED, GPIO_PIN_SET);

			/* Reset the software timer.  If the LED was previously off
			this call will start the timer.  If the LED was previously on
			this call will restart the timer. */
			xTimerReset( xBacklightTimer, xShortDelay );


		}

		/* Don't poll too quickly. */
		vTaskDelay( xShortDelay );
	}
}
/*-----------------------------------------------------------*/


inline void application15(void)
{

		BaseType_t xTimerStarted;
		/* Create the one shot timer, storing the handle to the created timer in
		xOneShotTimer. */
		xBacklightTimer = xTimerCreate( "Backlight",				/* Text name for the timer - not used by FreeRTOS. */
										mainBACKLIGHT_TIMER_PERIOD,	/* The timer's period in ticks. */
										pdFALSE,					/* Set uxAutoRealod to pdFALSE to create a one-shot timer. */
										0,							/* The timer ID is not used in this example. */
										prvBacklightTimerCallback );/* The callback function to be used by the timer being created. */


		xTaskCreate( vKeyHitTask, "Key poll", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY, NULL );

		/* Start the timer. */
		xTimerStarted=xTimerStart( xBacklightTimer, 0 );

		if (xTimerStarted==pdPASS)
			vTaskStartScheduler();

}
