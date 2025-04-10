#include "main.h"
#include "cmsis_os.h"
#include "application12.h"
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
/* The three sender task. */
static void vSenderTask1( void *pvParameters );
static void vSenderTask2( void *pvParameters );
static void vReceiverTask( void *pvParameters );

/*-----------------------------------------------------------*/

/* Declare two variables of type QueueHandle_t.  Both queues are added to the
same queue set. */
static QueueHandle_t xQueue1 = NULL, xQueue2 = NULL;

/* Declare a variable of type QueueSetHandle_t.  This is the queue set to which
the two queues are added. */
static QueueSetHandle_t xQueueSet = NULL;

static void vSenderTask1( void *pvParameters )
{
const TickType_t xBlockTime = pdMS_TO_TICKS( 100 );
uint16_t led1= GREEN_LED;

	/* As per most tasks, this task is implemented within an infinite loop. */
	for( ;; )
	{
		/* Block for 100ms. */
		vTaskDelay( xBlockTime );

		/* Send this task's string to xQueue1. It is not necessary to use a
		block time, even though the queue can only hold one item.  This is
		because the priority of the task that reads from the queue is higher
		than the priority of this task; as soon as this task writes to the queue
		it will be pre-empted by the task that reads from the queue, so the
		queue will already be empty again by the time the call to xQueueSend()
		returns.  The block time is set to 0. */
		xQueueSend( xQueue1, &led1, 0 );
	}
}
/*-----------------------------------------------------------*/

static void vSenderTask2( void *pvParameters )
{
const TickType_t xBlockTime = pdMS_TO_TICKS( 200 );
uint16_t led2= BLUE_LED;

	/* As per most tasks, this task is implemented within an infinite loop. */
	for( ;; )
	{
		/* Block for 200ms. */
		vTaskDelay( xBlockTime );

		/* Send this task's string to xQueue1. It is not necessary to use a
		block time, even though the queue can only hold one item.  This is
		because the priority of the task that reads from the queue is higher
		than the priority of this task; as soon as this task writes to the queue
		it will be pre-empted by the task that reads from the queue, so the
		queue will already be empty again by the time the call to xQueueSend()
		returns.  The block time is set to 0. */
		xQueueSend( xQueue2, &led2, 0 );
	}
}

static void vReceiverTask( void *pvParameters )
{
	QueueHandle_t xQueueThatContainsData;
	uint16_t ReceivedLedNumber;

		/* As per most tasks, this task is implemented within an infinite loop. */
		for( ;; )
		{
			/* Block on the queue set to wait for one of the queues in the set to
			contain data.  Cast the QueueSetMemberHandle_t values returned from
			xQueueSelectFromSet() to a QueueHandle_t as it is known that all the
			items in the set are queues (as opposed to semaphores, which can also be
			members of a queue set). */
			xQueueThatContainsData = ( QueueHandle_t ) xQueueSelectFromSet( xQueueSet, portMAX_DELAY );

			/* An indefinite block time was used when reading from the set so
			xQueueSelectFromSet() will not have returned unless one of the queues in
			the set contained data, and xQueueThatContansData must be valid.  Read
			from the queue.  It is not necessary to specify a block time because it
			is known that the queue contains data.  The block time is set to 0. */
			xQueueReceive( xQueueThatContainsData, &ReceivedLedNumber, 0 );

			/* Print the string received from the queue. */
			HAL_GPIO_TogglePin(GPIOD, ReceivedLedNumber);
		}
}


uint16_t led1=GREEN_LED;
uint16_t led2=BLUE_LED;

inline void application12(void)
{

	BaseType_t xReturned1, xReturned2, xReturned3;
	/* Create the two queues. The
	priority of the receiving task is above the priority of the sending tasks so
	the queues will never have more than one item in them at any one time. */
    xQueue1 = xQueueCreate( 1, sizeof( uint16_t * ) );
	xQueue2 = xQueueCreate( 1, sizeof( uint16_t * ) );

	/* Create the queue set.  There are two queues both of which can contain
	1 item, so the maximum number of queue handle the queue set will ever have
	to hold is 2 (1 item multiplied by 2 sets). */
	xQueueSet = xQueueCreateSet( 1 * 2 );

	/* Add the two queues to the set. */
	xQueueAddToSet( xQueue1, xQueueSet );
	xQueueAddToSet( xQueue2, xQueueSet );

	/* Create the tasks that send to the queues. */
	xReturned1 = xTaskCreate( vSenderTask1, "Sender1", 128, NULL, 1, NULL );
	xReturned2 = xTaskCreate( vSenderTask2, "Sender2", 128, NULL, 1, NULL );

	/* Create the receiver task. */
	xReturned3 = xTaskCreate( vReceiverTask, "Receiver", 128, NULL, 2, NULL );

	if((xReturned1==pdPASS)&&(xReturned2==pdPASS)&&(xReturned3==pdPASS))
						vTaskStartScheduler();


}
