/**********************************************************************************************************
*                                                FlexShell
*
*                               (c) Copyright 2018-2019, tony(gulonglong)
*                                          All Rights Reserved
**   FlexShell is provided in source form for FREE evaluation, for educational use or for peaceful research.
*
* File    : FlexShell_port.h
* By      : tony(gulonglong)
* Version : V1.0
*
**********************************************************************************************************/

#ifndef __FLEXSHELL_PORT_H__
#define __FLEXSHELL_PORT_H__

#include "TinyLog.h"
#define shell_log(...)           os_log(__VA_ARGS__)


void FlexShell_port_init(void);



#endif
