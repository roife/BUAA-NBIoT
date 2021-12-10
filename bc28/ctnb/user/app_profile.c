#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>

//RTOS
#include "thread.h"
#include "bsp.h"
#include "util.h"
#include "nb_thread.h"
#include "app_profile.h"


/******************************** define ************************************/
#define app_log(...)  m_log("\r\n[NB_log]");m_log(__VA_ARGS__);

#define SEND_DATA        "02"
#define RECEIVE_COMMAND  "06"
#define SEND_RESPONSE    "86"

/*********************************vriable*************************************/

THREAD_INIT(app_process, "app profile Thread");

// USER varibale
static BSP_status_t  BSP_status;

static soft_timer_t  *nb_timer_obj;
static unsigned int  reportPeriod = 0;
static bool          reportFlag   = false;

/******************************** functions ********************************/
static void nb_timer_cb(void *args);


static void send_sensor_data(void)
{
	char buff[32]={0,};
	uint32_t cmd       = 1;
	uint32_t dataSetID = 1;
	uint32_t dataLen   = 3;

	getStatus(BSP_ALL, &BSP_status);
	
	sprintf(buff,"%02x%04x%04x00%02x%02x",
	        cmd,
				  dataSetID,dataLen,
				 (BSP_status.led0_status<<4 | BSP_status.led1_status),
				  BSP_status.motor_status );
	
	m_log(buff);
	
	nb_send(buff, strlen(buff));
}

static void nb_timer_cb(void *args)
{
	static uint32_t i=0;
	char buff[32]={0,};
	uint8_t  cmd = 2;
	uint32_t humiditydataSetID = 2;
	uint32_t tempdataSetID = 3;
	uint32_t data_len = 4;
	char humidityData[10] = {0};
	char temperatureData[10] = {0};
	
	getStatus(HUM_TEMP, &BSP_status);
	
	htonf_hexstr1(BSP_status.hump, humidityData);
	htonf_hexstr1(BSP_status.temp, temperatureData);
	
	if( i++%2 == 0 )
	{
		sprintf(buff,"%02x%04x%04x%s", cmd, humiditydataSetID, data_len, humidityData);
		app_log(buff);
		nb_send(buff, strlen(buff));
		
		soft_timer_start(nb_timer_obj, 200);
	}
	else
	{
		sprintf(buff,"%02x%04x%04x%s", cmd, tempdataSetID, data_len, temperatureData);
		app_log(buff);
		nb_send(buff, strlen(buff));
		
		soft_timer_start(nb_timer_obj, reportPeriod-200);
	}
}

void decode_rev_data(char *data)
{
	char buff[64]={0,};
	char cmdTypeStr[3]    = {0};
	char datasetIDStr[5]  = {0};
	char taskIDStr[5]     = {0};
	char payloadLenStr[5] = {0};
	char byteValue[50]    = {0};
	char payload[50]      = {0};
	int payloadLen;
	uint32_t status;
	
	if(data == NULL)
		return;

	app_log("Start parsing cmd:%s",data);
	
	strncpy(cmdTypeStr, data, 2);
	strncpy(datasetIDStr, data + 2, 4);
	strncpy(taskIDStr, data + 6, 4);
	strncpy(payloadLenStr, data + 10, 4);
	
	app_log("cmdTypeStr:%s",   cmdTypeStr);
	app_log("datasetIDStr:%s", datasetIDStr);
	app_log("taskIDStr:%s",    taskIDStr);
	app_log("payloadLenStr:%s",payloadLenStr);
	
	if(strcmp(cmdTypeStr,RECEIVE_COMMAND) != 0)
		return;

	HexStr2byte2(payloadLenStr, (uint8_t *)byteValue, 2);
	payloadLen = byteValue[0] * 10 + byteValue[1];
	
	app_log("payloadLen:%d",payloadLen);
	
	strncpy(payload, data + 14, payloadLen * 2);
	app_log("payload:%s",payload);
	memset(byteValue, 0x00, sizeof(byteValue));
	//HexStr2byte2(payload, byteValue, payloadLen);
	
	if(strcmp(datasetIDStr,"1F41") == 0)//DatasetID = 8001
	{
		status = atoi((char *)payload);
		setStatus(MOTOR1 , status);
		strcpy(datasetIDStr, "2329");//DatasetID = 9001
		
		sprintf(buff, "%s%s%s%s%s", SEND_RESPONSE, datasetIDStr, taskIDStr, payloadLenStr, payload);
		app_log(buff);
		nb_send(buff, strlen(buff));
	}
	else if(strcmp(datasetIDStr,"1F42") == 0)//DatasetID = 8002
	{
		status = atoi((char *)payload);
		if(status == 0)
		{
			soft_timer_stop(nb_timer_obj);
			reportFlag = false;
		}
		else if(status == 1)
		{
			if(!reportFlag && reportPeriod != 0)
			{
				soft_timer_start(nb_timer_obj, reportPeriod);
				reportFlag = true;
			}
		}
		strcpy(datasetIDStr, "232A");//DatasetID = 9002
		
		sprintf(buff, "%s%s%s%s%s", SEND_RESPONSE, datasetIDStr, taskIDStr, payloadLenStr, payload);
		app_log(buff);
		nb_send(buff, strlen(buff));
	}
	else if(strcmp(datasetIDStr,"1F43") == 0)//DatasetID = 8003
	{
		reportPeriod = strtoul(payload, NULL, 16);
		reportPeriod *= 1000;
		
		app_log("Set reportPeriod: %d\n", reportPeriod);
		strcpy(datasetIDStr, "232B");//DatasetID = 9003
		
		sprintf(buff, "%s%s%s%s%s", SEND_RESPONSE, datasetIDStr, taskIDStr, payloadLenStr, payload);
		app_log(buff);
		nb_send(buff, strlen(buff));
	}
}
/**********************************************************************/

void app_profile_thread_init(void)
{
	thread_os_register(&app_process, NULL);
	
  soft_timer_register(&nb_timer_obj, OS_PERIODIC_MODE, nb_timer_cb, NULL);
	
	app_log("\r\nregister app thread.......[OK]\r\n");
}

PROCESS_THREAD(app_process, ev, pdata)
{
	uint8_t ret;
	
	THREAD_BEGIN()

	THREAD_WAIT_EVENT(1,&ret,0);
	
	while(1)
	{			
		
		//send_sensor_data();
		THREAD_OS_DELAY(3000);
	}	

	THREAD_END()
}
