#include "bsp.h"

static uint8_t led0_status=0;
static uint8_t led1_status=0;

uint8_t led_write(uint8_t device_id, uint8_t status)
{
	status &= 0x07;
	
	if( device_id == LED0_DEVICE_ID )
	{
		HAL_GPIO_WritePin(LED0_RED_PIN, (GPIO_PinState)!(status&LED_RED));
		HAL_GPIO_WritePin(LED0_BLUE_PIN, (GPIO_PinState)!(status&LED_BLUE));
		HAL_GPIO_WritePin(LED0_GREEN_PIN, (GPIO_PinState)!(status&LED_GREEN));
		
		led0_status = status;
	}
	else if( device_id == LED1_DEVICE_ID )
	{
		HAL_GPIO_WritePin(LED1_RED_PIN, (GPIO_PinState)!(status&LED_RED));
		HAL_GPIO_WritePin(LED1_BLUE_PIN, (GPIO_PinState)!(status&LED_BLUE));
		HAL_GPIO_WritePin(LED1_GREEN_PIN, (GPIO_PinState)!(status&LED_GREEN));
		
		led1_status = status;
	}
	else
	{
		return 1;
	}
	
	return 0;
}

void led_read(uint8_t device_id, uint8_t *status)
{
	if( device_id == LED0_DEVICE_ID )
	{
		*status = led0_status;
	}
	else if( device_id == LED1_DEVICE_ID )
	{
		*status = led1_status;
	}
}
/**********************************************************/
