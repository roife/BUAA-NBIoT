/**********************************************************************************************************
*                                                FlexShell
*
*                               (c) Copyright 2018-2019, tony(gulonglong)
*                                          All Rights Reserved
**   FlexShell is provided in source form for FREE evaluation, for educational use or for peaceful research.
*
* File    : FlexShell.h
* By      : tony(gulonglong)
* Version : V1.0
*
**********************************************************************************************************/

#ifndef __FLEXSHELL_H__
#define __FLEXSHELL_H__

#include <stdint.h>
#include "Csection.h"

#include "FlexShell_port.h"

#define  FLEXSHELL_VERSION      "V1.0.1"

/*******************************************************************************/

struct flexshell_cmd
{
	struct flexshell_cmd *next;
	void *cmd;
	char *help;
	void (*callback)(struct flexshell_cmd *, uint32_t, char **);
};
typedef struct flexshell_cmd flexshell_cmd_t;

#define FLEXSHELL_REGISTER_TABLE_START()  SECTION_DEF(FlexShellSection, flexshell_cmd_t);

#define FLEXSHELL_CMD_REGISTER(m_shell_cmd,cmd,help,callback) \
static void callback(flexshell_cmd_t *shell_cmd, uint32_t argc, char **args);\
static flexshell_cmd_t SECTION_ITEM_REGISTER(FlexShellSection, m_shell_cmd) = {NULL, cmd, help, callback};


void FlexShell_init(void *m_shell_read);

void FlexShell_thread_handle(void);

#endif
