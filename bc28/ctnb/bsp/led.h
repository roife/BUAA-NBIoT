#ifndef __LED_H
#define __LED_H	 


//#include "stm32l4xx.h"
#include "stm32f1xx_hal.h"


void led0_on(GPIO_PinState x,GPIO_PinState y,GPIO_PinState z);
void led1_on(GPIO_PinState x,GPIO_PinState y,GPIO_PinState z);
void led0_off(void);
void led1_off(void);


/*******************************add by tony ***********************************/
/*  led pins define */
#define LED0_RED_PIN     GPIOB,GPIO_PIN_2
#define LED0_BLUE_PIN    GPIOB,GPIO_PIN_11
#define LED0_GREEN_PIN   GPIOB,GPIO_PIN_10

#define LED1_RED_PIN     GPIOB,GPIO_PIN_12
#define LED1_BLUE_PIN    GPIOB,GPIO_PIN_14
#define LED1_GREEN_PIN   GPIOB,GPIO_PIN_13

/*  led status  */
#define LED_OFF           0
#define LED_GREEN         1
#define LED_RED           2
#define LED_BLUE          4
#define LED_PURPLE        LED_RED|LED_BLUE  
#define LED_YELLOW        LED_RED|LED_GREEN  
#define LED_PALE_BLUE     LED_BLUE|LED_GREEN  
#define LED_WHITE         LED_RED|LED_BLUE|LED_GREEN



uint8_t led_write(uint8_t device_id, uint8_t status);
void led_read(uint8_t device_id, uint8_t *status);


#endif
