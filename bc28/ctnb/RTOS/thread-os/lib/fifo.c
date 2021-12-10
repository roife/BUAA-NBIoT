/**********************************************************************************************************
*                                                THREAD-OS
*                                          The Real-Time Kernel
*                                  THREAD-OS Configuration File for V1.x
*
*                               (c) Copyright 2018-2019, tony(gulonglong)
*                                          All Rights Reserved
**   THREAD-OS is provided in source form for FREE evaluation, for educational use or for peaceful research.
*
* File    : fifo.c
* By      : tony(gulonglong)
* Version : V1.0
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

#include "fifo.h"
#include "TinyLog.h"

#define fifo_log(...)      //os_log("\r\n[fifo]");os_log(__VA_ARGS__);


int8_t fifo_init(fifo_ram_t *m_fifo_ram, void *buf, size_t size)
{
	if( m_fifo_ram == NULL )
	{
		fifo_log("invalid fifo pointer, please creat a fifo\r\n");
		return FIFO_PARAMS_ERR;
	}
	
	if( buf == NULL || size == 0)
	{
		fifo_log("fifo buf is NULL or len is zero\r\n");
		return FIFO_PARAMS_ERR;
	}
	
	m_fifo_ram->buf   = buf;
	m_fifo_ram->size  = size;
	m_fifo_ram->count = 0;
	m_fifo_ram->in    = 0;
	m_fifo_ram->out   = 0;
	
	return FIFO_OK;
}

int32_t fifo_get(fifo_ram_t *m_fifo_ram, char *buf, uint32_t len)
{
	uint32_t node_len;
	
	if( m_fifo_ram == NULL )
	{
		fifo_log("invalid fifo pointer,please creat a fifo\r\n");
		return FIFO_PARAMS_ERR;
	}
	
	if( len == 0 || buf == NULL )
  {
		fifo_log("output buf is NULL or out len is zero\r\n");
		return FIFO_PARAMS_ERR;
	}
	
	if( m_fifo_ram->count == 0 )
	{
		fifo_log("fifo is empty\r\n");
		return FIFO_EMPTY_ERR;
	}
	
	node_len = m_fifo_ram->buf[m_fifo_ram->out%m_fifo_ram->size];
	if( node_len > len )
	{
		fifo_log("get buf too small\r\n");
		return FIFO_BUF_SMALL_ERR;
	}
	else if( node_len == 0 )
	{
		fifo_log("get node-len is 0, err\r\n");
		return FIFO_ZREOR_ERR;
	}
	
	m_fifo_ram->out++;
	
	for(uint32_t i=0;i<node_len;i++)
	{
		buf[i] =  m_fifo_ram->buf[ (m_fifo_ram->out++)%m_fifo_ram->size ];
	}
	
	fifo_log("buf len:%d\r\n",node_len);
	
	/* modify fifo count */
	m_fifo_ram->count -= (node_len+1);
	
	return node_len;	
}

int32_t fifo_copy(fifo_ram_t *m_fifo_ram, char *buf, uint32_t len)
{
	uint32_t node_len;
	uint32_t old_out = 0;
	
	if( m_fifo_ram == NULL )
	{
		fifo_log("invalid fifo pointer,please creat a fifo\r\n");
		return FIFO_PARAMS_ERR;
	}
	
	if( len == 0 || buf == NULL )
  {
		fifo_log("output buf is NULL or out len is zero\r\n");
		return FIFO_PARAMS_ERR;
	}
	
	if( m_fifo_ram->count == 0 )
	{
		fifo_log("fifo is empty\r\n");
		return FIFO_EMPTY_ERR;
	}
	
	node_len = m_fifo_ram->buf[m_fifo_ram->out%m_fifo_ram->size];
	if( node_len > len )
	{
		fifo_log("get buf too small\r\n");
		return FIFO_BUF_SMALL_ERR;
	}
	else if( node_len == 0 )
	{
		fifo_log("get node-len is 0, err\r\n");
		return FIFO_ZREOR_ERR;
	}
	
	old_out = m_fifo_ram->out;
	
	m_fifo_ram->out++;
	
	for(uint32_t i=0;i<node_len;i++)
	{
		buf[i] =  m_fifo_ram->buf[ (m_fifo_ram->out++)%m_fifo_ram->size ];
	}
	
	m_fifo_ram->out = old_out;
	
	return node_len;	
}

int32_t fifo_put(fifo_ram_t *m_fifo_ram, const char *buf, uint32_t len)
{
	if( m_fifo_ram == NULL )
	{
		fifo_log("invalid fifo pointer,please creat a fifo\r\n");
		return FIFO_PARAMS_ERR;
	}
	
	if( len == 0 || buf == NULL )
  {
		fifo_log("input buf is NULL or in len is zero\r\n");
		return FIFO_PARAMS_ERR;
	}
	
	if( m_fifo_ram->count+len+1 > m_fifo_ram->size )
	{
		fifo_log("fifo is full\r\n",len);
		return FIFO_FULL_ERR;
	}
	
	m_fifo_ram->buf[ (m_fifo_ram->in++)%m_fifo_ram->size ]= len;
	for(uint32_t i=0;i<len;i++)
	{
		m_fifo_ram->buf[ (m_fifo_ram->in++)%m_fifo_ram->size ]= buf[i];
	}
	
	/* modify fifo count */
	m_fifo_ram->count += (len+1);
	
	return len+1;
}

int8_t fifo_get_node_count(fifo_ram_t *m_fifo_ram)
{
	if( m_fifo_ram == NULL )
	{
		fifo_log("invalid fifo pointer,please creat a fifo\r\n");
		return FIFO_PARAMS_ERR;
	}
	
	return m_fifo_ram->count;
}

int8_t fifo_delete_topnode(fifo_ram_t *m_fifo_ram)
{
	uint32_t node_len;
	
	if( m_fifo_ram == NULL )
	{
		fifo_log("invalid fifo pointer,please creat a fifo\r\n");
		return FIFO_PARAMS_ERR;
	}
	
	if( m_fifo_ram->count == 0 )
	{
		return FIFO_OK;
	}
	
	node_len = m_fifo_ram->buf[m_fifo_ram->out%m_fifo_ram->size];
	
	m_fifo_ram->out += (1 + node_len)%m_fifo_ram->size;
	
		/* modify fifo count */
	m_fifo_ram->count -= (node_len+1);
	
	return FIFO_OK;
}

int8_t fifo_flush(fifo_ram_t *m_fifo_ram)
{
	if( m_fifo_ram == NULL )
	{
		fifo_log("fifo flush err:invalid fifo pointers\r\n");
		return FIFO_PARAMS_ERR;
	}
	
	m_fifo_ram->count = 0;
	m_fifo_ram->in    = 0;
	m_fifo_ram->out   = 0;
	
	return FIFO_OK;
}

