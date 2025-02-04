#include "application20M.h"
#include "main.h"
#include "cmsis_os.h"
#include "timers.h"
#include "semphr.h"
#include "stm32f4xx_it.h"

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

TickType_t delay1=mainDELAY_LOOP_COUNT, delay2=mainDELAY_LOOP_COUNT/10;

unsigned int rand (void)
{
   static unsigned int z1 = 12345, z2 = 12345, z3 = 12345, z4 = 12345;
   unsigned int b;
   b  = ((z1 << 6) ^ z1) >> 13;
   z1 = ((z1 & 4294967294U) << 18) ^ b;
   b  = ((z2 << 2) ^ z2) >> 27;
   z2 = ((z2 & 4294967288U) << 2) ^ b;
   b  = ((z3 << 13) ^ z3) >> 21;
   z3 = ((z3 & 4294967280U) << 7) ^ b;
   b  = ((z4 << 3) ^ z4) >> 12;
   z4 = ((z4 & 4294967168U) << 13) ^ b;
   return (z1 ^ z2 ^ z3 ^ z4);
}

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
	case standard out. */
	xSemaphoreTake( xMutex, portMAX_DELAY );
	//xSemaphoreTake( xBinarySemaphore, portMAX_DELAY );
	{
		/* The following line will only execute once the semaphore has been
		successfully obtained - so standard out can be accessed freely. */

		for (i=0; i<10 ;i++)
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

}

static void prvBlinkLEDL( void *pvParameters )
{
	uint32_t *delay;
	const TickType_t xSlowDownDelay = pdMS_TO_TICKS( 500 );

	/* Two instances of this task are created.  The string printed by the task
	is passed into the task using the task's parameter.  The parameter is cast
	to the required type. */
	delay = ( uint32_t * ) pvParameters;


	for( ;; )
	{
		/* Print out the string using the newly defined function. */
		prvBlinky( delay );

		/* Wait a pseudo random time.  Note that rand() is not necessarily
		re-entrant, but in this case it does not really matter as the code does
		not care what value is returned.  In a more secure application a version
		of rand() that is known to be re-entrant should be used - or calls to
		rand() should be protected using a critical section. */
		vTaskDelay( rand() % xMaxBlockTimeTicks );

		/* Just to ensure the scrolling is not too fast! */
		vTaskDelay( xSlowDownDelay );
	}
}


static void prvBlinkLEDH( void *pvParameters )
{
	uint32_t *delay;
	const TickType_t xSlowDownDelay = pdMS_TO_TICKS( 50 );

	/* Two instances of this task are created.  The string printed by the task
	is passed into the task using the task's parameter.  The parameter is cast
	to the required type. */
	delay = ( uint32_t * ) pvParameters;
	//postpone the start of the high priority task, so that the low priority task will run first
	vTaskDelay( xSlowDownDelay );
	for( ;; )
	{
		/* Print out the string using the newly defined function. */
		prvBlinky( delay );

		/* Wait a pseudo random time.  Note that rand() is not necessarily
		re-entrant, but in this case it does not really matter as the code does
		not care what value is returned.  In a more secure application a version
		of rand() that is known to be re-entrant should be used - or calls to
		rand() should be protected using a critical section. */
		vTaskDelay( rand() % xMaxBlockTimeTicks );

		/* Just to ensure the scrolling is not too fast! */
		vTaskDelay( xSlowDownDelay );
	}
}


static void prvBlinkLEDM( void *pvParameters )
{
	uint32_t *delay;
	const TickType_t xSlowDownDelay = pdMS_TO_TICKS( 500 );
	volatile uint8_t i;
	volatile uint32_t ul;

	/* Two instances of this task are created.  The string printed by the task
	is passed into the task using the task's parameter.  The parameter is cast
	to the required type. */
	delay = ( uint32_t * ) pvParameters;
	//start the medium priority task after the high priority task
	vTaskDelay( xSlowDownDelay + 10 );
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
		/* Just to ensure the scrolling is not too fast! */
		vTaskDelay( xSlowDownDelay );
	}
}


inline void application20(void)
{
	 /* Before a semaphore is used it must be explicitly created.  In this example
		a mutex type semaphore is created. */
	 xMutex = xSemaphoreCreateMutex();
	// xBinarySemaphore = xSemaphoreCreateBinary();
	// xSemaphoreGive( xBinarySemaphore );
		/* Check the semaphore was created successfully. */
	if( xMutex != NULL )
	//if( xBinarySemaphore != NULL )
	{

		xTaskCreate( prvBlinkLEDL, "BlinkLedLowPriority", 1000, &delay1, 1, NULL );
		xTaskCreate( prvBlinkLEDM, "BlinkGreenLedMedium", 1000, &delay2, 2, NULL );
		xTaskCreate( prvBlinkLEDH, "BlinkLedHighPriority", 1000, &delay2, 3, NULL );
	}


}
