#include "main.h"
#include "cmsis_os.h"
#include "application8.h"

/* Used as a loop counter to create a very crude delay. */
#define mainDELAY_LOOP_COUNT		( 0xffffff )
#define GREEN_LED LD4_Pin
#define BLUE_LED LD6_Pin
#define RED_LED LD5_Pin
#define ORANGE_LED LD3_Pin

/* Used to hold the handle of Task2. */
TaskHandle_t xTask2Handle;

void vTask1( void *pvParameters )
{
UBaseType_t uxPriority;
	uint16_t *LED;
	LED = (uint16_t  *)pvParameters;
	uint32_t ul;
	/* This task will always run before Task2 as it has the higher priority.
	Neither Task1 nor Task2 ever block so both will always be in either the
	Running or the Ready state.

	Query the priority at which this task is running - passing in NULL means
	"return our own priority". */
	uxPriority = uxTaskPriorityGet( NULL );

	for( ;; )
	{
		HAL_GPIO_TogglePin(GPIOD, *LED);
		for (ul=0;ul<mainDELAY_LOOP_COUNT;ul++)
			;  //a blocking sofware delay, just to see the LED blink

		/* Setting the Task2 priority above the Task1 priority will cause
		Task2 to immediately start running (as then Task2 will have the higher
		priority of the two created tasks). */

		vTaskPrioritySet( xTask2Handle, ( uxPriority + 1 ) );

		/* Task1 will only run when it has a priority higher than Task2.
		Therefore, for this task to reach this point Task2 must already have
		executed and set its priority back down to 0. */
	}
}

/*-----------------------------------------------------------*/

void vTask2( void *pvParameters )
{
    UBaseType_t uxPriority;
    uint16_t *LED;
	LED = (uint16_t  *)pvParameters;
	uint32_t ul;
	/* Task1 will always run before this task as Task1 has the higher priority.
	Neither Task1 nor Task2 ever block so will always be in either the
	Running or the Ready state.

	Query the priority at which this task is running - passing in NULL means
	"return our own priority". */
	uxPriority = uxTaskPriorityGet( NULL );

	for( ;; )
	{
		/* For this task to reach this point Task1 must have already run and
		set the priority of this task higher than its own.*/

		HAL_GPIO_TogglePin(GPIOD, *LED);
		for (ul=0;ul<mainDELAY_LOOP_COUNT;ul++)
					;  //a blocking sofware delay, just to see the LED blink

		/* Set our priority back down to its original value.  Passing in NULL
		as the task handle means "change our own priority".  Setting the
		priority below that of Task1 will cause Task1 to immediately start
		running again. */

		vTaskPrioritySet( NULL, ( uxPriority - 2 ) );
	}
}
/*-----------------------------------------------------------*/


uint16_t  Task1LED = GREEN_LED;
uint16_t  Task2LED = BLUE_LED;
uint16_t  Task3LED = RED_LED;


inline void application8(void)
{
		BaseType_t xReturned1,xReturned2;
	/* Create the first task at priority 2.  The task handle is not used so is set to NULL. */
		xReturned1=xTaskCreate( vTask1, "Task 1", 128, (void*)&Task1LED, 2, NULL );
	          /* The task is created at priority 2 ^. */

		/* Create the second task at priority 1 - which is lower than the priority
		given to Task1.  BUT this time we want to obtain a handle to the task so pass in the address
		of the xTask2Handle variable. */
		xReturned2=xTaskCreate( vTask2, "Task 2", 128, (void*)&Task2LED, 1, &xTask2Handle );
	    /* The task handle is the last parameter                 ^^^^^^^^^^^^^ */
		if((xReturned1==pdPASS)&&(xReturned2==pdPASS))
			vTaskStartScheduler();

}
