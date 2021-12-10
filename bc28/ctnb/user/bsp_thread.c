#include <string.h>
#include <stdint.h>

#include "thread.h"
#include "TinyLog.h"
#include "bsp.h"


/*************************************variable********************/

THREAD_INIT(bsp_process, "BSP thread");

static float temp;
static uint16_t hum;
/*************************************functions*******************/

void bsp_thread_init(void)
{
	thread_os_register(&bsp_process, NULL);
	
	os_log("\r\nstart BSP thread .....[ok]\r\n");
}

void get_hum_temp(float *ptemp, uint16_t *phum)
{
	*ptemp = temp;
  *phum = hum;	
}


PROCESS_THREAD(bsp_process, ev, pdata)
{
  HTS221_StatusTypeDef ret;
	static int32_t i=0;
	
	THREAD_BEGIN()
	
	print_all_reg();
	
	setStatus(LED1_DEVICE_ID, LED_RED);
	
	i = 3;
	do
	{
		ret = HTS221_Calibration();
		if( ret != HTS221_OK )
		{
			os_log("HTS221 calibration err:%d\r\n",ret);
		}
		else
		{
			break;
		}
		
		THREAD_OS_DELAY(3000);
	}while(i--);
	
	if( i<= 0 )
	{
		HAL_NVIC_SystemReset();
	}
		
	while(1)
	{ 
			ret = HTS221_GetTemperature(&temp);
//		  if( ret != HTS221_OK )
//			{
//				os_log("HTS221 get temperature err:%d\r\n",ret);
//			}
//			else
//			{
//				os_log("temperature:%d\r\n",temp);
//			}
			
			ret = HTS221_GetHumidity(&hum);
//		  if( ret != HTS221_OK )
//			{
//				os_log("HTS221 get Humidity err:%d\r\n",ret);
//			}
//			else
//			{
//				os_log("Humidity:%d\r\n",hum);
//			}
			
			setStatus(LED1_DEVICE_ID, LED_OFF);
			THREAD_OS_DELAY(500);
			setStatus(LED1_DEVICE_ID, LED_GREEN);
			THREAD_OS_DELAY(500);
	}
		
  THREAD_END()
}


