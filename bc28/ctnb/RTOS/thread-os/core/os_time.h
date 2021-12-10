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
* File    : thread_time.h
* By      : tony(gulonglong)
* Version : V1.1
*
*********************************************************************************************************
*/
#ifndef __OS_TIME_H__
#define __OS_TIME_H__


void os_time_tick_handle(void);

uint32_t os_time_get_timetick(void);

void os_time_set_timetick(uint32_t tick);

#endif

