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

inline void application2(void)
{
	BaseType_t xReturned1, xReturned2;
	/* Create one of the two tasks. */
	xReturned1=xTaskCreate(	vTaskFunction,			/* Pointer to the function that implements the task. */
					"Task 1",				/* Text name for the task.  This is to facilitate debugging only. */
					128,					/* Stack depth */
					(void*)&Task1LED,	    /* Pass an identifier for the LED to be toggled. */
					1,						/* This task will run at priority 1. */
					NULL );					/* We are not using the task handle. */

	/* Create the other task in exactly the same way.  Note this time that we
	are creating the SAME task, but passing in a different parameter.  We are
	creating two instances of a single task implementation. */
	xReturned2=xTaskCreate( vTaskFunction, "Task 2", 128, (void*)&Task2LED, 1, NULL );
	if (( xReturned1 == pdPASS )&&( xReturned2 == pdPASS ))
				  vTaskStartScheduler();
}
