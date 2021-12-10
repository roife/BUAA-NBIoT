#include <string.h>
#include <stdint.h>

#include "bsp.h"
#include "thread.h"
#include "bsp_thread.h"
#include "nb_thread.h"
#include "app_aep_profile.h"
/*************************************variable********************/

void user_main(void)
{
	/*********** log init ****************/
	os_log_init();
	
	/************ platform init **********/
	platform_init();
	
	/*********** bsp init ****************/
	bsp_init();
	
	/************ thread-os start *******/
  thread_os_start();
}

void app_thread_init(void)
{
	bsp_thread_init();
	
	nb_thread_init();
	
	//app_profile_thread_init();
	aep_thread_init();
}










