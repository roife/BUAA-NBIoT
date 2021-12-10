#ifndef __TINY_LOG_H__
#define __TINY_LOG_H__

#include <stdio.h>
#include <stdint.h>
#include <stdarg.h>

#include "os_config.h"

#ifndef THREAD_OS_LOG
#error "error:no define THREAD_OS_LOG, you must include 'thread.h' file "
#endif

#if THREAD_OS_LOG == 1u || THREAD_OS_LOG == 3u

#include <SEGGER_RTT_Conf.h>
#include <SEGGER_RTT.h>

#define LOG_TERMINAL_NORMAL         (0)
#define LOG_TERMINAL_ERROR          (1)
#define LOG_TERMINAL_INPUT          (0)

#define LOG_CTRL_RESET         "\x1B[0m"         // Reset to default colors
#define LOG_CTRL_CLEAR         "\x1B[2J"         // Clear screen, reposition cursor to top left

#define LOG_COLOR_DEFAULT  "\x1B[0m"
#define LOG_COLOR_BLACK    "\x1B[1;30m"
#define LOG_COLOR_RED      "\x1B[1;31m"
#define LOG_COLOR_GREEN    "\x1B[1;32m"
#define LOG_COLOR_YELLOW   "\x1B[1;33m"
#define LOG_COLOR_BLUE     "\x1B[1;34m"
#define LOG_COLOR_MAGENTA  "\x1B[1;35m"
#define LOG_COLOR_CYAN     "\x1B[1;36m"
#define LOG_COLOR_WHITE    "\x1B[1;37m"

#elif THREAD_OS_LOG == 2u

#include "TinyPrintf.h"

#else

#endif

#if THREAD_OS_LOG == 1u

#define os_log(...)             SEGGER_RTT_printf(LOG_TERMINAL_NORMAL, ##__VA_ARGS__)
#define os_log_init()           log_rtt_init()

#elif THREAD_OS_LOG == 2u

#define os_log(...)             ty_printf(__VA_ARGS__)
#define os_log_init()           log_uart_init(115200)

#elif THREAD_OS_LOG == 3u

#include "ty_printf.h"

#define os_log(...)             {SEGGER_RTT_printf(LOG_TERMINAL_NORMAL, ##__VA_ARGS__);ty_printf(__VA_ARGS__);}
#define os_log_init()           {log_rtt_init();log_uart_init(115200);}

#else

#define os_log(...)
#define os_log_init()

#endif

uint32_t log_read(void *buf, uint32_t len);
uint32_t log_rtt_init(void);

#endif
