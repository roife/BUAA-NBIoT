#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>

//RTOS
#include "thread.h"
#include "bsp.h"
#include "util.h"
#include "fifo.h"
#include "BC28_thread.h"
#include "nb_thread.h"
#include "app_aep_profile.h"

/*********************************system vriable*************************************/
#define nb_log(...)         os_log("\r\n[NB_log]");os_log(__VA_ARGS__);
#define NB_APP_BUF_LEN      1024
#define NB_FIFO_RAM_SIZE    1024

//thread define
THREAD_INIT(nb_rsp_process, "AT rsp Thread");
THREAD_INIT(nb_init_process, "AT init Thread");
THREAD_INIT(nb_recvdata_process, "AT recvdata Thread");

// USER varibale
static args_block_t  m_args_data;

//fifo1
static uint8_t       recv_fifo_buff[NB_FIFO_RAM_SIZE];
static uint8_t       recv_buf[NB_APP_BUF_LEN]={0,};
static OSfifo_t      recv_os_fifo;          
static fifo_ram_t    recv_fifo;

//soft timer
static soft_timer_t  *send_timer;

//os event
os_event_t  NB_event;
os_event_t  NB_recv_event;

//msg
static msg_data_t *m_msg;

static syn_callback_t syn_cb;

static uint32_t messageID = 0;

char NB_IMEI[20];
char NB_ICCID[25];

/**************************************************************/
static int8_t at_get_cmd(uint8_t id);
static int8_t at_get_req_cmd(void);

static int8_t at_send(uint8_t at_id, char *buff, uint8_t mode, void *req_thread);
static void   release_req(void);
static int8_t at_process_rsp(void *data);
static void   *at_get_req_result(void);
static void   *at_get_URC_result(uint8_t at_id);


/***************************** at abstract module functions **********************/

at_abstract_module_t current_at_module = {
	//module
	.module = &at_module_BC28,
	
	//at parse callback
	.at_get_cmd = at_get_cmd,
	.at_send           = at_send,
	.release_req       = release_req,
	.at_process_rsp    = at_process_rsp,
	.at_get_result     = at_get_req_result,
	.at_get_URC_result = at_get_URC_result
};

static int8_t at_get_cmd(uint8_t id)
{
	for( uint8_t i=0;i<sizeof(current_at_module.module->cmd_table)/sizeof(current_at_module.module->cmd_table[0]);i++ )
	{
		if( current_at_module.module->cmd_table[i].at_id == id )
			return i;
	}
	
	return -1;
}

static int8_t at_send(uint8_t at_id, char *buff, uint8_t mode, void *req_thread)
{
	int cmd;
	
	cmd = at_get_cmd(at_id);
	if( cmd < 0 || req_thread == NULL )
		return -1;
		
	if( current_at_module.module->lock == AT_UNLOCK )
	{
		current_at_module.module->lock = AT_LOCK;
		if( buff == NULL )
		{
			printf("%s\r\n",current_at_module.module->cmd_table[cmd].cmd_str);	
			nb_log("%s\r\n",current_at_module.module->cmd_table[cmd].cmd_str);
		}
		else if( buff[0] == '?' )
		{
			printf("%s?\r\n",current_at_module.module->cmd_table[cmd].cmd_str);	
			nb_log("%s?\r\n",current_at_module.module->cmd_table[cmd].cmd_str);
		}
		else
		{
			printf("%s=%s\r\n",current_at_module.module->cmd_table[cmd].cmd_str, buff);	
			nb_log("%s=%s\r\n",current_at_module.module->cmd_table[cmd].cmd_str, buff);
		}
		
		if( mode == AT_CMD_MODE_SET )
		{
			current_at_module.module->cmd_table[cmd].process_rsp = at_set_rsp;
		}
		else if( mode == AT_CMD_MODE_GET )
		{
			current_at_module.module->cmd_table[cmd].process_rsp = at_get_rsp;
		}
		
		//set req_flag
		current_at_module.module->cmd_table[cmd].req_flag = true;
		current_at_module.module->cmd_table[cmd].req_thread = req_thread;
	}
	else
	{
		return -1;
	}
	
	return 1;
}

static void release_req(void)
{
	int at_cmd;
	
	at_cmd = at_get_req_cmd();
	if( at_cmd > 0)
	{
		//clear req_flag
		current_at_module.module->lock = AT_UNLOCK;       
		current_at_module.module->cmd_table[at_cmd].req_flag = false;
	}
}

static int8_t at_get_req_cmd(void)
{
	for( uint8_t i=0;i<sizeof(current_at_module.module->cmd_table)/sizeof(current_at_module.module->cmd_table[0]);i++ )
	{
		if( current_at_module.module->cmd_table[i].req_flag == true )
		{
			return i;
		}
	}
	
	return -1;
}

static int8_t at_process_rsp(void *data)
{
	int8_t at_cmd;
	
	at_cmd = at_get_req_cmd();
	if( at_cmd > 0 )
	{
		if( current_at_module.module->cmd_table[at_cmd].process_rsp != NULL )
		{
			nb_log("req at cmd:%s\r\n",current_at_module.module->cmd_table[at_cmd].cmd_str);
			current_at_module.module->cmd_table[at_cmd].process_rsp(at_cmd, data);
			
			if( current_at_module.module->cmd_table[at_cmd].req_thread == NB_event.recv_thread )
			{
				os_event_post_event(&NB_event, NB_EVENT_AT_ASY_RSP);
			}
			else
			{
					os_event_post_event((os_event_t *)current_at_module.module->event, NB_EVENT_AT_ASY_RSP);
			}
			return 1;		
		}
		else
		{
			nb_log("process_rsp is NULL\r\n");
			return -1;
		}
	}
	
	return -1;
}

static void *at_get_req_result(void)
{
	int at_cmd;
	
	at_cmd = at_get_req_cmd();
	if( at_cmd < 0)
		return NULL;

	nb_log("req rsp result:%d\r\n",current_at_module.module->cmd_table[at_cmd].rsp.result);
	nb_log("req rsp data:%s\r\n",current_at_module.module->cmd_table[at_cmd].rsp.data);
	current_at_module.release_req();
	
	return &current_at_module.module->cmd_table[at_cmd].rsp;
}

 void at_set_rsp(uint8_t cmd, void *args)
{
	args_block_t *m_args = args;
	
	for( uint8_t i=0;i<m_args->argc;i++)
	{
		if( m_args->args[i] != NULL )
		{
			if( strstr(m_args->args[i], current_at_module.module->cmd_table[cmd].rsp_OK) != 0 )
			{
				current_at_module.module->cmd_table[cmd].rsp.result = AT_RSP_OK;
				current_at_module.module->cmd_table[cmd].rsp.data = m_args->args[i];
				m_args->args[i] = NULL;
				return;
			}
			else if( strstr(m_args->args[i], current_at_module.module->cmd_table[cmd].rsp_ERROR) != 0 )
			{
				current_at_module.module->cmd_table[cmd].rsp.result = AT_RSP_ERROR;
				current_at_module.module->cmd_table[cmd].rsp.data   = m_args->args[i];
				m_args->args[i] = NULL;
				return;
			}
		}
	}
}

 void at_get_rsp(uint8_t cmd, void *args)
{
	args_block_t *m_args = args;
	
	for( uint8_t i=0;i<m_args->argc;i++)
	{
		if( m_args->args[i] != NULL )
		{
			if( strstr(m_args->args[i], current_at_module.module->cmd_table[cmd].rsp_ERROR) != 0 )
			{
				current_at_module.module->cmd_table[cmd].rsp.result = AT_RSP_ERROR;
				current_at_module.module->cmd_table[cmd].rsp.data   = m_args->args[i];
				m_args->args[i] = NULL;
				return;
			}
			
			if( strstr(m_args->args[i], current_at_module.module->cmd_table[cmd].rsp_OK) != 0 )
			{
				current_at_module.module->cmd_table[cmd].rsp.result = AT_RSP_DATA;
				m_args->args[i] = NULL;
				if( i>0 && m_args->args[i-1] != NULL )
				{
					if( strstr(m_args->args[i-1], current_at_module.module->cmd_table[cmd].rsp_data) != 0 )
					{
						current_at_module.module->cmd_table[cmd].rsp.data = m_args->args[i-1];
						m_args->args[i-1] = NULL;
					}
					else
					{
						current_at_module.module->cmd_table[cmd].rsp.result = AT_RSP_ERROR;
					}
				}
				else if( i == 0 )
				{
					current_at_module.module->cmd_table[cmd].rsp.result = AT_RSP_ERROR;
				}
				return;
			}

		}
	}
}

static void *at_get_URC_result(uint8_t at_id)
{
	int cmd;
	
	cmd = at_get_cmd(at_id);
	if( cmd < 0)
		return NULL;

	if( current_at_module.module->cmd_table[cmd].rsp.rsp_flag == true  )
	{
		current_at_module.module->cmd_table[cmd].rsp.rsp_flag = false;
		
		nb_log("URC result:%d",current_at_module.module->cmd_table[cmd].rsp.result);
		nb_log("URC data:%s\r\n",current_at_module.module->cmd_table[cmd].rsp.data);
		
		return &current_at_module.module->cmd_table[cmd].rsp;
	}
	
	return NULL;
}

static int8_t at_get_rsp_flag(void)
{
	for( uint8_t i=0;i<sizeof(current_at_module.module->cmd_table)/sizeof(current_at_module.module->cmd_table[0]);i++ )
	{
		if( current_at_module.module->cmd_table[i].rsp.rsp_flag == true )
		{
			return i;
		}
	}
	
	return -1;
}
/************************** at abstract module functions **************************/

/****************************************send and recv data******************************************/

static void module_cmd_table_init(void)
{
	current_at_module.module->lock = AT_UNLOCK;
	current_at_module.module->send_data_enable = 0;
	for( uint8_t i=0;i<sizeof(current_at_module.module->cmd_table)/sizeof(current_at_module.module->cmd_table[0]);i++ )
	{
		current_at_module.module->cmd_table[i].req_flag = 0;
		current_at_module.module->cmd_table[i].rsp.rsp_flag = 1;
		current_at_module.module->cmd_table[i].rsp.data = "";
		current_at_module.module->cmd_table[i].rsp.result = AT_RSP_ERROR;
		current_at_module.module->cmd_table[i].req_thread = NULL;
	}
}

static void send_timer_cb(void *args)
{
	if( m_msg != NULL && m_msg->callback != NULL)
	{                  
		m_msg->result = SEND_ASY_RSP_TIMEOUT;
		m_msg->callback(m_msg);//free msg
	}
}

void nb_thread_init(void)
{
	//thread
	thread_os_register(&nb_recvdata_process, NULL);
	thread_os_register(&nb_rsp_process,      NULL);
	thread_os_register(&nb_init_process,     NULL);

	//fifo
	THREAD_FIFO_INIT(&recv_os_fifo,                                   //os_fifo
	                 &nb_rsp_process,                                 //thread
	                 &recv_fifo, recv_fifo_buff, NB_FIFO_RAM_SIZE);   //fifo_ram
	
	app_uart_register_fifo(&recv_os_fifo);
	
	//soft timer
	soft_timer_register(&send_timer, OS_ONE_SHOT_MODE, send_timer_cb, NULL);
	
	os_event_create(&NB_event,      &nb_init_process);
	os_event_create(&NB_recv_event, &nb_recvdata_process);
	
	os_event_set_event(&NB_event,      NB_EVENT_AT_ASY_RSP|NB_EVENT_AT_SEND|NB_EVENT_AT_SYN_RSP);
	os_event_set_event(&NB_recv_event, NB_EVENT_RECVDATA);

	nb_log("\r\nregister NB thread.......[OK]\r\n");
}

void nb_syn_process(uint8_t result, uint32_t messageID)
{
	if( current_at_module.module->module_id == BC28 )
	{
		if( result == 5 )
		{
			result = SEND_SYN_RSP_RESET_MESSAGE;
		}
		else if( result == 4 )
		{
			result = SEND_SYN_RSP_SUCCESS;
		}
		else if( result == 3 )
		{
			result = SEND_SYN_RSP_TIMEOUT;
		}
		else if( result == 2 )
		{
			result = SEND_SYN_RSP_SENDFAILED;
		}
		else if( result == 1 )
		{
			result = SEND_SYN_RSP_WAITING;
		}
		else if( result == 0 )
		{
			result = SEND_SYN_RSP_NOTSEND;
		}
	}
	
	if( syn_cb != NULL )
	{
		syn_cb(result, messageID);
	}
}

void nb_register_syn_ack(void *callback)
{
	syn_cb = (syn_callback_t)callback;
}

int32_t at_send_data(msg_data_t *msg, void *thread)
{
	int32_t ret;
	nb_send_data_type_t send_data;
	
	send_data.data = msg->send_buff;
	send_data.mode = msg->mode;
	send_data.messageID = msg->messageID;
	send_data.thread = thread;
	
	ret = current_at_module.module->send_data(&send_data);
	
	return ret;
}

int8_t nb_send(msg_data_t *msg)
{
	if( msg == NULL || strlen(msg->send_buff) != msg->len )
	{
		nb_log("nb send params error\r\n");
		return -1;
	}
	
	if( m_msg != NULL && m_msg->valid == 1)
	{
		nb_log("msg is sending\r\n");
		return -1;
	}
			
	if( current_at_module.module->send_data_enable == 1 )
	{
		m_msg = msg;
    m_msg->valid = 1;
		m_msg->messageID = ++messageID;
		
		os_event_post_event(&NB_event, NB_EVENT_AT_SEND);
		soft_timer_start(send_timer, 10000);
	}
	else
	{
		nb_log("object 19/0/0 is not observe\r\n");
		return -1;
	}

	return 1;
}

/*********************************************************/

PROCESS_THREAD(nb_init_process, event, pdata)
{
	static uint8_t timeout;
	int8_t ret;
	uint8_t result;
	uint32_t id;
	rsp_msg_t *rsp_msg;
	
	THREAD_BEGIN()
	
	module_cmd_table_init();
	
	app_uart_init(9600,30);
	
	THREAD_OS_DELAY(1000);
	
	module_power_on();
	
  module_reset();
	
	THREAD_OS_DELAY(5000);
	
	//ATE0
	AT_PROCESS(current_at_module.at_send(AT_ATE0, NULL, AT_CMD_MODE_SET, p));
	AT_PROCESS_WAIT(NB_event,1000);
	AT_PROCESS_TRY(6);
	if( ret == THREAD_EVENT_TIMEOUT )
	{
		HAL_NVIC_SystemReset();
	}
	if( rsp_msg->result == AT_RSP_ERROR )
	{}
	
	//CMEE (set error indication)
	AT_PROCESS(current_at_module.at_send(AT_CMEE, "1", AT_CMD_MODE_SET, p));
	AT_PROCESS_WAIT(NB_event,1000);
	AT_PROCESS_TRY(3);
	AT_PROCESS_TIMEOUT(ret);
	if( rsp_msg->result == AT_RSP_ERROR )
	{}
	
	//ATI (get module version)
	ATI:
	AT_PROCESS(current_at_module.at_send(AT_ATI, NULL, AT_CMD_MODE_GET, p));
	AT_PROCESS_WAIT(NB_event,1000);
	AT_PROCESS_TRY(3);
	AT_PROCESS_TIMEOUT(ret);
	if( rsp_msg->result == AT_RSP_DATA )
	{
			if( strstr(rsp_msg->data,"BC28") != 0 )
			{
				current_at_module.module = &at_module_BC28;
				nb_log("Current module is BC28\r\n");
				BC28_thread_init();
			}
			else
			{
				THREAD_OS_DELAY(2000);
				goto ATI;
			}
	}
	else if( rsp_msg->result == AT_RSP_ERROR )
	{}
	
	while(1)
	{
		THREAD_WAIT_EVENT(1,&ret,0); 
		if( os_event_pend_event(&NB_event, &event, NULL) == OS_EVENT_OK ) 
		{      
			if( (event & NB_EVENT_AT_SEND) == NB_EVENT_AT_SEND )
			{				
				if( m_msg != NULL )
				{
					if( current_at_module.module->module_id == BC28 )
					{
						AT_PROCESS(at_get_CON_result(p));
						AT_PROCESS_WAIT(NB_event,1000);
						AT_PROCESS_TRY(3);
						if( ret == THREAD_EVENT_GET_OK )
						{
							if( rsp_msg->result == AT_RSP_DATA || rsp_msg->result == AT_RSP_OK )
							{
								if( strstr(rsp_msg->data, "+QLWULDATASTATUS:1") != 0 )
								{
									char *p =  rsp_msg->data;
									
									p += strlen("+QLWULDATASTATUS:1");
									if( *p == ',' )
									{
										p++;
										m_msg->messageID = strtol(p,NULL, 10);
									}
									else
									{
										m_msg->messageID = 0;
									}
									
									m_msg->result = SEND_ASY_RSP_SENDING;
									
									if( m_msg->callback != NULL )
									{
										m_msg->callback(m_msg);//free msg
									}
									soft_timer_stop(send_timer);
									m_msg = NULL;
									
									continue;
								}
							}
						}
						else
						{
							m_msg->result = SEND_ASY_RSP_TIMEOUT;
							m_msg->messageID = 0;
							
							if( m_msg->callback != NULL )
							{
								m_msg->callback(m_msg);//free msg
							}
							soft_timer_stop(send_timer);
							m_msg = NULL;
							continue;
						}
					}
				
					AT_PROCESS(at_send_data(m_msg,p));
					AT_PROCESS_WAIT(NB_event,1000);
					AT_PROCESS_TRY(3);
					if( ret == THREAD_EVENT_GET_OK )
					{
						char *p = NULL;
						if( rsp_msg->result == AT_RSP_DATA || rsp_msg->result == AT_RSP_OK )
						{
							if( current_at_module.module->module_id == BC28 )
							{
									m_msg->result = SEND_ASY_RSP_SUCCESS;
							}
						}
						else
						{
							if( current_at_module.module->module_id == BC28 )
							{
								if( strstr(rsp_msg->data, "+CME ERROR: ") != 0 )
								{
									p = rsp_msg->data;
									p += strlen("+CME ERROR: ");
									m_msg->messageID = strtol(p,NULL, 10); //err id
									m_msg->result = SEND_ASY_RSP_FAIL;
								}
							}
						}
					}
					else
					{
						m_msg->result = SEND_ASY_RSP_TIMEOUT;
						m_msg->messageID = 0;
					}
					
					if( current_at_module.module->module_id == BC28 )
					{
						if( m_msg->result != SEND_ASY_RSP_SUCCESS )
						{
							if( m_msg->callback != NULL )
							{
								m_msg->callback(m_msg);//free msg
							}
							soft_timer_stop(send_timer);
							m_msg = NULL;
						}
					}
				}
			}
			else if( (event & NB_EVENT_AT_SYN_RSP) == NB_EVENT_AT_SYN_RSP )
			{
				rsp_msg = current_at_module.at_get_URC_result(AT_SENDASY);
				if( rsp_msg != NULL && rsp_msg->result == AT_RSP_OK )
				{
					if( current_at_module.module->module_id == BC28 )
					{
						if( strstr(rsp_msg->data, "+NSMI:") != 0 )
						{
							if( strlen(rsp_msg->data) > 10 )
							{
								char *p;
								p = rsp_msg->data;
								p += strlen("+NSMI:");
								
								if( m_msg != NULL )
								{
									if( strstr(p, "SENT_TO_AIR_INTERFACE") != 0 )
									{
										m_msg->result = SEND_ASY_RSP_SUCCESS;
									}
									else if( strstr(p, "SENT") != 0 )
									{
										m_msg->result = SEND_ASY_RSP_SUCCESS;
									}
									else if( strstr(p, "DISCARDED") != 0 )
									{
										m_msg->result = SEND_ASY_RSP_FAIL;
									}
									else
									{
										m_msg->result = SEND_ASY_RSP_FAIL;
									}
									
									p = strstr(p,",");
									if( p[1] != 0 )
									{
										m_msg->messageID = strtol(p+1,NULL, 10);
									}
									else
									{
										m_msg->messageID = 0;
									}
									
									if( m_msg->callback != NULL )
									{
										m_msg->callback(m_msg);
									}
									soft_timer_stop(send_timer);
									m_msg = NULL;
								}
								else
								{
									if( strstr(p, "SENT_TO_AIR_INTERFACE") != 0 )
									{
										result = SEND_ASY_RSP_SUCCESS;
									}
									else if( strstr(p, "SENT") != 0 )
									{
										result = SEND_ASY_RSP_SUCCESS;
									}
									else if( strstr(p, "DISCARDED") != 0 )
									{
										result = SEND_ASY_RSP_FAIL;
									}
									else
									{
										result = SEND_ASY_RSP_FAIL;
									}
									
									p = strstr(p,",");
									if( p[1] != 0 )
									{
										id = strtol(p+1,NULL, 10);
									}
									
									nb_syn_process(result, id);
								}
							}
						}
						else if( strstr(rsp_msg->data, "+QLWULDATASTATUS:") != 0 )
						{
							char *p;
							p = rsp_msg->data;
							p += strlen("+QLWULDATASTATUS:");
							result = strtol(p,NULL, 10);
							if( p[1] == ',' )
							{
								id = strtol(p+2,NULL, 10);
							}
							else if( p[2] == ',' )
							{
								id = strtol(p+3,NULL, 10);
							}

							nb_syn_process(result, id);
						}
					}
				}
			}/* end if( (event & NB_EVENT_AT_XXX) == NB_EVENT_AT_XXX ) */
			
		}/* end if( os_event_pend_event(&NB_event, &event, NULL) == OS_EVENT_OK )  */  
	
		
	}/* end while(1) */
	
	THREAD_END()
	
}


PROCESS_THREAD(nb_recvdata_process, event, pdata)
{
	int8_t ret;
	rsp_msg_t *rsp_msg;
	
	THREAD_BEGIN()
	
	while(1)
	{
		THREAD_WAIT_EVENT(1,&ret,0); 
		if( os_event_pend_event(&NB_recv_event, &event, NULL) == OS_EVENT_OK ) 
		{      
			if( (event & NB_EVENT_RECVDATA) == NB_EVENT_RECVDATA )
			{	
				rsp_msg = current_at_module.at_get_URC_result(AT_RECV);
				if( rsp_msg != NULL && rsp_msg->result == AT_RSP_OK )
				{
					char *data;
					data = current_at_module.module->recv_data(rsp_msg->data);
					if( data != NULL )
					{
						decode_aep_data(data);
					}
				}
			}
		}
	}
	
	THREAD_END()
}

PROCESS_THREAD(nb_rsp_process, ev, pdata)
{
	int8_t ret;
	static uint8_t i = 0;
	
	THREAD_BEGIN()

	while(1)
	{			
			memset(recv_buf, 0, sizeof(recv_buf));
		
			THREAD_FIFO_GET(&recv_os_fifo, (char *)recv_buf, sizeof(recv_buf), &ret, 0)

			nb_log("[fifo:%s]\r\n",recv_buf);
				
			m_args_data.argc = SPLIT_PARAMS_MAX;
			if( shell_split((char *)recv_buf, "\r\n", m_args_data.args, &m_args_data.argc) < 0 )
			{
				  continue;
			}
			
			nb_log("argc:%d\r\n",m_args_data.argc);
			for( uint8_t i=0;i<m_args_data.argc;i++)
			{
				nb_log("args[%d]:%s\r\n",i,m_args_data.args[i]);
			}
			
			// process at response
			ret = current_at_module.at_process_rsp(&m_args_data);
			THREAD_OS_DELAY(10);
			
			// process URC event
			for( i=0; i<m_args_data.argc; i++)
			{
				if( m_args_data.args[i] != NULL )
				{
					current_at_module.module->process_URC(m_args_data.args[i]);	
					THREAD_OS_DELAY(5);
				}
			}
	}	

	THREAD_END()
}



