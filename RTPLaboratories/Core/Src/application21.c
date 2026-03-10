#include "application21.h"
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

/* Define the delays for blinking the LED
gatekeeper. */
static const int Delays[] =
{
	100,50,10
};

/* Declare a variable of type QueueHandle_t.  This is used to send messages from
the print tasks to the gatekeeper task. */
static QueueHandle_t xPrintQueue;

const TickType_t xMaxBlockTimeTicks = 0x20;

static void prvStdioGatekeeperTask( void *pvParameters )
{
int *delay;

	/* T As only one task
	blinks the LED there are no mutual exclusion or serialization issues
	to consider within this task itself. */
	for( ;; )
	{
		/* Wait for a message to arrive. */
		xQueueReceive( xPrintQueue, &delay, portMAX_DELAY );

		HAL_GPIO_TogglePin(GPIOD, BLUE_LED);
		vTaskDelay(pdMS_TO_TICKS(delay));

		/* Now simply go back to wait for the next message. */
	}
}
/*-----------------------------------------------------------*/

void vApplicationTickHook( void )
{
static int iCount = 0;

	/* Blink the LED 200 ticks.  The LED is not blinked directly, but sent to the gatekeeper task. */
	iCount++;
	if( iCount >= 200 )
	{
		/* In this case the last parameter (xHigherPriorityTaskWoken) is not
		actually used and is set to NULL. */
		xQueueSendToFrontFromISR( xPrintQueue, &( Delays[ 2 ] ), NULL );
		HAL_GPIO_TogglePin(GPIOD, RED_LED);
		/* Reset the count ready to print out the string again in 200 ticks
		time. */
		iCount = 0;
	}
}
/*-----------------------------------------------------------*/

static void prvPrintTask( void *pvParameters )
{
int iIndexToString;

	/* Two instances of this task are created so the index to the string the task
	will send to the gatekeeper task is passed in the task parameter.  Cast this
	to the required type. */
	iIndexToString = ( int ) pvParameters;

	for( ;; )
	{
		/* Pass the parameter to the
		gatekeeper task on the queue.  The queue is created before the scheduler is
		started so will already exist by the time this task executes.  A block time
		is not specified as there should always be space in the queue. */
		xQueueSendToBack( xPrintQueue, &( Delays[ iIndexToString ] ), 0 );
		HAL_GPIO_TogglePin(GPIOD, GREEN_LED);
		/* Wait a pseudo random time. */
		vTaskDelay( ( rand() % xMaxBlockTimeTicks ) );
	}
}
/*-----------------------------------------------------------*/



void vAssertCalled( uint32_t ulLine, const char * const pcFile )
{
/* The following two variables are just to ensure the parameters are not
optimised away and therefore unavailable when viewed in the debugger. */
volatile uint32_t ulLineNumber = ulLine, ulSetNonZeroInDebuggerToReturn = 0;
volatile const char * const pcFileName = pcFile;

	taskENTER_CRITICAL();
	{
		while( ulSetNonZeroInDebuggerToReturn == 0 )
		{
			/* If you want to set out of this function in the debugger to see
			the	assert() location then set ulSetNonZeroInDebuggerToReturn to a
			non-zero value. */
		}
	}
	taskEXIT_CRITICAL();

	/* Remove the potential for compiler warnings issued because the variables
	are set but not subsequently referenced. */
	( void ) pcFileName;
	( void ) ulLineNumber;
}
/*-----------------------------------------------------------*/

/* In other examples this function is implemented within the
supporting_functions.c source file - but that source file is not included in
this example as to include it would result in multiple definitions of the tick
hook function. */
void vApplicationMallocFailedHook( void )
{
	/* vApplicationMallocFailedHook() will only be called if
	configUSE_MALLOC_FAILED_HOOK is set to 1 in FreeRTOSConfig.h.  It is a hook
	function that will get called if a call to pvPortMalloc() fails.
	pvPortMalloc() is called internally by the kernel whenever a task, queue,
	timer, event group, or semaphore is created.  It is also called by various
	parts of the demo application.  If heap_1.c, heap_2.c or heap_4.c are used,
	then the size of the heap available to pvPortMalloc() is defined by
	configTOTAL_HEAP_SIZE in FreeRTOSConfig.h, and the xPortGetFreeHeapSize()
	API function can be used to query the size of free heap space that remains.
	More information is provided in the book text. */
	vAssertCalled( __LINE__, __FILE__ );
}
/*-----------------------------------------------------------*/

inline void application21(void)
{
	BaseType_t xReturned1, xReturned2, xReturned3;
	/* Before a queue is used it must be explicitly created.  The queue is created
	to hold a maximum of 5 character pointers. */
	xPrintQueue = xQueueCreate( 5, sizeof( char * ) );

	/* Check the queue was created successfully. */
	if( xPrintQueue != NULL )
	{
		/* Create two instances of the tasks that send messages to the gatekeeper.
		The tasks are created at different priorities so some pre-emption will occur. */
		xReturned1=xTaskCreate( prvPrintTask, "Task1", 128, ( void * ) 0, 1, NULL );
		xReturned2=xTaskCreate( prvPrintTask, "Task2", 128, ( void * ) 1, 2, NULL );

		/* Create the gatekeeper task.  This is the only task that is permitted
		to access standard out. */
		xReturned3=xTaskCreate( prvStdioGatekeeperTask, "Gatekeeper", 1000, NULL, 0, NULL );
	}
	if((xReturned1==pdPASS)&&(xReturned2==pdPASS)&&(xReturned3==pdPASS))
			vTaskStartScheduler();
}
