#ifndef __NB_THREAD_H__
#define __NB_THREAD_H__

#include "userconfig.h"


/***************************** NB event ********************************/

#define NB_EVENT_RECVDATA      0x01
#define NB_EVENT_AT_ASY_RSP    0x02
#define NB_EVENT_AT_SYN_RSP    0x04
#define NB_EVENT_AT_SEND       0x08

extern os_event_t  NB_event;
extern os_event_t  NB_recv_event;

/********************** at process *************************************/
#define THREAD_WAIT_MSG(os_event,ms)                                      \
		do                                                                 \
		{                                                                  \
			THREAD_WAIT_EVENT(1,&ret,(ms));                                   \
			if( ret == THREAD_EVENT_GET_OK )                                   \
			{                                                                   \
				if( os_event_pend_event(&os_event, &event, NULL) == OS_EVENT_OK )    \
				{                                                                 \
					if( (event&NB_EVENT_AT_ASY_RSP) == NB_EVENT_AT_ASY_RSP )     \
					{break;}                                                        \
				}                                                                 \
			}                                                                   \
			else                                                               \
			{break;}				\
		}while(1);  

#define AT_PROCESS(func)      	                                  \
	do                                                              \
	{		                                                            \
		if( func < 0 )                                                \
			THREAD_OS_DELAY(100);                                       

#define AT_PROCESS_WAIT(os_event,ms)		                          \
    THREAD_WAIT_MSG(os_event,ms);                                 \
		if( ret == THREAD_EVENT_GET_OK )                              \
		{                                                             \
			rsp_msg = current_at_module.at_get_result();                \
			if( rsp_msg != NULL )                                       \
			{break;}                                                    \
		}                                                             \
		else                                                          \
		{                                                             \
			current_at_module.release_req();                            \
		}                                                             

#define AT_PROCESS_TRY(count)                                     \
		if( ++timeout >= (count) ){timeout=0;break;}                  \
		THREAD_OS_DELAY((1000));                                      \
	}while(1); 

#define AT_PROCESS_TIMEOUT(ret)                                \
  do                                                           \
	{                                                            \
		if( ret == THREAD_EVENT_TIMEOUT )                          \
		{	                                                         \
			thread_os_restart(get_thread_name("AT init Thread"));    \
			if( get_thread_name("CTNB init Thread") != NULL )        \
				thread_os_delete(get_thread_name("CTNB init Thread")); \
			if( get_thread_name("BC28 init Thread") != NULL )        \
				thread_os_delete(get_thread_name("BC28 init Thread")); \
			return;                                                  \
		}                                                          \
	}while(0);	


#define AT_WAIT_SYN_ACK(os_event,event_type,ms)                              \
		do                                                                    \
		{                                                                     \
			THREAD_WAIT_EVENT(1,&ret,ms);                                       \
			if( ret == THREAD_EVENT_GET_OK )                                    \
			{                                                                   \
				if( os_event_pend_event(&os_event, &event, NULL) == OS_EVENT_OK ) \
				{                                                                 \
					if( (event & event_type) == event_type )                     \
					{                                                               \
						rsp_msg = current_at_module.at_get_URC_result(AT_CTM2MEVENT); \
						if( rsp_msg!= NULL && rsp_msg->result == AT_RSP_OK )          \
						{break;}                                                      \
					}                                                               \
				}                                                                 \
			}                                                                   \
			else                                                                \
			{break;}                                                            \
		}while(1);
	
/*****************************  at module struct *********************/
//module id
#define BC28                 0x02

//module lock
#define AT_LOCK              0x01
#define AT_UNLOCK            0x00

//at response
#define AT_RSP_OK            0x01
#define AT_RSP_ERROR         0x02
#define AT_RSP_DATA          0x04

//at type
#define AT_CMD_MODE_GET      1
#define AT_CMD_MODE_SET      2


//at id 0~19
typedef enum
{
	AT_AT    = 0,
	AT_ATE0  = 1,
	AT_ATI   = 2,
	AT_ATE1  = 3,
	AT_CEREG = 4,
	AT_CGATT = 5,
	AT_CPSMS = 6,
	AT_CGSN  = 7,
	AT_CMEE  = 8,
	
	AT_3GPP_MAX = 19
}AT_3GPP_cmd_e;

//at id 20~29
typedef enum
{
	AT_SENDDATA  = 21,
	AT_SENDASY   = 22, 
}AT_data_cmd_e;

//at id 30~59
typedef enum
{
	AT_NCCID    = 30,
	
	AT_NCDP     = 40,
	AT_QCFG     = 41,
	AT_REG      = 42,
	AT_STATUS   = 43,
	AT_NNMI     = 44,
	AT_NSMI     = 45,
	AT_QREGSWT  = 46,
	AT_QLWULDATASTATUS = 47,
	
	//URC
	AT_RECV  = 50,
	AT_EVENT = 51,
	
	AT_BC28_MAX   = 59
}AT_BC28_cmd_e;


typedef struct
{
	uint8_t rsp_flag;
	uint8_t result;
	char *data;
}rsp_msg_t;

typedef struct
{
	const uint8_t   at_id;
	const char * const cmd_str;
	const char * const rsp_data;     //+XXXX
	const char * const rsp_OK;       //OK
	const char * const rsp_ERROR;     //ERROR
	
	uint8_t   req_flag;
	
	rsp_msg_t rsp;
	void (*process_rsp)(uint8_t, void *);
	
	void *req_thread;
}at_cmd_table_t;

typedef struct
{
	char *data;
	uint32_t mode;
	uint8_t messageID;
	void *thread;
}nb_send_data_type_t;

typedef struct
{
	uint8_t module_id;
	char    *name;
	uint8_t lock;
	uint8_t send_data_enable;
	void    *thread;
	void    *event;
	
	at_cmd_table_t cmd_table[30];

	void   (*process_URC)(void *data);
	void*  (*recv_data)(char *data);
	int8_t (*send_data)(void *data);
}at_module_t;

typedef struct
{
	at_module_t *module;
	int8_t (*at_get_cmd)(uint8_t id);
	int8_t (*at_send)(uint8_t id, char *buff, uint8_t mode, void *req_thread);
	void   (*release_req)(void);
	int8_t (*at_process_rsp)(void *);
	void * (*at_get_result)(void);
	void * (*at_get_URC_result)(uint8_t at_id);
}at_abstract_module_t;


extern at_abstract_module_t current_at_module;
extern at_module_t at_module_BC28;


/******************************** send *****************************/

#define SEND_MODE_NON                        0
#define SEND_MODE_NON_RELEASE                1
#define SEND_MODE_NON_RELEASE_AFTER_REPLY    2
#define SEND_MODE_CON                        3
#define SEND_MODE_CON_RELEASE_AFTER_REPLY    4
#define SEND_MODE_DEFAULT                    SEND_MODE_NON   

//bc28 (huawei iot platform)send con data ack
/*
<result> Integer type. Status of CON data sending.
0 Have not sent (power on)
1 Sent, waiting response of IoT platform
2 Sent failed
3 Timeout
4 Success
5 Got reset message
<id> Sequence number. Range: 0-255. If <seq_num> is 0 it will be ignored.
*/


#define SEND_SYN_RSP_SUCCESS         4    
#define SEND_SYN_RSP_TIMEOUT         3    
#define SEND_SYN_RSP_SENDFAILED      2     
#define SEND_SYN_RSP_RESET_MESSAGE   5 
#define SEND_SYN_RSP_WAITING         1  
#define SEND_SYN_RSP_NOTSEND         0 



/************************* message ********************************/

#define SEND_ASY_RSP_SUCCESS      0
#define SEND_ASY_RSP_FAIL         1
#define SEND_ASY_RSP_TIMEOUT      2
#define SEND_ASY_RSP_SENDING      3   //use for BC28

typedef void (*asy_callback_t)(void *msg);
typedef void (*syn_callback_t)(uint8_t result, uint32_t massageID);

typedef struct 
{
	uint8_t   valid;
	uint32_t  len;
	char      send_buff[128];
	uint32_t  mode;
	uint32_t  messageID;
	uint32_t  result;     //syn rsp process result
	asy_callback_t callback;
}msg_data_t;




/*********************************************************/
void at_set_rsp(uint8_t cmd, void *args);
void at_get_rsp(uint8_t cmd, void *args);

void nb_thread_init(void);
int8_t nb_send(msg_data_t *m_msg);
void nb_register_syn_ack(void *callback);

#endif
