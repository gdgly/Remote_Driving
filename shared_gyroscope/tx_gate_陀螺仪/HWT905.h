
#ifndef __HWT905_H__
#define __HWT905_H__

#include "Std_Types.h"

//////////////////////////////////////////////////////////////////////////////////////
#define     MAX_SIZE_HWT905        200
#define     HWT_FRAME_SIZE          33
#define     HWT_PER_FRAME_SIZE      11
#define   REMOTE_SENSING_DATA_LENTG 80

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

void RS232_HWT905_CallBack(sint8 *buf, sint32 count);
int HWT905DataCheck();

#endif /* __HWT905_H__ */
