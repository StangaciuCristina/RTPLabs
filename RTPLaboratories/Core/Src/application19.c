#include "main.h"
#include "cmsis_os.h"
#include "timers.h"
#include "semphr.h"
#include "application19.h"
#include "stm32f4xx_it.h"

/* Used as a loop counter to create a very crude delay. */
#define mainDELAY_LOOP_COUNT		( 0xffffff )
#define GREEN_LED LD4_Pin
#define BLUE_LED LD6_Pin
#define RED_LED LD5_Pin
#define ORANGE_LED LD3_Pin

/* Declare two variables of type QueueHandle_t.  One queue will be read from
within an ISR, the other will be written to from within an ISR. */
QueueHandle_t xIntegerQueue, xStringQueue;

/**
 * @brief This is to provide the memory that is used by the RTOS daemon/time task.
 *
 * If configUSE_STATIC_ALLOCATION is set to 1, so the application must provide an
 * implementation of vApplicationGetTimerTaskMemory() to provide the memory that is
 * used by the RTOS daemon/time task.
 */
void vApplicationGetTimerTaskMemory( StaticTask_t ** ppxTimerTaskTCBBuffer,
                                     StackType_t ** ppxTimerTaskStackBuffer,
                                     uint32_t * pulTimerTaskStackSize )
{
    /* If the buffers to be provided to the Timer task are declared inside this
     * function then they must be declared static - otherwise they will be allocated on
     * the stack and so not exists after this function exits. */
    static StaticTask_t xTimerTaskTCB;
    static StackType_t uxTimerTaskStack[ configTIMER_TASK_STACK_DEPTH ];

    /* Pass out a pointer to the StaticTask_t structure in which the Idle
     * task's state will be stored. */
    *ppxTimerTaskTCBBuffer = &xTimerTaskTCB;

    /* Pass out the array that will be used as the Timer task's stack. */
    *ppxTimerTaskStackBuffer = uxTimerTaskStack;

    /* Pass out the size of the array pointed to by *ppxTimerTaskStackBuffer.
     * Note that, as the array is necessarily of type StackType_t,
     * configMINIMAL_STACK_SIZE is specified in words, not bytes. */
    *pulTimerTaskStackSize = configTIMER_TASK_STACK_DEPTH;
}
/*-----------------------------------------------------------*/

static void vIntegerGenerator( void *pvParameters )
{
TickType_t xLastExecutionTime;
const TickType_t xDelay = pdMS_TO_TICKS( 20UL ), xDontBlock = 0;
uint32_t ulValueToSend = 0;
BaseType_t i;

	/* Initialize the variable used by the call to vTaskDelayUntil(). */
	xLastExecutionTime = xTaskGetTickCount();

	for( ;; )
	{
		/* This is a periodic task.  Block until it is time to run again.
		The task will execute every 200ms. */
		vTaskDelayUntil( &xLastExecutionTime, xDelay );

		/* Send five numbers to the queue, each value one higher than the
		previous value.  The numbers are read from the queue by the interrupt
		service routine.  The interrupt	service routine always empties the
		queue, so this task is guaranteed to be able to write all five values
		without needing to specify a block time. */
		for( i = 0; i < 5; i++ )
		{
			xQueueSendToBack( xIntegerQueue, &ulValueToSend, xDontBlock );
			ulValueToSend++;
		}

		HAL_GPIO_TogglePin(GPIOD, GREEN_LED);
	}
}
/*-----------------------------------------------------------*/

static void Blinky( void *pvParameters )
{
int *delay;
TickType_t xLastExecutionTime;
xLastExecutionTime = xTaskGetTickCount();
	for( ;; )
	{
		/* Block on the queue to wait for data to arrive. */
		xQueueReceive( xStringQueue, &delay, portMAX_DELAY );

		HAL_GPIO_TogglePin(GPIOD, BLUE_LED);
		vTaskDelayUntil( &xLastExecutionTime,pdMS_TO_TICKS(delay));
	}
}

void EXTI0_IRQHandler(void)
{
	BaseType_t xHigherPriorityTaskWoken;
	uint32_t ulReceivedNumber;
  /* USER CODE BEGIN EXTI0_IRQn 0 */
  xHigherPriorityTaskWoken = pdFALSE;
  uint32_t tab[]={10,100,1000,1500};

  /* Read from the queue until the queue is empty. */
  while( xQueueReceiveFromISR( xIntegerQueue, &ulReceivedNumber, &xHigherPriorityTaskWoken ) != errQUEUE_EMPTY )
  {
  	/* Truncate the received value to the last two bits (values 0 to 3
  	inc.), then use the truncated value as an index into the pcStrings[]
  	array to select a string (char *) to send on the other queue. */
  	ulReceivedNumber &= 0x03;
  	xQueueSendToBackFromISR( xStringQueue, &tab[ ulReceivedNumber ], &xHigherPriorityTaskWoken );
  }


  HAL_GPIO_TogglePin(GPIOD, RED_LED);

  /* USER CODE END EXTI0_IRQn 0 */
  HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_0);
  /* USER CODE BEGIN EXTI0_IRQn 1 */

  /* USER CODE END EXTI0_IRQn 1 */
}




inline void application19(void)
{
	BaseType_t xReturned1, xReturned2;
	/* Before a queue can be used it must first be created.  Create both queues
	used by this example.  One queue can hold variables of type uint32_t,
	the other queue can hold variables of type char*.  Both queues can hold a
	maximum of 10 items.  A real application should check the return values to
	ensure the queues have been successfully created. */
	xIntegerQueue = xQueueCreate( 10, sizeof( uint32_t ) );
	xStringQueue = xQueueCreate( 10, sizeof( uint32_t ) );

	/* Create the task that uses a queue to pass integers to the interrupt
	service	routine.  The task is created at priority 1. */
	xReturned1=xTaskCreate( vIntegerGenerator, "IntGen", 128, NULL, 1, NULL );

	/* Create the task that prints out the strings sent to it from the interrupt
	service routine.  The task is created at the higher priority of 2. */
	xReturned2=xTaskCreate( Blinky, "String", 128, NULL, 2, NULL );
	if ((xReturned1==pdPASS)&&(xReturned2==pdPASS))
			vTaskStartScheduler();
}
