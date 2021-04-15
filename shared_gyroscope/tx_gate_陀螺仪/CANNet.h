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

typedef struct rgate_control_cmd_struct
{
    uint8 eps_ctl_mode;
    uint8 eps1;
    uint8 max_angular_velocity;
    uint16 steering_whell_angular_cmd;

    uint8 acceleration_cmd;

    uint8 AEB_collide_level;
    uint8 electrical_park_brake_cmd;
    uint8 dw;

    uint8 eps2;
    uint8 emergency_brake_cmd;

    uint8 selfdrive_ctl_cmd;
    uint8 door_ctl_cmd;
    uint8 beep_ctl;
    uint8 day_drivinglight;
    uint8 zzxd;
    uint8 yzxd;
    uint8 jsd;
    uint8 skd;
    uint8 dd;
    uint8 hwd;
    uint8 zwd;
    uint8 ywd;
    uint8 ndd;
    uint8 fwd;

    uint8 remotedriving_status;
    uint8 remotedriving_reliability;
    uint8 remotedriving_fault_level;
    uint8 air_condition_ctl;
    uint8 air_condition_temp;
    uint8 eps3;
} RGATE_CTRL_CMD_STRUCT;

//接收CAN消息函数接口
void CANNet_RecvMsgCAN_2(MCANDrv_infoMsgS_TYPE infoMsgS);

//发送CAN数据
void TCP_To_CAN_Send(uint8 *buf, uint32 len);

#endif