/**********************************************************************************************************
*                                                THREAD-OS
*                                          The Real-Time Kernel
*                                  THREAD-OS Configuration File for V1.x
*
*                               (c) Copyright 2018-2019, tony(gulonglong)
*                                          All Rights Reserved
**   THREAD-OS is provided in source form for FREE evaluation, for educational use or for peaceful research.
*
* File    : os_sem.c
* By      : tony(gulonglong)
* Version : V1.2
*
*********************************************************************************************************
*/
#include <stdint.h>

#include "thread.h"


int8_t OSSemCreat(OSSem_t *sem, struct process *p, uint32_t value)
{
	if( sem == NULL || p == NULL )
		return OS_SEM_ERR;
	
	sem->sem = value;
  sem->p = p;
	
	return OS_SEM_OK;
}

int8_t OSSemPost(OSSem_t *sem)
{
	if( sem == NULL || sem->p == NULL )
		return OS_SEM_ERR;
	
	sem->sem++;
	
	return OS_SEM_OK;
}

int8_t OSSemPend(OSSem_t *sem)
{
	if( sem == NULL || sem->p == NULL )
		return OS_SEM_ERR;
		
	if( sem->sem > 0 )
	{
		sem->sem--;
		return OS_SEM_OK;
	}
	else
	{
		return OS_SEM_ERR;
	}
}
