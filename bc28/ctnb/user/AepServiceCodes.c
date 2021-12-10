#include <ctype.h>
#include "AepServiceCodes.h"

//无符号整型16位  
uint_16 aep_htons(uint_16 source)  
{  

	if(AEP_ENDIANNESS == AEP_BIG_ENDIAN)
		return source;
	else
		return (uint_16)( 0
		| ((source & 0x00ff) << 8)
		| ((source & 0xff00) >> 8) );  
}  

//无符号整型32位
uint_32 aep_htoni(uint_32 source)  
{  
	if(AEP_ENDIANNESS == AEP_BIG_ENDIAN)
		return source;
	else
		return 0
		| ((source & 0x000000ff) << 24)
		| ((source & 0x0000ff00) << 8)
		| ((source & 0x00ff0000) >> 8)
		| ((source & 0xff000000) >> 24);  
}

//无符号整型64位
uint_64 aep_htonl(uint_64 source)  
{  
	if(AEP_ENDIANNESS == AEP_BIG_ENDIAN)
		return source;
	else
		return 0
		| ((source & (uint_64)(0x00000000000000ff)) << 56)
		| ((source & (uint_64)(0x000000000000ff00)) << 40)
		| ((source & (uint_64)(0x0000000000ff0000)) << 24)
		| ((source & (uint_64)(0x00000000ff000000)) << 8)
		| ((source & (uint_64)(0x000000ff00000000)) >> 8)
		| ((source & (uint_64)(0x0000ff0000000000)) >> 24)
		| ((source & (uint_64)(0x00ff000000000000)) >> 40)
		| ((source & (uint_64)(0xff00000000000000)) >> 56);
}

//float
float aep_htonf(float source)  
{  
	if(AEP_ENDIANNESS == AEP_BIG_ENDIAN)
		return source;
	else
	{
		uint_32 t= 0
			| ((*(uint_32*)&source & 0x000000ff) << 24)
			| ((*(uint_32*)&source & 0x0000ff00) << 8)
			| ((*(uint_32*)&source & 0x00ff0000) >> 8)
			| ((*(uint_32*)&source & 0xff000000) >> 24);
		return *(float*)&t;
	} 
}

//double
double aep_htond(double source)  
{  
	if(AEP_ENDIANNESS == AEP_BIG_ENDIAN)
		return source;
	else
	{
		uint_64 t= 0
			| ((*(uint_64*)&source & (uint_64)(0x00000000000000ff)) << 56)
			| ((*(uint_64*)&source & (uint_64)(0x000000000000ff00)) << 40)
			| ((*(uint_64*)&source & (uint_64)(0x0000000000ff0000)) << 24)
			| ((*(uint_64*)&source & (uint_64)(0x00000000ff000000)) << 8)
			| ((*(uint_64*)&source & (uint_64)(0x000000ff00000000)) >> 8)
			| ((*(uint_64*)&source & (uint_64)(0x0000ff0000000000)) >> 24)
			| ((*(uint_64*)&source & (uint_64)(0x00ff000000000000)) >> 40)
			| ((*(uint_64*)&source & (uint_64)(0xff00000000000000)) >> 56);
		return *(double*)&t;
	}
}

//16进制转字符串
void HexToStr(char *pbDest, char *pbSrc, int nLen)
{
	unsigned char ddl,ddh;
	int i;

	for (i=0; i<nLen; i++)
	{
		ddh = 48 + (unsigned char)pbSrc[i] / 16;
		ddl = 48 + (unsigned char)pbSrc[i] % 16;
		if (ddh > 57) ddh = ddh + 7;
		if (ddl > 57) ddl = ddl + 7;
		pbDest[i*2] = ddh;
		pbDest[i*2+1] = ddl;
	}

	//pbDest[nLen*2] = '\0';
}

//字符串转16进制
void StrToHex(char *pbDest, char *pbSrc, int nLen)
{
	unsigned char h1,h2;
	unsigned char s1,s2;
	int i;

	for (i=0; i<nLen; i++)
	{
		h1 = pbSrc[2*i];
		h2 = pbSrc[2*i+1];

		s1 = toupper(h1) - 0x30;
		if (s1 > 9) 
			s1 -= 7;

		s2 = toupper(h2) - 0x30;
		if (s2 > 9) 
			s2 -= 7;

		pbDest[i] = s1*16 + s2;
	}
}

//数据上报:设备信息上报
AepString info_report_CodeDataReport (info_report srcStruct)
{
	char* index;
	AepString resultStruct;
	unsigned short tempLen;

	unsigned short payloadLen = 0 + (srcStruct.manufacturer_name.len + 2) + (srcStruct.terminal_type.len + 2) + (srcStruct.module_type.len + 2) + (srcStruct.hardware_version.len + 2) + (srcStruct.software_version.len + 2) + (srcStruct.IMEI.len + 2) + (srcStruct.ICCID.len + 2);
	resultStruct.len = (1 + 2 + 2 + payloadLen) * 2;
	resultStruct.str = (char *)malloc(resultStruct.len + 1);
	memset(resultStruct.str, 0, resultStruct.len + 1);


	index = resultStruct.str;

	memcpy(index, "02", 2);
	index += 1 * 2;

	tempLen = aep_htons(3);//服务ID
	HexToStr(index, (char *)&tempLen, 2);
	index += 2 * 2;

	tempLen = aep_htons(payloadLen);
	HexToStr(index, (char *)&tempLen, 2);
	index += 2 * 2;

	tempLen = aep_htons(srcStruct.manufacturer_name.len);
	HexToStr(index, (char *)&tempLen, 2);
	index += 2 * 2;
	HexToStr(index, (char *)srcStruct.manufacturer_name.str, srcStruct.manufacturer_name.len);
	index += srcStruct.manufacturer_name.len * 2;

	tempLen = aep_htons(srcStruct.terminal_type.len);
	HexToStr(index, (char *)&tempLen, 2);
	index += 2 * 2;
	HexToStr(index, (char *)srcStruct.terminal_type.str, srcStruct.terminal_type.len);
	index += srcStruct.terminal_type.len * 2;

	tempLen = aep_htons(srcStruct.module_type.len);
	HexToStr(index, (char *)&tempLen, 2);
	index += 2 * 2;
	HexToStr(index, (char *)srcStruct.module_type.str, srcStruct.module_type.len);
	index += srcStruct.module_type.len * 2;

	tempLen = aep_htons(srcStruct.hardware_version.len);
	HexToStr(index, (char *)&tempLen, 2);
	index += 2 * 2;
	HexToStr(index, (char *)srcStruct.hardware_version.str, srcStruct.hardware_version.len);
	index += srcStruct.hardware_version.len * 2;

	tempLen = aep_htons(srcStruct.software_version.len);
	HexToStr(index, (char *)&tempLen, 2);
	index += 2 * 2;
	HexToStr(index, (char *)srcStruct.software_version.str, srcStruct.software_version.len);
	index += srcStruct.software_version.len * 2;

	tempLen = aep_htons(srcStruct.IMEI.len);
	HexToStr(index, (char *)&tempLen, 2);
	index += 2 * 2;
	HexToStr(index, (char *)srcStruct.IMEI.str, srcStruct.IMEI.len);
	index += srcStruct.IMEI.len * 2;

	tempLen = aep_htons(srcStruct.ICCID.len);
	HexToStr(index, (char *)&tempLen, 2);
	index += 2 * 2;
	HexToStr(index, (char *)srcStruct.ICCID.str, srcStruct.ICCID.len);
	index += srcStruct.ICCID.len * 2;


	return resultStruct;
}

//指令下发:电机控制
int motor_control_DecodeCmdDown (char* source, motor_control* dest)
{
	char* index = source;
	int srcStrLen = strlen(source);
	int len = 4;


	memset(dest, 0, sizeof(motor_control));

	StrToHex((char *)&dest->control_int, index, 4);
	dest->control_int = aep_htoni(dest->control_int);
	index += 4 * 2;



	if (len * 2 > srcStrLen)
	{
		return AEP_CMD_PAYLOAD_PARSING_FAILED;
	}
	return AEP_CMD_SUCCESS;
}

//指令下发响应:电机控制响应
AepString motor_control_ack_CodeCmdResponse (motor_control_ack srcStruct)
{
	char* index;
	AepString resultStruct;
	unsigned short tempLen;

	unsigned short payloadLen = 4;
	resultStruct.len = (1 + 2 + 2 + 2 + payloadLen) * 2;
	resultStruct.str = (char *)malloc(resultStruct.len + 1);
	memset(resultStruct.str, 0, resultStruct.len + 1);

	srcStruct.control_int = aep_htoni(srcStruct.control_int);

	index = resultStruct.str;

	memcpy(index, "86", 2);
	index += 1 * 2;

	tempLen = aep_htons(9001);//服务ID
	HexToStr(index, (char *)&tempLen, 2);
	index += 2 * 2;

	tempLen = aep_htons(srcStruct.taskId);//taskID
	HexToStr(index, (char *)&tempLen, 2);
	index += 2 * 2;

	tempLen = aep_htons(payloadLen);
	HexToStr(index, (char *)&tempLen, 2);
	index += 2 * 2;

	HexToStr(index, (char *)&srcStruct.control_int, 4);
	index += 4 * 2;


	return resultStruct;
}

//指令下发:电机和led0控制
int motor_led0_DecodeCmdDown (char* source, motor_led0* dest)
{
	char* index = source;
	int srcStrLen = strlen(source);
	int len = 4;


	memset(dest, 0, sizeof(motor_led0));

	StrToHex((char *)&dest->control_int, index, 4);
	dest->control_int = aep_htoni(dest->control_int);
	index += 4 * 2;



	if (len * 2 > srcStrLen)
	{
		return AEP_CMD_PAYLOAD_PARSING_FAILED;
	}
	return AEP_CMD_SUCCESS;
}

//指令下发响应:电机和led0控制响应
AepString motor_led0_ack_CodeCmdResponse (motor_led0_ack srcStruct)
{
	char* index;
	AepString resultStruct;
	unsigned short tempLen;

	unsigned short payloadLen = 4;
	resultStruct.len = (1 + 2 + 2 + 2 + payloadLen) * 2;
	resultStruct.str = (char *)malloc(resultStruct.len + 1);
	memset(resultStruct.str, 0, resultStruct.len + 1);

	srcStruct.control_int = aep_htoni(srcStruct.control_int);

	index = resultStruct.str;

	memcpy(index, "86", 2);
	index += 1 * 2;

	tempLen = aep_htons(9006);//服务ID
	HexToStr(index, (char *)&tempLen, 2);
	index += 2 * 2;

	tempLen = aep_htons(srcStruct.taskId);//taskID
	HexToStr(index, (char *)&tempLen, 2);
	index += 2 * 2;

	tempLen = aep_htons(payloadLen);
	HexToStr(index, (char *)&tempLen, 2);
	index += 2 * 2;

	HexToStr(index, (char *)&srcStruct.control_int, 4);
	index += 4 * 2;


	return resultStruct;
}

//指令下发:上报控制
int report_control_DecodeCmdDown (char* source, report_control* dest)
{
	char* index = source;
	int srcStrLen = strlen(source);
	int len = 4;


	memset(dest, 0, sizeof(report_control));

	StrToHex((char *)&dest->control_int, index, 4);
	dest->control_int = aep_htoni(dest->control_int);
	index += 4 * 2;



	if (len * 2 > srcStrLen)
	{
		return AEP_CMD_PAYLOAD_PARSING_FAILED;
	}
	return AEP_CMD_SUCCESS;
}

//指令下发响应:上报控制响应
AepString report_control_ack_CodeCmdResponse (report_control_ack srcStruct)
{
	char* index;
	AepString resultStruct;
	unsigned short tempLen;

	unsigned short payloadLen = 4;
	resultStruct.len = (1 + 2 + 2 + 2 + payloadLen) * 2;
	resultStruct.str = (char *)malloc(resultStruct.len + 1);
	memset(resultStruct.str, 0, resultStruct.len + 1);

	srcStruct.control_int = aep_htoni(srcStruct.control_int);

	index = resultStruct.str;

	memcpy(index, "86", 2);
	index += 1 * 2;

	tempLen = aep_htons(9002);//服务ID
	HexToStr(index, (char *)&tempLen, 2);
	index += 2 * 2;

	tempLen = aep_htons(srcStruct.taskId);//taskID
	HexToStr(index, (char *)&tempLen, 2);
	index += 2 * 2;

	tempLen = aep_htons(payloadLen);
	HexToStr(index, (char *)&tempLen, 2);
	index += 2 * 2;

	HexToStr(index, (char *)&srcStruct.control_int, 4);
	index += 4 * 2;


	return resultStruct;
}

//指令下发:参数配置
int report_period_set_DecodeCmdDown (char* source, report_period_set* dest)
{
	char* index = source;
	int srcStrLen = strlen(source);
	int len = 4;


	memset(dest, 0, sizeof(report_period_set));

	StrToHex((char *)&dest->set_int, index, 4);
	dest->set_int = aep_htoni(dest->set_int);
	index += 4 * 2;



	if (len * 2 > srcStrLen)
	{
		return AEP_CMD_PAYLOAD_PARSING_FAILED;
	}
	return AEP_CMD_SUCCESS;
}

//指令下发响应:参数配置响应
AepString report_period_set_ack_CodeCmdResponse (report_period_set_ack srcStruct)
{
	char* index;
	AepString resultStruct;
	unsigned short tempLen;

	unsigned short payloadLen = 4;
	resultStruct.len = (1 + 2 + 2 + 2 + payloadLen) * 2;
	resultStruct.str = (char *)malloc(resultStruct.len + 1);
	memset(resultStruct.str, 0, resultStruct.len + 1);

	srcStruct.set_int = aep_htoni(srcStruct.set_int);

	index = resultStruct.str;

	memcpy(index, "86", 2);
	index += 1 * 2;

	tempLen = aep_htons(9003);//服务ID
	HexToStr(index, (char *)&tempLen, 2);
	index += 2 * 2;

	tempLen = aep_htons(srcStruct.taskId);//taskID
	HexToStr(index, (char *)&tempLen, 2);
	index += 2 * 2;

	tempLen = aep_htons(payloadLen);
	HexToStr(index, (char *)&tempLen, 2);
	index += 2 * 2;

	HexToStr(index, (char *)&srcStruct.set_int, 4);
	index += 4 * 2;


	return resultStruct;
}

//数据上报:传感器上报
AepString sensor_report_CodeDataReport (sensor_report srcStruct)
{
	char* index;
	AepString resultStruct;
	unsigned short tempLen;

	unsigned short payloadLen = 3;
	resultStruct.len = (1 + 2 + 2 + payloadLen) * 2;
	resultStruct.str = (char *)malloc(resultStruct.len + 1);
	memset(resultStruct.str, 0, resultStruct.len + 1);


	index = resultStruct.str;

	memcpy(index, "02", 2);
	index += 1 * 2;

	tempLen = aep_htons(1);//服务ID
	HexToStr(index, (char *)&tempLen, 2);
	index += 2 * 2;

	tempLen = aep_htons(payloadLen);
	HexToStr(index, (char *)&tempLen, 2);
	index += 2 * 2;

	HexToStr(index, (char *)srcStruct.sensor_data, 3);
	index += 3 * 2;


	return resultStruct;
}

//数据上报:信号数据上报
AepString signal_report_CodeDataReport (signal_report srcStruct)
{
	char* index;
	AepString resultStruct;
	unsigned short tempLen;

	unsigned short payloadLen = 20;
	resultStruct.len = (1 + 2 + 2 + payloadLen) * 2;
	resultStruct.str = (char *)malloc(resultStruct.len + 1);
	memset(resultStruct.str, 0, resultStruct.len + 1);

	srcStruct.rsrp = aep_htoni(srcStruct.rsrp);
	srcStruct.sinr = aep_htoni(srcStruct.sinr);
	srcStruct.pci = aep_htoni(srcStruct.pci);
	srcStruct.ecl = aep_htoni(srcStruct.ecl);
	srcStruct.cell_id = aep_htoni(srcStruct.cell_id);

	index = resultStruct.str;

	memcpy(index, "02", 2);
	index += 1 * 2;

	tempLen = aep_htons(2);//服务ID
	HexToStr(index, (char *)&tempLen, 2);
	index += 2 * 2;

	tempLen = aep_htons(payloadLen);
	HexToStr(index, (char *)&tempLen, 2);
	index += 2 * 2;

	HexToStr(index, (char *)&srcStruct.rsrp, 4);
	index += 4 * 2;

	HexToStr(index, (char *)&srcStruct.sinr, 4);
	index += 4 * 2;

	HexToStr(index, (char *)&srcStruct.pci, 4);
	index += 4 * 2;

	HexToStr(index, (char *)&srcStruct.ecl, 4);
	index += 4 * 2;

	HexToStr(index, (char *)&srcStruct.cell_id, 4);
	index += 4 * 2;


	return resultStruct;
}

//数据上报:温湿度上报
AepString temperature_humidity_report_CodeDataReport (temperature_humidity_report srcStruct)
{
	char* index;
	AepString resultStruct;
	unsigned short tempLen;

	unsigned short payloadLen = 8;
	resultStruct.len = (1 + 2 + 2 + payloadLen) * 2;
	resultStruct.str = (char *)malloc(resultStruct.len + 1);
	memset(resultStruct.str, 0, resultStruct.len + 1);

	srcStruct.temperature_data = aep_htonf(srcStruct.temperature_data);
	srcStruct.humidity_data = aep_htonf(srcStruct.humidity_data);

	index = resultStruct.str;

	memcpy(index, "02", 2);
	index += 1 * 2;

	tempLen = aep_htons(4);//服务ID
	HexToStr(index, (char *)&tempLen, 2);
	index += 2 * 2;

	tempLen = aep_htons(payloadLen);
	HexToStr(index, (char *)&tempLen, 2);
	index += 2 * 2;

	HexToStr(index, (char *)&srcStruct.temperature_data, 4);
	index += 4 * 2;

	HexToStr(index, (char *)&srcStruct.humidity_data, 4);
	index += 4 * 2;


	return resultStruct;
}

//数据上报:温湿度和传感器数据
AepString ths_data_CodeDataReport (ths_data srcStruct)
{
	char* index;
	AepString resultStruct;
	unsigned short tempLen;

	unsigned short payloadLen = 11;
	resultStruct.len = (1 + 2 + 2 + payloadLen) * 2;
	resultStruct.str = (char *)malloc(resultStruct.len + 1);
	memset(resultStruct.str, 0, resultStruct.len + 1);

	srcStruct.humidity_data = aep_htonf(srcStruct.humidity_data);
	srcStruct.temperature_data = aep_htonf(srcStruct.temperature_data);

	index = resultStruct.str;

	memcpy(index, "02", 2);
	index += 1 * 2;

	tempLen = aep_htons(5);//服务ID
	HexToStr(index, (char *)&tempLen, 2);
	index += 2 * 2;

	tempLen = aep_htons(payloadLen);
	HexToStr(index, (char *)&tempLen, 2);
	index += 2 * 2;

	HexToStr(index, (char *)srcStruct.sensor_data, 3);
	index += 3 * 2;

	HexToStr(index, (char *)&srcStruct.humidity_data, 4);
	index += 4 * 2;

	HexToStr(index, (char *)&srcStruct.temperature_data, 4);
	index += 4 * 2;


	return resultStruct;
}

AepCmdData decodeCmdDownFromStr(char* source)
{
	char* index;
	AepCmdData result;
	char cmdType;
	unsigned short serviceId;
	unsigned short payloadLen;

	memset(&result, 0, sizeof(AepCmdData));

	index = source;

	//解析指令类型
	StrToHex(&cmdType, index, 1);
	index += 1 * 2;
	if (cmdType != 0x06)
	{
		result.code = AEP_CMD_INVALID_DATASET_TYPE;
	}

	//服务Id解析
	StrToHex((char *)&serviceId, index, 2);
	serviceId = aep_htons(serviceId);
	index += 2 * 2;

	StrToHex((char *)&result.taskId, index, 2);
	result.taskId = aep_htons(result.taskId);
	index += 2 * 2;

	//payload长度解析
	StrToHex((char *)&payloadLen, index, 2);
	payloadLen = aep_htons(payloadLen);
	index += 2 * 2;

	if (strlen(index) < payloadLen * 2)
	{
		result.code = AEP_CMD_PAYLOAD_PARSING_FAILED;
		return result;
	}


	if (serviceId == 8001)
	{
		result.serviceIdentifier = "motor_control";
		result.data = malloc(sizeof(motor_control));
		memset(result.data, 0, sizeof(motor_control));
		result.code = motor_control_DecodeCmdDown(index, (motor_control*)result.data);
	}
	else if (serviceId == 8006)
	{
		result.serviceIdentifier = "motor_led0";
		result.data = malloc(sizeof(motor_led0));
		memset(result.data, 0, sizeof(motor_led0));
		result.code = motor_led0_DecodeCmdDown(index, (motor_led0*)result.data);
	}
	else if (serviceId == 8002)
	{
		result.serviceIdentifier = "report_control";
		result.data = malloc(sizeof(report_control));
		memset(result.data, 0, sizeof(report_control));
		result.code = report_control_DecodeCmdDown(index, (report_control*)result.data);
	}
	else if (serviceId == 8003)
	{
		result.serviceIdentifier = "report_period_set";
		result.data = malloc(sizeof(report_period_set));
		memset(result.data, 0, sizeof(report_period_set));
		result.code = report_period_set_DecodeCmdDown(index, (report_period_set*)result.data);
	}
	else 
	{
		result.serviceIdentifier = NULL;
		result.data = malloc(payloadLen);
		memset(result.data, 0, sizeof(payloadLen));
		StrToHex((char *)result.data, index, payloadLen);
		result.code = AEP_CMD_INVALID_DATASET_IDENTIFIER;
	}

	return result;
}

AepCmdData decodeCmdDownFromBytes(char* source, int len)
{
	char * str = malloc(len * 2 + 1);
	AepCmdData result;
	HexToStr(str, source, len);
	str[len * 2] = 0;
	
	result = decodeCmdDownFromStr(str);
	free(str);
	return result;
}

AepString codeDataReportByIdToStr (int serviceId, void * srcStruct)
{
	if (serviceId == 3)
	{
		return info_report_CodeDataReport(*(info_report*)srcStruct);
	}
	else if (serviceId == 9001)
	{
		return motor_control_ack_CodeCmdResponse(*(motor_control_ack*)srcStruct);
	}
	else if (serviceId == 9006)
	{
		return motor_led0_ack_CodeCmdResponse(*(motor_led0_ack*)srcStruct);
	}
	else if (serviceId == 9002)
	{
		return report_control_ack_CodeCmdResponse(*(report_control_ack*)srcStruct);
	}
	else if (serviceId == 9003)
	{
		return report_period_set_ack_CodeCmdResponse(*(report_period_set_ack*)srcStruct);
	}
	else if (serviceId == 1)
	{
		return sensor_report_CodeDataReport(*(sensor_report*)srcStruct);
	}
	else if (serviceId == 2)
	{
		return signal_report_CodeDataReport(*(signal_report*)srcStruct);
	}
	else if (serviceId == 4)
	{
		return temperature_humidity_report_CodeDataReport(*(temperature_humidity_report*)srcStruct);
	}
	else if (serviceId == 5)
	{
		return ths_data_CodeDataReport(*(ths_data*)srcStruct);
	}
	else 
	{
		AepString result = {0};
		return result;
	}
}

AepBytes codeDataReportByIdToBytes(int serviceId, void * srcStruct)
{
	AepString temp = codeDataReportByIdToStr(serviceId, srcStruct);
	AepBytes result = {0};
	result.len = temp.len / 2;
	if (result.len > 0)
	{
		result.str = malloc(result.len);
		StrToHex(result.str, temp.str, result.len);
		free(temp.str);
	}
	return result;
}

AepString codeDataReportByIdentifierToStr (char* serviceIdentifier, void * srcStruct)
{
	if (strcmp(serviceIdentifier, "info_report") == 0)
	{
		return info_report_CodeDataReport(*(info_report*)srcStruct);
	}
	else if (strcmp(serviceIdentifier, "motor_control_ack") == 0)
	{
		return motor_control_ack_CodeCmdResponse(*(motor_control_ack*)srcStruct);
	}
	else if (strcmp(serviceIdentifier, "motor_led0_ack") == 0)
	{
		return motor_led0_ack_CodeCmdResponse(*(motor_led0_ack*)srcStruct);
	}
	else if (strcmp(serviceIdentifier, "report_control_ack") == 0)
	{
		return report_control_ack_CodeCmdResponse(*(report_control_ack*)srcStruct);
	}
	else if (strcmp(serviceIdentifier, "report_period_set_ack") == 0)
	{
		return report_period_set_ack_CodeCmdResponse(*(report_period_set_ack*)srcStruct);
	}
	else if (strcmp(serviceIdentifier, "sensor_report") == 0)
	{
		return sensor_report_CodeDataReport(*(sensor_report*)srcStruct);
	}
	else if (strcmp(serviceIdentifier, "signal_report") == 0)
	{
		return signal_report_CodeDataReport(*(signal_report*)srcStruct);
	}
	else if (strcmp(serviceIdentifier, "temperature_humidity_report") == 0)
	{
		return temperature_humidity_report_CodeDataReport(*(temperature_humidity_report*)srcStruct);
	}
	else if (strcmp(serviceIdentifier, "ths_data") == 0)
	{
		return ths_data_CodeDataReport(*(ths_data*)srcStruct);
	}
	else 
	{
		AepString result = {0};
		return result;
	}
}

AepBytes codeDataReportByIdentifierToBytes(char* serviceIdentifier, void * srcStruct)
{
	AepString temp = codeDataReportByIdentifierToStr(serviceIdentifier, srcStruct);
	AepBytes result = {0};
	result.len = temp.len / 2;
	if (result.len > 0)
	{
		result.str = malloc(result.len);
		StrToHex(result.str, temp.str, result.len);
		free(temp.str);
	}
	return result;
}

