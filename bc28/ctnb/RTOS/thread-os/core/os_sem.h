/**********************************************************************************************************
*                                                THREAD-OS
*                                          The Real-Time Kernel
*                                  THREAD-OS Configuration File for V1.x
*
*                               (c) Copyright 2018-2019, tony(gulonglong)
*                                          All Rights Reserved
**   THREAD-OS is provided in source form for FREE evaluation, for educational use or for peaceful research.
*
* File    : os_sem.h
* By      : tony(gulonglong)
* Version : V1.2
*
*********************************************************************************************************
*/
#ifndef __OS_SEM_H__
#define __OS_SEM_H__

#include <stdint.h>
#include "thread.h"


#define OS_SEM_OK   0
#define OS_SEM_ERR  -1


typedef struct 
{
	uint32_t sem;
	struct process *p;
}OSSem_t;


#define THREAD_SEM_CREAT(sem,p,value)        OSSemCreat(sem,p,value);
#define THREAD_SEM_PEND(sem,ret,delayms)     THREAD_WAIT_EVENT( (OSSemPend(sem) == OS_SEM_ERR), ret, delayms)
#define THREAD_SEM_POST(sem)                 OSSemPost(sem);THREAD_POST_EVENT((sem)->p);
		

int8_t OSSemCreat(OSSem_t *sem, struct process *process, uint32_t value);
int8_t OSSemPost(OSSem_t *sem);
int8_t OSSemPend(OSSem_t *sem);

#endif
