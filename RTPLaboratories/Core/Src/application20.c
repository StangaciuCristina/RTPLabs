#include "application20.h"
#include "main.h"
#include "cmsis_os.h"
#include "timers.h"
#include "semphr.h"
#include "stm32f4xx_it.h"
#include <stdlib.h>

/* Used as a loop counter to create a very crude delay. */
#define mainDELAY_LOOP_COUNT		( 0xffffff )
#define GREEN_LED LD4_Pin
#define BLUE_LED LD6_Pin
#define RED_LED LD5_Pin
#define ORANGE_LED LD3_Pin

/* Used as a loop counter to create a very crude delay. */
#define mainDELAY_LOOP_COUNT		( 0xffffff )

/* Declare a variable of type SemaphoreHandle_t.  This is used to reference the
mutex type semaphore that is used to ensure mutual exclusive access to the RED LED. */
SemaphoreHandle_t xMutex;


const TickType_t xMaxBlockTimeTicks = 0x20;

TickType_t delay1=mainDELAY_LOOP_COUNT, delay2=mainDELAY_LOOP_COUNT/10;


static void prvBlinky( uint32_t *delay )
{
	volatile uint8_t i;
	volatile uint32_t ul;
	/* The semaphore is created before the scheduler is started so already
	exists by the time this task executes.

	Attempt to take the semaphore, blocking indefinitely if the mutex is not
	available immediately.  The call to xSemaphoreTake() will only return when
	the semaphore has been successfully obtained so there is no need to check the
	return value.  If any other delay period was used then the code must check
	that xSemaphoreTake() returns pdTRUE before accessing the resource (in this
	case the RED LED. */
	xSemaphoreTake( xMutex, portMAX_DELAY );
	{
		/* The following line will only execute once the semaphore has been
		successfully obtained. */

		for (i=0; i<10 ;i++)
		{
			HAL_GPIO_TogglePin(GPIOD, RED_LED);
			/* Delay for a period. */
			for (ul = 0; ul < *delay; ul++)
			{
				/* This loop is just a very crude delay implementation. */
			}
		}
	}
	xSemaphoreGive( xMutex );

}

static void prvBlinkLED( void *pvParameters )
{
	uint32_t *delay;
	const TickType_t xSlowDownDelay = pdMS_TO_TICKS( 5UL );

	/* Two instances of this task are created. */
	delay = ( uint32_t * ) pvParameters;

	for( ;; )
	{
		/* Print out the string using the newly defined function. */
		prvBlinky( delay );

		/* Wait a pseudo random time.*/
		vTaskDelay( rand() % xMaxBlockTimeTicks );

		/* Just to ensure the scrolling is not too fast! */
		vTaskDelay( xSlowDownDelay );
	}
}

inline void application20(void)
{
	BaseType_t xReturned1, xReturned2;
	/* Before a semaphore is used it must be explicitly created.  In this example
		a mutex type semaphore is created. */
	xMutex = xSemaphoreCreateMutex();

	/* Check the semaphore was created successfully. */
	if( xMutex != NULL )
	{
		/* Create two instances of the tasks that attempt to blink the LED with different frequencies  The
		string they attempt to write is passed into the task as the task's
		parameter.  The tasks are created at different priorities so some
		pre-emption will occur. */
		xReturned1=xTaskCreate( prvBlinkLED, "Print1", 128, &delay1, 1, NULL );
		xReturned2=xTaskCreate( prvBlinkLED, "Print2", 128, &delay2, 2, NULL );
	}
	if ((xReturned1==pdPASS)&&(xReturned2==pdPASS))
				vTaskStartScheduler();


}
