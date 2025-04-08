#include "application20M.h"
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
SemaphoreHandle_t xBinarySemaphore;

/* The tasks block for a pseudo random time between 0 and xMaxBlockTime ticks. */
const TickType_t xMaxBlockTimeTicks = 0x20;

TickType_t delay1=mainDELAY_LOOP_COUNT, delay2=mainDELAY_LOOP_COUNT/5, delay3=mainDELAY_LOOP_COUNT/10;

static void prvBlinkLED( void *pvParameters )
{
	uint32_t *delay;
	const TickType_t xDelay = pdMS_TO_TICKS( 500 );

	/* Two instances of this task are created.  The string printed by the task
	is passed into the task using the task's parameter.  The parameter is cast
	to the required type. */
	delay = ( uint32_t * ) pvParameters;

	uint32_t cnt,ul;
	for( ;; )
	{
		/* Print out the string using the newly defined function. */
		xSemaphoreTake( xMutex, portMAX_DELAY );
			//xSemaphoreTake( xBinarySemaphore, portMAX_DELAY );
			{
				/* The following line will only execute once the semaphore has been
				successfully obtained - so standard out can be accessed freely. */
				for (cnt=0; cnt<10 ;cnt++)
				{
					HAL_GPIO_TogglePin(GPIOD, RED_LED);
					/* Delay for a period. */
					for (ul = 0; ul < (*delay)/2; ul++)
					{
						/* This loop is just a very crude delay implementation. */
					}
				}
			}
			xSemaphoreGive( xMutex );
			//xSemaphoreGive( xBinarySemaphore );

		/* Wait a pseudo random time.  Note that rand() is not necessarily
		re-entrant, but in this case it does not really matter as the code does
		not care what value is returned.  In a more secure application a version
		of rand() that is known to be re-entrant should be used - or calls to
		rand() should be protected using a critical section. */
		vTaskDelay( rand() % xMaxBlockTimeTicks );

		/* Just to ensure the scrolling is not too fast! */
		vTaskDelay( xDelay );
	}
}


static void prvBlinkLEDM( void *pvParameters )
{
	uint32_t *delay;
	const TickType_t xDelay = pdMS_TO_TICKS( 500 );
	volatile uint8_t i;
	volatile uint32_t ul;

	/* Two instances of this task are created.  The string printed by the task
	is passed into the task using the task's parameter.  The parameter is cast
	to the required type. */
	delay = ( uint32_t * ) pvParameters;
	//start the medium priority task after the low priority task
	vTaskDelay( xDelay );
	for( ;; )
	{
		for (i=0; i<20 ;i++)
		{
			HAL_GPIO_TogglePin(GPIOD, GREEN_LED);
			/* Delay for a period. */
			for (ul = 0; ul < (*delay); ul++)
			{
				/* This loop is just a very crude delay implementation. */
			}
		}

		vTaskDelay( xDelay );
	}
}


inline void application20M(void)
{
	BaseType_t xReturned1, xReturned2, xReturned3;
	 /* Before a semaphore is used it must be explicitly created.  In this example
		a mutex type semaphore is created. */
	 xMutex = xSemaphoreCreateMutex();
	// xBinarySemaphore = xSemaphoreCreateBinary();
	// xSemaphoreGive( xBinarySemaphore );
	/* Check the semaphore was created successfully. */
	if( xMutex != NULL )
	//if( xBinarySemaphore != NULL )
	{

		xReturned1=xTaskCreate( prvBlinkLED, "BlinkRedLedLowPriority", 128, &delay1, 1, NULL );
		xReturned2=xTaskCreate( prvBlinkLEDM, "BlinkGreenLedMedium", 128, &delay2, 2, NULL );
		xReturned3=xTaskCreate( prvBlinkLED, "BlinkRedLedHighPriority", 128, &delay3, 3, NULL );
	}
	if((xReturned1==pdPASS)&&(xReturned2==pdPASS)&&(xReturned3==pdPASS))
		vTaskStartScheduler();
}
