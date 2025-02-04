#include "application22.h"
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

/* Definitions for the event bits in the event group. */
#define mainFIRST_TASK_BIT	( 1UL << 0UL ) /* Event bit 0, which is set by a task. */
#define mainSECOND_TASK_BIT	( 1UL << 1UL ) /* Event bit 1, which is set by a task. */
#define mainISR_BIT			( 1UL << 2UL ) /* Event bit 2, which is set by an ISR. */

/* Declare the event group in which bits are set from both a task and an ISR. */
EventGroupHandle_t xEventGroup;


static void vEventBitReadingTask( void *pvParameters )
{
const EventBits_t xBitsToWaitFor = ( mainFIRST_TASK_BIT | mainSECOND_TASK_BIT | mainISR_BIT );
EventBits_t xEventGroupValue;

for( ;; )
{
	/* Block to wait for event bits to become set within the event group. */
	xEventGroupValue = xEventGroupWaitBits( /* The event group to read. */
											xEventGroup,

											/* Bits to test. */
											xBitsToWaitFor,

											/* Clear bits on exit if the
											unblock condition is met. */
											pdTRUE,

											/* Don't wait for all bits. */
											pdFALSE,

											/* Don't time out. */
											portMAX_DELAY );

	/* Print a message for each bit that was set. */
	if( ( xEventGroupValue & mainFIRST_TASK_BIT ) != 0 )
	{
		HAL_GPIO_TogglePin(GPIOD, GREEN_LED);
	}

	if( ( xEventGroupValue & mainSECOND_TASK_BIT ) != 0 )
	{
		HAL_GPIO_TogglePin(GPIOD, GREEN_LED);
	}

	if( ( xEventGroupValue & mainISR_BIT ) != 0 )
	{
		HAL_GPIO_TogglePin(GPIOD, GREEN_LED);
	}


}
}
/*-----------------------------------------------------------*/

void vPrintStringFromDaemonTask( void *pvParameter1, uint32_t ulParameter2 )
{
	HAL_GPIO_TogglePin(GPIOD, ORANGE_LED);
}
/*-----------------------------------------------------------*/

static void vIntegerGenerator( void *pvParameters )
{
TickType_t xLastExecutionTime;
const TickType_t xDelay500ms = pdMS_TO_TICKS( 500UL );

	/* Initialize the variable used by the call to vTaskDelayUntil(). */
	xLastExecutionTime = xTaskGetTickCount();

	for( ;; )
	{
		/* This is a periodic task.  Block until it is time to run again.
		The task will execute every 500ms. */
		vTaskDelayUntil( &xLastExecutionTime, xDelay500ms );

	}
}



void EXTI0_IRQHandler(void)
{
	BaseType_t xHigherPriorityTaskWoken;
	/* The string is not printed within the interrupt service, but is instead
	sent to the RTOS daemon task for printing.  It is therefore declared static to
	ensure the compiler does not allocate the string on the stack of the ISR (as the
	ISR's stack frame will not exist when the string is printed from the daemon
	task. */

	static const char *pcString = "Bit setting ISR -\t about to set bit 2.\r\n";

	/* As always, xHigherPriorityTaskWoken is initialized to pdFALSE. */
	xHigherPriorityTaskWoken = pdFALSE;

	/* Print out a message to say bit 2 is about to be set.  Messages cannot be
	printed from an ISR, so defer the actual output to the RTOS daemon task by
	pending a function call to run in the context of the RTOS daemon task. */
	xTimerPendFunctionCallFromISR( vPrintStringFromDaemonTask, ( void * ) pcString, 0, &xHigherPriorityTaskWoken );

	/* Set bit 2 in the event group. */
	xEventGroupSetBitsFromISR( xEventGroup, mainISR_BIT, &xHigherPriorityTaskWoken );

	/* xEventGroupSetBitsFromISR() writes to the timer command queue.  If
	writing to the timer command queue results in the RTOS daemon task leaving
	the Blocked state, and if the priority of the RTOS daemon task is higher
	than the priority of the currently executing task (the task this interrupt
	interrupted) then xHigherPriorityTaskWoken will have been set to pdTRUE
	inside xEventGroupSetBitsFromISR().*/

  HAL_GPIO_TogglePin(GPIOD, RED_LED);

  /* USER CODE END EXTI0_IRQn 0 */
  HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_0);
  /* USER CODE BEGIN EXTI0_IRQn 1 */

  /* USER CODE END EXTI0_IRQn 1 */
}

static void vEventBitSettingTask( void *pvParameters )
{
const TickType_t xDelay200ms = pdMS_TO_TICKS( 200UL );

	for( ;; )
	{
		/* Delay for a short while before starting the next loop. */
		vTaskDelay( xDelay200ms );

		/*  event bit 0 is about to be set by the
		task, then set event bit 0. */
		HAL_GPIO_TogglePin(GPIOD, BLUE_LED);

		xEventGroupSetBits( xEventGroup, mainFIRST_TASK_BIT );

		/* Delay for a short while before setting the other bit set within this
		task. */
		vTaskDelay( xDelay200ms );

		/*  event bit 1 is about to be set by the
		task, then set event bit 1. */
		HAL_GPIO_TogglePin(GPIOD, BLUE_LED);
		xEventGroupSetBits( xEventGroup, mainSECOND_TASK_BIT );
	}
}
/*-----------------------------------------------------------*/


inline void application22(void)
{
	/* Before an event group can be used it must first be created. */
	xEventGroup = xEventGroupCreate();

	/* Create the task that sets event bits in the event group. */
	xTaskCreate( vEventBitSettingTask, "BitSetter", 1000, NULL, 1, NULL );

	/* Create the task that waits for event bits to get set in the event
	group. */
	xTaskCreate( vEventBitReadingTask, "BitReader", 1000, NULL, 2, NULL );

	/* Create the task that is used to periodically generate a software
	interrupt. */
	xTaskCreate( vIntegerGenerator, "IntGen", 1000, NULL, 3, NULL );

}
