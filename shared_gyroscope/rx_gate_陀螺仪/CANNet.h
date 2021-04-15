/*******************************************************************************
* 工程名称：整车管理
* 文件名： CANNet.h
* 功能描述 ： CAN总线管理头文件
* 引用记录：
* 备注： 
* 修订记录：
*		日期		      版本		  作者		描述
*                                           
*******************************************************************************/

#ifndef __CANNET_H
#define __CANNET_H

#include "./MX61/SkyHardwareMX61Lib.h"

//接收CAN消息函数接口      
void CANNet_RecvMsgCAN_2(MCANDrv_infoMsgS_TYPE infoMsgS);

//发送CAN数据
void TCP_To_CAN_Send(uint8 * buf, uint32 len);

void IMX_TakeOver_Flag_CanSend();
int Can2Kv(unsigned int p_id,uint8 *p_data,unsigned int p_datalen);

#endif