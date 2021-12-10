/*
*********************************************************************************************************
*                                                THREAD-OS
*                                          The Real-Time Kernel
*                                  THREAD-OS Configuration File for V1.x
*
*                               (c) Copyright 2018-2019, tony(gulonglong)
*                                          All Rights Reserved
**   THREAD-OS is provided in source form for FREE evaluation, for educational use or for peaceful research.
*
* File    : os_port.c
* By      : tony(gulonglong)
* Version : V1.1
*
*********************************************************************************************************
*/
#include <stdint.h>

#include "thread.h"


void thread_os_tick(void)
{
	
	//for thread-os time manage 
#if THREAD_OS_TIME_EN == 1
	if( os_running == 1 )
	{
		os_time_tick_handle();
	}
#endif
	
	//for thread-os timer manage
#if THREAD_OS_SOFT_TIMER_EN == 1
	if( os_running == 1 )
	{
		thread_timer_tick();
	}
#endif
	
}
