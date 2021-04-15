#include    <string.h>
#include    <stdlib.h>
#include    <stdio.h> 
#include    <math.h> 
#include "./COMM/sky_trace.h"
#include "Std_Types.h"
#include "HWT905.h"
#include"NET/CloudComm.h"
#include "./NET/UdpClient.h"

#define LITTLE_ENDIAN_CHOICE  1



using namespace std;

// //////////////////////////////////////////////////////////////////////////////////////
// #define     MAX_SIZE_HWT905        200
// #define     HWT_FRAME_SIZE          33
// #define     HWT_PER_FRAME_SIZE      11

//////////////////////////////////////////////////////////////////////////////////////
extern CloudComm g_CloudComm;
extern string gettimestamp(void);

//////////////////////////////////////////////////////////////////////////////////////
uint8 hwt_read_buf[MAX_SIZE_HWT905+1];
uint32 hwt_position = 0;
CUdpClient g_Udp2Client_RemoteSensing;
uint8 udp_send_frame[100] = {0};
UdpData_t udp_frame_data;
//////////////////////////////////////////////////////////////////////////////////////
uint8 hwt_calc_sum(const uint8 *buff, uint32 buff_sz)
{
    uint8 sum = 0;
    uint32 i;

    for(i = 0; i < buff_sz; i++)
        sum += buff[i];

    return sum;
}

float GetValueByByte(uint8 low, uint8 high)
{
    short s_tmp;
    float f_tmp;

    s_tmp = 0;
    s_tmp = (short)(s_tmp ^ high);
    s_tmp = (short)(s_tmp << 8);
    s_tmp = (short)(s_tmp ^ low);
    f_tmp = s_tmp;

    /*
    s_tmp = (low << 8) |  low;
    f_tmp = s_tmp;*/
    return f_tmp;
}

void SearchOneFrameData(UdpData_t &udp_frame_data, uint8 *data)
{
    udp_frame_data.command = 1;

    //提取原始数据
    udp_frame_data.angular_a_x = GetValueByByte(data[2], data[3]);
    udp_frame_data.angular_a_y = GetValueByByte(data[4], data[5]);
    udp_frame_data.angular_a_z = GetValueByByte(data[6], data[7]);

    udp_frame_data.angular_v_x = GetValueByByte(data[13], data[14]);
    udp_frame_data.angular_v_y = GetValueByByte(data[15], data[16]);
    udp_frame_data.angular_v_z = GetValueByByte(data[17], data[18]);

    udp_frame_data.angle_x = GetValueByByte(data[24], data[25]);
    udp_frame_data.angle_y = GetValueByByte(data[26], data[27]);
    udp_frame_data.angle_z = GetValueByByte(data[28], data[29]);

    //数据转换
    //加速度
    udp_frame_data.angular_a_x = udp_frame_data.angular_a_x * 16 * 9.8f / 32767;
    udp_frame_data.angular_a_y = udp_frame_data.angular_a_y * 16 * 9.8f / 32767;
    udp_frame_data.angular_a_z = udp_frame_data.angular_a_z * 16 * 9.8f / 32767;

    //角速度
    udp_frame_data.angular_v_x = udp_frame_data.angular_v_x * 2000 / 32767;
    udp_frame_data.angular_v_y = udp_frame_data.angular_v_y * 2000 / 32767;
    udp_frame_data.angular_v_z = udp_frame_data.angular_v_z * 2000 / 32767;

    //  角度 °
    // udp_frame_data.angle_x = udp_frame_data.angle_x * 180 / 32767;
    // udp_frame_data.angle_y = udp_frame_data.angle_y * 180 / 32767;
    // udp_frame_data.angle_z = udp_frame_data.angle_z * 180 / 32767;

    //角度 弧度
    udp_frame_data.angle_x = udp_frame_data.angle_x * 3.14 / 32767;
    udp_frame_data.angle_y = udp_frame_data.angle_y * 3.14 / 32767;
    udp_frame_data.angle_z = udp_frame_data.angle_z * 3.14 / 32767;

    SKY_TRACE_INFO("纵向加速度y:%f, 横向加速度x：%f, 垂向加速度z：%f, 方位角速度Z：%f, 俯仰角速度X：%f, 侧倾角速度Y：%f, 方位角Z：%f, 俯仰角X: %f, 侧倾角Y：%f\n",
    udp_frame_data.angular_a_y, udp_frame_data.angular_a_x, udp_frame_data.angular_a_z, 
    udp_frame_data.angular_v_z, udp_frame_data.angular_v_x, udp_frame_data.angular_v_y,
    udp_frame_data.angle_z, udp_frame_data.angle_x, udp_frame_data.angle_y);
}

void saveFloatToBuf(uint8_t *buf, uint32_t len, float *data)
{
    //float tmp_flt = htonf(*data);

    #if LITTLE_ENDIAN_CHOICE
        memcpy(buf, data, len);
        SKY_TRACE_INFO("[%s] 字节序：%s\n", __FUNCTION__, "Little Endian" );
    #else
        unsigned char temp_buf[4] = {0};
        memcpy(temp_buf,data,len);
        buf[0] = temp_buf[3];
        buf[1] = temp_buf[2];
        buf[2] = temp_buf[1];
        buf[3] = temp_buf[0];
        SKY_TRACE_INFO("[%s] 字节序： %s\n", __FUNCTION__, "Big Endian" );
    #endif
/*
   buf[0] = (data )&0xFF;
   buf[1] = (data >> 8)&0xFF;
   buf[2] = (data >> 16)&0xFF;
   buf[3] = (data >> 24)&0xFF;*/
    // len += 4;
}

void WriteSensorOneFrame(uint8* byte_buf, UdpData_t data)
{
    uint32_t len = 4;
    uint8_t buf[len];
    int i;
    uint8 bb;

    //报文头   
    byte_buf[0] = 0x6b;
    byte_buf[1] = 0x6c;

    //命令
    byte_buf[3] = data.command;

    //方位角速度 z
    //buf = BitConverter.GetBytes(data.angular_v_z);
    saveFloatToBuf(buf, len, &data.angular_v_z);
    for (i = 0; i < 4; i++)
        byte_buf[4 + i] = buf[i];
    
    //俯仰角速度 x
    //buf = BitConverter.GetBytes(data.angular_v_x);
    saveFloatToBuf(buf, len, &data.angular_v_x);
    for (i = 0; i < 4; i++)
        byte_buf[8 + i] = buf[i];

    //侧倾角速度 
    //buf = BitConverter.GetBytes(data.angular_v_y);
    saveFloatToBuf(buf, len, &data.angular_v_y);
    for (i = 0; i < 4; i++)
        byte_buf[12 + i] = buf[i];


    //方位角度 z 
    //buf = BitConverter.GetBytes(data.angle_z);
    saveFloatToBuf(buf, len, &data.angle_z);
    for (i = 0; i < 4; i++)
        byte_buf[16 + i] = buf[i];
    //俯仰角度 x
    //buf = BitConverter.GetBytes(data.angle_x);
    saveFloatToBuf(buf, len, &data.angle_x);
    for (i = 0; i < 4; i++)
        byte_buf[20 + i] = buf[i];
    //侧倾角度 y
    //buf = BitConverter.GetBytes(data.angle_y);
    saveFloatToBuf(buf, len, &data.angle_y);
    for (i = 0; i < 4; i++)
        byte_buf[24 + i] = buf[i];

    //纵向加速度 y
    //buf = BitConverter.GetBytes(data.angular_a_y);
    saveFloatToBuf(buf, len, &data.angular_a_y);
    for (i = 0; i < 4; i++)
        byte_buf[28 + i] = buf[i];
    //横向加速度 x
    //buf = BitConverter.GetBytes(data.angular_a_x);
    saveFloatToBuf(buf, len, &data.angular_a_x);
    for (i = 0; i < 4; i++)
        byte_buf[32 + i] = buf[i];
    //垂向加速度 z
    //buf = BitConverter.GetBytes(data.angular_a_z);
    saveFloatToBuf(buf, len, &data.angular_a_z);
    for (i = 0; i < 4; i++)
        byte_buf[36 + i] = buf[i];

    //其他没有的数据，以及保留的字节，填0
    for (i = 40; i < 75; i++)
        byte_buf[i] = 0;

    //帧尾
    byte_buf[76] = 0x54;
    byte_buf[77] = 0x6f;
    byte_buf[78] = 0x43;
    byte_buf[79] = 0x41;

    //计算校验字节
    bb = 0;
    for (i = 3; i < 76; i++)
        //bb = bb + (byte) byte_buf[i];
        bb += byte_buf[i];
    byte_buf[2] = bb;

    SKY_TRACE_INFO("[%s] udp data to remote sensor: [方位角速度Z] 0x%x 0x%x 0x%x 0x%x\n", __FUNCTION__, 
    byte_buf[4], byte_buf[5], byte_buf[6], byte_buf[7]);

    SKY_TRACE_INFO("[%s] udp data to remote sensor: [俯仰角速度X] 0x%x 0x%x 0x%x 0x%x\n", __FUNCTION__, 
    byte_buf[8], byte_buf[9], byte_buf[10], byte_buf[11]);

    SKY_TRACE_INFO("[%s] udp data to remote sensor: [侧倾角速度Y] 0x%x 0x%x 0x%x 0x%x\n", __FUNCTION__, 
    byte_buf[12], byte_buf[13], byte_buf[14], byte_buf[15]);

    SKY_TRACE_INFO("[%s] udp data to remote sensor: [方位角Z] 0x%x 0x%x 0x%x 0x%x\n", __FUNCTION__, 
    byte_buf[16], byte_buf[17], byte_buf[18], byte_buf[19]);

    SKY_TRACE_INFO("[%s] udp data to remote sensor: [俯仰角X] 0x%x 0x%x 0x%x 0x%x\n", __FUNCTION__, 
    byte_buf[20], byte_buf[21], byte_buf[22], byte_buf[23]);

    SKY_TRACE_INFO("[%s] udp data to remote sensor: [俯仰角Y] 0x%x 0x%x 0x%x 0x%x\n", __FUNCTION__, 
    byte_buf[24], byte_buf[25], byte_buf[26], byte_buf[27]);

    SKY_TRACE_INFO("[%s] udp data to remote sensor: [纵向加速度y] 0x%x 0x%x 0x%x 0x%x\n", __FUNCTION__, 
    byte_buf[28], byte_buf[29], byte_buf[30], byte_buf[31]);    

    SKY_TRACE_INFO("[%s] udp data to remote sensor: [横向加速度x] 0x%x 0x%x 0x%x 0x%x\n", __FUNCTION__, 
    byte_buf[32], byte_buf[33], byte_buf[34], byte_buf[35]); 

    SKY_TRACE_INFO("[%s] udp data to remote sensor: [垂向加速度z] 0x%x 0x%x 0x%x 0x%x\n", __FUNCTION__, 
    byte_buf[36], byte_buf[37], byte_buf[38], byte_buf[39]); 


}


void HwtAddStringToBuf(uint8 * buf, sint32 count)
{
    if(hwt_position >= MAX_SIZE_HWT905)        //缓冲区满了
    {
        hwt_position = MAX_SIZE_HWT905;
        return;
    }

    if((hwt_position + count) >= MAX_SIZE_HWT905)
        count = MAX_SIZE_HWT905 - hwt_position ;

    memcpy(hwt_read_buf + hwt_position, buf , count);

    hwt_position += count;
}

int HWT905DataCheck()                //g_CloudComm.m_hwt_buf
{
    static int hwt_num;
	if (hwt_position > 2)
    {
        if ((hwt_read_buf[0] == 0x55) && ((hwt_read_buf[1] == 0x51) || (hwt_read_buf[1] == 0x52) || (hwt_read_buf[1] == 0x53)))   
        {
            if(hwt_position > 11)
            {
                SKY_TRACE_INFO("[%s], hwt_read_buf[0]:0x%x, [1]:0x%x, [2]:0x%x, [3]:0x%x, [4]:0x%x, [5]:0x%x, [6]:0x%x, [7]:0x%x, [8]:0x%x, [9]:0x%x, [10]:0x%x\n", __FUNCTION__, 
                 hwt_read_buf[0], hwt_read_buf[1], hwt_read_buf[2], hwt_read_buf[3], hwt_read_buf[4], hwt_read_buf[5], hwt_read_buf[6], hwt_read_buf[7], hwt_read_buf[8]
                 , hwt_read_buf[9], hwt_read_buf[10]);

                //  SKY_TRACE_INFO("[%s], mode: %d, ret:  %d, m_nSeqCount: %d\n", __FUNCTION__, mode, ret, m_nSeqCount);

                if(hwt_read_buf[1] == 0x51)
                {
                    memcpy(g_CloudComm.m_hwt_buf, hwt_read_buf, HWT_PER_FRAME_SIZE);
                }
                if(hwt_read_buf[1] == 0x52)
                {
                    memcpy(&g_CloudComm.m_hwt_buf[11], hwt_read_buf, HWT_PER_FRAME_SIZE);
                }
                if(hwt_read_buf[1] == 0x53)
                {
                    memcpy(&g_CloudComm.m_hwt_buf[22], hwt_read_buf, HWT_PER_FRAME_SIZE);
                }

                memcpy(hwt_read_buf, &hwt_read_buf[11], hwt_position - HWT_PER_FRAME_SIZE);
                hwt_position = hwt_position - HWT_PER_FRAME_SIZE;
                HWT905DataCheck();
            }
            else
            {
                return 0;
            }
        }
        else
        {
            memcpy(hwt_read_buf, &hwt_read_buf[1], hwt_position - 1);
            hwt_position = hwt_position - 1;
            HWT905DataCheck();
        }
    }

    g_CloudComm.SendHWT905ToCloud();



    // if ((g_CloudComm.m_hwt_buf[0] == 0x55)&&(g_CloudComm.m_hwt_buf[1]=0x51) && \
    // (g_CloudComm.m_hwt_buf[11] == 0x55)&&(g_CloudComm.m_hwt_buf[12]=0x52) && \
    // (g_CloudComm.m_hwt_buf[22] == 0x55)&&(g_CloudComm.m_hwt_buf[23]=0x53))
    // { 
    //     //检查校验和
    //     if( (hwt_calc_sum(&g_CloudComm.m_hwt_buf[0], 10) == g_CloudComm.m_hwt_buf[10]) && \
    //         (hwt_calc_sum(&g_CloudComm.m_hwt_buf[11], 10) == g_CloudComm.m_hwt_buf[21]) && \
    //         (hwt_calc_sum(&g_CloudComm.m_hwt_buf[22], 10) == g_CloudComm.m_hwt_buf[32]) )
    //     {
    //         // SKY_TRACE_INFO(" Vehicle checksum success! \n");
    //         SearchOneFrameData(udp_frame_data, g_CloudComm.m_hwt_buf);
    //         // SKY_TRACE_INFO("[%s], udp_frame_data.angular_a_x = %f\n", __FUNCTION__, udp_frame_data.angular_a_x );
    //         WriteSensorOneFrame(udp_send_frame, udp_frame_data);
    //         //遥感udp客户端
    //         if (g_Udp2Client_RemoteSensing.get_udp_status() == 0)
    //         {
    //             g_Udp2Client_RemoteSensing.set_read_callback_func(NULL);
    //             g_Udp2Client_RemoteSensing.init_udp_client("192.168.1.150", 8080);  
    //             // g_Udp2Client_RemoteSensing.init_udp_client(gRemoteSensingClientIpStr, gRemoteSensingClientPort);  
    //         }
    //         g_Udp2Client_RemoteSensing.send_udp_data(udp_send_frame, REMOTE_SENSING_DATA_LENTG);
    //         printf("\n Vehicle Send HWT905 Data To Yaogan:[%s]:",gettimestamp().c_str()); 
    //     } 
    // }


    return 0;
}

void RS232_HWT905_CallBack(sint8 *buf, sint32 count)
{
    // printf("Vehicle recive HWT905 data:[%s]: ",gettimestamp().c_str()); 
    // for (int i = 0; i < count; i++)
    // {
    //     printf("%02X ",buf[i]);
    // } 
    printf(" \n"); 
    // SKY_TRACE_INFO("[%s], hwt data received \n", __FUNCTION__);
    HwtAddStringToBuf((uint8 *)buf, count);                                               // 写入数组hwt_read_buf    写到位置 hwt_position
    //printf("collect %d bytes\n", count);
    HWT905DataCheck();                                                                    // m_hwt_buf[HWT_FRAME_SIZE];
}