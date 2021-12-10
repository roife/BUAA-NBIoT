/**********************************************************************************************************
*                                                THREAD-OS
*                                          The Real-Time Kernel
*                                  THREAD-OS Configuration File for V1.x
*
*                               (c) Copyright 2018-2019, tony(gulonglong)
*                                          All Rights Reserved
**   THREAD-OS is provided in source form for FREE evaluation, for educational use or for peaceful research.
*
* File    : fifo.h
* By      : tony(gulonglong)
* Version : V1.0
*
* This file is a ringbuf module. node composition as follows:
*
*---->fifo{ [len1][------data1--------] [len2][---data2----] [lenx][----lenx------]}
*
*********************************************************************************************************
*/
#ifndef __FIFO_H__
#define __FIFO_H__


#define FIFO_OK                0
#define FIFO_PARAMS_ERR        -1
#define FIFO_FULL_ERR          -2
#define FIFO_EMPTY_ERR         -3
#define FIFO_BUF_SMALL_ERR     -4
#define FIFO_ZREOR_ERR         -5

/*********************************** use fifo ******************************/
typedef struct 
{
	char     *buf;
	uint32_t size;
	uint32_t count;
	uint32_t in;
	uint32_t out;
}fifo_ram_t;


int8_t  fifo_init (fifo_ram_t *m_fifo_ram, void *buf, size_t size);
int32_t fifo_get  (fifo_ram_t *m_fifo_ram, char *buf, uint32_t len);
int32_t fifo_copy(fifo_ram_t *m_fifo_ram, char *buf, uint32_t len);
int32_t fifo_put  (fifo_ram_t *m_fifo_ram, const char *buf, uint32_t len);
int8_t  fifo_flush(fifo_ram_t *m_fifo_ram);
int8_t  fifo_delete_topnode(fifo_ram_t *m_fifo_ram);


#endif
