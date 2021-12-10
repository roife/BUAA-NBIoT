/*
*********************************************************************************************************
*                                                FlexShell
*
*                               (c) Copyright 2018-2019, tony(gulonglong)
*                                          All Rights Reserved
**   FlexShell is provided in source form for FREE evaluation, for educational use or for peaceful research.
*
* File    : FlexShellSystem.c
* By      : tony(gulonglong)
* Version : V1.0
*
** Example: 
*
*         #include "FlexShell.h"
*
*         FLEXSHELL_REGISTER_BEGIN()
*
*        // register table
*         FLEXSHELL_CMD_REGISTER
*				  ( 
*						led_cmd,
*						"$test",
*						"$test:  test function\r\n",
*						test_cmd_func
*					)
*
*        // functions
*         static void test_cmd_func(flexshell_cmd_t *shell_cmd, uint32_t argc, char **args);
*					{
*         	if( argc == 1 )
*	          {
*             shell_log("%s", shell_cmd->cmd_str);
*           }
*           else
*           {
*             //add user code
*
*           }
*
*         }
*********************************************************************************************************
*/

#include <string.h>
#include <stdlib.h>
#include <time.h>

#include "thread.h"
#include "FlexShell.h"

/***************************************** register table *****************************************/

FLEXSHELL_REGISTER_TABLE_START()

FLEXSHELL_CMD_REGISTER
( 
	help_cmd,
	"?",
	"?: dispaly current cmd\r\n",
	do_show_help
)

FLEXSHELL_CMD_REGISTER
( 
	help_cmd2,
	"help",
	"help: dispaly current cmd\r\n",
	do_show_help
)

FLEXSHELL_CMD_REGISTER
( 
	sys_reset_cmd,
	"reset",
	"reset: restart system\r\n",
	sys_reset
)

FLEXSHELL_CMD_REGISTER
(
	ps_cmd,
	"ps",
	"ps: display current user thread\r\n",
	ps_func
)

FLEXSHELL_CMD_REGISTER
(
	suspend_cmd,
	"suspend",
	"suspend:<x>  x:suspend pid\r\n",
	suspend_func
)

FLEXSHELL_CMD_REGISTER
(
	resume_cmd,
	"resume",
	"resume:<x>  x:resume pid\r\n",
	resume_func
)

FLEXSHELL_CMD_REGISTER
(
	restart_cmd,
	"restart",
	"restart:<x>  x:restart pid\r\n",
	restart_func
)

FLEXSHELL_CMD_REGISTER
(
	clear_cmd,
	"clear",
	"clear:   Clear screen, reposition cursor to top left\r\n",
	clr_dis_func
)

FLEXSHELL_CMD_REGISTER
(
	SetTime_cmd,
	"settime",
	"settime:<x>  x:(decimal numbers)unix(posix)timestamp\r\n",
	set_timestamp
)

FLEXSHELL_CMD_REGISTER
(
	getTime_cmd,
	"gettime",
	"gettime:   display unix(posix)timestamp and UTC time\r\n",
	get_time
)

/************************************ functions ************************************/

static void do_show_help(flexshell_cmd_t *shell_cmd, uint32_t argc, char **args)
{
	uint8_t i = 0;
	flexshell_cmd_t *q = (flexshell_cmd_t *)SECTION_START_ADDR(FlexShellSection);
	
	shell_log("\r\n");
	for( i=0;i<SECTION_LENGTH(FlexShellSection,flexshell_cmd_t);i++)
	{
		shell_log("%s",q[i].help);
	}
}

/********************************** functions body **********************************/

static void get_time(flexshell_cmd_t *shell_cmd, uint32_t argc, char **args)
{
	time_t     timestamp;
	struct tm  *calendar;
	
	timestamp = os_time_get_timetick()/THREAD_OS_TICKS_PER_SEC;
	
	shell_log("timestamp:%d\r\n", timestamp);
	calendar = localtime(&timestamp);
	
	shell_log("GMT calendar time:%s\r\n", asctime(calendar));
	
	timestamp += 8*3600;
	shell_log("BeiJing calendar time:%s\r\n", ctime(&timestamp));
}

static void set_timestamp(flexshell_cmd_t *shell_cmd, uint32_t argc, char **args)
{
	uint64_t timestamp;
	
	if( argc == 0x01 )
	{
		shell_log("%s", shell_cmd->help);
	}
	else if( argc == 0x02 )
	{
		timestamp = atol(args[1]);
		if( timestamp != 0 )
		{
			shell_log("timestamp:%d\r\n", timestamp);
			os_time_set_timetick(timestamp*THREAD_OS_TICKS_PER_SEC);
			shell_log("timestamp:%d\r\n", os_time_get_timetick()/THREAD_OS_TICKS_PER_SEC);
		}
		else
		{
			shell_log("timestamp format err\r\n");
		}
	}
}

static void clr_dis_func(flexshell_cmd_t *shell_cmd, uint32_t argc, char **args)
{
	//shell_log("%s%s%s",LOG_CTRL_CLEAR,LOG_CTRL_CLEAR,LOG_CTRL_CLEAR);
}

#include "stm32f1xx_hal.h"
static void sys_reset(flexshell_cmd_t *shell_cmd, uint32_t argc, char **args)
{	
	HAL_NVIC_SystemReset();
}

static void ps_func(flexshell_cmd_t *shell_cmd, uint32_t argc, char **args)
{
	process_t *q = process_list;
	
	shell_log("\r\n*********************************************************************************\r\n");
	shell_log("                          thread-os thread status\r\n\r\n");
	shell_log("      [PID   ]: this is the thread NO.. you can use it to operate the user thread \r\n");
	shell_log("      [STATUS]: 2 (READY),\r\n");
	shell_log("                ");
	shell_log("4 (SUSPEND),\r\n");
	shell_log("                ");
	shell_log("8 (SUSPEND_TICK),\r\n");
	shell_log("                ");
	shell_log("10(THREAD_SUSPEND_EVENT),\r\n");
	shell_log("                ");
	shell_log("16(SUSPEND_TICK|THREAD_SUSPEND_EVENT),\r\n");
	shell_log("                ");
	shell_log("11(STOP)\r\n\r\n");
	
	shell_log("---------------------------------------------------");
	shell_log("\r\n    [thread]       [PID][STATUS]   [RUNTIME]\r\n");
	shell_log("---------------------------------------------------\r\n");
	
	do
	{
		if(strlen(q->ThreadName) <20)
		{
			shell_log("%s",q->ThreadName);
			
			for(uint8_t i=0;i<20-strlen(q->ThreadName);i++)
				shell_log(" ");
			
			shell_log("[%02d]    [%02d]    [%d]\r\n",q->pid, q->status, THREAD_GET_RUNTIME(q));
		}
		else
		{
			shell_log("%s  [%02d]    [%02d]    [%d]\r\n",q->ThreadName, q->pid, q->status, THREAD_GET_RUNTIME(q));
		}
		
		q = q->next;
	}while( q != process_list );
	
	shell_log("\r\n**********************************************************************************\r\n");
}


static void suspend_func(flexshell_cmd_t *shell_cmd, uint32_t argc, char **args)
{
	process_t *op_td=NULL;
	uint32_t pid;
	
	if( argc == 0x01 )
	{
		shell_log("%s", shell_cmd->help);
	}
	else if( argc == 0x02 )
	{
		pid = atoi(args[1]);
		
		if( pid != 0 )
		{
			op_td = get_thread(pid);
		}
		else
		{
			op_td = get_thread_name(args[1]);
		}
		
		if( op_td != NULL )
		{
			thread_os_suspend(op_td);
		}
		else
		{
			shell_log("[%s] is err\r\n", args[1]);
		}
	}
	else
	{
		shell_log("\r\nerror:invalid cmd\r\n");
	}
}

static void resume_func(flexshell_cmd_t *shell_cmd, uint32_t argc, char **args)
{
	process_t *op_td=NULL;
	uint32_t pid;
	
	if( argc == 0x01 )
	{
		shell_log("%s", shell_cmd->help);
	}
	else if( argc == 0x02 )
	{
		pid = atoi(args[1]);
		
		if( pid != 0 )
		{
			op_td = get_thread(pid);
		}
		else
		{
			op_td = get_thread_name(args[1]);
		}
		
		if( op_td != NULL )
		{
			if( strcmp(op_td->ThreadName,"shell thread") == 0 )
				return;
						
			thread_os_resume(op_td);
		}
		else
		{
			shell_log("[%s] is err\r\n", args[1]);
		}
	}
	else
	{
		shell_log("\r\nerror:invalid cmd\r\n");
	}
}

static void restart_func(flexshell_cmd_t *shell_cmd, uint32_t argc, char **args)
{
	process_t *op_td=NULL;
	uint32_t pid;
	
	if( argc == 0x01 )
	{
		shell_log("%s",shell_cmd->help);
	}
	else if( argc == 0x02 )
	{
		pid = atoi(args[1]);
		
		if( pid != 0 )
		{
			op_td = get_thread(pid);
		}
		else
		{
			op_td = get_thread_name(args[1]);
		}
		
		if( op_td != NULL )
		{
			if( strcmp(op_td->ThreadName,"shell thread") == 0 )
				return;
						
			thread_os_restart(op_td);
		}
		else
		{
			shell_log("[%s] is err\r\n", args[1]);
		}
	}
	else
	{
		shell_log("\r\nerror:invalid cmd\r\n");
	}
}


