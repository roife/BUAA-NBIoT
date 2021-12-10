/*
*********************************************************************************************************
*                                                FlexShell
*
*                               (c) Copyright 2018-2019, tony(gulonglong)
*                                          All Rights Reserved
**   THREAD-OS is provided in source form for FREE evaluation, for educational use or for peaceful research.
*
* File    : FlexShellUser.c
* By      : tony(gulonglong)
* Version : V1.0
* note    : You can define user command in the this file. 
            Here is an example of one usage that you can use as a reference.
* Example: 
*
*         #include "FlexShell.h"
*
*        // register table
*
*         FLEXSHELL_REGISTER_TABLE_START()
*
*         FLEXSHELL_CMD_REGISTER
*				  ( 
*						led_cmd,
*						"$test",
*						"$test:  test function\r\n",
*						test_cmd_func
*					)
*
*        // functions
*         static void test_cmd_func(flexshell_cmd_t *shell_cmd, uint32_t argc, char **args);
*					{
*         	if( argc == 1 )
*	          {
*             shell_log("%s", shell_cmd->cmd_str);
*           }
*           else
*           {
*             //add user code
*
*           }
*
*         }
*
*
*********************************************************************************************************
*/

#include <string.h>
#include <stdlib.h>

#include "FlexShell.h"

#include "fifo.h"
#include "bsp.h"


/***************************************** register table *****************************************/

FLEXSHELL_REGISTER_TABLE_START()

FLEXSHELL_CMD_REGISTER
( 
	led_cmd,
	"led",
	"led:<x>,<y>  x:device id ; y:mode\r\n",
	bsp_cmd_func
)

FLEXSHELL_CMD_REGISTER
( 
	motor_cmd,
	"motor",
	"motor:<x>,<y>  x:device id ; y:mode\r\n",
	bsp_cmd_func
)

FLEXSHELL_CMD_REGISTER
( 
	infrared_cmd,
	"infrared",
	"infrared:<x>,<y>  x:device id ; y:mode\r\n",
	bsp_cmd_func
)

FLEXSHELL_CMD_REGISTER
( 
	humitemp_cmd,
	"humitemp",
	"humitemp:<x>,<y>  x:device id ; y:mode\r\n",
	bsp_cmd_func
)

FLEXSHELL_CMD_REGISTER
( 
	fifo_cmd,
	"fifo",
	"fifo:<x>   x=1 fifo in,x=2 fifo out\r\n",
	fifo_func
)

FLEXSHELL_CMD_REGISTER
(
	at_cmd,
	"AT",
	"AT:<x> x= Manufacturer of module 'at string'\r\n",
	manufacturer_at_func
)

FLEXSHELL_CMD_REGISTER
(
	board_cmd,
	"board",
	"board: nb_starter_kit\r\n",
	get_board_infor
)

FLEXSHELL_CMD_REGISTER
(
	module_cmd,
	"module",
	"module: NB module information\r\n",
	nb_module_infor
)



/********************************************* functions body **********************************/


static void bsp_cmd_func(flexshell_cmd_t *shell_cmd, uint32_t argc, char **args)
{
	BSP_status_t BSP_status;
	
	getStatus(BSP_ALL,&BSP_status);
	
	if( argc == 0x01 )
	{
		if( strcmp(args[0], led_cmd.cmd) == 0 )
		{
			shell_log("%s\r\n",led_cmd.help);
			shell_log("x=%d or %d, %d is LED0;%d is LED1\r\n",LED0_DEVICE_ID,LED1_DEVICE_ID,LED0_DEVICE_ID,LED1_DEVICE_ID);
			shell_log("y=%d~%d \r\nLED_OFF:%d\r\nLED_GREEN:%d\r\nLED_RED:%d\r\nLED_BLUE:%d\r\nLED_PURPLE:%d\r\nLED_YELLOW:%d\r\nLED_PALE_BLUE:%d\r\nLED_WHITE:%d\r\n"
						,LED_OFF,LED_WHITE,LED_OFF,LED_GREEN,LED_RED,LED_BLUE,LED_PURPLE,LED_YELLOW,LED_PALE_BLUE,LED_WHITE);
		}
		else if( strcmp(args[0], motor_cmd.cmd) == 0 )
		{
			shell_log("%s\r\n",motor_cmd.help);
			shell_log("x=%d\r\n",MOTOR1);
			shell_log("y=%d~%d \r\nMOTOR_OFF:%d\r\nMOTOR_FORWARD:%d\r\nMOTOR_BACK_UP:%d\r\n",MOTOR_OFF,MOTOR_BACK_UP, MOTOR_OFF,MOTOR_FORWARD,MOTOR_BACK_UP);
		}
		else if( strcmp(args[0], infrared_cmd.cmd) == 0 )
		{
			shell_log("infrared_status:%d\r\n",BSP_status.IR_status);
		}
		else if( strcmp(args[0], humitemp_cmd.cmd) == 0 )
		{
			char temp[16]={0};
			sprintf(temp, "%.2f",BSP_status.temp);
			shell_log("humi:%dRH,temp:%sC\r\n",BSP_status.hump,temp);
		}
	}
	else if(argc == 3)
	{
		if( strcmp(args[0], motor_cmd.cmd) == 0 || strcmp(args[0], led_cmd.cmd) == 0)
		{
			shell_log("cmd has process\r\n");
			setStatus((BSP_DEVICE_ID)atoi(args[1]), atoi(args[2]));
		}
	}
}


static fifo_ram_t m_fifo_ram;

static uint8_t test_buf[32]={0,};

static void fifo_func(flexshell_cmd_t *shell_cmd, uint32_t argc, char **args)
{
	uint8_t type=0;
	uint8_t test[6]={'1','2','3','4','5','6'};
	
	if( argc == 1 )
	{
		shell_log("%s", shell_cmd->help);
		
		fifo_init(&m_fifo_ram, test_buf, 32);
	}
	else if( argc == 2 )
	{		
		type = atoi(args[1]);
		if( type == 1 )
		{
			if( fifo_put(&m_fifo_ram, (void *)test,6) == 0)
			{
				shell_log("fifo put err\r\n");
			}
			else
			{
				shell_log("\r\ncount:%d,in:%d,out:%d",m_fifo_ram.count, m_fifo_ram.in%m_fifo_ram.size, m_fifo_ram.out%m_fifo_ram.size);
			}
		}
		else if( type == 2 )
		{
			uint8_t len;
			if( (len = fifo_get(&m_fifo_ram, (void *)test, 6)) != 0)
			{
				shell_log("\r\ncount:%d,in:%d,out:%d",m_fifo_ram.count, m_fifo_ram.in%m_fifo_ram.size, m_fifo_ram.out%m_fifo_ram.size);
				shell_log("len:%d,paylod:%c%c%c%c%c%c\r\n",len,test[0],test[1],test[2],test[3],test[4],test[5]);
			}
			else
			{
				shell_log("fifo get err\r\n");
			}
		}
		else if( type == 3 )
		{
			shell_log("\r\nbuf:");
			for(uint8_t i=0;i<32;i++)
				shell_log("%02x ", m_fifo_ram.buf[i]);
			shell_log("\r\n");
		}
	}
}

static void manufacturer_at_func(flexshell_cmd_t *shell_cmd, uint32_t argc, char **args)
{
	if( argc == 1 )
	{
		shell_log("%s",shell_cmd->help);
	}
	else if( argc >= 2 )
	{
		char temp[256];
	  char *p=temp;
		uint32_t len=0;
		
		//check string len
		for( uint8_t i=1;i<argc;i++)
		{
			if( args[i] != NULL )
				len += strlen(args[i]);
		}
		if( len >=256 ) 
		{
			return;
		}
		
		sprintf(temp,"%s",args[1]);
		p += strlen(args[1]);
		for( uint8_t i=2;i<argc;i++)
		{
			sprintf(p,",%s",args[i]);
			p += strlen(args[i]) + 1;
		}
		*p = '\0';
		
		shell_log("%s\r\n",temp);
		printf("%s\r\n",temp);
	}
}

static void get_board_infor(flexshell_cmd_t *shell_cmd, uint32_t argc, char **args)
{
	if( argc == 1 )
	{
		shell_log("%s",shell_cmd->help);
		shell_log("nb_starter_kit_hard_version:%s\r\n",NB_STARTER_KIT_HARD_VERSION);
		shell_log("nb_starter_kit_soft_version:%s\r\n",NB_STARTER_KIT_SOFT_VERSION);
	}
}

extern char NB_IMEI[];
extern char NB_ICCID[];
static void nb_module_infor(flexshell_cmd_t *shell_cmd, uint32_t argc, char **args)
{
	if( argc == 1 )
	{
		shell_log("%s",shell_cmd->help);
	}
	else if( argc == 2 )
	{
		if( strcmp(args[1], "-iccid") == 0 )
		{
			shell_log("iccid:%s\r\n",NB_ICCID);
		}
		else if( strcmp(args[1], "-imei") == 0 )
		{
			shell_log("imei:%s\r\n",NB_IMEI);
		}
	}
}
