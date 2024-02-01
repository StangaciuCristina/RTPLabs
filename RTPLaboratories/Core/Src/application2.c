#include "main.h"
#include "cmsis_os.h"
#include "application2.h"

/* Used as a loop counter to create a very crude delay. */
#define mainDELAY_LOOP_COUNT		( 0xffffff )
#define GREEN_LED LD4_Pin
#define BLUE_LED LD6_Pin
#define RED_LED LD5_Pin
#define ORANGE_LED LD3_Pin

void vTaskFunction( void *pvParameters )
{
uint16_t *LED;
volatile uint32_t ul;

	/* The string to print out is passed in via the parameter.  Cast this to a
	character pointer. */
	LED = ( uint16_t  * ) pvParameters;

	/* As per most tasks, this task is implemented in an infinite loop. */
	for( ;; )
	{
		/* toggle the right LED. */
		HAL_GPIO_TogglePin(GPIOD, *LED);

		/* Delay for a period. */
		for( ul = 0; ul < mainDELAY_LOOP_COUNT; ul++ )
		{
			/* This loop is just a very crude delay implementation.  There is
			nothing to do in here.  Later exercises will replace this crude
			loop with a proper delay/sleep function. */
		}
	}
}

uint16_t  Task1LED = GREEN_LED;
uint16_t  Task2LED = BLUE_LED;


void application2(void)
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
	xTaskCreate( vTaskFunction, "Task 2", 1000, (void*)&Task2LED, 1, NULL );

}
