/**
* \file       can_msg_process.c
* \author     yangkun
* \date       2019/09/07
* \version    Version 1.0
* \brief
* \note       Copyright (c) 2000-2020
* \remarks    certification functions
******************************************************************************/

#include <time.h>
#include <stdio.h>
#include "tcp.h"
#include "udp.h"
#include "../COMM/macro_def.h"
#include "../NET/UdpClient.h"
#include "../CANNet.h"
#include "can_msg_process.h"
#include "../COMM/sky_trace.h"


//#define REMOTE_IPADDR_STR			"192.168.0.11"		//for test
//#define REMOTE_IPADDR				0xC0A8000b			//for test
#define REMOTE_IPADDR_STR			"114.216.201.153"
#define REMOTE_IPADDR				0x72D8C999
#define REMOTE_PORT					54323

#define LOCAL_IPADDR_STR			"192.168.0.31"		//for test
#define LOCAL_ADDR					0xC0A8001f			//for test
//#define LOCAL_IPADDR_STR			"192.168.0.150"
//#define LOCAL_ADDR					0xC0A80096
#define LOCAL_PORT_CTRL				10000
#define LOCAL_PORT_STATUS			10011

#define CAN_ID_VEHICLE_POW_STATE		0x1804A0B0
#define CAN_ID_VEHICLE_LAMP_STATE		0x1806A0B0
#define CAN_ID_VEHICLE_ELEC_STATE		0x1810A0B0
#define CAN_ID_VEHICLE_BAT_STATE		0x1811A0B0
#define CAN_ID_VEHICLE_STATE4			0x1812A0B0
#define CAN_ID_VEHICLE_FAULT			0x1813A0B0
#define CAN_ID_VEHICLE_EPS_CMD			0x1801B0C0
#define CAN_ID_VEHICLE_SPEED_CMD		0x1803B0C0
#define CAN_ID_VEHICLE_CTRL_CMD			0x1805B0C0

#define CAN_ID_TRUCK_POW_STATE			0x18F101D0
#define CAN_ID_TRUCK_ENDU_STATE			0x18F103D0
#define CAN_ID_TRUCK_ACC_STATE			0x18F104D0
#define CAN_ID_TRUCK_ELEC_STATE			0x18F501F0
#define CAN_ID_TRUCK_ELEC_STATE2		0x18F502F0
#define CAN_ID_TRUCK_TOTOALMILES		0x18F40217
#define CAN_ID_TRUCK_LAMP_STATE			0x18F40117

#define MSG_TYPE_GPS				0xAAAAAAAA
#define MSG_TYPE_CAN				0xABABABAB
#define MSG_TYPE_PASS				0xACACACAC

#define CAN_TYPE_TRUCK				0x01
#define CAN_TYPE_REMOTE				0x02

#define CAN_PKT_LEN					13

#define VEHICLE_ID_LEN				16
#define VEHICLE_ID_STRLEN			32

#define GPS_MSG_LEN					49
#define CAN_MSG_LEN					41
#define PASS_MSG_LEN				45

T_TCPSERVER_INFO tTcpServCan = {-1};

CHAR gStrVehicleID[VEHICLE_ID_STRLEN] = {0};
WORD32 gSequenceNumber = 0;
extern CUdpClient udp_sky_cloud;

extern uint32 gCloudIp;
extern uint32 gCloudPort;
extern char gCloudIpStr[];

T_REMOTE_VEHICLE_POW_STATE tRemotePowerState = {0};
T_REMOTE_VEHICLE_LAMP_STATE tRemoteLampState = {0};
T_REMOTE_VEHICLE_BAT_STATE tRemoteBatteryState = {0};
T_REMOTE_VEHICLE_ELEC_STATE tRemoteElectricState = {0};
T_REMOTE_VEHICLE_CTRL_CMD tRemoteControlCmd = {0};
T_REMOTE_VEHICLE_SPEED_CMD tRemoteSpeedCmd = {0};
T_REMOTE_VEHICLE_EPS_CTRLCMD tRemoteEPSCmd = {0};

SWORD32 parse_truck_pow_state_msg(BYTE *pCanPkt)
{
	WORD16 wSpeed = *((WORD16 *)(pCanPkt+1));
	BYTE ucGear = pCanPkt[3] & 0x07;

	if (ucGear == 1)
	{
		ucGear = 2;
	}
	else if (ucGear > 1)
	{
		ucGear = 1;
	}

	tRemoteLampState.ucSpeed = (BYTE)wSpeed & (0xFF);

	tRemoteLampState.dwSegmentFlag |= REMOTE_LAMP_SPEED_FLAG;
	
	tRemotePowerState.ucGearState = ucGear;

	tRemotePowerState.dwSegmentFlag |= REMOTE_POW_GEAR_FLAG;

	return 0;
}

SWORD32 parse_truck_endu_state_msg(BYTE *pCanPkt)
{
	WORD16 wTorque = *((WORD16 *)(pCanPkt + 6));

	tRemotePowerState.wTorque = wTorque;

	tRemotePowerState.dwSegmentFlag |= REMOTE_POW_TORQUE_FLAG;

	return 0;
}

SWORD32 parse_truck_accelerate_state_msg(BYTE *pCanPkt)
{
	WORD16 wAcc = *((WORD16 *)(pCanPkt + 6));

	wAcc = wAcc - 1000 + 128;

	tRemotePowerState.ucAccelerate = (BYTE)wAcc & 0xFF;

	tRemotePowerState.dwSegmentFlag |= REMOTE_POW_ACC_FLAG;

	return 0;
}

SWORD32 parse_truck_electric_state_msg(BYTE *pCanPkt)
{
	BYTE ucMotiveMachineryTemp = pCanPkt[0];
	BYTE ucMotiveCtrlTemp = pCanPkt[1];
	WORD16 wVoltage = *((WORD16 *)(pCanPkt + 2));

	tRemoteBatteryState.ucElectricMachineryTemp = ucMotiveMachineryTemp;
	tRemoteBatteryState.ucElectricCtrlTemp = ucMotiveCtrlTemp;

	tRemoteBatteryState.dwSegmentFlag |= REMOTE_BAT_MACHINERYTEMP_FLAG;
	tRemoteBatteryState.dwSegmentFlag |= REMOTE_BAT_CONTROLTEMP_FLAG;

	tRemoteElectricState.wMotiveVoltage = wVoltage;

	tRemoteElectricState.dwSegmentFlag |= REMOTE_ELECTRIC_VOLTAGE_FLAG;

	return 0;
}

SWORD32 parse_truck_electric_state2_msg(BYTE * pCanPkt)
{
	WORD16 wElecSpeed = *((WORD16 *)(pCanPkt));
	WORD16 wTorque = *((WORD16 *)(pCanPkt + 2));
	WORD16 wCurrent = *((WORD16 *)(pCanPkt + 6));

	wCurrent = (wCurrent -3200)/2 + 500;
	
	tRemotePowerState.wTorque = wTorque;
	tRemotePowerState.wRPM = wElecSpeed + 15000;

	tRemotePowerState.dwSegmentFlag |= REMOTE_POW_TORQUE_FLAG;
	tRemotePowerState.dwSegmentFlag |= REMOTE_POW_RPM_FLAG;

	tRemoteElectricState.wMotiveCurrent = wCurrent;

	tRemoteElectricState.dwSegmentFlag |= REMOTE_ELECTRIC_CURRENT_FLAG;

	return 0;
}

SWORD32 parse_truck_totalmiles_msg(BYTE *pCanPkt)
{
	WORD32 dwTotalmiles = *((WORD32 *)pCanPkt);

	dwTotalmiles /= 4;

	tRemoteLampState.wTotalMiles = (WORD16)dwTotalmiles & 0xFFFF;
	tRemoteLampState.dwSegmentFlag |= REMOTE_LAMP_TOTOALMILES_FLAG;

	return 0;
}

SWORD32 parse_truck_lamp_msg(BYTE *pCanPkt)
{
	BYTE ucRightturnLamp = (pCanPkt[1] & 0x80) >> 7;
	BYTE ucLeftturnLamp = (pCanPkt[1] & 0x40) >> 6;
	BYTE ucPosLamp = (pCanPkt[1] & 0x20) >> 5;
	BYTE ucBackDoorStatus = (pCanPkt[1] & 0x04) >> 2;
	BYTE ucFrontDoorStatus = (pCanPkt[1] & 0x02) >> 1;
	BYTE ucMidDoorStatus = (pCanPkt[1] & 0x01);
	BYTE ucBackFogLamp = (pCanPkt[5] & 0x10) >> 4;
	BYTE ucFrontFogLamp = (pCanPkt[5] & 0x08) >> 3;
	BYTE ucNearHeadLamp = (pCanPkt[5] & 0x04) >> 2;
	BYTE ucFarHeadLamp = (pCanPkt[5] & 0x02) >> 1;

	tRemoteLampState.ucDriveLamp = ucNearHeadLamp;
	tRemoteLampState.ucLeftTurnLamp = ucLeftturnLamp;
	tRemoteLampState.ucRightTurnLamp = ucRightturnLamp;
	tRemoteLampState.ucPosLamp = ucPosLamp;
	tRemoteLampState.ucHeadLamp = ((ucFarHeadLamp == 1) ? 2:1);
	tRemoteLampState.ucBackFogLamp = ucBackFogLamp;
	tRemoteLampState.ucLeftFogLamp = ucFrontFogLamp;
	tRemoteLampState.ucRightFogLamp = ucFrontFogLamp;
	tRemoteLampState.ucDoorState = (ucBackDoorStatus | ucFrontDoorStatus | ucMidDoorStatus);

	tRemoteLampState.dwSegmentFlag |= REMOTE_LAMP_DRIVELAMP_FLAG;
	tRemoteLampState.dwSegmentFlag |= REMOTE_LAMP_LEFTLAMP_FLAG;
	tRemoteLampState.dwSegmentFlag |= REMOTE_LAMP_RIGHTLAMP_FLAG;
	tRemoteLampState.dwSegmentFlag |= REMOTE_LAMP_POSLAMP_FLAG;
	tRemoteLampState.dwSegmentFlag |= REMOTE_LAMP_HEADLAMP_FLAG;
	tRemoteLampState.dwSegmentFlag |= REMOTE_LAMP_BACKFOGLAMP_FLAG;
	tRemoteLampState.dwSegmentFlag |= REMOTE_LAMP_LEFTFOGLAMP_FLAG;
	tRemoteLampState.dwSegmentFlag |= REMOTE_LAMP_RIGHTFOGLAMP_FLAG;
	tRemoteLampState.dwSegmentFlag |= REMOTE_LAMP_DOORSTATE_FLAG;

	return 0;
}

SWORD32 parse_truck_can_msg(BYTE *pCanPkt)
{
	SWORD32 swRet = -1;
	WORD32 dwCanID = 0;
	if (NULL == pCanPkt)
	{
		SKY_TRACE_ERROR("Input Param Error!\n");
		return -1;
	}

	dwCanID = *((WORD32 *)pCanPkt);
	pCanPkt += 4;
	switch (dwCanID)
	{
		case CAN_ID_TRUCK_POW_STATE:
			swRet = parse_truck_pow_state_msg(pCanPkt);
			break;
		case CAN_ID_TRUCK_ENDU_STATE:
			swRet = parse_truck_endu_state_msg(pCanPkt);
			break;
		case CAN_ID_TRUCK_ACC_STATE:
			swRet = parse_truck_accelerate_state_msg(pCanPkt);
			break;
		case CAN_ID_TRUCK_ELEC_STATE:
			swRet = parse_truck_electric_state_msg(pCanPkt);
			break;
		case CAN_ID_TRUCK_ELEC_STATE2:
			swRet = parse_truck_electric_state2_msg(pCanPkt);
			break;
		case CAN_ID_TRUCK_TOTOALMILES:
			swRet = parse_truck_totalmiles_msg(pCanPkt);
			break;
		case CAN_ID_TRUCK_LAMP_STATE:
			swRet = parse_truck_lamp_msg(pCanPkt);
			break;
		default:
			break;
	}
	return -1;
}

SWORD32 construct_remote_power_state_msg(BYTE *pCanPkt)
{
	WORD32 dwIdx = 0;
	WORD32 dwCanID = CAN_ID_VEHICLE_POW_STATE;
	BYTE ucCheckSum = 0;

	MEMCPY(pCanPkt, &dwCanID, 4);
	MEMCPY(&pCanPkt[5], &tRemotePowerState.wRPM, 2);
	MEMCPY(&pCanPkt[7], &tRemotePowerState.wTorque, 2);

	pCanPkt[4] |= tRemotePowerState.ucGearState<<2;
	pCanPkt[10] = tRemotePowerState.ucAccelerate;

	for (dwIdx = 4; dwIdx < 11; dwIdx++)
	{
		ucCheckSum ^= pCanPkt[dwIdx];
	}

	pCanPkt[11] = ucCheckSum;

	return 0;
}

SWORD32 construct_remote_lamp_state_msg(BYTE *pCanPkt)
{
	WORD32 dwCanID = CAN_ID_VEHICLE_LAMP_STATE;
	MEMCPY(pCanPkt, &dwCanID, 4);
	MEMCPY(&pCanPkt[9], &tRemoteLampState.wTotalMiles, 2);

	pCanPkt[4] |= tRemoteLampState.ucDriveMode;
	pCanPkt[4] |= tRemoteLampState.ucDoorState<<2;
	pCanPkt[4] |= tRemoteLampState.ucDriveLamp<<3;
	pCanPkt[4] |= tRemoteLampState.ucLeftTurnLamp<<4;
	pCanPkt[4] |= tRemoteLampState.ucRightTurnLamp<<5;
	pCanPkt[4] |= tRemoteLampState.ucWarningLamp<<6;
	pCanPkt[4] |= tRemoteLampState.ucPosLamp<<7;

	pCanPkt[5] |= tRemoteLampState.ucHeadLamp;
	pCanPkt[5] |= tRemoteLampState.ucBackFogLamp<<2;
	pCanPkt[5] |= tRemoteLampState.ucLeftFogLamp<<3;
	pCanPkt[5] |= tRemoteLampState.ucRightFogLamp<<4;
	pCanPkt[5] |= tRemoteLampState.ucTopLamp<<5;
	pCanPkt[5] |= tRemoteLampState.ucAmbientLamp<<6;

	pCanPkt[6] = tRemoteLampState.ucSpeed;
	pCanPkt[7] = tRemoteLampState.ucSOC;
	pCanPkt[11] = tRemoteLampState.ucKeepAlive;

	return 0;
}

SWORD32 construct_remote_electric_state_msg(BYTE *pCanPkt)
{
	WORD32 dwCanID = CAN_ID_VEHICLE_ELEC_STATE;
	MEMCPY(pCanPkt, &dwCanID, 4);
	MEMCPY(&pCanPkt[4],&tRemoteElectricState.wMotiveVoltage,2);
	MEMCPY(&pCanPkt[6],&tRemoteElectricState.wMotiveCurrent,2);

	return 0;
}

SWORD32 construct_remote_battery_state_msg(BYTE *pCanPkt)
{
	WORD32 dwCanID = CAN_ID_VEHICLE_BAT_STATE;
	
	MEMCPY(pCanPkt, &dwCanID, 4);
	
	pCanPkt[9] = tRemoteBatteryState.ucElectricMachineryTemp;
	pCanPkt[10] = tRemoteBatteryState.ucElectricCtrlTemp;

	return 0;
}

SWORD32 parse_remote_can_msg(BYTE *pCanPkt)
{
	WORD32 dwCanID = 0;
	if (NULL == pCanPkt)
	{
		SKY_TRACE_ERROR("Input Param Error!\n");
		return -1;
	}

	dwCanID = *((WORD32 *)pCanPkt);
	switch (dwCanID)
	{
		case CAN_ID_VEHICLE_POW_STATE:
			break;
		case CAN_ID_VEHICLE_LAMP_STATE:
			break;
		case CAN_ID_VEHICLE_ELEC_STATE:
			break;
		case CAN_ID_VEHICLE_BAT_STATE:
			break;
		case CAN_ID_VEHICLE_EPS_CMD:
			break;
		case CAN_ID_VEHICLE_SPEED_CMD:
			break;
		case CAN_ID_VEHICLE_CTRL_CMD:
			break;
		default:
			break;
	}
	
	return -1;
}

SWORD32 parse_src_can_msg(BYTE *pCanPkt, WORD32 dwType)
{
	SWORD32 swRet = -1;

	if ((NULL == pCanPkt) || ((dwType != CAN_TYPE_TRUCK) && (dwType != CAN_TYPE_REMOTE)))
	{
		SKY_TRACE_ERROR("Input Param Error!\n");
		return -1;
	}

	if (dwType == CAN_TYPE_TRUCK)
	{
		swRet = parse_truck_can_msg(pCanPkt);
	}
	else if (dwType = CAN_TYPE_REMOTE)
	{
		swRet = parse_remote_can_msg(pCanPkt);
	}

	return swRet;
}

void construct_gps_msg(BYTE *pGpsMsg, WORD32 dwLongtitude, WORD32 dwLatitude)
{
	WORD32 dwIdx = 0;
	WORD16 wCheckSum = 0;
	WORD32 dwMsgType = MSG_TYPE_GPS;

	if (NULL == pGpsMsg)
	{
		SKY_TRACE_ERROR("Input Param Error!\n");
		return;
	}

	MEMCPY(pGpsMsg,gStrVehicleID,VEHICLE_ID_LEN);
	MEMCPY(&pGpsMsg[16],&gSequenceNumber,SIZEOF(WORD32));
	MEMCPY(&pGpsMsg[20],&dwMsgType,SIZEOF(WORD32));

	pGpsMsg[24] = 0x14;
	pGpsMsg[25] = 0x00;
	pGpsMsg[28] = 0xAA;
	pGpsMsg[29] = 0x14;
	pGpsMsg[31] = 0x01;

	dwLatitude = htonl(dwLatitude);
	dwLongtitude = htonl(dwLongtitude);
	MEMCPY(&pGpsMsg[37], &dwLatitude, SIZEOF(WORD32));
	MEMCPY(&pGpsMsg[41], &dwLongtitude, SIZEOF(WORD32));

	for (;dwIdx < GPS_MSG_LEN; dwIdx++)
	{
		wCheckSum += pGpsMsg[dwIdx];
	}

	MEMCPY(&pGpsMsg[26],&wCheckSum,SIZEOF(WORD16));

	SKY_TRACE_INFO("Vehicle ID: %s Sequence Number: %u Message Type: %X ",gStrVehicleID,gSequenceNumber,dwMsgType);
	SKY_TRACE_INFO("Latitude: %u Longtitude: %u",dwLatitude,dwLongtitude);

	gSequenceNumber++;

	return;
}

void construct_can_msg(BYTE *pCanSendPkt,BYTE *pSrcCanPkt)
{
	
	WORD32 dwIdx = 0;
	WORD16 wCheckSum = 0;
	WORD32 dwMsgType = MSG_TYPE_CAN;

	if ((NULL == pCanSendPkt) || (NULL == pSrcCanPkt))
	{
		SKY_TRACE_ERROR("Input Param Error!\n");
		return;
	}

	MEMCPY(pCanSendPkt,gStrVehicleID,VEHICLE_ID_LEN);
	MEMCPY(&pCanSendPkt[16],&gSequenceNumber,SIZEOF(WORD32));
	MEMCPY(&pCanSendPkt[20],&dwMsgType,SIZEOF(WORD32));

	pCanSendPkt[24] = 0x0D;
	pCanSendPkt[25] = 0x00;

	MEMCPY(&pCanSendPkt[28],pSrcCanPkt,CAN_PKT_LEN);
	
	for (;dwIdx < CAN_MSG_LEN; dwIdx++)
	{
		wCheckSum += pCanSendPkt[dwIdx];
	}

	MEMCPY(&pCanSendPkt[26],&wCheckSum,SIZEOF(WORD16));

	SKY_TRACE_INFO("Vehicle ID: %s Sequence Number: %u Message Type: %X ",gStrVehicleID,gSequenceNumber,dwMsgType);
	SKY_TRACE_INFO("CAN Data: %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X",
					pSrcCanPkt[0],pSrcCanPkt[1],pSrcCanPkt[2],pSrcCanPkt[3],pSrcCanPkt[4],
					pSrcCanPkt[5],pSrcCanPkt[6],pSrcCanPkt[7],pSrcCanPkt[8],pSrcCanPkt[9],
					pSrcCanPkt[10],pSrcCanPkt[11],pSrcCanPkt[12]);
	
	gSequenceNumber++;

	return;
}

void construct_passenger_msg(BYTE * pPassSendPkt,WORD32 dwInPass, WORD32 dwUpPass, WORD32 dwDownPass)
{
	WORD32 dwIdx = 0;
	WORD16 wCheckSum = 0;
	WORD32 dwTime = 0;
	WORD32 dwMsgType = MSG_TYPE_PASS;
	struct timeval tv;

	if (NULL == pPassSendPkt)
	{
		SKY_TRACE_ERROR("Input Param Error!\n");
		return;
	}

	MEMCPY(pPassSendPkt,gStrVehicleID,VEHICLE_ID_LEN);
	MEMCPY(&pPassSendPkt[16],&gSequenceNumber,SIZEOF(WORD32));
	MEMCPY(&pPassSendPkt[20],&dwMsgType,SIZEOF(WORD32));

	pPassSendPkt[24] = 0x11;
	pPassSendPkt[25] = 0x00;

	pPassSendPkt[28] = 0xAC;
	pPassSendPkt[29] = 0x0F;
	MEMCPY(&pPassSendPkt[30],&gStrVehicleID[STRLEN(gStrVehicleID)-8],8);
	pPassSendPkt[38] = (BYTE)(dwInPass & 0xFF);
	pPassSendPkt[39] = (BYTE)(dwUpPass & 0xFF);
	pPassSendPkt[40] = (BYTE)(dwDownPass & 0xFF);

	gettimeofday(&tv, NULL);
	dwTime = tv.tv_sec * 1000 + tv.tv_usec/1000;
	MEMCPY(&pPassSendPkt[41],&dwTime,SIZEOF(WORD32));
	
	for (;dwIdx < PASS_MSG_LEN; dwIdx++)
	{
		wCheckSum += pPassSendPkt[dwIdx];
	}

	MEMCPY(&pPassSendPkt[26],&wCheckSum,SIZEOF(WORD16));

	SKY_TRACE_INFO("Vehicle ID: %s Sequence Number: %u Message Type: %X ",gStrVehicleID,gSequenceNumber,dwMsgType);
	SKY_TRACE_INFO("In Passenger: %u Up Passenger: %u Down Passenger: %u TimeSec: %u",dwInPass,dwUpPass,dwDownPass,dwTime);

	gSequenceNumber++;

	return;
}

void * can_msg_process(void *pTcpServParam)
{
	WORD32 dwIdx = 0;
	WORD32 dwSinglePktLen = 13;
	uint8 *cBuf = NULL;
	SWORD32 swRecvDataLen = 0;

	cBuf = (uint8 *)MALLOC(TCP_TRANS_BUF_LEN);
    if (NULL == cBuf)
    {
        goto Exit;
    }

	while (TRUE)
	{
		MEMSET(cBuf, 0, TCP_TRANS_BUF_LEN);
        swRecvDataLen = RECV(((T_TCPSERVER_PARAM*)pTcpServParam)->swClientSock, cBuf, TCP_TRANS_BUF_LEN,0);
		
		if (swRecvDataLen == 0)
		{
			SKY_TRACE_ERROR("Remote disconnected, Receive data len: %u",swRecvDataLen);
			goto Exit;
		}

		if(swRecvDataLen < 0)
		{
			SKY_TRACE_ERROR("Receive data len -1, error number: %u",ERRNO);
			if ((ERRNO == EINTR) || (ERRNO == EWOULDBLOCK) || (ERRNO == EAGAIN))
			{
				continue;
			}
			else
			{
				goto Exit;
			}
		}
		for (dwIdx = 0; dwIdx < (swRecvDataLen / dwSinglePktLen); dwIdx++)
		{
			TCP_To_CAN_Send(cBuf, CAN_PKT_LEN);
		}
	}

	Exit:
		if (NULL != cBuf)
		{
			FREE(cBuf);
		}
	
		if(-1 != ((T_TCPSERVER_PARAM*)pTcpServParam)->swClientSock)
		{
			CLOSE(((T_TCPSERVER_PARAM*)pTcpServParam)->swClientSock);
			((T_TCPSERVER_PARAM*)pTcpServParam)->swClientSock = -1;
		}
	
		return NULL;
}

void * StartServer(void * arg)
{
	PTHREAD_T pServerCtrlThread;
	int policy, priority;
	pthread_getschedparam(pthread_self(), &policy, (struct sched_param *)&priority);
	SKY_TRACE_INFO("StartServer policy=%d, priority=%d\n", policy, priority);

	prctl(PR_SET_NAME, "StartServer");

	while (TRUE)
	{
		if (tTcpServCan.swSock == -1)
		{
			tTcpServCan.swSock = tcp_server_init(gCloudIp, gCloudPort);

			if (tTcpServCan.swSock != -1)
			{
				tTcpServCan.wPort = LOCAL_PORT_CTRL;
				tTcpServCan.pThreadFun = can_msg_process;
				PTHREAD_CREATE(&pServerCtrlThread,0,tcp_server_proc,(void *)&tTcpServCan);
				PTHREAD_DETACH(pServerCtrlThread);
			}
			else
			{
				printf("socket error!\n");
			}
		}
		sleep(5);
	}

}

SWORD32 server_init()
{
	PTHREAD_T pServerCtrlThread;

	PTHREAD_CREATE(&pServerCtrlThread,0,StartServer,NULL);
	PTHREAD_DETACH(pServerCtrlThread);

	return 0;
}

SWORD32 udp_send(CHAR *dst_ip, WORD16 dst_port, CHAR *data, WORD32 data_len)
{
    SWORD32 sock = -1;
    SWORD32 send_len = -1;
    struct sockaddr_in toAddr;
    
    sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if(sock < 0)
    {
        return -1;
    }

    memset(&toAddr,0,sizeof(toAddr));
    toAddr.sin_family = AF_INET;
    toAddr.sin_addr.s_addr = inet_addr(dst_ip);
    toAddr.sin_port = htons(dst_port);

    send_len = sendto(sock, data, data_len, 0, (struct sockaddr*)&toAddr, sizeof(toAddr));

    close(sock);
    
    return send_len;
}


SWORD32 send_gps_msg2cloud(WORD32 dwLongtitude,WORD32 dwLatitude)
{
	SWORD32 swRet = 0;
	BYTE ucGpsMsg[GPS_MSG_LEN] = {0};

	construct_gps_msg(ucGpsMsg, dwLongtitude, dwLatitude);
	
	swRet = udp_sky_cloud.send_udp_data((CHAR*)ucGpsMsg,GPS_MSG_LEN);
	
	return swRet;
}

SWORD32 send_can_msg2cloud(BYTE *pCanPkt,WORD32 dwPktLen)
{
	SWORD32 swRet = 0;
	WORD32 dwIdx = 0;
	BYTE ucCanMsg[CAN_MSG_LEN] = {0};

	for (; dwIdx < (dwPktLen / CAN_PKT_LEN); dwIdx++)
	{
		construct_can_msg(ucCanMsg, pCanPkt);

		swRet = udp_sky_cloud.send_udp_data((CHAR*)ucCanMsg,CAN_MSG_LEN);
	}
	
	return swRet;

}

SWORD32 send_pass_msg2cloud(WORD32 dwInPass,WORD32 dwUpPass,WORD32 dwDownPass)
{
	SWORD32 swRet = 0;
	BYTE ucPassMsg[PASS_MSG_LEN] = {0};

	construct_passenger_msg(ucPassMsg, dwInPass, dwUpPass, dwDownPass);
	
	swRet = udp_sky_cloud.send_udp_data((CHAR*)ucPassMsg,PASS_MSG_LEN);
}

SWORD32 send_can_msg2client(BYTE *pCanPkt,WORD32 dwPktLen)
{
	WORD32 dwIdx = 0;
	SWORD32 swRet = -1;

	if (NULL == pCanPkt)
	{
		SKY_TRACE_ERROR("Input Param Error!\n");
		return -1;
	}

	for (;dwIdx < tTcpServCan.dwClientNum; dwIdx++)
	{
		swRet = SEND(tTcpServCan.swClientSock[dwIdx], pCanPkt, dwPktLen, 0);
		if (swRet <= 0)
		{
			SWORD32 swClientSock[MAX_SOCKET_CONN];
			MEMCPY(swClientSock,&tTcpServCan.swClientSock[dwIdx+1],(tTcpServCan.dwClientNum - dwIdx - 1) * SIZEOF(SWORD32));
			MEMCPY(&tTcpServCan.swClientSock[dwIdx],swClientSock,(tTcpServCan.dwClientNum - dwIdx - 1) * SIZEOF(SWORD32));
			tTcpServCan.dwClientNum--;
		}
	}

	swRet = send_can_msg2cloud(pCanPkt, dwPktLen);

	return swRet;
}
