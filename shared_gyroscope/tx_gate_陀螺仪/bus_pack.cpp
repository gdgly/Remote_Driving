#include <string.h>

#include "Std_Types.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>

#include "Std_Types.h"
#include "COMM/inirw.h"

uint32 gComNumber = 0;
uint32 gComBaud = 115200;
uint32 gCanNumber = 2;
uint32 gCanBaud = 250000;
sint8 gCloudIpStr[16] = {0};
uint32 gCloudIp = 0;
uint32 gCloudPort = 0;
sint8 gLocalIpStr[16] = {0};
uint32 gLocalIp = 0;
uint32 gLocalPort = 0;
sint8 gVehicleIDStr[32] = {0};
sint8 gIMEINum[17] = {0};

sint8 get_xor_sum(sint8 * buf, uint32 count)
{
    sint8 sum=0;
    uint32 i;
    for(i=0; i< count; i++)
        sum ^= buf[i];
    return sum;
}


void load_config_file()
{
	iniFileLoad("./config");

	//读取串口配置
	gComNumber = iniGetInt("RS232","COM_NUM",0);
	gComBaud = iniGetInt("RS232","COM_BAUD",115200);

	//读取CAN配置
	gCanNumber = iniGetInt("CAN","CAN_NUM",2);
	gCanBaud = iniGetInt("CAN","CAN_BAUD",250000);

	//读取TCP server 配置
	iniGetString("TCP","CLOUD_IP",gCloudIpStr,16,"114.216.201.153");
	gCloudIp = inet_addr(gCloudIpStr);
	gCloudIp = ntohl(gCloudIp);
	gCloudPort = iniGetInt("TCP","CLOUD_PORT",54323);

	//读取车辆ID
	iniGetString("VEHICLE_ID","VEHICLE_ID",gVehicleIDStr,32,"855585021100001");

	iniFileFree();
}

void load_config_file_2()
{
	FILE *fp = NULL;
	uint32 dwFileLen = 0;
	sint8 *FileData = NULL;

	fp = fopen("./config","r");

	if (NULL == fp)
	{
		return;
	}

	fseek(fp, 0, SEEK_END);
	dwFileLen = ftell(fp);

	FileData = (sint8 *)malloc(dwFileLen+1);

	if (NULL == FileData)
	{
		return;
	}

	memset(FileData,0,dwFileLen+1);
	fseek(fp, 0, SEEK_SET);

	while (fgets(FileData, dwFileLen, fp) != NULL)
	{
		if (strstr(FileData,"COM_NUM"))
		{
			strsep(&FileData, "=");
			gComNumber = atoi(FileData);
		}
		else if (strstr(FileData, "COM_BAUD"))
		{
			strsep(&FileData, "=");
			gComBaud = atoi(FileData);
		}
		else if (strstr(FileData, "CAN_NUM"))
		{
			strsep(&FileData, "=");
			gCanNumber = atoi(FileData);
		}
		else if (strstr(FileData, "CAN_BAUD"))
		{
			strsep(&FileData, "=");
			gCanBaud = atoi(FileData);
		}
		else if (strstr(FileData, "CLOUD_IP"))
		{
			strsep(&FileData, "=");
			strcpy(gCloudIpStr,FileData);
			for (int i = 0; i < 16; i++)
			{
				if (gCloudIpStr[i] == '\n')
					gCloudIpStr[i] = '\0';
			}
			gCloudIp = inet_addr(gCloudIpStr);
			gCloudIp = ntohl(gCloudIp);
		}
		else if (strstr(FileData, "CLOUD_PORT"))
		{
			strsep(&FileData, "=");
			gCloudPort = atoi(FileData);
		}
		else if (strstr(FileData, "LOCAL_IP"))
		{
			strsep(&FileData, "=");
			strcpy(gLocalIpStr,FileData);
			for (int i = 0; i < 16; i++)
			{
				if (gLocalIpStr[i] == '\n')
					gLocalIpStr[i] = '\0';
			}
			gLocalIp = inet_addr(gLocalIpStr);
			gLocalIp = ntohl(gLocalIp);
		}
		else if (strstr(FileData, "LOCAL_PORT"))
		{
			strsep(&FileData, "=");
			gLocalPort = atoi(FileData);
		}
		else if (strstr(FileData, "VEHICLE_ID"))
		{
			strsep(&FileData, "=");
			strcpy(gVehicleIDStr,FileData);
			for (int i = 0; i < 18; i++)
			{
				if (gVehicleIDStr[i] == '\n')
					gVehicleIDStr[i] = '\0';
			}
			memcpy(gVehicleIDStr,gVehicleIDStr,17);
		}
		else if (strstr(FileData, "IMEI"))
		{
			strsep(&FileData, "=");
			strcpy(gIMEINum,FileData);
			for (int i = 0; i < 16; i++)
			{
				if (gIMEINum[i] == '\n')
					gIMEINum[i] = '\0';
			}
		}
	}

	fclose(fp);
	return;
}

