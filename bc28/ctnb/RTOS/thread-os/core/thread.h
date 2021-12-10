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
* File    : thread.h
* By      : tony(gulonglong)
* Version : V1.3
*
*********************************************************************************************************
*/

#ifndef __THREAD_H__
#define __THREAD_H__

#include "TinyLog.h"

#include "os_config.h"
#include "os_core.h"
#include "os_time.h"
#include "os_sem.h"
#include "os_fifo.h"
#include "os_event.h"

#if defined(THREAD_OS_SOFT_TIMER_EN) && THREAD_OS_SOFT_TIMER_EN == 1
#include "os_soft_timer.h"
#endif

#if defined(THREAD_OS_SHELL_EN) && THREAD_OS_SHELL_EN == 1
#include "FlexShell_port.h"
#endif

/******************************* util ****************************************/

#define THREAD_CEIL_DIV(a, b)      (((a) + (b) - 1) / (b))
#define TIME2TICK(ms)              THREAD_CEIL_DIV((ms),(1000/THREAD_OS_TICKS_PER_SEC)) 
    
/******************************* process status ******************************/

#define THREAD_NOUSE               0
#define THREAD_RUN                 0x01u
#define THREAD_READY               0x02u
#define THREAD_SUSPEND             0x04u
#define THREAD_SUSPEND_TICK        0x08u
#define THREAD_SUSPEND_EVENT       0x10u
#define THREAD_STOP                0x20u

/******************************* event *****************************************/
#define THREAD_EVENT_NO            0
#define THREAD_EVENT_SET           0x01u
#define THREAD_EVENT_TIMEOUT       0x02u   
#define THREAD_EVENT_GET_OK        0x04u

/******************************* process variables ******************************/

typedef uint32_t process_event_t;
typedef void *        process_data_t;


struct process 
{
  struct process  *next;
	uint32_t tp;
	char *ThreadName;
	uint8_t status;
	uint8_t pid;
	uint32_t timetick;
	uint32_t RunTime;
	uint32_t event;
  void (* thread)(struct process *, process_event_t, process_data_t);
};
typedef struct process process_t;


extern process_t *process_list;

/********************************* functions **********************************/
														
void thread_os_register(process_t *p, const char *arg);
void thread_os_suspend(process_t *p);
void thread_os_resume(process_t *p);
void thread_os_stop(process_t *p);
void thread_os_restart(process_t *p);
void thread_os_delete(process_t *p);

process_t *get_thread_name(void *ThreadName);
process_t *get_thread(uint32_t pid);



/***************************************************************PT******************************************************************/

#define PROCESS_THREAD(name, ev, data) 		static void process_thread_##name(process_t *p, process_event_t ev,process_data_t data)

#define THREAD_INIT(name, namestr)				PROCESS_THREAD(name, ev, data);			                   \
																					static process_t name = { .next = NULL,                \
																						                        .tp = 0,                     \
																					                          .ThreadName = namestr,       \
																					                          .status = THREAD_NOUSE,      \
																					                          .pid = 0,                    \
																					                          .timetick = 0,               \
																						                        .RunTime = 0,                \
																						                        .event = THREAD_EVENT_NO,    \
																					                          .thread = process_thread_##name }
	
#define THREAD_VARIABLE                   static 
	
#define THREAD_BEGIN()               			volatile uint8_t THREAD_OS_YEILD=0;switch(p->tp){case 0:

#define THREAD_EXIT()                     p->tp=0;p->status=THREAD_STOP;return;
	
#define THREAD_END()                  		default:p->tp=0;p->status=THREAD_STOP;break;}

#define THREAD_SWITCH()                   return;
#define THREAD_YEILD(ThreadStatus)        p->status=ThreadStatus;return;                                     

#define THREAD_POST_EVENT(p)              do                                                            \
																				  {                                                             \
																						if( (p) != NULL )                                           \
																						{                                                           \
																								if( ((p)->status&THREAD_SUSPEND) != THREAD_SUSPEND )    \
																								{                                                       \
																									if( ((p)->status&THREAD_SUSPEND_EVENT) == THREAD_SUSPEND_EVENT ||  \
																										  ((p)->status&THREAD_SUSPEND_TICK) == THREAD_SUSPEND_TICK)      \
																										{                                                   \
																											(p)->event  = THREAD_EVENT_NO;                    \
																											(p)->status = THREAD_READY;                       \
																										}                                                   \
																								}                                                       \
																						}                                                           \
																					}while(0);


#define THREAD_WAIT_EVENT(event_hande, ret, delayms)   do                                \
																											 {                                  \
																												THREAD_OS_YEILD=1;                \
																												p->tp = __LINE__;case __LINE__:   \
																												if( event_hande )                 \
																												{                                 \
																													if( THREAD_OS_YEILD == 1 )      \
																													{                               \
																														if( delayms >0 )              \
																														{                             \
																															p->timetick = TIME2TICK(delayms) + \
																															os_time_get_timetick();     \
																															p->event = THREAD_EVENT_SET;  \
																															THREAD_YEILD(THREAD_SUSPEND_TICK);  \
																														}                             \
																														else                          \
																														{                             \
																															THREAD_YEILD(THREAD_SUSPEND_EVENT);  \
																														}                             \
																													}                               \
																													else                            \
																													{  /* ready by resume or timeout */       \
																														if( ret != NULL )             \
																														{                               \
																															if( (p->event&THREAD_EVENT_TIMEOUT) == THREAD_EVENT_TIMEOUT ) \
																															{                               \
																																*ret = THREAD_EVENT_TIMEOUT;  \
																															}                               \
																															else                            \
																															{                               \
																																*ret = THREAD_EVENT_GET_OK;   \
																															}                               \
																														}                                 \
																														p->event = THREAD_EVENT_NO;     \
																													}                               \
																												}                                 \
																												else                              \
																												{                                 \
																													if( ret != NULL )               \
																													{                               \
																														p->event = THREAD_EVENT_NO;   \
																														*ret = THREAD_EVENT_GET_OK;   \
																													}                               \
																													p->event = THREAD_EVENT_NO;     \
																												}                                 \
																											}while(0);              																				
	
#if THREAD_OS_TIME_EN == 1																						
#define THREAD_OS_DELAY(delayms)              do                                      \
																							{                                       \
																								  if(delayms >0)                      \
																									{                                   \
																										THREAD_OS_YEILD=1;                \
																										p->tp = __LINE__;case __LINE__:   \
																										if( THREAD_OS_YEILD == 1 )        \
																										{                                 \
																											p->timetick = TIME2TICK(delayms) + os_time_get_timetick(); \
																											p->status = THREAD_SUSPEND_TICK;\
																											return;                         \
																										}                                 \
																								  }                                   \
																							}while(0);  
#else
#define THREAD_OS_DELAY(delayms)
#endif


																							
																							
/***************************************PT END*************************************/												
																			
		
#endif
