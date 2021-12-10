#ifndef __BC28_THREAD_H__
#define __BC28_THREAD_H__


#define BC28_MODE_NON                         0x0000
#define BC28_MODE_NON_RELEASE                 0x0001
#define BC28_MODE_NON_RELEASE_AFTER_REPLY     0x0010
#define BC28_MODE_CON                         0x0100
#define BC28_MODE_CON_RELEASE_AFTER_REPLY     0x0101

#define BC28_SEND_MODE_DEFAULT    BC28_MODE_NON


void BC28_thread_init(void);

int8_t at_get_CON_result(void *thread);
	
#endif
