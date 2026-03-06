#include "main.h"
#include "cmsis_os.h"
#include "application11.h"
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

typedef enum
{
	eSender1,
	eSender2
} DataSource_t;

/* Define the structure type that will be passed on the queue. */
typedef struct
{
	uint16_t ucValue;
	DataSource_t eDataSource;
} Data_t;

/* Declare two variables of type Data_t that will be passed on the queue. */
static const Data_t xStructsToSend[ 2 ] =
{
	{ GREEN_LED, eSender1 }, /* Used by Sender1. */
	{ BLUE_LED, eSender2 }  /* Used by Sender2. */
};

static void vSenderTask( void *pvParameters )
{

	const TickType_t xTicksToWait = pdMS_TO_TICKS( 100UL );
    /* Two instances are created of this task so the value that is sent to the
	queue is passed in via the task parameter rather than be hard coded.  This way
	each instance can use a different value.  Cast the parameter to the required
	type. */


	/* As per most tasks, this task is implemented within an infinite loop. */
	for( ;; )
	{
		/*The data represents the LED to blink and the sender ID*/
		xQueueSendToBack( xQueue, pvParameters, xTicksToWait );


	}
}
/*-----------------------------------------------------------*/

static void vReceiverTask( void *pvParameters )
{
	/* Declare the variable that will hold the values received from the queue. */
	Data_t xReceivedStructure;
	BaseType_t xStatus;
	const TickType_t xTicksToWait = pdMS_TO_TICKS( 500UL );
	/* This task is also defined within an infinite loop. */
	for( ;; )
	{
		uxQueueMessagesWaiting( xQueue );

		/*The data represents the LED to blink*/
		xStatus = xQueueReceive( xQueue, &xReceivedStructure, 0 );

		if( xStatus == pdPASS )
		{
			/* Data was successfully received from the queue. */
			if( xReceivedStructure.eDataSource == eSender1 )
				HAL_GPIO_TogglePin(GPIOD, xReceivedStructure.ucValue);
			else
				HAL_GPIO_TogglePin(GPIOD, xReceivedStructure.ucValue);

		}
		vTaskDelay(xTicksToWait);

	}
}


inline void application11(void)
{

    /* The queue is created to hold a maximum of 3 structures of type Data_t. */
    xQueue = xQueueCreate( 3, sizeof( Data_t ) );
    BaseType_t xReturned1, xReturned2, xReturned3;
		if( xQueue != NULL )
		{
			/* Create two instances of the task that will write to the queue.  Both tasks are created at
			priority 2. */
			xReturned1 = xTaskCreate( vSenderTask, "Sender1", 128, ( void * ) &( xStructsToSend[ 0 ]), 2, NULL );
			xReturned2 = xTaskCreate( vSenderTask, "Sender2", 128, ( void * ) &( xStructsToSend[ 1 ]), 2, NULL );

			/* Create the task that will read from the queue.  The task is created with
			priority 1, so below the priority of the sender tasks. */
			xReturned3 = xTaskCreate( vReceiverTask, "Receiver", 128, NULL, 1, NULL );
		}
		if((xReturned1==pdPASS)&&(xReturned2==pdPASS)&&(xReturned3==pdPASS))
					vTaskStartScheduler();

}
