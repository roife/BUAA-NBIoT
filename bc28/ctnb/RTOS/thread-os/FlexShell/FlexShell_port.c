/*
*********************************************************************************************************
*                                                FlexShell
*
*                               (c) Copyright 2018-2019, tony(gulonglong)
*                                          All Rights Reserved
**   FlexShellSystem is provided in source form for FREE evaluation, for educational use or for peaceful research.
*
* File    : FlexShell_port.c
* By      : tony(gulonglong)
* Version : V1.0
*
*********************************************************************************************************
*/

#include <string.h>
#include <stdlib.h>

#include "thread.h"
#include "FlexShell.h"

#define SHELL_SLEEP_TIME         50
#define SHELL_DELAY(time)        THREAD_OS_DELAY(time);
//#include "stm32F1xx_hal.h"


/********************************* thread init ******************************************/

THREAD_INIT(FlexShell_process, "FlexShell thread");


void FlexShell_port_init(void)
{
	/*if you use thread or task, you can init it in here.*/
	thread_os_register(&FlexShell_process, NULL);
	
	FlexShell_init(log_read);
}

PROCESS_THREAD(FlexShell_process, ev, pdata)
{
	THREAD_BEGIN()
	
	while(1)
	{
	  FlexShell_thread_handle();
		
		SHELL_DELAY(SHELL_SLEEP_TIME);
	}
	
	THREAD_END()
}

