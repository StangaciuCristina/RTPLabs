#include "main.h"
#include "cmsis_os.h"
#include "timers.h"
#include "semphr.h"
#include "application18.h"
#include "stm32f4xx_it.h"

/* Used as a loop counter to create a very crude delay. */
#define mainDELAY_LOOP_COUNT		( 0xffffff )
#define GREEN_LED LD4_Pin
#define BLUE_LED LD6_Pin
#define RED_LED LD5_Pin
#define ORANGE_LED LD3_Pin

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


/******************************************************************************/
/* STM32F4xx Peripheral Interrupt Handlers                                    */
/* Add here the Interrupt Handlers for the used peripherals.                  */
/* For the available peripheral interrupt handler names,                      */
/* please refer to the startup file (startup_stm32f4xx.s).                    */
/******************************************************************************/



/**
  * @brief This function handles EXTI line0 interrupt.
  */

static void vDeferredHandlingFunction( void *pvParameter1, uint32_t ulParameter2 )
{
	/* Remove the compiler warning indicating that pvParameter1 is not used, as
	pvParameter1 is not used in this example. */
	( void ) pvParameter1;

	HAL_GPIO_TogglePin(GPIOD, BLUE_LED);
}

void EXTI0_IRQHandler(void)
{
  static uint32_t ulParameterValue = 0;
  BaseType_t xHigherPriorityTaskWoken;
  /* USER CODE BEGIN EXTI0_IRQn 0 */
  xHigherPriorityTaskWoken = pdFALSE;

  xTimerPendFunctionCallFromISR( vDeferredHandlingFunction, NULL, ulParameterValue, &xHigherPriorityTaskWoken );
  ulParameterValue++;

  HAL_GPIO_TogglePin(GPIOD, RED_LED);

  /* USER CODE END EXTI0_IRQn 0 */
  HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_0);
  /* USER CODE BEGIN EXTI0_IRQn 1 */

  /* USER CODE END EXTI0_IRQn 1 */
}


static void vPeriodicTask( void *pvParameters )
{
const TickType_t xDelay500ms = pdMS_TO_TICKS( 500UL );

	/* As per most tasks, this task is implemented within an infinite loop. */
	for( ;; )
	{

		vTaskDelay( xDelay500ms );

		HAL_GPIO_TogglePin(GPIOD, GREEN_LED);
	}
}


void application18(void)
{
	const UBaseType_t ulPeriodicTaskPriority = configTIMER_TASK_PRIORITY - 1;

	/* Create the task that will periodically generate a software interrupt. */
	xTaskCreate( vPeriodicTask, "Periodic", 1000, NULL, ulPeriodicTaskPriority, NULL );

}
