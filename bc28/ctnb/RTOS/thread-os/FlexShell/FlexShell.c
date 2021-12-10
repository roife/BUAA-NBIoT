/***********************************************************************************************************
*                                                FlexShell
*
*                               (c) Copyright 2018-2019, tony(gulonglong)
*                                          All Rights Reserved
**   FlexShell is provided in source form for FREE evaluation, for educational use or for peaceful research.
*
* File    : FlexShell.c
* By      : tony(gulonglong)
* Version : V1.0
*
**********************************************************************************************************/

#include <string.h>
#include <stdint.h>
#include <stdlib.h>

#include "FlexShell.h"

#include "util.h"


SECTION_DEF(FlexShellSection, flexshell_cmd_t);

typedef uint32_t (*FlexShell_read_cb)(void *, uint32_t);
static FlexShell_read_cb  shell_read_cb = NULL;

static uint8_t shell_recv_buf[512]={0,};
static uint8_t shell_recv_len=0;


//------------------------------------------------------------------------
void FlexShell_init(void *shell_read)
{
	/*register shell read callback*/
	shell_read_cb = (FlexShell_read_cb)shell_read;
	
	shell_log("\r\nFlexShell start, soft verison:%s....[ok]\r\n",FLEXSHELL_VERSION);
}

//------------------------------------------------------------------------
static void do_shell_cmd(const char *buf, uint32_t length)
{
	uint8_t i = 0;
	args_block_t m_args_block={0,};
	flexshell_cmd_t *q = (flexshell_cmd_t *)SECTION_START_ADDR(FlexShellSection);
	
	//m_strtok((char *)buf, m_args_block.args, &m_args_block.argc);
	t_strtok((char *)buf, " ,", m_args_block.args, &m_args_block.argc);
	
	for( i=0;i<SECTION_LENGTH(FlexShellSection,flexshell_cmd_t);i++)
	{
		if( strcmp(q[i].cmd, m_args_block.args[0]) == 0 )
		{
			if( q[i].callback != NULL )
			{
				q[i].callback(&q[i], m_args_block.argc, m_args_block.args);
				return;
			}
		}
	}
}

//--------------------------------------------------------------------
/* user call this function to run the FlexShell in the FlexShell_port.c file
explame :

void thread_hadle(void *pdata)
{
	while(1)
	{
	  FlexShell_thread_handle();
		
		SHELL_DELAY(SHELL_SLEEP_TIME);
	}
}

*/

	
void FlexShell_thread_handle(void)
{	
	if( ( shell_recv_len=shell_read_cb(shell_recv_buf,sizeof(shell_recv_buf)) ) != 0 )
	{
		do_shell_cmd((const char *)shell_recv_buf,shell_recv_len);
	}
}


