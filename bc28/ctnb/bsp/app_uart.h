#ifndef __APP_UART_H__
#define __APP_UART_H__

#include <stdint.h>

#include "stm32f1xx_hal.h"


typedef struct
{
	uint32_t len;
	uint32_t count;
	uint8_t *buf;
	uint16_t delay_ms;
	uint8_t  timeout;
}uart_buf_t;



HAL_StatusTypeDef app_uart_Init(void *m_uart, uint32_t m_BaudRate);

void app_uart_init(uint32_t m_BaudRate, uint32_t timeout);

//void app_uart_register(uart_buf_t *uart_buf);

int8_t app_uart_register_fifo(void *fifo_ram);

uint32_t log_uart_read(void *buf, uint32_t len);

#endif
