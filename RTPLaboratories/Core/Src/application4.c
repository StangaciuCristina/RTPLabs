#include "main.h"
#include "cmsis_os.h"
#include "application4.h"

/* Used as a loop counter to create a very crude delay. */
#define mainDELAY_LOOP_COUNT		( 0xffffff )
#define GREEN_LED LD4_Pin
#define BLUE_LED LD6_Pin
#define RED_LED LD5_Pin
#define ORANGE_LED LD3_Pin

void vTaskFunction( void *pvParameters )
{
	uint16_t *LED;
	const TickType_t xDelay250ms = pdMS_TO_TICKS( 250UL );
	/* The string to print out is passed in via the parameter.  Cast this to a
	character pointer. */
	LED = ( uint16_t  * ) pvParameters;

	/* As per most tasks, this task is implemented in an infinite loop. */
	for( ;; )
	{
		/* toggle the right LED. */
		HAL_GPIO_TogglePin(GPIOD, *LED);

		/* Delay for a period.  This time a call to vTaskDelay() is used which
				places the task into the Blocked state until the delay period has
				expired.  The parameter takes a time specified in 'ticks', and the
				pdMS_TO_TICKS() macro is used (where the xDelay250ms constant is
				declared) to convert 250 milliseconds into an equivalent time in
				ticks. */
		vTaskDelay( xDelay250ms );
	}
}

uint16_t  Task1LED = GREEN_LED;
uint16_t  Task2LED = BLUE_LED;


inline void application4(void)
{

	/* Create one of the two tasks. */
	xTaskCreate(	vTaskFunction,			/* Pointer to the function that implements the task. */
					"Task 1",				/* Text name for the task.  This is to facilitate debugging only. */
					1000,					/* Stack depth - most small microcontrollers will use much less stack than this. */
					(void*)&Task1LED,	    /* Pass an identifier for the LED to be toggled. */
					1,						/* This task will run at priority 1. */
					NULL );					/* We are not using the task handle. */

	/* Create the other task in exactly the same way.  Note this time that we
	are creating the SAME task, but passing in a different parameter.  We are
	creating two instances of a single task implementation. */
	xTaskCreate( vTaskFunction, "Task 2", 1000, (void*)&Task2LED, 2, NULL );

}
