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
* File    : thread_time.c
* By      : tony(gulonglong)
* Version : V1.1
*
*********************************************************************************************************
*/
#include <string.h>
#include <stdint.h>
#include "thread.h"

#if THREAD_OS_TIME_EN == 1

static uint32_t timetick=0;

void os_time_tick_handle(void)
{
	process_t *q = process_list;
	
	if( q == NULL )
		return;
	
	timetick++;
	
	//for os time manage
	do
	{
		if( (q->status&THREAD_SUSPEND)      != THREAD_SUSPEND && 
			  (q->status&THREAD_SUSPEND_TICK) == THREAD_SUSPEND_TICK )
		{
			if( timetick >= q->timetick )
			{
					if( (q->event&THREAD_EVENT_SET) == THREAD_EVENT_SET )
					{
						q->event = THREAD_EVENT_TIMEOUT;
					}
					
					q->status = THREAD_READY;
					q->timetick = 0;
			}
		}
		q = q->next;
	}while(q != process_list);
	
}

uint32_t os_time_get_timetick(void)
{
	return timetick;
}

void os_time_set_timetick(uint32_t tick)
{
	timetick = tick;
}
#endif
