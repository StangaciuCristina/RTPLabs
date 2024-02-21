#include "application23.h"
#include "main.h"
#include "cmsis_os.h"
#include "timers.h"
#include "semphr.h"
#include "time.h"
#include "stm32f4xx_it.h"

/* Used as a loop counter to create a very crude delay. */
#define mainDELAY_LOOP_COUNT		( 0xffffff )
#define GREEN_LED LD4_Pin
#define BLUE_LED LD6_Pin
#define RED_LED LD5_Pin
#define ORANGE_LED LD3_Pin

/* Used as a loop counter to create a very crude delay. */
#define mainDELAY_LOOP_COUNT		( 0xffffff )

/* Definitions for the event bits in the event group. */
#define mainFIRST_TASK_BIT	( 1UL << 0UL ) /* Event bit 0, which is set by the first task. */
#define mainSECOND_TASK_BIT	( 1UL << 1UL ) /* Event bit 1, which is set by the second task. */
#define mainTHIRD_TASK_BIT	( 1UL << 2UL ) /* Event bit 2, which is set by the third task. */


/*-----------------------------------------------------------*/

/* Use by the pseudo random number generator. */
static uint32_t ulNextRand;

/* Declare the event group used to synchronize the three tasks. */
EventGroupHandle_t xEventGroup;

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


static uint32_t prvRand( void )
{
const uint32_t ulMultiplier = 0x015a4e35UL, ulIncrement = 1UL;
uint32_t ulReturn;

	/* Utility function to generate a pseudo random number as the MSVC rand()
	function has unexpected consequences. */
	taskENTER_CRITICAL();
		ulNextRand = ( ulMultiplier * ulNextRand ) + ulIncrement;
		ulReturn = ( ulNextRand >> 16UL ) & 0x7fffUL;
	taskEXIT_CRITICAL();
	return ulReturn;
}
/*-----------------------------------------------------------*/

static void prvSRand( uint32_t ulSeed )
{
	/* Utility function to seed the pseudo random number generator. */
	ulNextRand = ulSeed;
}
/*-----------------------------------------------------------*/


static void vSyncingTask1( void *pvParameters )
{
const EventBits_t uxAllSyncBits = ( mainFIRST_TASK_BIT | mainSECOND_TASK_BIT | mainTHIRD_TASK_BIT );
const TickType_t xMaxDelay = pdMS_TO_TICKS( 4000UL );
const TickType_t xMinDelay = pdMS_TO_TICKS( 200UL );
TickType_t xDelayTime;
EventBits_t uxThisTasksSyncBit;

	/* Three instances of this task are created - each task uses a different
	event bit in the synchronization.  The event bit to use by this task
	instance is passed into the task using the task's parameter.  Store it in
	the uxThisTasksSyncBit variable. */
	uxThisTasksSyncBit = ( EventBits_t ) pvParameters;

	for( ;; )
	{
		/* Simulate this task taking some time to perform an action by delaying
		for a pseudo random time.  This prevents all three instances of this
		task from reaching the synchronization point at the same time, and
		allows the example's behavior to be observed more easily. */
		xDelayTime = ( prvRand() % xMaxDelay ) + xMinDelay;
		vTaskDelay( xDelayTime );

		/* Print out a message to show this task has reached its synchronization
		point.  pcTaskGetTaskName() is an API function that returns the name
		assigned to the task when the task was created. */
		//vPrintTwoStrings( pcTaskGetTaskName( NULL ), "reached sync point" );
		HAL_GPIO_TogglePin(GPIOD, RED_LED);

		/* Wait for all the tasks to have reached their respective
		synchronization points. */
		xEventGroupSync( /* The event group used to synchronize. */
						 xEventGroup,

						 /* The bit set by this task to indicate it has reached
						 the synchronization point. */
						 uxThisTasksSyncBit,

						 /* The bits to wait for, one bit for each task taking
						 part in the synchronization. */
						 uxAllSyncBits,

						 /* Wait indefinitely for all three tasks to reach the
						 synchronization point. */
						 portMAX_DELAY );

		/* Print out a message to show this task has passed its synchronization
		point.  As an indefinite delay was used the following line will only be
		reached after all the tasks reached their respective synchronization
		points. */
		//vPrintTwoStrings( pcTaskGetTaskName( NULL ), "exited sync point" );
		HAL_GPIO_TogglePin(GPIOD, RED_LED);
	}
}
/*-----------------------------------------------------------*/

static void vSyncingTask2( void *pvParameters )
{
const EventBits_t uxAllSyncBits = ( mainFIRST_TASK_BIT | mainSECOND_TASK_BIT | mainTHIRD_TASK_BIT );
const TickType_t xMaxDelay = pdMS_TO_TICKS( 4000UL );
const TickType_t xMinDelay = pdMS_TO_TICKS( 200UL );
TickType_t xDelayTime;
EventBits_t uxThisTasksSyncBit;

	/* Three instances of this task are created - each task uses a different
	event bit in the synchronization.  The event bit to use by this task
	instance is passed into the task using the task's parameter.  Store it in
	the uxThisTasksSyncBit variable. */
	uxThisTasksSyncBit = ( EventBits_t ) pvParameters;

	for( ;; )
	{
		/* Simulate this task taking some time to perform an action by delaying
		for a pseudo random time.  This prevents all three instances of this
		task from reaching the synchronization point at the same time, and
		allows the example's behavior to be observed more easily. */
		xDelayTime = ( prvRand() % xMaxDelay ) + xMinDelay;
		vTaskDelay( xDelayTime );

		/* Print out a message to show this task has reached its synchronization
		point.  pcTaskGetTaskName() is an API function that returns the name
		assigned to the task when the task was created. */
		//vPrintTwoStrings( pcTaskGetTaskName( NULL ), "reached sync point" );
		HAL_GPIO_TogglePin(GPIOD, BLUE_LED);

		/* Wait for all the tasks to have reached their respective
		synchronization points. */
		xEventGroupSync( /* The event group used to synchronize. */
						 xEventGroup,

						 /* The bit set by this task to indicate it has reached
						 the synchronization point. */
						 uxThisTasksSyncBit,

						 /* The bits to wait for, one bit for each task taking
						 part in the synchronization. */
						 uxAllSyncBits,

						 /* Wait indefinitely for all three tasks to reach the
						 synchronization point. */
						 portMAX_DELAY );

		/* Print out a message to show this task has passed its synchronization
		point.  As an indefinite delay was used the following line will only be
		reached after all the tasks reached their respective synchronization
		points. */
		//vPrintTwoStrings( pcTaskGetTaskName( NULL ), "exited sync point" );
		HAL_GPIO_TogglePin(GPIOD, BLUE_LED);
	}
}
/*-----------------------------------------------------------*/

static void vSyncingTask3( void *pvParameters )
{
const EventBits_t uxAllSyncBits = ( mainFIRST_TASK_BIT | mainSECOND_TASK_BIT | mainTHIRD_TASK_BIT );
const TickType_t xMaxDelay = pdMS_TO_TICKS( 4000UL );
const TickType_t xMinDelay = pdMS_TO_TICKS( 200UL );
TickType_t xDelayTime;
EventBits_t uxThisTasksSyncBit;

	/* Three instances of this task are created - each task uses a different
	event bit in the synchronization.  The event bit to use by this task
	instance is passed into the task using the task's parameter.  Store it in
	the uxThisTasksSyncBit variable. */
	uxThisTasksSyncBit = ( EventBits_t ) pvParameters;

	for( ;; )
	{
		/* Simulate this task taking some time to perform an action by delaying
		for a pseudo random time.  This prevents all three instances of this
		task from reaching the synchronization point at the same time, and
		allows the example's behavior to be observed more easily. */
		xDelayTime = ( prvRand() % xMaxDelay ) + xMinDelay;
		vTaskDelay( xDelayTime );

		/* Print out a message to show this task has reached its synchronization
		point.  pcTaskGetTaskName() is an API function that returns the name
		assigned to the task when the task was created. */
		//vPrintTwoStrings( pcTaskGetTaskName( NULL ), "reached sync point" );
		HAL_GPIO_TogglePin(GPIOD, ORANGE_LED);

		/* Wait for all the tasks to have reached their respective
		synchronization points. */
		xEventGroupSync( /* The event group used to synchronize. */
						 xEventGroup,

						 /* The bit set by this task to indicate it has reached
						 the synchronization point. */
						 uxThisTasksSyncBit,

						 /* The bits to wait for, one bit for each task taking
						 part in the synchronization. */
						 uxAllSyncBits,

						 /* Wait indefinitely for all three tasks to reach the
						 synchronization point. */
						 portMAX_DELAY );

		/* Print out a message to show this task has passed its synchronization
		point.  As an indefinite delay was used the following line will only be
		reached after all the tasks reached their respective synchronization
		points. */
		//vPrintTwoStrings( pcTaskGetTaskName( NULL ), "exited sync point" );
		HAL_GPIO_TogglePin(GPIOD, ORANGE_LED);
	}
}
/*-----------------------------------------------------------*/

void application23(void)
{
	/* The tasks created in this example block for a random time.  The block
	time is generated using rand() - seed the random number generator. */
	prvSRand( (uint32_t)rand );

	/* Before an event group can be used it must first be created. */
	xEventGroup = xEventGroupCreate();

	/* Create three instances of the task.  Each task is given a different name,
	which is later printed out to give a visual indication of which task is
	executing.  The event bit to use when the task reaches its synchronization
	point is passed into the task using the task parameter. */
	xTaskCreate( vSyncingTask1, "Task 1", 1000, ( void * ) mainFIRST_TASK_BIT, 1, NULL );
	xTaskCreate( vSyncingTask2, "Task 2", 1000, ( void * ) mainSECOND_TASK_BIT, 1, NULL );
	xTaskCreate( vSyncingTask3, "Task 3", 1000, ( void * ) mainTHIRD_TASK_BIT, 1, NULL );

}
