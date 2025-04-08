#include "application20Simple.h"
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
mutex type semaphore that is used to ensure mutual exclusive access to stdout. */
SemaphoreHandle_t xMutex;

/* The tasks block for a pseudo random time between 0 and xMaxBlockTime ticks. */
const TickType_t xMaxBlockTimeTicks = 0x20;

TickType_t delay1=mainDELAY_LOOP_COUNT, delay2=mainDELAY_LOOP_COUNT/10;

static void prvBlinkLED( void *pvParameters )
{
	uint32_t *delay;
    /*The delay values ensure that the low priority task starts first*/
	delay = ( uint32_t * ) pvParameters;
	uint32_t cnt,ul;

	for( ;; )
	{
		xSemaphoreTake( xMutex, portMAX_DELAY );
		{
				/* The following line will only execute once the semaphore has been
				successfully obtained - so standard out can be accessed freely. */
				for (cnt=0; cnt<10 ;cnt++)
				{
					HAL_GPIO_TogglePin(GPIOD, RED_LED);
					/* Delay for a period. */
					for (ul = 0; ul < *delay; ul++)
					{
						;/* This loop is just a very crude blocking delay implementation. */
					}
				}
		}
		xSemaphoreGive( xMutex );
		/* Wait a pseudo random time. */
		vTaskDelay( rand() % xMaxBlockTimeTicks );
	}
}

inline void application20Simple(void)
{
	BaseType_t xReturned1, xReturned2;
	/* Before a semaphore is used it must be explicitly created.  In this example
		a mutex type semaphore is created. */
	xMutex = xSemaphoreCreateMutex();

	/* Check the semaphore was created successfully. */
	if( xMutex != NULL )
	{
		/* Create two instances of the tasks that attempt to use the same resource (the RED LED in our case).
		 * The tasks are created at different priorities so some pre-emption will occur. */
	    /*The delay values ensure that the low priority task starts first*/
		xReturned1=xTaskCreate( prvBlinkLED, "Task1", 128, &delay1, 1, NULL );
		xReturned2=xTaskCreate( prvBlinkLED, "Task2", 128, &delay2, 2, NULL );
	}
	if ((xReturned1==pdPASS)&&(xReturned2==pdPASS))
				vTaskStartScheduler();


}
