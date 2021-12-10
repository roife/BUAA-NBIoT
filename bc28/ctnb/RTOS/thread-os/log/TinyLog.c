#include <stdarg.h>
#include <string.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#include "thread.h"
#include "bsp.h"

#ifndef THREAD_OS_LOG
#error "error:no define THREAD_OS_LOG, you must include 'thread.h' file "
#endif

#if THREAD_OS_LOG == 1u || THREAD_OS_LOG == 3u

static char buf_normal_up[BUFFER_SIZE_UP];
static char buf_down[BUFFER_SIZE_DOWN];

uint32_t log_rtt_init(void)
{
    static bool initialized = false;
    if (initialized)
    {
        return false;
    }

    if (SEGGER_RTT_ConfigUpBuffer(LOG_TERMINAL_NORMAL,
                                  "Normal",
                                  buf_normal_up,
                                  BUFFER_SIZE_UP,
                                  SEGGER_RTT_MODE_NO_BLOCK_TRIM
                                 )
        != 0)
    {
        return false;
    }

    if (SEGGER_RTT_ConfigDownBuffer(LOG_TERMINAL_INPUT,
                                   "Input",
                                   buf_down,
                                   BUFFER_SIZE_DOWN,
                                   SEGGER_RTT_MODE_NO_BLOCK_SKIP
                                  )
        != 0)
    {
        return false;
    }

    initialized = true;

    return true;
}
#endif

uint32_t log_read(void *buf, uint32_t len)
{
	  uint8_t ret=0;
	
		if( buf == NULL || len == 0 )
			return 0;
		
#if THREAD_OS_LOG == 0
		return 0;
#elif THREAD_OS_LOG == 1u
		ret=SEGGER_RTT_Read(0,buf,len);
#elif THREAD_OS_LOG == 2u
    ret=log_uart_read(buf,len);
#elif THREAD_OS_LOG == 3u
    ret=log_uart_read(buf,len);
		if( ret == 0 )
		{
			ret=SEGGER_RTT_Read(0,buf,len);
		}
#else
		return 0;
#endif
		
		return ret;
}

