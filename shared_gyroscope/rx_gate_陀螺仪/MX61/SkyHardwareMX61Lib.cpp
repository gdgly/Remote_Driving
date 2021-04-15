
#include "RS232.h"
#include "IO_AD.h"
#include "SkyCan.h"

#include "SkyHardwareMX61Lib.h"

CSkyCan canUI[4] = CSkyCan(100);
//CSkyCan canUI0[4] = {CSkyCan(100),CSkyCan(100),CSkyCan(100),CSkyCan(100)};
CRS232  rs232UI[4];


//获取通道的电压值，电压值精度1/4095V/bit
uint16 ATDDrv_GetChanResult(uint8 idxChanUB)
{
	return GetADC(idxChanUB);
}

 //获取通道开关状态
 uint8 DINDrv_GetChanState(uint8 idxChanUB)
 {
	 return GetInputIO(idxChanUB);
 }

//bLvlUB表示电平状态，0为低电平，1为高电平
void DODrv_SetChanState(uint8 idxChanUB, uint8 bLvlUB)
{
	SetOutputIO(idxChanUB, bLvlUB);
}

//LED灯控制
uint8 DORrv_TurnOnLed(uint8 led_index, uint8 lamp_status)
{
	TurnOnLed(led_index,lamp_status);
}

uint8 MCANDrv_SetBaudrate(uint8 idxCANUB, uint32 baudrateUI)
{
	if(idxCANUB >= _MCAN_NUM_CHAN )
		return MCAN_Status_wrongParam;
	
	//CAN卡已经初始化了
	if(canUI[idxCANUB].GetCanStatus())
	{
		canUI[idxCANUB].CloseCan();
	}
	return canUI[idxCANUB].init_can(idxCANUB,baudrateUI);
}

//函数MCANDrv_SetCANMode在MX61中不需要，为了兼容易控的库，做了个空壳
uint8 MCANDrv_SetCANMode(uint8 idxCANUB, MCANDrv_idxNodeModeE_TYPE idxNodeModeE)
{
	idxCANUB = 0;
}

//函数MCANDrv_SetMsgObj在MX61中不需要，为了兼容易控的库，做了个空壳
uint8 MCANDrv_SetMsgObj(const MCANDrv_infoMsgConfS_TYPE infoMsgConfS, void *pFunc)
{
	pFunc = 0;
}


uint8 MCANDrv_SetCallbackFunc(uint8 idxCANUB, const pFunc_RxMsgCallback pRxFunc, const pFunc_TxMsgCallback pTxFunc)
{
	if(idxCANUB >= _MCAN_NUM_CHAN )
		return MCAN_Status_wrongParam;

	canUI[idxCANUB].SetReadCallbackFunc(pRxFunc);

	return 0;
}

//CAN的通道号在结构体中
MCAN_stRetE_TYPE MCANDrv_SendMsg(const MCANDrv_infoMsgS_TYPE infMsgS)
{
	if(infMsgS.infoConfS.infoMsgParU.B.CAN >= _MCAN_NUM_CHAN )
		return MCAN_Status_wrongParam;

	if(canUI[infMsgS.infoConfS.infoMsgParU.B.CAN].SendOneFrameData(infMsgS) == 0)
		return MCAN_Status_ok;
	
	return MCAN_Status_notInitialised;
}

//返回1表示有busoff错误
uint8 MCANDrv_CheckIfBusOff(uint8 idxCANUB)
{
	if(idxCANUB >= _MCAN_NUM_CHAN )
		return MCAN_Status_wrongParam;
	return canUI[idxCANUB].GetLastError();
}


//RS232初始化串口
//参数是串口号+波特率
//返回0，初始化正常
uint8 MRS232Drv_SetBaudrate(uint8 idxRS232UB, uint32 baudrateUI)
{
	uint8 ret;

	if(idxRS232UB >= _MRS232_NUM_CHAN )
		return 1;

	ret = rs232UI[idxRS232UB].OpenUart(idxRS232UB + 1);
	if(ret) return ret;

	ret = rs232UI[idxRS232UB].set_speed(baudrateUI);
	if(ret) return ret;

	ret = rs232UI[idxRS232UB].set_Parity(8,1,'N');
	if(ret) return ret;

	return 0;
}

//设置串口通信回调函数
uint8 MRS232Drv_SetCallbackFunc(uint8 idxRS232UB, const pFunc_ReadCallback pRxFunc)
{
	if(idxRS232UB >= _MRS232_NUM_CHAN )
		return 1;

	rs232UI[idxRS232UB].SetReadCallbackFunc(pRxFunc);

	return 0;
}

//串口发送数据函数
//参数：idxRS232UB，串口号，0-3
//		RS232Data，数据指针
//		DataLength, 数据长度
//返回值：负数表示出错，正数表示实际发送数据的长度
uint8 MRS232Drv_SendData(uint8 idxRS232UB, sint8 * RS232Data, const sint32 DataLength)
{
	if(idxRS232UB >= _MRS232_NUM_CHAN )
		return -2;

	return rs232UI[idxRS232UB].ComWrite(RS232Data,DataLength);
}

//串口关闭函数
uint8 MRS232Drv_ClosePort(uint8 idxRS232UB)
{
	if(idxRS232UB >= _MRS232_NUM_CHAN )
		return 1;

	rs232UI[idxRS232UB].CloseUart();
	return 0;
}



