/**********************************************************************************************************
*                                                THREAD-OS
*                                          The Real-Time Kernel
*                                  THREAD-OS Configuration File for V1.x
*
*                               (c) Copyright 2018-2019, tony(gulonglong)
*                                          All Rights Reserved
**   THREAD-OS is provided in source form for FREE evaluation, for educational use or for peaceful research.
*
* File    : os_event.h
* By      : tony(gulonglong)
* Version : V1.0
*
*********************************************************************************************************
*/
#ifndef __OS_EVENT_H__
#define __OS_EVENT_H__

#include <stdint.h>
#include "thread.h"


#define OS_EVENT_OK      0
#define OS_EVENT_ERR     -1

#define OS_EVENT_CLEAR   0

#define OS_EVENT_NO_MASK  0xffffffff

typedef struct 
{
	uint32_t valid;
	uint32_t MaskValue;
	uint32_t event;
	void     *data;
	struct process *recv_thread;
}os_event_t;


int8_t os_event_create(os_event_t *OSEvent, struct process *thread);
int8_t os_event_set_event(os_event_t *OSEvent, uint32_t MaskValue);
int8_t os_event_post_event(os_event_t *OSEvent, uint32_t value);
int8_t os_event_pend_event(os_event_t *OSEvent, uint32_t *event, void **data);

#define OS_EVENT_PEND(ret,timeout,os_ret,os_event,event_value,msg)    THREAD_WAIT_EVENT(1,&ret, timeout)   \
                                                                      os_ret = os_event_pend_event(os_event, &event_value, msg)

#endif
