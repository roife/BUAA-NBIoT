/**********************************************************************************************************
*                                                THREAD-OS
*                                          The Real-Time Kernel
*                                  THREAD-OS Configuration File for V1.x
*
*                               (c) Copyright 2018-2019, tony(gulonglong)
*                                          All Rights Reserved
**   THREAD-OS is provided in source form for FREE evaluation, for educational use or for peaceful research.
*
* File    : os_core.c
* By      : tony(gulonglong)
* Version : V1.3
*
*********************************************************************************************************
*/
#include "thread.h"

uint8_t  os_running = 0;
uint32_t thread_use_tick = 0;
process_t *current_thread = NULL;


void __weak app_thread_init(void)
{
	os_log("app_thread_init is weak, not find app thread\r\n");
}

static void thread_os_init(void)
{
	os_log("\r\n***********thread-os init **********\r\n");
	
	os_log("\r\nthread-os version:%s\r\n",THREAD_OS_VERSION);
	
	os_log("\r\nsystem thread:\r\n");
	#if THREAD_OS_SOFT_TIMER_EN == 1
	soft_timer_init();
	#endif
	
	#if THREAD_OS_SHELL_EN == 1
	FlexShell_port_init();
	#endif
	
	os_log("\r\nuser thread:\r\n");
	app_thread_init();
}

void thread_os_start(void)
{	
	thread_os_init();
	
	current_thread = process_list;
	
	os_log("\r\n***********thread-os start*********\r\n");
	
	if( current_thread == NULL )
	{
		os_log("\r\n***********no thread to running*********\r\n");
		while(1){};
	}
	
	os_running = 1;
	
	for(;;)
	{
		if( current_thread->status == THREAD_READY )
		{
			//current_thread->status = THREAD_RUN;
			
			thread_use_tick = os_time_get_timetick();
			current_thread->thread(current_thread,NULL,NULL);
			
			thread_use_tick = ((int32_t)(os_time_get_timetick() - thread_use_tick) >= 0)?(os_time_get_timetick() - thread_use_tick):(0xffffffff-thread_use_tick+os_time_get_timetick());
			//os_log("-----%d-----\r\n",thread_use_tick);
			
			if( thread_use_tick > current_thread->RunTime ) 
      {
				current_thread->RunTime = thread_use_tick; 
			}				
			
			//current_thread->status = THREAD_READY;
		}
		
		current_thread = current_thread->next;
	}
}


