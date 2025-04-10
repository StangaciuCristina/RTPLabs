#include "main.h"
#include "cmsis_os.h"
#include "application7.h"

/* Used as a loop counter to create a very crude delay. */
#define mainDELAY_LOOP_COUNT		( 0xffffff )
#define GREEN_LED LD4_Pin
#define BLUE_LED LD6_Pin
#define RED_LED LD5_Pin
#define ORANGE_LED LD3_Pin

void vTaskFunction( void *pvParameters )
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

		/* We want this task to execute exactly every 250 milliseconds. */
		vTaskDelayUntil(&xLastWakeTime,xDelay250ms );
		//osDelayUntil(&xLastWakeTime, 250UL);
	}
}

/* A variable that is incremented by the idle task hook function. */
static uint32_t ulIdleCycleCount = 0UL;
/* Idle hook functions MUST be called vApplicationIdleHook(), take no parameters,
and return void. */
void vApplicationIdleHook( void )
{
	/* Increment a counter. */
	ulIdleCycleCount++;
	/* toggle the a LED. */
	HAL_GPIO_TogglePin(GPIOD, ORANGE_LED);
}

uint16_t  Task1LED = GREEN_LED;
uint16_t  Task2LED = BLUE_LED;
uint16_t  Task3LED = RED_LED;


inline void application7(void)
{

	BaseType_t xReturned1, xReturned2;
	/* Create the first task at priority 1... */
	xReturned1 = xTaskCreate( vTaskFunction, "Task 1", 128, (void*)&Task1LED, 1, NULL );
	/* ... and the second task at priority 2.  The priority is the second to
			last parameter. */
	xReturned2 = xTaskCreate( vTaskFunction, "Task 2", 128, (void*)&Task2LED, 2, NULL );
	if ((xReturned1==pdPASS)&&(xReturned2==pdPASS))
		vTaskStartScheduler();


}
