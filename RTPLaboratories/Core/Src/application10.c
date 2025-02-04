#include "main.h"
#include "cmsis_os.h"
#include "application10.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

/* Used as a loop counter to create a very crude delay. */
#define mainDELAY_LOOP_COUNT		( 0xffffff )
#define GREEN_LED LD4_Pin
#define BLUE_LED LD6_Pin
#define RED_LED LD5_Pin
#define ORANGE_LED LD3_Pin

/* The tasks to be created.  Two instances are created of the sender task while
only a single instance is created of the receiver task. */
static void vSenderTask( void *pvParameters );
static void vReceiverTask( void *pvParameters );

/*-----------------------------------------------------------*/

/* Declare a variable of type QueueHandle_t.  This is used to store the queue
that is accessed by all three tasks. */
QueueHandle_t xQueue;

static void vSenderTask( void *pvParameters )
{
    uint16_t *lValueToSend;

    /* Two instances are created of this task so the value that is sent to the
	queue is passed in via the task parameter rather than be hard coded.  This way
	each instance can use a different value.  Cast the parameter to the required
	type. */
	lValueToSend = ( uint16_t * )pvParameters;
	const TickType_t xTicksToWait = pdMS_TO_TICKS( 100UL );
	/* As per most tasks, this task is implemented within an infinite loop. */
	for( ;; )
	{
		/* The first parameter is the queue to which data is being sent.  The
		queue was created before the scheduler was started, so before this task
		started to execute.

		The second parameter is the address of the data to be sent.

		The third parameter is the Block time – the time the task should be kept
		in the Blocked state to wait for space to become available on the queue
		should the queue already be full.  In this case we don’t specify a block
		time because there should always be space in the queue. */
		xQueueSendToBack( xQueue, lValueToSend, 0 );
		vTaskDelay(xTicksToWait);


	}
}
/*-----------------------------------------------------------*/

static void vReceiverTask( void *pvParameters )
{
	/* Declare the variable that will hold the values received from the queue. */
	uint16_t lReceivedValue;
	const TickType_t xTicksToWait = pdMS_TO_TICKS( 500UL );
	BaseType_t xStatus;
	/* This task is also defined within an infinite loop. */
	for( ;; )
	{
		//uxQueueMessagesWaiting( xQueue );

		/* The first parameter is the queue from which data is to be received.  The
		queue is created before the scheduler is started, and therefore before this
		task runs for the first time.

		The second parameter is the buffer into which the received data will be
		placed.  In this case the buffer is simply the address of a variable that
		has the required size to hold the received data.

		the last parameter is the block time – the maximum amount of time that the
		task should remain in the Blocked state to wait for data to be available should
		the queue already be empty. */
		xStatus = xQueueReceive( xQueue, &lReceivedValue, xTicksToWait );

		if( xStatus == pdPASS )
		{
			/* Data was successfully received from the queue. */
			HAL_GPIO_TogglePin(GPIOD, lReceivedValue);

		}

	}
}

uint16_t led1=GREEN_LED;
uint16_t led2=BLUE_LED;

inline void application10(void)
{

	/* The queue is created to hold a maximum of 5 short values. */
	    xQueue = xQueueCreate( 5, sizeof( int16_t ) );

		if( xQueue != NULL )
		{
			/* Create two instances of the task that will write to the queue.  The
			parameter is used to pass the value that the task should write to the queue,
			so one task will continuously write 100 to the queue while the other task
			will continuously write 200 to the queue.  Both tasks are created at
			priority 1. */
			xTaskCreate( vSenderTask, "Sender1", 1000, ( void * ) &led1, 1, NULL );
			xTaskCreate( vSenderTask, "Sender2", 1000, ( void * ) &led2, 1, NULL );

			/* Create the task that will read from the queue.  The task is created with
			priority 2, so above the priority of the sender tasks. */
			xTaskCreate( vReceiverTask, "Receiver", 1000, NULL, 2, NULL );
		}


}
