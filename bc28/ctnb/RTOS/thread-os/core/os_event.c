/**********************************************************************************************************
*                                                THREAD-OS
*                                          The Real-Time Kernel
*                                  THREAD-OS Configuration File for V1.x
*
*                               (c) Copyright 2018-2019, tony(gulonglong)
*                                          All Rights Reserved
**   THREAD-OS is provided in source form for FREE evaluation, for educational use or for peaceful research.
*
* File    : os_event.c
* By      : tony(gulonglong)
* Version : V1.0
*
*********************************************************************************************************
*/
#include <stdint.h>
#include <stdbool.h>
#include "thread.h"


#define event_log(...)      os_log("\r\n[event]");os_log(__VA_ARGS__);

int8_t os_event_create(os_event_t *OSEvent, struct process *thread)
{
	if( OSEvent == NULL )
	{
		event_log("creat event fail\r\n");
		return OS_EVENT_ERR;
	}
	
	//updata init value
	OSEvent->valid = true;
	OSEvent->MaskValue = 0;
  OSEvent->recv_thread = thread;
	OSEvent->event = OS_EVENT_CLEAR;
	
	return OS_EVENT_OK;
}

int8_t os_event_set_event(os_event_t *OSEvent, uint32_t MaskValue)
{
	if( OSEvent == NULL || OSEvent->recv_thread== NULL || OSEvent->valid == false )
	{
		event_log("set event fail\r\n");
		return OS_EVENT_ERR;
	}
	
	OSEvent->MaskValue |= MaskValue;
	
	return OS_EVENT_OK;
}

int8_t os_event_post_event(os_event_t *OSEvent, uint32_t event)
{
	if( OSEvent == NULL || OSEvent->recv_thread == NULL || event == 0 || OSEvent->valid == false )
	{
		event_log("post event fail\r\n");
		return OS_EVENT_ERR;
	}
	
	if( (OSEvent->MaskValue & event) > 0 )
	{
			OSEvent->event |= OSEvent->MaskValue & event;
			THREAD_POST_EVENT(OSEvent->recv_thread);
	}
	else
	{
		event_log("post invalid event type\r\n");
		return OS_EVENT_ERR;
	}
	
	return OS_EVENT_OK;
}

int8_t os_event_pend_event(os_event_t *OSEvent, uint32_t *event, void **data)
{
	if( OSEvent == NULL || OSEvent->recv_thread == NULL || event == NULL || OSEvent->valid == false )
	{
		event_log("pend event fail\r\n");
		return OS_EVENT_ERR;
	}
		
	if( (OSEvent->MaskValue & OSEvent->event) > 0 )
	{
		*event = OSEvent->event;
		if( data != NULL )
		{
			*data = OSEvent->data;
		}
		OSEvent->event = OS_EVENT_CLEAR;
		return OS_EVENT_OK;
	}
	else
	{
		event_log("pend invalid event type\r\n");
		return OS_EVENT_ERR;
	}
}

