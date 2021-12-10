/***
*AepServiceCodes.h - 定义上下行数据的结构体，还有提供组装上行报文的函数和解析下行报文的函数
*
*Purpose:
*	1.数据结构体命名和平台定义的服务标识一致
*	2.codeDataReportByIdToStr、codeDataReportByIdToBytes、codeDataReportByIdentifierToStr、codeDataReportByIdentifierToBytes为组装上报数据的函数，具体说明见函数前的注释
*	3.decodeCmdDownFromStr、decodeCmdDownFromBytes为解析平台发送过来数据的函数，具体说明见函数前的注释
****/
#ifndef AEPSERVICECODES_H
#define AEPSERVICECODES_H

#include <stdlib.h>
#include <string.h>


#define AEP_BIG_ENDIAN 'b'
#define AEP_LITTLE_ENDIAN 'l'

static union { char c[4]; unsigned long mylong; } endian_test = {{ 'l', '?', '?', 'b' } };
#define AEP_ENDIANNESS ((char)endian_test.mylong)


typedef unsigned long long uint_64;
typedef unsigned int uint_32;  
typedef unsigned short uint_16;

//命令解析响应码
#define AEP_CMD_SUCCESS 0						//执行成功
#define AEP_CMD_FAILED 1						//执行失败
#define AEP_CMD_INVALID_DATASET_TYPE 2			//无效数据集类型
#define AEP_CMD_INVALID_DATASET_IDENTIFIER 3	//无效数据集标识
#define AEP_CMD_PAYLOAD_PARSING_FAILED 4		//指令数据集Payload解析失败,紧凑二进制编码内容长度不符等


typedef struct AepStrStruct
{
	unsigned short len;
	char* str;
} AepString;
typedef AepString AepBytes;

//无符号整型16位  
uint_16 aep_htons(uint_16 source);

//无符号整型32位
uint_32 aep_htoni(uint_32 source);

//无符号整型64位
uint_64 aep_htonl(uint_64 source);

//float
float aep_htonf(float source);

//double
double aep_htond(double source);

//16进制转字符串
void HexToStr(char *pbDest, char *pbSrc, int nLen);

//字符串转16进制
void StrToHex(char *pbDest, char *pbSrc, int nLen);


//根据服务id生成上报数据的十六进制字符串,srcStruct需要根据服务定义传入对应的类型,返回结果为字符串
AepString codeDataReportByIdToStr(int serviceId, void * srcStruct);

//根据服务id生成上报数据的字节流,srcStruct需要根据服务定义传入对应的类型,返回结果为字节流
AepBytes codeDataReportByIdToBytes(int serviceId, void * srcStruct);

//根据服务标识生成上报数据的十六进制字符串,srcStruct需要根据服务定义传入对应的类型,返回结果为字符串
AepString codeDataReportByIdentifierToStr(char * serviceIdentifier, void * srcStruct);

//根据服务标识生成上报数据的字节流,srcStruct需要根据服务定义传入对应的类型,返回结果为字节流
AepBytes codeDataReportByIdentifierToBytes(char * serviceIdentifier, void * srcStruct);

//指令解析返回结构体，data在使用时需要根据serviceId强转为对应类型
typedef struct CmdStruct 
{
	char* serviceIdentifier;
	unsigned short taskId;
	void * data;
	int code;
} AepCmdData;
//解析接受到的报文数据,入参为十六进制字符串
AepCmdData decodeCmdDownFromStr(char* source);
//解析接受到的报文数据,入参为原始字节流
AepCmdData decodeCmdDownFromBytes(char* source, int len);



typedef struct info_reportStruct 
{
	AepString manufacturer_name;
	AepString terminal_type;
	AepString module_type;
	AepString hardware_version;
	AepString software_version;
	AepString IMEI;
	AepString ICCID;
} info_report;
//数据上报:设备信息上报
AepString info_report_CodeDataReport (info_report srcStruct);


typedef struct motor_controlStruct 
{
	int control_int;
} motor_control;
//指令下发:电机控制
int motor_control_DecodeCmdDown (char* source, motor_control* dest);


typedef struct motor_control_ackStruct 
{
	unsigned short taskId;
	int control_int;
} motor_control_ack;
//指令下发响应:电机控制响应
AepString motor_control_ack_CodeCmdResponse (motor_control_ack srcStruct);


typedef struct motor_led0Struct 
{
	int control_int;
} motor_led0;
//指令下发:电机和led0控制
int motor_led0_DecodeCmdDown (char* source, motor_led0* dest);


typedef struct motor_led0_ackStruct 
{
	unsigned short taskId;
	int control_int;
} motor_led0_ack;
//指令下发响应:电机和led0控制响应
AepString motor_led0_ack_CodeCmdResponse (motor_led0_ack srcStruct);


typedef struct report_controlStruct 
{
	int control_int;
} report_control;
//指令下发:上报控制
int report_control_DecodeCmdDown (char* source, report_control* dest);


typedef struct report_control_ackStruct 
{
	unsigned short taskId;
	int control_int;
} report_control_ack;
//指令下发响应:上报控制响应
AepString report_control_ack_CodeCmdResponse (report_control_ack srcStruct);


typedef struct report_period_setStruct 
{
	int set_int;
} report_period_set;
//指令下发:参数配置
int report_period_set_DecodeCmdDown (char* source, report_period_set* dest);


typedef struct report_period_set_ackStruct 
{
	unsigned short taskId;
	int set_int;
} report_period_set_ack;
//指令下发响应:参数配置响应
AepString report_period_set_ack_CodeCmdResponse (report_period_set_ack srcStruct);


typedef struct sensor_reportStruct 
{
	char sensor_data[3];
} sensor_report;
//数据上报:传感器上报
AepString sensor_report_CodeDataReport (sensor_report srcStruct);


typedef struct signal_reportStruct 
{
	int rsrp;
	int sinr;
	int pci;
	int ecl;
	int cell_id;
} signal_report;
//数据上报:信号数据上报
AepString signal_report_CodeDataReport (signal_report srcStruct);


typedef struct temperature_humidity_reportStruct 
{
	float temperature_data;
	float humidity_data;
} temperature_humidity_report;
//数据上报:温湿度上报
AepString temperature_humidity_report_CodeDataReport (temperature_humidity_report srcStruct);


typedef struct ths_dataStruct 
{
	char sensor_data[3];
	float humidity_data;
	float temperature_data;
} ths_data;
//数据上报:温湿度和传感器数据
AepString ths_data_CodeDataReport (ths_data srcStruct);



#endif
