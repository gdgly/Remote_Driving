/*******************************************************************************
* 工程名称：整车管理
* 文件名： CANNet.c
* 功能描述 ： CAN总线管理
* 引用记录：
* 备注： 
* 修订记录：
*		日期		      版本		  作者		描述
*                                             
*******************************************************************************/

#include "CANNet.h"
#include <string.h>
#include <stdio.h>
#include "./NET/UdpClient.h"
#include "./NET/CloudComm.h"

#include "./MX61/SkyHardwareMX61Lib.h"
//#include "./YK/can_msg_process.h"
//#include "./CanPack.h"

extern CUdpClient g_Udp2Client_Dashboard;
extern CloudComm g_CloudComm;
extern uint32 gCanNumber;

// #define MOCK_ENABLE
RGATE_CTRL_CMD_STRUCT g_rgate_ctrl_cmd;

static sint8 g_OldDrivingmode = 20; //20 is a invalid value.
//*********************************************************************************//

//字节1为帧信息。第7位（FF）表示帧格式，在扩展帧中，FF＝1；
//第6位（RTR）表示帧的类型，RTR=0表示为数据帧，RTR=1表示为远程帧；DLC在字节bit3-0，表示在数据帧时实际的数据长度。
//字节2～5为报文识别码，其高29位有效。  B2(b3-0), B3,B4,B5(b7-3)
//字节6～13数据帧的实际数据，远程帧时无效。
void MsgToBuf(MCANDrv_infoMsgS_TYPE *p_infoMsgS, sint8 *buf)
{
    uint8 tmp;
    uint32 id;

    memcpy(buf + 5, p_infoMsgS->datMsgUBA, 8);

    id = p_infoMsgS->infoConfS.infoMsgIDUI;

    tmp = id % 32;
    buf[4] = tmp << 3;
    id = id >> 5;
    tmp = id % 256;
    buf[3] = tmp;
    id = id >> 8;
    tmp = id % 256;
    buf[2] = tmp;
    id = id >> 8;
    tmp = id % 256;
    buf[1] = tmp;

    buf[0] = 0x88; // 扩展帧，数据帧，帧长固定8
}

uint8 GetDrivingmode(sint8 *buf)
{
    return (uint8)(buf[5] & 0x3);
}

sint8 BufToMsg(MCANDrv_infoMsgS_TYPE *p_infoMsgS, uint8 *buf)
{
    int i;
    uint32 id;
    uint8 tmp;

    tmp = buf[0] & 0x0f; //数据长度，bit3-bit0
    if (tmp != 8)        //CAN总线上数据全是数据帧，数据长度为8
        return -1;

    tmp = buf[0] & 0x80; //标准帧、扩展帧，bit7
    if (tmp != 0x80)     //CAN总线上全是扩展帧，ID是29bit
        return -2;

    memcpy(p_infoMsgS->datMsgUBA, buf + 5, 8);

    id = buf[1];
    id = id << 8;
    id += buf[2];
    id = id << 8;
    id += buf[3];
    id = id << 5;
    id += buf[4] >> 3;

    p_infoMsgS->infoConfS.infoMsgIDUI = id;
    p_infoMsgS->infoConfS.infoMsgParU.B.DLC = 8;          //数据长度
    p_infoMsgS->infoConfS.infoMsgParU.B.IDE = 1;          //1扩展帧，0标准帧
    p_infoMsgS->infoConfS.infoMsgParU.B.CAN = gCanNumber; //第几路CAN

    return 0;
}

//1. CAN2接收消息处理函数
//2. 该函数用于接收车端VCU发出的车辆状态数据
void CANNet_RecvMsgCAN_2(MCANDrv_infoMsgS_TYPE infoMsgS)
{
    sint8 send_buf[15];
    uint32 CANNet_infoIDCAN2UI;

    CANNet_infoIDCAN2UI = infoMsgS.infoConfS.infoMsgIDUI;
    //CAN2接收分类处理
    switch (CANNet_infoIDCAN2UI)
    {
    case 0x1804A0B0: //行车状态
    case 0x1806A0B0: //车辆状态1
    case 0x1810A0B0: //车辆状态2
    case 0x1811A0B0: //车辆状态3
    case 0x1812A0B0: //车辆状态4
    case 0x1813A0B0: //车辆故障状态
    case 0x1808A0B0: //车辆状态5
    case 0x1802A0B0: //EPS状态
        MsgToBuf(&infoMsgS, send_buf);
        SKY_TRACE_INFO("车端VCU发出的车辆状态, CANNet_infoIDCAN2UI:0x%x, send_buf:0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x\n",
                       CANNet_infoIDCAN2UI,
                       send_buf[0], send_buf[1], send_buf[2], send_buf[3], send_buf[4],
                       send_buf[5], send_buf[6], send_buf[7], send_buf[8], send_buf[9],
                       send_buf[10], send_buf[11], send_buf[12]);
        g_CloudComm.AddSendData(CANNet_infoIDCAN2UI, send_buf);

        if (CANNet_infoIDCAN2UI == 0x1806A0B0)
        {
            uint8 mode;
            mode = GetDrivingmode(send_buf);
            if (mode != g_OldDrivingmode)
            {
                g_CloudComm.SendDrivingmode(mode);
                g_OldDrivingmode = mode;
            }
        }

        break;
    default:
        //printf("Call YK send_can_msg2client, CAN ID is not need!\n");
        //MsgToBuf(&infoMsgS, send_buf);
        //send_can_msg2client((uint8 *)send_buf,13);  //for test
        break;
    }
}

void process_rgate_eps_cmd(MCANDrv_infoMsgS_TYPE infoMsgS)
{
    //0x1801B0C0: //远程控制器EPS命令
    uint8 *p_buf = infoMsgS.datMsgUBA;
    SKY_TRACE_INFO("0x1801B0C0,远程控制器EPS命令\n");

    g_rgate_ctrl_cmd.eps_ctl_mode = p_buf[0];
    SKY_TRACE_INFO("EPS控制模式: %d\n", g_rgate_ctrl_cmd.eps_ctl_mode);
    g_rgate_ctrl_cmd.eps1 = p_buf[1];
    SKY_TRACE_INFO("心跳值1: %d\n", g_rgate_ctrl_cmd.eps1);
    g_rgate_ctrl_cmd.max_angular_velocity = p_buf[2];
    g_rgate_ctrl_cmd.steering_whell_angular_cmd = p_buf[3] << 8 + p_buf[4];
    SKY_TRACE_INFO("方向盘角度指令: %d\n", g_rgate_ctrl_cmd.steering_whell_angular_cmd);

#ifdef MOCK_ENABLE
    //Feign 0x1802A0B0
    SKY_TRACE_INFO("0x1802A0B0, 整车控制器EPS状态\n");
    infoMsgS.infoConfS.infoMsgIDUI = 0x1802A0B0;
    infoMsgS.datMsgUBA[7] = p_buf[1];
    SKY_TRACE_INFO("整车控制器EPS状态, 心跳值: %d\n", infoMsgS.datMsgUBA[7]);
    CANNet_RecvMsgCAN_2(infoMsgS);
#endif
}

void process_rgate_speed_cmd(MCANDrv_infoMsgS_TYPE infoMsgS)
{
    //0x1803B0C0: //远程控制器车速命令
    uint8 *p_buf = infoMsgS.datMsgUBA;
    SKY_TRACE_INFO("0x1803B0C,远程控制器车速命令");
    g_rgate_ctrl_cmd.acceleration_cmd = p_buf[0];
    SKY_TRACE_INFO("加速度指令: %d\n", g_rgate_ctrl_cmd.acceleration_cmd);
    g_rgate_ctrl_cmd.AEB_collide_level = p_buf[1] & 0x3;
    g_rgate_ctrl_cmd.electrical_park_brake_cmd = p_buf[1] & 0xc;
    g_rgate_ctrl_cmd.dw = (p_buf[1] & 0xf0) >> 4;
    SKY_TRACE_INFO("档位指令: %d\n", g_rgate_ctrl_cmd.dw);

    g_rgate_ctrl_cmd.eps2 = p_buf[2];
    SKY_TRACE_INFO("心跳值2: %d\n", g_rgate_ctrl_cmd.eps2);
    g_rgate_ctrl_cmd.emergency_brake_cmd = p_buf[3] & 0x1;

#ifdef MOCK_ENABLE
    //Feign 0x1804A0B0
    SKY_TRACE_INFO("0x1804A0B0, 行车状态\n");
    infoMsgS.infoConfS.infoMsgIDUI = 0x1804A0B0;
    infoMsgS.datMsgUBA[0] = g_rgate_ctrl_cmd.dw << 2;
    infoMsgS.datMsgUBA[1] = 0;
    infoMsgS.datMsgUBA[2] = 0;
    infoMsgS.datMsgUBA[6] = p_buf[0];

    SKY_TRACE_INFO("行车状态, 档位 in bit3~bit6 of datMsgUBA[0]: %d\n", infoMsgS.datMsgUBA[0]);
    CANNet_RecvMsgCAN_2(infoMsgS);
#endif
}
void process_rgate_control_cmd(MCANDrv_infoMsgS_TYPE infoMsgS)
{
    //0x1805B0C0: //远程车辆控制命令
    uint8 *p_buf = infoMsgS.datMsgUBA;
    SKY_TRACE_INFO("0x1805B0C0, 远程控制器车速命令\n");

    g_rgate_ctrl_cmd.selfdrive_ctl_cmd = p_buf[0] & 0x3;
    SKY_TRACE_INFO("自驾控制模式: %d\n", g_rgate_ctrl_cmd.selfdrive_ctl_cmd);
    g_rgate_ctrl_cmd.door_ctl_cmd = (p_buf[0] & 0xc) >> 2;
    g_rgate_ctrl_cmd.beep_ctl = (p_buf[0] & 0x30) >> 4;
    g_rgate_ctrl_cmd.day_drivinglight = (p_buf[0] & 0xc0) >> 6;
    g_rgate_ctrl_cmd.zzxd = (p_buf[1] & 0x3) >> 0;
    g_rgate_ctrl_cmd.yzxd = (p_buf[1] & 0xc) >> 2;
    g_rgate_ctrl_cmd.jsd = (p_buf[1] & 0x30) >> 4;
    g_rgate_ctrl_cmd.skd = (p_buf[1] & 0xc0) >> 6;
    g_rgate_ctrl_cmd.dd = p_buf[2] & 0x3;
    g_rgate_ctrl_cmd.hwd = (p_buf[2] & 0xc) >> 2;
    g_rgate_ctrl_cmd.zwd = (p_buf[2] & 0x30) >> 4;
    g_rgate_ctrl_cmd.ywd = (p_buf[2] & 0xc0) >> 6;
    g_rgate_ctrl_cmd.ndd = p_buf[3] & 0x3;
    g_rgate_ctrl_cmd.fwd = (p_buf[3] & 0xc) >> 2;

    g_rgate_ctrl_cmd.remotedriving_status = p_buf[4] & 0x3;
    g_rgate_ctrl_cmd.remotedriving_reliability = (p_buf[5] & 0xc) >> 2;
    g_rgate_ctrl_cmd.remotedriving_fault_level = (p_buf[5] & 0x30) >> 4;

    g_rgate_ctrl_cmd.air_condition_ctl = p_buf[5] & 0x7;
    g_rgate_ctrl_cmd.air_condition_temp = (p_buf[5] & 0xf8) >> 3;
    g_rgate_ctrl_cmd.eps3 = p_buf[7];
    SKY_TRACE_INFO("心跳值3: %d\n", g_rgate_ctrl_cmd.eps3);

#ifdef MOCK_ENABLE
    //Feign 0x1806A0B0
    SKY_TRACE_INFO("0x1806A0B0, 车辆状态1\n");
    infoMsgS.infoConfS.infoMsgIDUI = 0x1806A0B0;
    infoMsgS.datMsgUBA[0] = (p_buf[0] & 0x3) | (g_rgate_ctrl_cmd.door_ctl_cmd << 2) | (g_rgate_ctrl_cmd.zwd << 3) | (g_rgate_ctrl_cmd.ywd << 4) | (g_rgate_ctrl_cmd.ndd << 5) | (g_rgate_ctrl_cmd.fwd << 6);

    infoMsgS.datMsgUBA[1] = g_rgate_ctrl_cmd.dd | (g_rgate_ctrl_cmd.hwd << 2) | (g_rgate_ctrl_cmd.day_drivinglight << 3) | (g_rgate_ctrl_cmd.zzxd << 4) | (g_rgate_ctrl_cmd.yzxd << 5) | (g_rgate_ctrl_cmd.jsd << 6) | (g_rgate_ctrl_cmd.skd << 7);

    infoMsgS.datMsgUBA[2] = 0;
    infoMsgS.datMsgUBA[3] = 70;
    infoMsgS.datMsgUBA[4] = 0;

    SKY_TRACE_INFO("车辆状态1, datMsgUBA[0]: %d\n", infoMsgS.datMsgUBA[0]);
    SKY_TRACE_INFO("车辆状态1, datMsgUBA[1]: %d\n", infoMsgS.datMsgUBA[1]);

    CANNet_RecvMsgCAN_2(infoMsgS);

    //Feign 0x1812A0B0
    SKY_TRACE_INFO("0x1812A0B0, 车辆状态4\n");
    infoMsgS.infoConfS.infoMsgIDUI = 0x1812A0B0;
    infoMsgS.datMsgUBA[0] = 140;
    infoMsgS.datMsgUBA[1] = 110;
    infoMsgS.datMsgUBA[2] = 3;

    infoMsgS.datMsgUBA[3] = 100;
    infoMsgS.datMsgUBA[4] = 0;

    SKY_TRACE_INFO("车辆状态4, datMsgUBA[0]: %d\n", infoMsgS.datMsgUBA[0]);
    SKY_TRACE_INFO("车辆状态4, datMsgUBA[1]: %d\n", infoMsgS.datMsgUBA[1]);

    SKY_TRACE_INFO("车辆状态4, datMsgUBA[2]: %d\n", infoMsgS.datMsgUBA[2]);
    SKY_TRACE_INFO("车辆状态4, datMsgUBA[3]: %d\n", infoMsgS.datMsgUBA[3]);

    CANNet_RecvMsgCAN_2(infoMsgS);
#endif
}

#ifdef MOCK_ENABLE
void mock_ParseRxCanReport(MCANDrv_infoMsgS_TYPE infoMsgS)
{
    uint32 CANNet_infoIDCAN2UI;

    CANNet_infoIDCAN2UI = infoMsgS.infoConfS.infoMsgIDUI;

    //舱端接收本地CAN报文
    switch (CANNet_infoIDCAN2UI)
    {
    case 0x1801B0C0: //远程控制器EPS命令
    {
        process_rgate_eps_cmd(infoMsgS);
        break;
    }
    case 0x1803B0C0: //远程控制器车速命令
    {
        process_rgate_speed_cmd(infoMsgS);
        break;
    }
    case 0x1805B0C0: //远程车辆控制命令
    {
        process_rgate_control_cmd(infoMsgS);
        break;
    }
    default:
        break;
    }
}
#endif

//1. TCP接收到舱的CAN报文，统统的发车端CAN总线上，收到就发
//2. 车端VCU接收到这些can报文后就去执行报文所要求的控制命令
void TCP_To_CAN_Send(uint8 *buf, uint32 len)
{
    MCANDrv_infoMsgS_TYPE infMsgS;
    uint32 CANNet_infoIDCAN2UI;

    memset(&infMsgS, 0, sizeof(MCANDrv_infoMsgS_TYPE));
    if (BufToMsg(&infMsgS, buf) != 0) //帧内容有错
        return;
    CANNet_infoIDCAN2UI = infMsgS.infoConfS.infoMsgIDUI;

    SKY_TRACE_INFO("车端VCU收到的舱端can报文,CANNet_infoIDCAN2UI:0x%x, buf:0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x\n",
                   CANNet_infoIDCAN2UI, buf[0], buf[1], buf[2], buf[3], buf[4],
                   buf[5], buf[6], buf[7], buf[8], buf[9], buf[10], buf[11], buf[12]);
    //CAN2接收分类处理
    switch (CANNet_infoIDCAN2UI)
    {
    //case 0x1801B0A0:    //远程控制器EPS命令
    //case 0x1803B0A0:    //远程控制器车速命令
    //case 0x1805B0A0:    //远程车辆控制命令
    //case 0x1807B0A0:    //车辆控制命令 2
    case 0x1801B0C0: //远程控制器EPS命令
    case 0x1803B0C0: //远程控制器车速命令
    case 0x1805B0C0: //远程车辆控制命令
        break;
    default:
        //printf("TCP Can ID is not need!\n");
        //MCANDrv_SendMsg(infMsgS);       //for test
        return;
    }

#ifdef MOCK_ENABLE
    //The following is only for test.
    mock_ParseRxCanReport(infMsgS);
#else
    MCANDrv_SendMsg(infMsgS);
#endif
}
