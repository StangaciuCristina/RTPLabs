#include "main.h"
#include "cmsis_os.h"
#include "application5.h"

/* Used as a loop counter to create a very crude delay. */
#define mainDELAY_LOOP_COUNT		( 0xffffff )
#define GREEN_LED LD4_Pin
#define BLUE_LED LD6_Pin
#define RED_LED LD5_Pin
#define ORANGE_LED LD3_Pin

void vTaskFunction(void *pvParameters)
{
	uint16_t *LED;
	TickType_t xLastWakeTime;
	const TickType_t xDelay250ms = pdMS_TO_TICKS( 250UL );

	LED = (uint16_t  *)pvParameters;
	/* The xLastWakeTime variable needs to be initialized with the current tick
	count.  Note that this is the only time we access this variable.  From this
	point on xLastWakeTime is managed automatically by the vTaskDelayUntil()
	API function. */
	xLastWakeTime = xTaskGetTickCount();

	/* As per most tasks, this task is implemented in an infinite loop. */
	for (;;)
	{
		/* toggle the right LED. */
		HAL_GPIO_TogglePin(GPIOD, *LED);

		/* We want this task to execute exactly every 250 milliseconds.  As per
				the vTaskDelay() function, time is measured in ticks, and the
				pdMS_TO_TICKS() macro is used to convert this to milliseconds.
				xLastWakeTime is automatically updated within vTaskDelayUntil() so does not
				have to be updated by this task code. */
		//osDelayUntil(&xLastWakeTime, 250UL);
		vTaskDelayUntil(&xLastWakeTime,xDelay250ms);
		/*		in CMSIS-OS API, vTaskDelayUntil is wrapped to osDelayUntil, the diference is that the second parameter is in ms*/
	}
}

uint16_t  Task1LED = GREEN_LED;
uint16_t  Task2LED = BLUE_LED;


void application5(void)
{

	/* Create one of the two tasks. */
	xTaskCreate(vTaskFunction,			/* Pointer to the function that implements the task. */
		"Task 1",				/* Text name for the task.  This is to facilitate debugging only. */
		1000,					/* Stack depth - most small microcontrollers will use much less stack than this. */
		(void*)&Task1LED,	    /* Pass an identifier for the LED to be toggled. */
		1,						/* This task will run at priority 1. */
		NULL);					/* We are not using the task handle. */

	/* Create the other task in exactly the same way.  Note this time that we
	are creating the SAME task, but passing in a different parameter.  We are
	creating two instances of a single task implementation. */
	xTaskCreate(vTaskFunction, "Task 2", 1000, (void*)&Task2LED, 2, NULL);

}
