#ifndef __MOTOR_H
#define __MOTOR_H	 

#include <stdint.h>

#define MOTOR_RUN 1
#define MOTOR_OFF 0
#define MOTOR_FORWARD 1
#define MOTOR_BACK_UP 2

void GPIO_MOTOR_Configuration(void);
void motor_write(uint8_t device_id, uint8_t status);
void motor_read(uint8_t device_id, uint8_t *status);

#endif
