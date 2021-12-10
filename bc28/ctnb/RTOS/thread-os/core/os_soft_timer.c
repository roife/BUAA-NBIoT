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
* File    : thread_timer.c
* By      : tony(gulonglong)
* Version : V1.2 
*

soft timer init :
                 void app_timer_init(void)

soft timer core :
                 void thread_timer_tick(void)
								 PROCESS_THREAD(thread_timer_process, ev, pdata)
								 
soft timer user api :
                 time_retval_t soft_timer_register(soft_timer_t **m_timer_obj, uint8_t mode, void *timeout_func, void *args)
								 time_retval_t soft_timer_start(soft_timer_t *m_timer_obj, uint32_t ms)
								 time_retval_t soft_timer_stop(soft_timer_t *m_timer_obj)
								 time_retval_t soft_timer_updata(soft_timer_t *m_timer_obj, uint32_t ms)
*********************************************************************************************************
*/
#include <string.h>

#include "thread.h"
#include "os_sem.h"

#if THREAD_OS_SOFT_TIMER_EN == 1


#define OS_TIMER_DISABLE  0
#define OS_TIMER_ENABLE   1

typedef struct SWTCB_t
{
	soft_timer_t soft_timer[SOFT_TIMER_COUNT_MAX];
	uint8_t cur_timer_cnt;

	soft_timer_t soft_rt_timer[SOFT_RT_TIMER_COUNT_MAX];
	uint8_t cur_rt_timer_cnt;

	OSSem_t  OSSem;

	uint32_t soft_timer_tick;
	uint32_t rt_timer_tick;
} OS_STCB_t;
/*********************************** variables *****************************************/

OS_STCB_t OS_SoftTimer;

THREAD_INIT(thread_timer_process, "ostimer thread");

/************************************ functions ***************************************/
static void os_rt_timer_tick(void);

/********************** soft timer api **********************************/

time_retval_t soft_timer_register(soft_timer_t **m_timer_obj, uint8_t mode, void *timeout_func, void *args)
{	
	if( mode >= OS_TIMER_RESERVE || mode == 0 )
		return OS_TIME_NO_MODE_ERR;
	
	if( mode >= OS_ONE_SHOT_MODE && mode <= OS_EVENT_MODE )
	{
		if( OS_SoftTimer.cur_timer_cnt >= SOFT_TIMER_COUNT_MAX )
			return OS_TIME_NO_MEM_ERR;
			
		OS_SoftTimer.soft_timer[OS_SoftTimer.cur_timer_cnt].mode = mode;
		OS_SoftTimer.soft_timer[OS_SoftTimer.cur_timer_cnt].start = OS_TIMER_DISABLE;
		
		if( timeout_func!=NULL )
		{
			OS_SoftTimer.soft_timer[OS_SoftTimer.cur_timer_cnt].time_cb = (os_time_callback)timeout_func;
			OS_SoftTimer.soft_timer[OS_SoftTimer.cur_timer_cnt].args = args;
		}
		else
		{
			OS_SoftTimer.soft_timer[OS_SoftTimer.cur_timer_cnt].time_cb= NULL;
			OS_SoftTimer.soft_timer[OS_SoftTimer.cur_timer_cnt].args = NULL;
		}
		
		*m_timer_obj = &OS_SoftTimer.soft_timer[OS_SoftTimer.cur_timer_cnt++];
		
		return OS_TIME_OK;
	}
	else if( mode >= OS_RT_ONE_SHOT_MODE && mode <= OS_RT_EVENT_MODE )
	{
		if( OS_SoftTimer.cur_rt_timer_cnt >= SOFT_RT_TIMER_COUNT_MAX )
			return OS_TIME_NO_MEM_ERR;
		
		OS_SoftTimer.soft_rt_timer[OS_SoftTimer.cur_rt_timer_cnt].mode = mode;
		OS_SoftTimer.soft_rt_timer[OS_SoftTimer.cur_rt_timer_cnt].start = OS_TIMER_DISABLE;
		
		if( timeout_func!=NULL )
		{
			OS_SoftTimer.soft_rt_timer[OS_SoftTimer.cur_rt_timer_cnt].time_cb = (os_time_callback)timeout_func;
			OS_SoftTimer.soft_rt_timer[OS_SoftTimer.cur_rt_timer_cnt].args = args;
		}
		else
		{
			OS_SoftTimer.soft_rt_timer[OS_SoftTimer.cur_rt_timer_cnt].time_cb= NULL;
			OS_SoftTimer.soft_rt_timer[OS_SoftTimer.cur_rt_timer_cnt].args = NULL;
		}
		
		*m_timer_obj = &OS_SoftTimer.soft_rt_timer[OS_SoftTimer.cur_rt_timer_cnt++];
		
		return OS_TIME_OK;
	}
	
	return OS_TIME_NO_MODE_ERR;
}	

time_retval_t soft_timer_start(soft_timer_t *m_timer_obj, uint32_t ms)
{
	if( m_timer_obj == NULL )
		return OS_TIME_HAND_ERR;
	
	if( ms == 0 )
		return OS_TIME_VALUE_ERR;
	
	if( m_timer_obj->mode >= OS_ONE_SHOT_MODE && m_timer_obj->mode <= OS_EVENT_MODE )
	{
		m_timer_obj->time_tick = TIME2TICK(ms) + OS_SoftTimer.soft_timer_tick;
		
	}
	else
	{
		m_timer_obj->time_tick = TIME2TICK(ms) + OS_SoftTimer.rt_timer_tick;
	}
	
	m_timer_obj->dest_tick = TIME2TICK(ms);
	m_timer_obj->start = OS_TIMER_ENABLE;
	
	return OS_TIME_OK;
}

time_retval_t soft_timer_stop(soft_timer_t *m_timer_obj)
{
	if( m_timer_obj == NULL )
		return OS_TIME_HAND_ERR;
	
	m_timer_obj->start = OS_TIMER_DISABLE;
	
	return OS_TIME_OK;
}

time_retval_t soft_timer_updata(soft_timer_t *m_timer_obj, uint32_t ms)
{
	if( m_timer_obj == NULL )
		return OS_TIME_HAND_ERR;
	
	if( ms == 0 )
		return OS_TIME_VALUE_ERR;
	
	if( m_timer_obj->start != OS_TIMER_ENABLE )
	  return OS_TIME_NOT_RUN_ERR;
	
	if( m_timer_obj->mode >= OS_ONE_SHOT_MODE && m_timer_obj->mode <= OS_EVENT_MODE )
	{
		m_timer_obj->time_tick = TIME2TICK(ms) + OS_SoftTimer.soft_timer_tick;
	}
	else
	{
		m_timer_obj->time_tick = TIME2TICK(ms) + OS_SoftTimer.rt_timer_tick;
	}
	
	m_timer_obj->dest_tick = TIME2TICK(ms);

	return OS_TIME_OK;
}


/********************** soft timer core **********************************/

void soft_timer_init(void)
{
	OS_SoftTimer.cur_rt_timer_cnt = 0;
	OS_SoftTimer.cur_timer_cnt    = 0;
	OS_SoftTimer.rt_timer_tick    = 0;
	OS_SoftTimer.soft_timer_tick  = 0;
	
	memset(OS_SoftTimer.soft_timer, 0, sizeof(soft_timer_t)*SOFT_TIMER_COUNT_MAX);
	memset(OS_SoftTimer.soft_rt_timer, 0, sizeof(soft_timer_t)*SOFT_RT_TIMER_COUNT_MAX);
	
	THREAD_SEM_CREAT(&OS_SoftTimer.OSSem, &thread_timer_process, 0);
	
	thread_os_register(&thread_timer_process, NULL);
	
	os_log("\r\nregister soft timer thread.....[ok]\r\n");
}

void thread_timer_tick(void)
{
	THREAD_SEM_POST(&OS_SoftTimer.OSSem);
	
	/* you can add hard timer */
	os_rt_timer_tick();
}

PROCESS_THREAD(thread_timer_process, ev, pdata)
{
	uint8_t ret;
	
	THREAD_BEGIN()
	
	while(1)
	{
		THREAD_SEM_PEND(&OS_SoftTimer.OSSem, &ret, 0);
		
		OS_SoftTimer.soft_timer_tick++;
		
		for( uint8_t i=0;i< OS_SoftTimer.cur_timer_cnt;i++ )
		{
			if( OS_SoftTimer.soft_timer[i].start == OS_TIMER_ENABLE )
			{
				if( OS_SoftTimer.soft_timer_tick == OS_SoftTimer.soft_timer[i].time_tick )
				{					
					if( OS_SoftTimer.soft_timer[i].time_cb != NULL )
					{
						#if false
						//os_time_callback m_time_cb;
						//m_time_cb = soft_timer[i].time_cb;
						//(*m_time_cb)(soft_timer[i].args);
						#else
						OS_SoftTimer.soft_timer[i].time_cb(OS_SoftTimer.soft_timer[i].args);
						#endif
					}
					
					if( OS_SoftTimer.soft_timer[i].mode == OS_ONE_SHOT_MODE )
					{
						OS_SoftTimer.soft_timer[i].start = OS_TIMER_DISABLE;
					}
					else
					{
						OS_SoftTimer.soft_timer[i].time_tick = OS_SoftTimer.soft_timer_tick + OS_SoftTimer.soft_timer[i].dest_tick;
					}	
				}
			}
		}//end for
	}//end while(1)
	
	THREAD_END()
}

/**************************no block timer API*****************************/

static void os_rt_timer_tick(void)
{
		OS_SoftTimer.rt_timer_tick++;
	
		for( uint8_t i=0;i< OS_SoftTimer.cur_rt_timer_cnt;i++ )
		{
			if( OS_SoftTimer.soft_rt_timer[i].start == OS_TIMER_ENABLE )
			{
				if( OS_SoftTimer.rt_timer_tick == OS_SoftTimer.soft_rt_timer[i].time_tick )
				{					
					if( OS_SoftTimer.soft_rt_timer[i].time_cb != NULL )
					{
						OS_SoftTimer.soft_rt_timer[i].time_cb(OS_SoftTimer.soft_rt_timer[i].args);
					}
					
					if( OS_SoftTimer.soft_rt_timer[i].mode == OS_RT_ONE_SHOT_MODE )
					{
						OS_SoftTimer.soft_rt_timer[i].start = OS_TIMER_DISABLE;
					}
					else
					{
						OS_SoftTimer.soft_rt_timer[i].time_tick = OS_SoftTimer.rt_timer_tick + OS_SoftTimer.soft_rt_timer[i].dest_tick;
					}
				}
			}
		}//end for
}
/*********************************************************************/


//note:thread-os v1.0 api func,v1.1 not use
#if false
uint8_t thread_delay(uint8_t handle)
{
	if( soft_timer[handle].is_timeout == 0x01 )
	{
		soft_timer[handle].is_timeout = 0;
		soft_timer[handle].counter = 0;
		
		return 0x01;
	}
	else
	{
		return 0;
	}
}
#endif

#endif
