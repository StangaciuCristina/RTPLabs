#include "main.h"
#include "cmsis_os.h"
#include "application0.h"
/* Used as a loop counter to create a very crude delay. */
#define mainDELAY_LOOP_COUNT		( 0xffffff )

void Led_Test(void *pvParameters)
{
	volatile uint32_t ul;
	for (;;)
	{
		/* Toggle green led. */
		HAL_GPIO_TogglePin(GPIOD, LD4_Pin);
		/* Delay for a period. */
		for (ul = 0; ul < mainDELAY_LOOP_COUNT; ul++)
		{
			/* This loop is just a very crude delay implementation.  There is
			nothing to do in here.  Later exercises will replace this crude
			loop with a proper delay/sleep function. */
		}
	}
}

void application0(void)
{
	xTaskCreate(Led_Test, "led_Green", configMINIMAL_STACK_SIZE, 0, 2, NULL);
}
