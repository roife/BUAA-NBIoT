#include <string.h>
#include <stdbool.h>

#include "app_uart.h"
#include "os_soft_timer.h"
#include "TinyLog.h"
#include "os_fifo.h"

#define UART_BUFFER_LEN           256u

static soft_timer_t *uart_time_obj;

static uart_buf_t current_buf;

static OSfifo_t  *m_fifo=NULL;

uint8_t uart_buf[UART_BUFFER_LEN]={0,};

/**************************************************************************/
static HAL_StatusTypeDef uart_Init(void *m_uart, uint32_t m_BaudRate);

static void app_uart_timout_cb(void *args);


#ifdef __GNUC__ 
  /* With GCC/RAISONANCE, small printf (option LD Linker->Libraries->Small printf 
     set to 'Yes') calls __io_putchar() */ 
  #define PUTCHAR_PROTOTYPE int __io_putchar(int ch) 
#else 
  #define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f) 
#endif /* __GNUC__ */ 

	
PUTCHAR_PROTOTYPE
{
  USART1->DR = (ch & (uint8_t)0xFFU);
	while( !((USART1->SR & UART_FLAG_TXE) == UART_FLAG_TXE) ){};
	
  return ch;
}


int8_t app_uart_register_fifo(void *os_fifo)
{
	if( os_fifo == NULL )
	{
		os_log("fifo is NULL\r\n");
		return -1;
	}
	
	m_fifo = (OSfifo_t *)os_fifo;
	return 0;
}



void app_uart_init(uint32_t m_BaudRate, uint32_t timeout)
{
	uart_Init(USART1, m_BaudRate);
	
	current_buf.buf = uart_buf;
	current_buf.len = UART_BUFFER_LEN;
	current_buf.delay_ms = timeout;
}

extern void UART_SetConfig(UART_HandleTypeDef *huart);

/* UART4 init function */
static HAL_StatusTypeDef uart_Init(void *m_uart, uint32_t m_BaudRate)
{
	static UART_HandleTypeDef m_huart;

  m_huart.Instance                    = (USART_TypeDef *)m_uart;
  m_huart.Init.BaudRate               = m_BaudRate;
  m_huart.Init.WordLength             = UART_WORDLENGTH_8B;
  m_huart.Init.StopBits               = UART_STOPBITS_1;
  m_huart.Init.Parity                 = UART_PARITY_NONE;
  m_huart.Init.Mode                   = UART_MODE_TX_RX;
  m_huart.Init.HwFlowCtl              = UART_HWCONTROL_NONE;
  m_huart.Init.OverSampling           = UART_OVERSAMPLING_16;
  
     /* Init the low level hardware : GPIO, CLOCK */
   HAL_UART_MspInit(&m_huart);


  /* Disable the Peripheral */
  __HAL_UART_DISABLE(&m_huart);

  /* Set the UART Communication parameters */
  UART_SetConfig(&m_huart);
	
	  /* In asynchronous mode, the following bits must be kept cleared: 
     - LINEN and CLKEN bits in the USART_CR2 register,
     - SCEN, HDSEL and IREN  bits in the USART_CR3 register.*/
  CLEAR_BIT(m_huart.Instance->CR2, (USART_CR2_LINEN | USART_CR2_CLKEN));
  CLEAR_BIT(m_huart.Instance->CR3, (USART_CR3_SCEN | USART_CR3_HDSEL | USART_CR3_IREN));

  /* Enable the Peripheral */
  __HAL_UART_ENABLE(&m_huart);

	/* Enable the UART Error Interrupt: (Frame error, noise error, overrun error) */
  //SET_BIT(m_huart.Instance->CR3, USART_CR3_EIE);
	
	/* Enable the PEIE and RXNEIE */
	SET_BIT(m_huart.Instance->CR1, USART_CR1_RXNEIE);
	
	return HAL_OK;
}



static void app_uart_timout_cb(void *args)
{
	uint32_t len = current_buf.count;
	
	current_buf.buf[current_buf.count++] = 0;
	
	//os_log("[uart:len:%d,%s]\r\n",len,current_buf.buf);
	
	if( m_fifo != NULL )
	{
		THREAD_FIFO_PUT(m_fifo, (const char *)current_buf.buf, len);
	}
	
	if( (len + 1) == current_buf.count )
	{
		current_buf.count = 0;
	}
	else if( (len +1) < current_buf.count )
	{
		current_buf.count = 0;
		os_log("uart timeout time is too big!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\r\n");
	}
}



void USART1_IRQHandler(void)
{
	uint16_t  uhdata;
	static bool reg_flag = true;
	
	if( reg_flag == true )
	{
		reg_flag = false;
		soft_timer_register(&uart_time_obj, OS_RT_ONE_SHOT_MODE, app_uart_timout_cb, NULL);
	}
	
	if( (USART1->SR & USART_SR_RXNE) == USART_SR_RXNE )
	{
		/* Clear RXNE interrupt flag */
		USART1->SR &=~ USART_SR_RXNE;
	
	  uhdata = (uint16_t)(USART1->DR);
		
		if( current_buf.buf == NULL )
			return;
		
    current_buf.buf[current_buf.count++] = (uint8_t)(uhdata & 0xff);
	
	  if( current_buf.count >= current_buf.len )
	  {
			current_buf.count = 0;
	  }
		
	  soft_timer_start(uart_time_obj, current_buf.delay_ms);
	}
	//else if( (USART1->SR & USART_SR_ORE) == USART_SR_ORE )
	//{
	//	USART1->CR1 = USART_CR1_ORECF;
	//}
}

