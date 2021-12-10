#ifndef __LOG_UART_H__
#define __LOG_UART_H__

#include "stm32f1xx_hal.h"

#define LOG_UART_BUFF_LEN   128

typedef struct
{
	uint32_t count;
	uint8_t  buf[LOG_UART_BUFF_LEN];
}log_uart_buf_t;


void log_uart_init(uint32_t m_BaudRate);
uint32_t log_uart_read(void *buf, uint32_t len);
uint32_t log_uart_write(uint8_t *buf, uint32_t len);

#endif
