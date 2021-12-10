/**********************************************************************************************************
*                                                THREAD-OS
*                                          The Real-Time Kernel
*                                  THREAD-OS Configuration File for V1.x
*
*                               (c) Copyright 2018-2019, tony(gulonglong)
*                                          All Rights Reserved
**   THREAD-OS is provided in source form for FREE evaluation, for educational use or for peaceful research.
*
* File    : os_fifo.h
* By      : tony(gulonglong)
* Version : V1.3
*
* This file is a ringbuf module. node composition as follows:
*
*---->fifo{ [len1][------data1--------] [len2][---data2----] [lenx][----lenx------]}
*
*********************************************************************************************************
*/
#ifndef __OS_FIFO_H__
#define __OS_FIFO_H__

#include "thread.h"

#if defined(THREAD_OS_OSFIFO_EN) && THREAD_OS_OSFIFO_EN ==1


#include "fifo.h"


typedef struct
{
	struct process *p;
	
	fifo_ram_t *fifo;
}OSfifo_t;

#define THREAD_FIFO_INIT(OSfifo,process,myfifo,buf,size)  (OSfifo)->p = process;(OSfifo)->fifo = myfifo;fifo_init((myfifo),(buf),(size))

#define THREAD_FIFO_GET(OSfifo,buf,len,ret,delayms)       THREAD_WAIT_EVENT( (fifo_get((OSfifo)->fifo,(buf),(len)) <= 0), (ret), (delayms))

#define THREAD_FIFO_PUT(OSfifo,buf,len)                   fifo_put((OSfifo)->fifo,(buf),(len));THREAD_POST_EVENT((OSfifo)->p)

#endif

#endif
