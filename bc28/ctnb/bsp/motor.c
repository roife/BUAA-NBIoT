#include "bsp.h"

#define M_PORT1   GPIOB
#define M_PORT2   GPIOB
#define M_PIN1    GPIO_PIN_0
#define M_PIN2    GPIO_PIN_1

#define MOTOR_PIN1  GPIOB, GPIO_PIN_0
#define MOTOR_PIN2  GPIOB, GPIO_PIN_1

#define SET_HIGH  GPIO_PIN_SET
#define SET_LOW   GPIO_PIN_RESET


static uint8_t motor_status = MOTOR_OFF;



void motor_write(uint8_t device_id, uint8_t status)
{
	(void)device_id;
	
	if( status > MOTOR_BACK_UP )
		return;
	if( status&MOTOR_FORWARD )
	{
		HAL_GPIO_WritePin(MOTOR_PIN1, (GPIO_PinState)MOTOR_RUN);
	  HAL_GPIO_WritePin(MOTOR_PIN2, (GPIO_PinState)MOTOR_OFF);
		motor_status = status;
	}
	else if( status&MOTOR_BACK_UP )
	{
		HAL_GPIO_WritePin(MOTOR_PIN1, (GPIO_PinState)MOTOR_OFF);
	  HAL_GPIO_WritePin(MOTOR_PIN2, (GPIO_PinState)MOTOR_RUN);
		motor_status = status;
	}
	else if( status == 0 )
	{
		HAL_GPIO_WritePin(MOTOR_PIN1, (GPIO_PinState)MOTOR_OFF);
	  HAL_GPIO_WritePin(MOTOR_PIN2, (GPIO_PinState)MOTOR_OFF);
		motor_status = status;
	}
	
	
}

void motor_read(uint8_t device_id, uint8_t *status)
{
	*status = motor_status;
}
