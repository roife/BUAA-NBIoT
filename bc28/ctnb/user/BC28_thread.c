#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>

//RTOS
#include "thread.h"
#include "app_uart.h"
#include "util.h"
#include "fifo.h"
#include "nb_thread.h"
#include "BC28_thread.h"

/******************************* define  **************************************/

#define nb_log(...)         os_log("\r\n[NB_log]");os_log(__VA_ARGS__);
#define NB_APP_BUF_LEN      1024
#define NB_FIFO_RAM_SIZE    1024

/********************************* thread init *********************************/

THREAD_INIT(BC28_init_process, "BC28 init Thread");

THREAD_INIT(BC28_event_process, "BC28 event Thread");

/********************************AT object**************************************/

static void   at_process_URC(void *args);
static void*  at_recv_data(char *data);
static int8_t at_send_data(void *data);

/******************** os event *******************************/

#define BC28_EVENT_CTM2MEVENT   0x0020
#define BC28_EVENT_SYSTEMEVENT  0x0040
#define BC28_EVENT_LOGIN        0x0080
#define BC28_EVENT_LOGOUT       0x0100

//os event
os_event_t  BC28_event;
static os_event_t  BC28_recv_event;

/***********************************************************************************/

/*  BC28 module  */ 
at_module_t at_module_BC28={
	BC28,              //module
	"BC28",            //module name
	0,                 //lock
	0,                 //object 19/0/0 send data enable (0:close 1: open)
	&BC28_init_process,//thread
	&BC28_event,       //event
	
	{
		/* ID          at string         return data       OK    ERROR          req       result               callback     req thread*/
		//3GPP
    AT_AT,        "AT",              "",              "OK", "ERROR",   0,   {0, AT_RSP_ERROR, ""},         at_set_rsp,  NULL,
		AT_ATE0,      "ATE0",            "",              "OK", "ERROR",   0,   {0, AT_RSP_ERROR, ""},         at_set_rsp,  NULL,
		AT_ATE1,      "ATE1",            "",              "OK", "ERROR",   0,   {0, AT_RSP_ERROR, ""},         at_set_rsp,  NULL,
    AT_CMEE,      "AT+CMEE",         "",              "OK", "ERROR",   0,   {0, AT_RSP_ERROR, ""},         at_set_rsp,  NULL,
		AT_CPSMS,     "AT+CPSMS",        "",              "OK", "ERROR",   0,   {0, AT_RSP_ERROR, ""},         at_set_rsp,  NULL,
	  AT_CEREG,     "AT+CEREG",        "+CEREG:",       "OK", "ERROR",   0,   {0, AT_RSP_ERROR, ""},         at_get_rsp,  NULL,
    AT_CGATT,     "AT+CGATT",        "+CGATT:",       "OK", "ERROR",   0,   {0, AT_RSP_ERROR, ""},         at_get_rsp,  NULL,
    AT_ATI,       "ATI",             "Revision:",     "OK", "ERROR",   0,   {0, AT_RSP_ERROR, ""},         at_get_rsp,  NULL,
		AT_CGSN,      "AT+CGSN",         "+CGSN:",        "OK", "ERROR",   0,   {0, AT_RSP_ERROR, ""},         at_get_rsp,  NULL,
		
		//module
    AT_NCCID,     "AT+NCCID",        "+NCCID:",      "OK", "ERROR",  0,   {0, AT_RSP_ERROR, ""},          at_get_rsp,   NULL,
    
		//protocols
		AT_QREGSWT,   "AT+QREGSWT",      "",              "OK", "ERROR",   0,   {0, AT_RSP_ERROR, ""},         at_set_rsp,  NULL,
    AT_NCDP,      "AT+NCDP",         "",              "OK", "ERROR",   0,   {0, AT_RSP_ERROR, ""},         at_set_rsp,  NULL,
		AT_QCFG,      "AT+QCFG",         "",              "OK", "ERROR",   0,   {0, AT_RSP_ERROR, ""},         at_set_rsp,  NULL,
    AT_REG,       "AT+QLWSREGIND",   "",              "OK", "ERROR",   0,   {0, AT_RSP_ERROR, ""},         at_set_rsp,  NULL,
		AT_SENDDATA,  "AT+QLWULDATAEX",  "",              "OK", "ERROR",   0,   {0, AT_RSP_ERROR, ""},         at_set_rsp,  NULL,
		AT_STATUS,    "AT+NMSTATUS",     "+NMSTATUS:",    "OK", "ERROR",   0,   {0, AT_RSP_ERROR, ""},         at_get_rsp,  NULL,
		AT_NNMI,      "AT+NNMI",         "",              "OK", "ERROR",   0,   {0, AT_RSP_ERROR, ""},         at_set_rsp,  NULL,
    AT_NSMI,      "AT+NSMI",         "",              "OK", "ERROR",   0,   {0, AT_RSP_ERROR, ""},         at_set_rsp,  NULL,
		AT_QLWULDATASTATUS, "AT+QLWULDATASTATUS","",      "OK", "ERROR",   0,   {0, AT_RSP_ERROR, ""},         at_get_rsp,  NULL,
		
		//URC - recv data
		AT_RECV,      "",                "+NNMI:",        "",   "",        0,   {0, AT_RSP_ERROR, ""},         NULL,        NULL,
    //URC - event
		AT_EVENT,     "",                "+QLWEVTIND:",   "",   "",        0,   {0, AT_RSP_ERROR, ""},         NULL,        NULL,
		AT_SENDASY,   "",         "++QLWULDATASTATUS:",   "",   "",        0,   {0, AT_RSP_ERROR, ""},         NULL,        NULL,
	},
	
	//function
	at_process_URC,
	at_recv_data,
	at_send_data
};

extern char NB_IMEI[];
extern char NB_ICCID[];

/***************************** functions **********************/

static void at_process_URC(void *data)
{
	int cmd;
	char *urc_data = data;
	
	if( urc_data != NULL )
	{
		if( strstr(urc_data, "+QLWEVTIND:") != 0 )
		{
			cmd = current_at_module.at_get_cmd(AT_EVENT);
			if( cmd < 0)
				return ;
			current_at_module.module->cmd_table[cmd].rsp.rsp_flag = true;
			current_at_module.module->cmd_table[cmd].rsp.result = AT_RSP_OK;
			current_at_module.module->cmd_table[cmd].rsp.data = urc_data;
			os_event_post_event(&BC28_recv_event, BC28_EVENT_CTM2MEVENT);
		}
		else if( strstr(urc_data, "+QLWULDATASTATUS:") != 0 )
		{
			cmd = current_at_module.at_get_cmd(AT_SENDASY);
			if( cmd < 0)
				return ;
			current_at_module.module->cmd_table[cmd].rsp.rsp_flag = true;
			current_at_module.module->cmd_table[cmd].rsp.result   = AT_RSP_OK;
			current_at_module.module->cmd_table[cmd].rsp.data     = urc_data;
			os_event_post_event(&NB_event, NB_EVENT_AT_SYN_RSP);
		}
		else if( strstr(urc_data, "+NSMI:") != 0 )
		{
			cmd = current_at_module.at_get_cmd(AT_SENDASY);
			if( cmd < 0)
				return ;
			current_at_module.module->cmd_table[cmd].rsp.rsp_flag = true;
			current_at_module.module->cmd_table[cmd].rsp.result   = AT_RSP_OK;
			current_at_module.module->cmd_table[cmd].rsp.data     = urc_data;
			os_event_post_event(&NB_event, NB_EVENT_AT_SYN_RSP);
		}
		else if( strstr(urc_data, "+NNMI:" ) != 0)
		{
			cmd = current_at_module.at_get_cmd(AT_RECV);
			if( cmd < 0)
				return ;
			current_at_module.module->cmd_table[cmd].rsp.rsp_flag = true;
			current_at_module.module->cmd_table[cmd].rsp.result = AT_RSP_OK;
			current_at_module.module->cmd_table[cmd].rsp.data = urc_data;
			os_event_post_event(&NB_recv_event, NB_EVENT_RECVDATA);
		}
		else
		{
			nb_log("URC:%s\r\n",urc_data);

			cmd = current_at_module.at_get_cmd(AT_EVENT);
			if( cmd < 0)
				return ;
			current_at_module.module->cmd_table[cmd].rsp.rsp_flag = true;
			current_at_module.module->cmd_table[cmd].rsp.result = AT_RSP_OK;
			current_at_module.module->cmd_table[cmd].rsp.data = urc_data;
			os_event_post_event(&BC28_recv_event, BC28_EVENT_SYSTEMEVENT);
		}
	}
}

static void *at_recv_data(char *data)
{
	char *p = strstr(data, "+NNMI:");
					
	if( p != NULL )
	{
		p += strlen("+NNMI:");
		p = strstr(p, ",");
		return (p+1);
	}
	
	return NULL;
					
}

static int8_t at_send_data(void *data_type)
{
	nb_send_data_type_t *send_data;
	int8_t ret;
	char buff[512]={0,};
	
	send_data = (nb_send_data_type_t *)data_type;
	
	if( send_data->mode == SEND_MODE_NON )
	{
		send_data->mode = BC28_MODE_NON;
	}
	else if(  send_data->mode == SEND_MODE_CON )
	{
		send_data->mode = BC28_MODE_CON;
	}
	else
	{
		send_data->mode = BC28_MODE_NON;
	}
		
	sprintf(buff, "%d,%s,0x%04x,%d", strlen(send_data->data)/2, send_data->data, send_data->mode, send_data->messageID);
	ret = current_at_module.at_send(AT_SENDDATA, buff, AT_CMD_MODE_SET, send_data->thread);
	
	return ret;			
}

/********************************lwm2m api***********************************/
static int8_t at_config_IP_port(char *serverIP, uint16_t port)
{
	int8_t ret;
	char buff[64]={0,};
	
	sprintf(buff, "%s,%d\r\n",serverIP, port);
	ret = current_at_module.at_send(AT_NCDP, buff, AT_CMD_MODE_SET, current_at_module.module->thread);
	
	return ret;			
}

static int8_t at_config_lifetime(uint32_t lifetime)
{
	int8_t ret;
	char buff[64]={0,};
	
	sprintf(buff, "\"LWM2M/Lifetime\",%d", lifetime);
	ret = current_at_module.at_send(AT_QCFG, buff, AT_CMD_MODE_SET, current_at_module.module->thread);
	
	return ret;			
}

static int8_t at_config_endpoint_name(const char *IMEI)
{
	int8_t ret;
	char buff[64]={0,};
	
	if( IMEI == NULL )
		return -1;
	
	sprintf(buff, "\"LWM2M/EndpointName\",\"urn:imei:%s\"", IMEI);
	ret = current_at_module.at_send(AT_QCFG, buff, AT_CMD_MODE_SET, current_at_module.module->thread);
	
	return ret;			
}

/*
@function:  at_config_recv_msg_indication
@params  :  value <0>  No indications
                  <1>  Indications and message
                  <2>  Indications only

@return  : 

*/
static int8_t at_config_recv_msg_indication(uint32_t value)
{
	int8_t ret;
	
	if( value > 2 )
		return -1;
	
	switch(value)
	{
		case 0:
		ret = current_at_module.at_send(AT_NNMI, "0", AT_CMD_MODE_SET, current_at_module.module->thread);
		break;
		
		case 1:
		ret = current_at_module.at_send(AT_NNMI, "1", AT_CMD_MODE_SET, current_at_module.module->thread);
		break;
		
		case 2:
		ret = current_at_module.at_send(AT_NNMI, "2", AT_CMD_MODE_SET, current_at_module.module->thread);
		break;
		
		default:
			return -1;
	}
	
	return ret;
}

/*
@function:  at_config_send_msg_indication
@params  :  value <0>  No indications
                  <1>  Indications and message

						status: SENT, SENT_TO_AIR_INTERFACE, DISCARDED
@return  : 

*/
static int8_t at_config_send_msg_indication(uint32_t value)
{
	int8_t ret;
	
	if( value > 1 )
		return -1;
	
	switch(value)
	{
		case 0:
		ret = current_at_module.at_send(AT_NSMI, "0", AT_CMD_MODE_SET, current_at_module.module->thread);
		break;
		
		case 1:
		ret = current_at_module.at_send(AT_NSMI, "1", AT_CMD_MODE_SET, current_at_module.module->thread);
		break;
		
		default:
			return -1;
	}
	
	return ret;
}

static int8_t at_lwm2m_register(void)
{
	int8_t ret;

	ret = current_at_module.at_send(AT_REG, "0", AT_CMD_MODE_SET, current_at_module.module->thread);
	
	return ret;			
}

static int8_t at_lwm2m_deregister(void)
{
	int8_t ret;

	ret = current_at_module.at_send(AT_REG, "1", AT_CMD_MODE_SET, current_at_module.module->thread);
	
	return ret;			
}

static int8_t at_lwm2m_get_status(void)
{
	int8_t ret;

	ret = current_at_module.at_send(AT_STATUS, "?", AT_CMD_MODE_GET, current_at_module.module->thread);
	
	return ret;			
}

int8_t at_get_CON_result(void *thread)
{
	int8_t ret;
	
	ret = current_at_module.at_send(AT_QLWULDATASTATUS, "?", AT_CMD_MODE_GET, thread);
	
	return ret;
}

/*********************************************************/
void BC28_thread_init(void)
{
	thread_os_register(&BC28_init_process, NULL);
	thread_os_register(&BC28_event_process, NULL);

	os_event_create(&BC28_event, &BC28_init_process);
	os_event_create(&BC28_recv_event, &BC28_event_process);
	
	os_event_set_event(&BC28_event, NB_EVENT_AT_ASY_RSP);
  os_event_set_event(&BC28_recv_event, BC28_EVENT_CTM2MEVENT|BC28_EVENT_SYSTEMEVENT);
	
	nb_log("\r\nBC28 init thread.......[OK]\r\n");
}

PROCESS_THREAD(BC28_init_process, event, pdata)
{
	static uint8_t timeout;
	uint8_t ret;
	rsp_msg_t *rsp_msg;
	
	THREAD_BEGIN()
	
	//QREGSWT=0 <Manual registration mode>
	AT_PROCESS(current_at_module.at_send(AT_QREGSWT, "0", AT_CMD_MODE_SET, p));
	AT_PROCESS_WAIT(BC28_event,1000);
	AT_PROCESS_TRY(3);
	AT_PROCESS_TIMEOUT(ret);
	if( rsp_msg->result == AT_RSP_ERROR )
	{}
	
	//CPSMS=0
	AT_PROCESS(current_at_module.at_send(AT_CPSMS, "0", AT_CMD_MODE_SET, p));
	AT_PROCESS_WAIT(BC28_event,1000);
	AT_PROCESS_TRY(3);
	AT_PROCESS_TIMEOUT(ret);
	if( rsp_msg->result == AT_RSP_ERROR )
	{}
	
  //CGSN (get module IMEI)
	AT_CGSN:
	AT_PROCESS(current_at_module.at_send(AT_CGSN, "1", AT_CMD_MODE_GET, p));
	AT_PROCESS_WAIT(BC28_event,1000);
	AT_PROCESS_TRY(3);
	AT_PROCESS_TIMEOUT(ret);
	if( rsp_msg->result == AT_RSP_DATA )
	{
		if( strlen(rsp_msg->data) > 10 )
		{
			memcpy(NB_IMEI, rsp_msg->data+6,15);
			nb_log("IMEI is :%s\r\n",NB_IMEI);
		}
    else
		{
			THREAD_OS_DELAY(2000);
			goto AT_CGSN;
		}			
	}
	else if( rsp_msg->result == AT_RSP_ERROR )
	{}
	
	//ICCID (get module ICCID)
	AT_NCCID:
	AT_PROCESS(current_at_module.at_send(AT_NCCID, "?", AT_CMD_MODE_GET, p));
	AT_PROCESS_WAIT(BC28_event,1000);
	AT_PROCESS_TRY(3);
	AT_PROCESS_TIMEOUT(ret);
	if( rsp_msg->result == AT_RSP_DATA )
	{
		if( strlen(rsp_msg->data) > 15 )
		{
			memcpy(NB_ICCID, rsp_msg->data+7, 20);
			nb_log("iccid is :%s\r\n",NB_ICCID);
		}	
		else
		{
			THREAD_OS_DELAY(2000);
			goto AT_NCCID;
		}
	}
	else if( rsp_msg->result == AT_RSP_ERROR )
	{
		THREAD_OS_DELAY(1000);
		goto AT_NCCID;
	}
	
	//CREEG?
  AT_CEREG:
	AT_PROCESS(current_at_module.at_send(AT_CEREG, "?", AT_CMD_MODE_GET, p));
	AT_PROCESS_WAIT(BC28_event,1000);
	AT_PROCESS_TRY(3);
	AT_PROCESS_TIMEOUT(ret);
	if( rsp_msg->result == AT_RSP_DATA )
	{
		if( strstr(rsp_msg->data,"+CEREG:0,1") != 0 )
		{
			nb_log("CEREG OK\r\n");
		}
		else
		{
			THREAD_OS_DELAY(2000);
			goto AT_CEREG;
		}
	}
	else if( rsp_msg->result == AT_RSP_ERROR )
	{}
		
	//CGATT?
	AT_CGATT:
	AT_PROCESS(current_at_module.at_send(AT_CGATT, "?", AT_CMD_MODE_GET, p));
	AT_PROCESS_WAIT(BC28_event,1000);
	AT_PROCESS_TRY(3);
	AT_PROCESS_TIMEOUT(ret);
	if( rsp_msg->result == AT_RSP_DATA )
	{
		if( strstr(rsp_msg->data,"+CGATT:1") != 0 )
		{
			nb_log("CGATT OK\r\n");
		}
		else
		{
			THREAD_OS_DELAY(2000);
			goto AT_CGATT;
		}
	}
	else if( rsp_msg->result == AT_RSP_ERROR )
	{}

	THREAD_OS_DELAY(4000);
		
	/*********************  ctiot at ******************/
	//config recv message indication
	AT_PROCESS(at_config_recv_msg_indication(1)); //set
	AT_PROCESS_WAIT(BC28_event,1000);
	AT_PROCESS_TRY(3);
	AT_PROCESS_TIMEOUT(ret);
	if( rsp_msg->result == AT_RSP_ERROR )
	{}
		
	//config send message indication
	AT_PROCESS(at_config_send_msg_indication(1)); //set
	AT_PROCESS_WAIT(BC28_event,1000);
	AT_PROCESS_TRY(3);
	AT_PROCESS_TIMEOUT(ret);
	if( rsp_msg->result == AT_RSP_ERROR )
	{}
		
	//configure server and port
	AT_PROCESS(at_config_IP_port(CTIOT_INIT_IP, CTIOT_INIT_PORT)); //set
	AT_PROCESS_WAIT(BC28_event,1000);
	AT_PROCESS_TRY(3);
	AT_PROCESS_TIMEOUT(ret);
	if( rsp_msg->result == AT_RSP_ERROR )
	{}
	
	// configure lifetime
	AT_PROCESS(at_config_lifetime(CTIOT_REG_LIFETIME)); //set
	AT_PROCESS_WAIT(BC28_event,1000);
	AT_PROCESS_TRY(3);
	AT_PROCESS_TIMEOUT(ret);
	if( rsp_msg->result == AT_RSP_ERROR )
	{}

#if 0	
	// configure endpoint name
	AT_PROCESS(at_config_endpoint_name(BC28_IMEI)); //set
	AT_PROCESS_WAIT(BC28_event,1000);
	AT_PROCESS_TRY(3);
	AT_PROCESS_TIMEOUT(ret);
	if( rsp_msg->result == AT_RSP_ERROR )
	{}
#endif
	
	//login
	login:
	AT_PROCESS(at_lwm2m_register()); //set
	AT_PROCESS_WAIT(BC28_event,1000);
	AT_PROCESS_TRY(3);
	AT_PROCESS_TIMEOUT(ret);
	if( rsp_msg->result == AT_RSP_ERROR )
	{}
	
	//get login status
	login_status:
	AT_PROCESS(at_lwm2m_get_status()); //get
	AT_PROCESS_WAIT(BC28_event,1000);
	AT_PROCESS_TRY(3);
	AT_PROCESS_TIMEOUT(ret);
	if( rsp_msg->result == AT_RSP_DATA )
	{
		if( strcmp(rsp_msg->data, "+NMSTATUS:REGISTERED") == 0 || strcmp(rsp_msg->data, "+NMSTATUS:REGISTERING") == 0)
		{
			THREAD_OS_DELAY(2000);
			goto login_status;
		}
		else if( strcmp(rsp_msg->data, "+NMSTATUS:MO_DATA_ENABLED") == 0 )
		{
			current_at_module.module->send_data_enable = 1;
		}
		else if( strcmp(rsp_msg->data, "+NMSTATUS:REJECTED_BY_SERVER") == 0 )
		{
			nb_log("rejected by server\r\n");
			THREAD_OS_DELAY(10000);
			goto login;
		}
	}
	else if( rsp_msg->result == AT_RSP_ERROR )
	{}

	nb_log("login success\r\n");

	THREAD_END()
}

PROCESS_THREAD(BC28_event_process, event, pdata)
{
	uint8_t ret;
	rsp_msg_t *rsp_msg;
	
	THREAD_BEGIN()
	
	while(1)
	{
		THREAD_WAIT_EVENT(1,&ret,0); 
		if( os_event_pend_event(&BC28_recv_event, &event, NULL) == OS_EVENT_OK ) 
		{      
			if( (event & BC28_EVENT_CTM2MEVENT) == BC28_EVENT_CTM2MEVENT )
			{
				rsp_msg = current_at_module.at_get_URC_result(AT_EVENT);
				if( rsp_msg != NULL && rsp_msg->result == AT_RSP_OK )
				{
					if( strstr(rsp_msg->data, "+QLWEVTIND:") != 0 )
					{
						char *p = rsp_msg->data;
						
						p += strlen("+QLWEVTIND:");
						if( strcmp(p,"0") == 0 )
						{
							
						}
						else if( strcmp(p,"1") == 0 )
						{
							
						}
						else if( strcmp(p,"2") == 0 )
						{
							
						}
						else if( strcmp(p,"3") == 0 )
						{
							current_at_module.module->send_data_enable = 1;
						}
						else if( strcmp(p,"4") == 0 )
						{
							
						}
						else if( strcmp(p,"5") == 0 )
						{
							
						}
						else if( strcmp(p,"6") == 0 )
						{
							
						}
						else if( strcmp(p,"7") == 0 )
						{
							
						}
						else if( strcmp(p,"8") == 0 )
						{
							
						}
						else if( strcmp(p,"9") == 0 )
						{
							current_at_module.module->send_data_enable = 0;
							thread_os_restart(&BC28_init_process);
						}
					}
					
					//clear data
					ret = current_at_module.at_get_cmd(AT_EVENT);
					if( ret > 0 )
					{
						current_at_module.module->cmd_table[ret].rsp.data = "";
						current_at_module.module->cmd_table[ret].rsp.result = AT_RSP_ERROR;
					}
				}
			}
			else if( (event & BC28_EVENT_SYSTEMEVENT) == BC28_EVENT_SYSTEMEVENT )
			{
				rsp_msg = current_at_module.at_get_URC_result(AT_EVENT);
				if( rsp_msg != NULL && rsp_msg->result == AT_RSP_OK )
				{
					
				}
				
				//clear data
				ret = current_at_module.at_get_cmd(AT_EVENT);
				if( ret > 0 )
				{
					current_at_module.module->cmd_table[ret].rsp.data = "";
					current_at_module.module->cmd_table[ret].rsp.result = AT_RSP_ERROR;
				}
			}	
		}
	}
	
	THREAD_END()
	
}
