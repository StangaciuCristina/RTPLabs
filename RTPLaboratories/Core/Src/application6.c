#include "main.h"
#include "cmsis_os.h"
#include "application6.h"

/* Used as a loop counter to create a very crude delay. */
#define mainDELAY_LOOP_COUNT		( 0xffffff )
#define GREEN_LED LD4_Pin
#define BLUE_LED LD6_Pin
#define RED_LED LD5_Pin
#define ORANGE_LED LD3_Pin

void vContinuousProcessingTask( void *pvParameters )
{
	uint16_t *LED;
	LED = (uint16_t  *)pvParameters;

	/* As per most tasks, this task is implemented in an infinite loop. */
	for( ;; )
	{
		/* toggle the right LED. */
		HAL_GPIO_TogglePin(GPIOD, *LED);
	}
}
/*-----------------------------------------------------------*/
void vPeriodicTask( void *pvParameters )
{
	uint16_t *LED;
	LED = (uint16_t  *)pvParameters;
	TickType_t xLastWakeTime;
	const TickType_t xDelay250ms = pdMS_TO_TICKS( 250UL );
	/* The xLastWakeTime variable needs to be initialized with the current tick
	count.  Note that this is the only time we access this variable.  From this
	point on xLastWakeTime is managed automatically by the vTaskDelayUntil()
	API function. */
	xLastWakeTime = xTaskGetTickCount();

	/* As per most tasks, this task is implemented in an infinite loop. */
	for( ;; )
	{
		/* toggle the right LED. */
		HAL_GPIO_TogglePin(GPIOD, *LED);

		/* We want this task to execute exactly every 100 milliseconds. */
		//osDelayUntil(&xLastWakeTime, 100UL);
		vTaskDelayUntil(&xLastWakeTime,xDelay250ms);
	}
}

uint16_t  Task1LED = GREEN_LED;
uint16_t  Task2LED = BLUE_LED;
uint16_t  Task3LED = RED_LED;


inline void application6(void)
{
	BaseType_t xReturned1,xReturned2,xReturned3;
	/* Create two instances of the continuous processing task, both at priority	1. */
	xReturned1=xTaskCreate( vContinuousProcessingTask, "Task 1", 128, (void*)&Task1LED, 1, NULL );
	xReturned2=xTaskCreate( vContinuousProcessingTask, "Task 2", 128, (void*)&Task2LED, 1, NULL );
	/* Create one instance of the periodic task at priority 2. */
	xReturned3=xTaskCreate( vPeriodicTask, "Task 3", 128, (void*)&Task3LED, 2, NULL );
	if (( xReturned1 == pdPASS )&&( xReturned2 == pdPASS )&&( xReturned3 == pdPASS ))
				  vTaskStartScheduler();
}
