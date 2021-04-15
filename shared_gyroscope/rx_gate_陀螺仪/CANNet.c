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
#include "./CanPack.h"

uint16 Remote_Protocol_Version;
extern CloudComm g_CloudComm;
extern char *vin_untakeover;
extern uint32 gCanNumber;

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

//接收本地CAN2消息处理函数
void CANNet_RecvMsgCAN_2(MCANDrv_infoMsgS_TYPE infoMsgS)
{
    sint8 send_buf[15];
    uint32 CANNet_infoIDCAN2UI;

    CANNet_infoIDCAN2UI = infoMsgS.infoConfS.infoMsgIDUI;

    //舱端接收本地CAN报文
    switch (CANNet_infoIDCAN2UI)
    {
    case 0x1801B0C0: //远程控制器EPS命令
    case 0x1803B0C0: //远程控制器车速命令
        break;
    case 0x1805B0C0: //远程车辆控制命令
        Remote_Protocol_Version = ((infoMsgS.datMsgUBA[7] & 0x0f) << 8) + infoMsgS.datMsgUBA[6];
        break;
    default:
        return;
    }

    //The following is only for test
    //    mock_ParseCanReport(infoMsgS);

    MsgToBuf(&infoMsgS, send_buf);
    // if (CANNet_infoIDCAN2UI == 0x1805B0C0)
    //     send_buf[8] =send_buf[8] | (0x1 << 4);

    // SKY_TRACE_INFO("舱端本地CAN消息: CANNet_infoIDCAN2UI:0x%x, send_buf:0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x\n",
    //                CANNet_infoIDCAN2UI,
    //                send_buf[0], send_buf[1], send_buf[2], send_buf[3], send_buf[4],
    //                send_buf[5], send_buf[6], send_buf[7], send_buf[8], send_buf[9],
    //                send_buf[10], send_buf[11], send_buf[12]);
    SKY_TRACE_INFO("舱端CAN: CANNet_infoIDCAN2UI:0x%x\n", CANNet_infoIDCAN2UI);
    g_CloudComm.AddSendData(CANNet_infoIDCAN2UI, send_buf);
}

//舱端接收到远程CAN报文
void TCP_To_CAN_Send(uint8 *buf, uint32 len)
{
    MCANDrv_infoMsgS_TYPE infMsgS;
    uint32 CANNet_infoIDCAN2UI;
    uint8 mode = 0;

    memset(&infMsgS, 0, sizeof(MCANDrv_infoMsgS_TYPE));
    if (BufToMsg(&infMsgS, buf) != 0) //帧内容有错
    {
        SKY_TRACE_ERROR("收到车端CAN报文:帧内容有错\n");
        return;
    }

    CANNet_infoIDCAN2UI = infMsgS.infoConfS.infoMsgIDUI;
    //CAN2接收分类处理
    switch (CANNet_infoIDCAN2UI)
    {
    case 0x1804A0B0: //行车状态
                     //        case 0x1806A0B0:    //车辆状态1
    case 0x1810A0B0: //车辆状态2
    case 0x1811A0B0: //车辆状态3
    case 0x1812A0B0: //车辆状态4
    case 0x1813A0B0: //车辆故障状态
    case 0x1808A0B0: //车辆状态5
    case 0x1802A0B0: //EPS状态
        SKY_TRACE_INFO("收到车端CAN,CANNet_infoIDCAN2UI:%8X\n", CANNet_infoIDCAN2UI);
        MCANDrv_SendMsg(infMsgS);
        break;
    case 0x1806A0B0: //车辆状态1
        if (Remote_Protocol_Version == 0x541)
        {
            SKY_TRACE_INFO("远程发送版本号: 1号蓝鲸号 v_1.4.5\n");
            infMsgS.datMsgUBA[1] = ((infMsgS.datMsgUBA[1]) | (g_CloudComm.m_TakeoverFlag << 7));
        }
        SKY_TRACE_INFO("0x1806A0B0, m_unTakeoverFlag:%d, m_unTakeoverFlag_cloud:%d\n",
                       g_CloudComm.m_unTakeoverFlag, g_CloudComm.m_unTakeoverFlag_cloud);
        mode = (infMsgS.datMsgUBA[0] & 0x03);
        SKY_TRACE_INFO("0x1806A0B0, 车端返回的自驾控制模式:%d\n", mode);
        g_CloudComm.AddDriverModeData("0", mode);
        if (g_CloudComm.m_unTakeoverFlag && ((infMsgS.datMsgUBA[0] & 0x03) != 0x03))
        {
            SKY_TRACE_INFO("舱端取消接管[%s]: 0x1806A0B0, 车端返回的自驾控制模式:%d, 取消车辆的vin:%s \n", __FUNCTION__, (infMsgS.datMsgUBA[0] & 0x03), vin_untakeover);
            g_CloudComm.CabinUnTakeover(vin_untakeover);
            g_CloudComm.m_unTakeoverFlag = 0;
        }
        else if (g_CloudComm.m_unTakeoverFlag_cloud && ((infMsgS.datMsgUBA[0] & 0x03) != 0x03))
        {
            SKY_TRACE_INFO("云端取消接管[%s]: 0x1806A0B0, 车端返回的自驾控制模式:%d\n", __FUNCTION__, (infMsgS.datMsgUBA[0] & 0x03));
            g_CloudComm.CloudUntakeoverPostProcess();
            g_CloudComm.m_unTakeoverFlag_cloud = 0;
        }

        MCANDrv_SendMsg(infMsgS);
        break;
    default:
        break;
    }

    // SKY_TRACE_INFO("远程发送版本号: 矿卡 v_2.0.2 或者 新版蓝鲸号 v_1.4.8\n");
    IMX_TakeOver_Flag_CanSend();

    Can2Kv(CANNet_infoIDCAN2UI, buf, len);
}

void IMX_TakeOver_Flag_CanSend()
{
    MCANDrv_infoMsgS_TYPE infMsgS_add;
    // memcpy(&infMsgS_add,&infMsgS,sizeof(MCANDrv_infoMsgS_TYPE));
    infMsgS_add.infoConfS.infoMsgIDUI = 0x18AAB0C0;
    infMsgS_add.infoConfS.infoMsgParU.B.DLC = 8;          //数据长度
    infMsgS_add.infoConfS.infoMsgParU.B.IDE = 1;          //1扩展帧，0标准帧
    infMsgS_add.infoConfS.infoMsgParU.B.CAN = gCanNumber; //第几路CAN
    // memset(infMsgS_add.datMsgUBA, 0, sizeof(infMsgS_add.datMsgUBA[8]));
    memset(infMsgS_add.datMsgUBA, 0, 8);
    infMsgS_add.datMsgUBA[1] = ((infMsgS_add.datMsgUBA[1]) | (g_CloudComm.m_TakeoverFlag << 7));
    // SKY_TRACE_INFO("CANID:0x%x, Can_Data:0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x \n",
    //                infMsgS_add.infoConfS.infoMsgIDUI,
    //                infMsgS_add.datMsgUBA[0], infMsgS_add.datMsgUBA[1], infMsgS_add.datMsgUBA[2], infMsgS_add.datMsgUBA[3], infMsgS_add.datMsgUBA[4],
    //                infMsgS_add.datMsgUBA[5], infMsgS_add.datMsgUBA[6], infMsgS_add.datMsgUBA[7]);
    MCANDrv_SendMsg(infMsgS_add);
}

int Can2Kv(unsigned int p_id, uint8 *p_data, unsigned int p_datalen)
{
    // SKY_TRACE_INFO("收到车端CAN报文: CANNet_infoIDCAN2UI:0x%x, p_data:0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x\n",
    //                p_id,
    //                p_data[0], p_data[1], p_data[2], p_data[3], p_data[4],
    //                p_data[5], p_data[6], p_data[7], p_data[8], p_data[9],
    //                p_data[10], p_data[11], p_data[12]);
    switch (p_id)
    {

    case 0x1806A0B0:
        return VehicleState1(&p_data[5]);
    case 0x1804A0B0:
        return DrivingState(&p_data[5]);
    case 0x1802A0B0:
        return VCUEPSState(&p_data[5]);
    case 0x1812A0B0:
        return VehicleState4(&p_data[5]);
    default:
        break;
    }
    return -1;
}
