#ifndef __TY_PRINTF_H__
#define __TY_PRINTF_H__

void ty_printf_init(void *write_cb);
uint32_t ty_printf(const char * sFormat, ...);

#endif
