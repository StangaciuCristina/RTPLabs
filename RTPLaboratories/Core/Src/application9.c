#include "main.h"
#include "cmsis_os.h"
#include "application9.h"

/* Used as a loop counter to create a very crude delay. */
#define mainDELAY_LOOP_COUNT		( 0xffffff )
#define GREEN_LED LD4_Pin
#define BLUE_LED LD6_Pin
#define RED_LED LD5_Pin
#define ORANGE_LED LD3_Pin

/* Used to hold the handle of Task2. */
TaskHandle_t xTask2Handle;

void vTask2( void *pvParameters );

void vTask1( void *pvParameters )
{
const TickType_t xDelay100ms = pdMS_TO_TICKS( 100UL );

	for( ;; )
	{
		/* Toggle green led. */
		HAL_GPIO_TogglePin(GPIOD, GREEN_LED);

		/* Create task 2 at a higher priority.  Again the task parameter is not
		used so is set to NULL - BUT this time we want to obtain a handle to the
		task so pass in the address of the xTask2Handle variable. */
		xTaskCreate( vTask2, "Task 2", 1000, NULL, 2, &xTask2Handle );
			 /* The task handle is the last parameter ^^^^^^^^^^^^^ */

		/* Task2 has/had the higher priority, so for Task1 to reach here Task2
		must have already executed and deleted itself.  Delay for 100
		milliseconds. */
		vTaskDelay( xDelay100ms );
	}
}

/*-----------------------------------------------------------*/

void vTask2( void *pvParameters )
{
	/* Task2 blinks once delete itself.  To do this it could call vTaskDelete()
	using a NULL parameter, but instead and purely for demonstration purposes it
	instead calls vTaskDelete() with its own task handle. */
	/* Toggle green led. */
	HAL_GPIO_TogglePin(GPIOD, BLUE_LED);
	vTaskDelete( xTask2Handle );
}
/*-----------------------------------------------------------*/


void application9(void)
{

	/* Create the first task at priority 1.  This time the task parameter is
	not used and is set to NULL.  The task handle is also not used so likewise
	is also set to NULL. */
	xTaskCreate( vTask1, "Task 1", 1000, NULL, 1, NULL );
    /* The task is created at priority 1 ^. */

}
