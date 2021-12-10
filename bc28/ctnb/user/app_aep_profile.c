#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>

//RTOS
#include "thread.h"
#include "bsp.h"
#include "util.h"
#include "nb_thread.h"
#include "AepServicecodes.h"

/**************************** define ****************************************/

#define app_log(...)  os_log("\r\n[app_log]");os_log(__VA_ARGS__);

/*********************************vriable*************************************/

THREAD_INIT(app_process, "aep Thread");

// USER varibale
static BSP_status_t  BSP_status;

static soft_timer_t  *nb_timer;
static soft_timer_t  *nb_warning_timer;
static unsigned int  reportPeriod = 3000;
static bool          reportFlag = false;
static bool          warningFlag = false;

/******************************** functions ********************************/
static void nb_timer_cb(void *args);
static void nb_warning_timer_cb(void *args);

static void send_data_asy_callback(uint8_t result, uint32_t id)
{
	app_log("send syn ack, result:%d,messageID:%d\r\n", result, id);
	
	switch( result )
	{
		case SEND_SYN_RSP_SUCCESS:
			break;
		
		case SEND_SYN_RSP_TIMEOUT:
			break;
		
		case SEND_SYN_RSP_SENDFAILED:
			break;
		
		case SEND_SYN_RSP_RESET_MESSAGE:
			break;
		
		case SEND_SYN_RSP_WAITING:
			break;
		
		case SEND_SYN_RSP_NOTSEND:
			break;
		
		default:
			app_log("invalid syn ack!\r\n");
			break;
	}
}

static void send_data_syn_callback(void *msg)
{
	msg_data_t *m_msg = msg;

	if( m_msg == NULL )
		return;
	
	if( m_msg->result == SEND_ASY_RSP_SUCCESS )
	{
		app_log("send asy ack, messageID:%d\r\n",m_msg->messageID);
	}
	else if( m_msg->result == SEND_ASY_RSP_FAIL )
	{
		if( m_msg->messageID != 0 )
		{
			app_log("send error, error id:%d\r\n",m_msg->messageID);
		}
		else
		{
			app_log("send error\r\n");
		}
	}
	else if( m_msg->result == SEND_ASY_RSP_TIMEOUT )
	{
		app_log("send error, mudule ack timeout\r\n");
	}
	else if( m_msg->result ==  SEND_ASY_RSP_SENDING )
	{
		app_log("last msg(id=%d) in sending,not send CON or NON data\r\n",m_msg->messageID);
	}
	
	if( m_msg->mode == SEND_MODE_NON || m_msg->mode == SEND_MODE_NON_RELEASE || m_msg->mode == SEND_MODE_NON_RELEASE_AFTER_REPLY)
	{
		
	}
	else if( m_msg->mode == SEND_MODE_CON || m_msg->mode == SEND_MODE_CON_RELEASE_AFTER_REPLY )
	{
		
	}
	
	//user can save messageID
	free(m_msg);
}

static int8_t send_msg(char *data, uint32_t len, uint8_t mode)
{
	int8_t ret;
	msg_data_t *msg;
	
	if( data == NULL || strlen(data) != len )
	{
		app_log("nb send params error:%d,%d\r\n",strlen(data),len);
		return -1;
	}
	
	msg = malloc(sizeof(msg_data_t));
	if( msg != NULL )
	{
		memcpy(msg->send_buff, data, len+1); 
		msg->mode = mode;
		msg->len = len;
		msg->callback = send_data_syn_callback;
		
		ret = nb_send(msg);
		if( ret < 0 )
		{
			free(msg);
		}
	}
	else
	{
		app_log("malloc error\r\n");
		ret = -1;
	}
	
	return ret;
}

/*************************************************************************************/

static void send_sensor_data(void)
{
	int8_t ret;
	sensor_report reportFlag;
	AepString      reportstr;
	
	getStatus(BSP_ALL, &BSP_status);
	
	//fill sensor data
	reportFlag.sensor_data[0] = BSP_status.IR_status;
	reportFlag.sensor_data[1] = BSP_status.led0_status;
	reportFlag.sensor_data[2] = BSP_status.motor_status;
	
	reportstr = codeDataReportByIdentifierToStr("sensor_report", &reportFlag);
	app_log(reportstr.str);
	
  ret = send_msg(reportstr.str, reportstr.len, SEND_MODE_CON);
	if( ret < 0 )
	{
		app_log("send sensor data err\r\n");
	}
	
	free(reportstr.str);
}

static void nb_timer_cb(void *args)
{
	int8_t ret;
	
	getStatus(BSP_ALL, &BSP_status);
	
	ths_data reportFlag;
	AepString      reportstr;
		
	reportFlag.temperature_data = ((float)((int)(BSP_status.temp*10)))/10;
	reportFlag.humidity_data = BSP_status.hump;
	reportFlag.sensor_data[0] = BSP_status.IR_status;
	reportFlag.sensor_data[1] = BSP_status.led0_status;
	reportFlag.sensor_data[2] = BSP_status.motor_status;
	reportstr = codeDataReportByIdentifierToStr("ths_data", &reportFlag);
	app_log(reportstr.str);
	
  ret = send_msg(reportstr.str, reportstr.len, SEND_MODE_CON);
	if( ret < 0 )
	{
		app_log("send ths data err\r\n");
	}
	free(reportstr.str);
		
	soft_timer_start(nb_timer, reportPeriod);
}

static void nb_warning_timer_cb(void *args)
{
	getStatus(BSP_ALL, &BSP_status);
	
	if (BSP_status.led0_status == LED_OFF) {
		setStatus(LED0_DEVICE_ID,LED_RED);
	} else {
		setStatus(LED0_DEVICE_ID,LED_OFF);
	}

	soft_timer_start(nb_warning_timer, 1000);
}

void decode_aep_data(char *data)
{
	int8_t ret;
	AepCmdData result;
	
	result = decodeCmdDownFromStr(data);
	if( result.code == AEP_CMD_SUCCESS )
	{
		if( strcmp(result.serviceIdentifier, "motor_control") == 0 )
		{
			motor_control_ack ack;
			AepString rsp;
		  uint8_t value;
			
			value = *((uint8_t *)result.data);
			app_log("get value:%d\r\n",value);
			if( value <= 1 )
			{
				setStatus(MOTOR1 , value);
			}
			
			ack.control_int = 1;
			ack.taskId      = result.taskId;
			rsp = codeDataReportByIdentifierToStr("motor_control_ack", &ack);
			ret = send_msg(rsp.str, rsp.len, SEND_MODE_CON);
			if( ret < 0 )
			{
				app_log("send motor_control_ack err\r\n");
			}
			free(rsp.str);
		}
		else if( strcmp(result.serviceIdentifier, "report_period_set") == 0 )
		{
			report_period_set_ack ack;
			AepString rsp;
		  uint8_t value;
			
			value = *((uint8_t *)result.data);
			app_log("get value:%d\r\n",value);
			if( value <= 100 )
			{
				reportPeriod = value*1000;
				app_log("Set reportPeriod: %d\n", reportPeriod);
			}
			
			ack.set_int = 1;
			ack.taskId      = result.taskId;
			rsp = codeDataReportByIdentifierToStr("report_period_set_ack", &ack);
      ret = send_msg(rsp.str, rsp.len, SEND_MODE_CON);
			if( ret < 0 )
			{
				app_log("send report_period_set_ack err\r\n");
			}
			free(rsp.str);
		}
		else if( strcmp(result.serviceIdentifier, "report_control") == 0 )
		{
			report_control_ack ack;
			AepString rsp;
		  uint8_t value;
			
			value = *((uint8_t *)result.data);
			app_log("get value:%d\r\n",value);
			if(value == 0)
			{
				app_log("stop time\r\n");
				soft_timer_stop(nb_timer);
				reportFlag = false;
			}
			else if(value == 1)
			{
				if( reportFlag == false && reportPeriod != 0)
				{
					app_log("start time\r\n");
					soft_timer_start(nb_timer, reportPeriod);
					reportFlag = true;
				}
			}
			
			ack.control_int = 1;
			ack.taskId      = result.taskId;
			rsp = codeDataReportByIdentifierToStr("report_control_ack", &ack);
      		ret = send_msg(rsp.str, rsp.len, SEND_MODE_CON);
			if( ret < 0 )
			{
				app_log("send report_control_ack err\r\n");
			}
			free(rsp.str);
		} else if( strcmp(result.serviceIdentifier, "motor_led0") == 0 ) {
			motor_led0_ack ack;
			AepString rsp;
		  	uint8_t value;
			
			value = *((uint8_t *)result.data);
			app_log("get value:%d\r\n",value);
			if(value == 0)
			{
				app_log("stop warning\r\n");
				setStatus(MOTOR1 , 0);
				soft_timer_stop(nb_warning_timer);
				warningFlag = false;
			}
			else if(value == 1)
			{
				if( reportFlag == false)
				{
					app_log("start warning\r\n");
					setStatus(MOTOR1 , 1);
					soft_timer_start(nb_warning_timer, 1000);
					warningFlag = true;
				}
			}
			
			ack.control_int = 1;
			ack.taskId      = result.taskId;
			rsp = codeDataReportByIdentifierToStr("motor_led0_ack", &ack);
      		ret = send_msg(rsp.str, rsp.len, SEND_MODE_CON);
			if( ret < 0 )
			{
				app_log("send motor_led0_ack err\r\n");
			}
			free(rsp.str);
		}
		
	}
	
	if( result.data != NULL )
	{
		free(result.data);
	}
}

/**********************************************************************/

void aep_thread_init(void)
{
	thread_os_register(&app_process, NULL);
	
  soft_timer_register(&nb_timer, OS_PERIODIC_MODE, nb_timer_cb, NULL);

  soft_timer_register(&nb_warning_timer, OS_PERIODIC_MODE, nb_warning_timer_cb, NULL);
	
	nb_register_syn_ack(send_data_asy_callback);
	
	app_log("\r\nregister app thread.......[OK]\r\n");
}

PROCESS_THREAD(app_process, ev, pdata)
{
	int8_t ret;
	static uint8_t IR_status = true;
	
	THREAD_BEGIN()
	
	getStatus(IR, &BSP_status);
  IR_status = BSP_status.IR_status;
	if( IR_status == 1 )
	{
		setStatus(LED0_DEVICE_ID,LED_BLUE);
	}
	else
	{
		setStatus(LED0_DEVICE_ID,LED_RED);
	}
				
	while(1)
	{
	#if 1
		getStatus(KEY2, &BSP_status);
		if( BSP_status.key_value == 0 )
		{			
			THREAD_OS_DELAY(30);
			
			getStatus(KEY2, &BSP_status);
			if( BSP_status.key_value == 0)
			{
				os_log("key2 pull down\r\n");
				
				do
				{
					getStatus(KEY2, &BSP_status);
					if( BSP_status.key_value == 1)
					{
						THREAD_OS_DELAY(30);
						
						getStatus(KEY2, &BSP_status);
						if( BSP_status.key_value == 1)
						{
							os_log("key2 pull up\r\n");
							setStatus(MOTOR1 , 0);
							soft_timer_stop(nb_warning_timer);
							break;
						}
					}
					THREAD_OS_DELAY(30);
				}while(1);
			}
		}
	#endif
		
		//IR
		getStatus(IR, &BSP_status);
		if( BSP_status.IR_status != IR_status )
		{			
			THREAD_OS_DELAY(500);
			
			getStatus(IR, &BSP_status);
			if( BSP_status.IR_status != IR_status)
			{
				os_log("IR status change\r\n");
				if( BSP_status.IR_status == 1 )
				{
					setStatus(LED0_DEVICE_ID,LED_BLUE);
				}
				else
				{
					setStatus(LED0_DEVICE_ID,LED_RED);
				}
				IR_status = BSP_status.IR_status;
				send_sensor_data();
			}
		}
		nb_timer_cb(NULL);
		THREAD_OS_DELAY(200);
	}

	THREAD_END()
}



