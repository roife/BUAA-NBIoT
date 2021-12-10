/**********************************************************************************************************
*                                                THREAD-OS
*                                          The Real-Time Kernel
*                                  THREAD-OS Configuration File for V1.x
*
*                               (c) Copyright 2018-2019, tony(gulonglong)
*                                          All Rights Reserved
**   THREAD-OS is provided in source form for FREE evaluation, for educational use or for peaceful research.
*
* File    : os_core.h
* By      : tony(gulonglong)
* Version : V1.2
*
*********************************************************************************************************
*/


#ifndef __OS_CORE_H__
#define __OS_CORE_H__

#define THREAD_GET_RUNTIME(p)   (p)->RunTime




extern uint8_t os_running;

void thread_os_start(void);	

 uint32_t thread_get_runtime(void *p);

#endif

