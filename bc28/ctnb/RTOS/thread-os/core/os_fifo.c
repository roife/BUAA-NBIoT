/**********************************************************************************************************
*                                                THREAD-OS
*                                          The Real-Time Kernel
*                                  THREAD-OS Configuration File for V1.x
*
*                               (c) Copyright 2018-2019, tony(gulonglong)
*                                          All Rights Reserved
**   THREAD-OS is provided in source form for FREE evaluation, for educational use or for peaceful research.
*
* File    : os_fifo.c
* By      : tony(gulonglong)
* Version : V1.3
*
* This file is a ringbuf module. node composition as follows:
*
*---->fifo{ [len1][------data1--------] [len2][---data2----] [lenx][----datax------]}
*
*note£º fifo get :get data first then modify the fifo count
*       fifo put :put data first then modify the fifo count
*********************************************************************************************************
*/

#include <string.h>
#include <stdint.h>

#include "thread.h"

/******************************  thread-os fifo***************************/

#if defined(THREAD_OS_OSFIFO_EN) && THREAD_OS_OSFIFO_EN ==1


#define fifo_log(...)      //os_log("\r\n[fifo]");os_log(__VA_ARGS__);

int8_t thread_fifo_init(OSfifo_t *m_fifo, struct process *p, void *buf, size_t size)
{
	if( p == NULL || buf == NULL || size == 0)
		return FIFO_PARAMS_ERR;
	
	m_fifo->p = p;
	m_fifo->fifo->buf = buf;
	m_fifo->fifo->size = size;
	m_fifo->fifo->count = 0;
	m_fifo->fifo->in    = 0;
	m_fifo->fifo->out   = 0;
	
	return FIFO_OK;
}

uint32_t thread_fifo_get(OSfifo_t *m_fifo, char *buf, uint32_t len)
{
	uint32_t node_len;
	
	if( m_fifo == NULL || m_fifo->p == NULL || buf == NULL || len ==0)
		return 0;
	
	if( m_fifo->fifo->count == 0 )
	{
		fifo_log("fifo is empty\r\n");
		return 0;
	}
	
	node_len = m_fifo->fifo->buf[m_fifo->fifo->out%m_fifo->fifo->size];
	if( node_len > len )
	{
		fifo_log("input mem not enough\r\n");
		return 0;
	}
	else if( node_len == 0 )
	{
		fifo_log("node_len 0 err\r\n");
		return 0;
	}
	
	m_fifo->fifo->out++;
	
	for(uint32_t i=0;i<node_len;i++)
	{
		buf[i] =  m_fifo->fifo->buf[ (m_fifo->fifo->out++)%m_fifo->fifo->size ];
	}
	
	/* modify fifo count */
	m_fifo->fifo->count -= (node_len+1);
	
	return node_len;	
}

uint32_t thread_fifo_put(OSfifo_t *m_fifo, const char *buf, uint32_t len)
{
	if( m_fifo == NULL || m_fifo->p == NULL || buf == NULL || len ==0 )
		return 0;
		
	if( m_fifo->fifo->count+len+1 > m_fifo->fifo->size )
	{
		fifo_log("fifo is full\r\n");
		return 0;
	}
	
	m_fifo->fifo->buf[ (m_fifo->fifo->in++)%m_fifo->fifo->size ]= len;
	for(uint32_t i=0;i<len;i++)
	{
		m_fifo->fifo->buf[ (m_fifo->fifo->in++)%m_fifo->fifo->size ]= buf[i];
	}
	
	/* modify fifo count */
	m_fifo->fifo->count += (len+1);
	
  if( m_fifo->p->status == THREAD_SUSPEND )
	{
		m_fifo->p->status = THREAD_READY;
	}
	
	return len+1;
}

int8_t thread_fifo_flush(OSfifo_t *m_fifo)
{
	if( m_fifo == NULL )
		return FIFO_PARAMS_ERR;
		
	m_fifo->fifo->count = 0;
	m_fifo->fifo->in    = 0;
	m_fifo->fifo->out   = 0;
	
	return FIFO_OK;
}

#endif

