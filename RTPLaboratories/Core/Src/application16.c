#include "main.h"
#include "cmsis_os.h"
#include "semphr.h"
#include "application16.h"
#include "stm32f4xx_it.h"

/* Used as a loop counter to create a very crude delay. */
#define mainDELAY_LOOP_COUNT		( 0xffffff )
#define GREEN_LED LD4_Pin
#define BLUE_LED LD6_Pin
#define RED_LED LD5_Pin
#define ORANGE_LED LD3_Pin

/******************************************************************************/
/* STM32F4xx Peripheral Interrupt Handlers                                    */
/* Add here the Interrupt Handlers for the used peripherals.                  */
/* For the available peripheral interrupt handler names,                      */
/* please refer to the startup file (startup_stm32f4xx.s).                    */
/******************************************************************************/

/* Declare a variable of type SemaphoreHandle_t.  This is used to reference the
semaphore that is used to synchronize a task with an interrupt. */
SemaphoreHandle_t xBinarySemaphore;


/**
  * @brief This function handles EXTI line0 interrupt.
  */
void EXTI0_IRQHandler(void)
{
	BaseType_t xHigherPriorityTaskWoken;
  /* USER CODE BEGIN EXTI0_IRQn 0 */
  xHigherPriorityTaskWoken = pdFALSE;

  HAL_GPIO_TogglePin(GPIOD, RED_LED);
  /* 'Give' the semaphore to unblock the task. */
  xSemaphoreGiveFromISR( xBinarySemaphore, &xHigherPriorityTaskWoken );

  /* USER CODE END EXTI0_IRQn 0 */
  HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_0);
  /* USER CODE BEGIN EXTI0_IRQn 1 */

  /* USER CODE END EXTI0_IRQn 1 */
}

static void vHandlerTask( void *pvParameters )
{
	/* As per most tasks, this task is implemented within an infinite loop. */
	for( ;; )
	{
		/* Use the semaphore to wait for the event.  The semaphore was created
		before the scheduler was started so before this task ran for the first
		time.  The task blocks indefinitely meaning this function call will only
		return once the semaphore has been successfully obtained - so there is
		no need to check the returned value. */
		xSemaphoreTake( xBinarySemaphore, portMAX_DELAY );

		HAL_GPIO_TogglePin(GPIOD, BLUE_LED);
	}
}

static void vPeriodicTask( void *pvParameters )
{
const TickType_t xDelay = pdMS_TO_TICKS( 50UL );

	/* As per most tasks, this task is implemented within an infinite loop. */
	for( ;; )
	{

		vTaskDelay( xDelay );

		HAL_GPIO_TogglePin(GPIOD, GREEN_LED);
	}
}

inline void application16(void)
{
	BaseType_t xReturned1, xReturned2;
	/* Before a semaphore is used it must be explicitly created.  In this
	example	a binary semaphore is created. */
	xBinarySemaphore = xSemaphoreCreateBinary();

	/* Check the semaphore was created successfully. */
	if( xBinarySemaphore != NULL )
	{
		/* Create the 'handler' task, which is the task to which interrupt
		processing is deferred, and so is the task that will be synchronized
		with the interrupt.  The handler task is created with a high priority to
		ensure it runs immediately after the interrupt exits.  In this case a
		priority of 3 is chosen. */
		xReturned1=xTaskCreate( vHandlerTask, "Handler", 128, NULL, 3, NULL );

		/* Create the task that will periodically generate a software interrupt.
		This is created with a priority below the handler task to ensure it will
		get preempted each time the handler task exits the Blocked state. */
		xReturned2=xTaskCreate( vPeriodicTask, "Periodic", 128, NULL, 1, NULL );

	}
	if ((xReturned1==pdPASS)&&(xReturned2==pdPASS))
		vTaskStartScheduler();
}
