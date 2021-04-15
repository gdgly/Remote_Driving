#ifndef _HWT_PACK_H_
#define _HWT_PACK_H_

#include <stdio.h>
#include <stdlib.h>
#include "Std_Types.h"

typedef struct UdpData
{
    uint8 command;           //命令

    float angular_v_x;      //X角速度
    float angular_v_z;      //Z角速度
    float angular_v_y;      //Y角速度

    float angle_x;          //X角度
    float angle_z;          //Z角度
    float angle_y;          //Y角度

    float angular_a_x;      //X加速度
    float angular_a_z;      //Z加速度
    float angular_a_y;      //Y加速度
}UdpData_t;

void SearchOneFrameData(UdpData_t &udp_frame_data, uint8 *data);

void WriteSensorOneFrame(uint8* byte_buf, UdpData data);

uint8 hwt_calc_sum(const uint8 *buff, uint32 buff_sz);

float GetValueByByte(uint8 low, uint8 high);

#endif