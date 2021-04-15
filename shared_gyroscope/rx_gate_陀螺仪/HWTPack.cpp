#include "HWTPack.h"
#include "./NET/CloudComm.h"

#define LITTLE_ENDIAN_CHOICE 1



extern CloudComm g_CloudComm;


/**
 * \brief Calculate control sum of binary buffer
 */
uint8 hwt_calc_sum(const uint8 *buff, uint32 buff_sz)
{
    uint8 sum = 0;
    uint32 i;

    for(i = 0; i < buff_sz; i++)
        sum += buff[i];

    return sum;
}

void SearchOneFrameData(UdpData_t &udp_frame_data, uint8 *data)
{
    udp_frame_data.command = 1;

    //提取原始数据
    udp_frame_data.angular_a_x = GetValueByByte(data[45 + 2], data[45 + 3]);
    udp_frame_data.angular_a_y = GetValueByByte(data[45 + 4], data[45 + 5]);
    udp_frame_data.angular_a_z = GetValueByByte(data[45 + 6], data[45 + 7]);

    udp_frame_data.angular_v_x = GetValueByByte(data[45 + 13], data[45 + 14]);
    udp_frame_data.angular_v_y = GetValueByByte(data[45 + 15], data[45 + 16]);
    udp_frame_data.angular_v_z = GetValueByByte(data[45 + 17], data[45 + 18]);

    udp_frame_data.angle_x = GetValueByByte(data[45 + 24], data[45 + 25]);
    udp_frame_data.angle_y = GetValueByByte(data[45 + 26], data[45 + 27]);
    udp_frame_data.angle_z = GetValueByByte(data[45 + 28], data[45 + 29]);

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

float htonf(float x)
{
    uint32 *pData32;
    uint32 data;

    pData32 = (uint32 *)&x;
    data = htonl(*pData32);
    memcpy(&x, &data, 4);
    return x;
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




