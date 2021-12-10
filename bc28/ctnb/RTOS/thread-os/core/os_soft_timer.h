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
* File    : thread_timer.h
* By      : tony(gulonglong)
* Version : V1.2
*
*********************************************************************************************************
*/
#ifndef __OS_SOFT_TIMER_H__
#define __OS_SOFT_TIMER_H__

#include <stdint.h>

/********************* os time count for user *****************************/
#define SOFT_TIMER_COUNT_MAX     20

#define SOFT_RT_TIMER_COUNT_MAX  5

/********************* os timer mode **************************************/
#define OS_ONE_SHOT_MODE     1
#define OS_PERIODIC_MODE     2
#define OS_EVENT_MODE        3
#define OS_RT_ONE_SHOT_MODE  4
#define OS_RT_PERIODIC_MODE  5
#define OS_RT_EVENT_MODE     6
#define OS_TIMER_RESERVE     7

/********************* os timer err code **********************************/
#define OS_TIME_OK               0
#define OS_TIME_NO_MEM_ERR       -1
#define OS_TIME_NO_MODE_ERR      -2
#define OS_TIME_HAND_ERR         -3
#define OS_TIME_VALUE_ERR        -4
#define OS_TIME_NOT_RUN_ERR      -5

typedef int8_t time_retval_t;

/*********************  STCB struct **************************************/

typedef void (*os_time_callback)(void *);

typedef struct
{
	uint8_t  start;
	uint8_t  mode;
	uint32_t dest_tick;
	uint32_t time_tick;
	void *args;
	os_time_callback time_cb;
}soft_timer_t;

/************************** soft timer functions ************************/

void soft_timer_init(void);
	
//soft timer user api
time_retval_t soft_timer_register(soft_timer_t **m_timer_obj, uint8_t mode, void *timeout_func, void *args);
time_retval_t soft_timer_start(soft_timer_t *m_timer_obj, uint32_t ms);
time_retval_t soft_timer_stop(soft_timer_t *m_timer_obj);
time_retval_t soft_timer_updata(soft_timer_t *m_timer_obj, uint32_t ms);

//soft timer core
void thread_timer_tick(void);

//thread-os v1.0 api, new version not use it
#if 0
uint8_t thread_delay(uint8_t handle);
#endif


#endif
