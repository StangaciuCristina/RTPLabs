#include "main.h"
#include "cmsis_os.h"
#include "application1.h"
/* Used as a loop counter to create a very crude delay. */
#define mainDELAY_LOOP_COUNT		( 0xffffff )

#define GREEN_LED LD4_Pin
#define BLUE_LED LD6_Pin
#define RED_LED LD5_Pin
#define ORANGE_LED LD3_Pin

void vTask1(void *pvParameters)
{

	volatile uint32_t ul;

	/* As per most tasks, this task is implemented in an infinite loop. */
	for (;;)
	{
		/* Toggle green led. */
		HAL_GPIO_TogglePin(GPIOD, GREEN_LED);

		/* Delay for a period. */
		for (ul = 0; ul < mainDELAY_LOOP_COUNT; ul++)
		{
			/* This loop is just a very crude delay implementation.  There is
			nothing to do in here.  Later exercises will replace this crude
			loop with a proper delay/sleep function. */
		}
	}
}
/*-----------------------------------------------------------*/

void vTask2(void *pvParameters)
{

	volatile uint32_t ul;

	/* As per most tasks, this task is implemented in an infinite loop. */
	for (;;)
	{
		/* Toggle blue led. */
		HAL_GPIO_TogglePin(GPIOD, BLUE_LED);

		/* Delay for a period. */
		for (ul = 0; ul < mainDELAY_LOOP_COUNT; ul++)
		{
			/* This loop is just a very crude delay implementation.  There is
			nothing to do in here.  Later exercises will replace this crude
			loop with a proper delay/sleep function. */
		}
	}
}

inline void application1(void)
{


	/* Create one of the two tasks. */
	xTaskCreate(vTask1,		/* Pointer to the function that implements the task. */
		"Task 1",	/* Text name for the task.  This is to facilitate debugging only. */
		1000,		/* Stack depth - most small microcontrollers will use much less stack than this. */
		NULL,		/* We are not using the task parameter. */
		1,			/* This task will run at priority 1. */
		NULL);		/* We are not using the task handle. */

	/* Create the other task in exactly the same way. */
	xTaskCreate(vTask2, "Task 2", 1000, NULL, 1, NULL);

}
