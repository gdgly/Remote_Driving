#include    <string.h>
#include    <stdlib.h>
#include    <stdio.h> 
#include    <math.h> 
#include "../COMM/sky_trace.h"
#include "../Std_Types.h"
#include "../MapSky/common_data_type.h"
#include "imu_data_decode.h"
#include "IMU_Auto_Adjust.h"

//////////////////////////////////////////////////////////////////////////////////////
#define     MAX_SIZE_HWT905        200
#define     HWT_FRAME_SIZE          33

//////////////////////////////////////////////////////////////////////////////////////
extern uint8 imu_received ;             //IMU数据接收到标志
//IMU数据全局变量（结构体）
extern IMU_DATA_Info imu_data[2];


//////////////////////////////////////////////////////////////////////////////////////
uint8 hwt_read_buf[MAX_SIZE_HWT905+1];
uint32 hwt_position = 0;

//////////////////////////////////////////////////////////////////////////////////////

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

uint32 Get_HWT905Data(uint8 * buf)
{
	int i, nRet = 0;

	if (hwt_position < HWT_FRAME_SIZE)
		return 0;

	for (i = 0; i<hwt_position - HWT_FRAME_SIZE + 1; i++)
	{
		if ((hwt_read_buf[i+0 ] == 0x55)&&(hwt_read_buf[i+1 ]=0x51)     \
          &&(hwt_read_buf[i+11] == 0x55)&&(hwt_read_buf[i+12]=0x52)     \
          &&(hwt_read_buf[i+22] == 0x55)&&(hwt_read_buf[i+23]=0x53))
        {
            //检查校验和
            if( (hwt_calc_sum(&hwt_read_buf[i+0 ], 10) == hwt_read_buf[i+10]) && \
                (hwt_calc_sum(&hwt_read_buf[i+11], 10) == hwt_read_buf[i+21]) && \
                (hwt_calc_sum(&hwt_read_buf[i+22], 10) == hwt_read_buf[i+32]) )
            {
                memcpy(buf, hwt_read_buf + i, HWT_FRAME_SIZE);   //COPY一帧HWT905数据
                //buf[HWT_FRAME_SIZE] = 0;
                memmove(hwt_read_buf, hwt_read_buf + i + HWT_FRAME_SIZE, hwt_position - i - HWT_FRAME_SIZE);   //去除有效数据头前的无效数据
                hwt_position = hwt_position - i - HWT_FRAME_SIZE;
                nRet = 1;
                break;
            } 
        }
	}

	if (i == hwt_position - HWT_FRAME_SIZE + 1) //没有找到数据，丢弃头部
	{
        memmove(hwt_read_buf, hwt_read_buf + i, hwt_position - i);   //去除无效数据
        hwt_position = hwt_position - i;
	}

    return nRet;
}


sint32 get_value_by_byte(uint8 low, uint8 high)
{
    sint16 tmp;

    tmp = high;
    tmp = (sint16)tmp << 8;
    tmp += (sint16)low;

    return (sint32)tmp;
}

sint8 hwt905_parse(uint8 *line, IMU_DATA_Info *pimu)
{
    sint32 tmp;

    //单位要跟HI220保持一致，兼容HI220
    //加速度是0.001G，整形
    pimu->x_accel = get_value_by_byte(line[2],line[3])*1000*16/32768;
    pimu->y_accel = get_value_by_byte(line[4],line[5])*1000*16/32768;
    pimu->z_accel = get_value_by_byte(line[6],line[7])*1000*16/32768;

    //角速度单位是0.1°/S，整形
    pimu->x_speed = get_value_by_byte(line[13],line[14])*20000/32768;
    pimu->y_speed = get_value_by_byte(line[15],line[16])*20000/32768;
    pimu->z_speed = get_value_by_byte(line[17],line[18])*20000/32768;

    //角度单位是°，浮点型，欧拉角输出
    pimu->x_angle = ((float32)get_value_by_byte(line[24],line[25]))*180.0f/32768.0f;
    pimu->y_angle = ((float32)get_value_by_byte(line[26],line[27]))*180.0f/32768.0f;
    pimu->z_angle = ((float32)get_value_by_byte(line[28],line[29]))*180.0f/32768.0f;

    //加速度先去除重力
    remove_gravity(pimu);
    //单位换算,由归一单位换算成m/s2

    pimu->z_angle = GetCourse(pimu->z_angle);       //角度格式转变
    pimu->z_angle_correct = GetCourseByAdjust(pimu->z_angle);   //软件校准

    return 1;
}

void show_hwt905(IMU_DATA_Info *pimu)
{
    printf("acc x=%d, y=%d, z=%d\n", pimu->x_accel, pimu->y_accel, pimu->z_accel);
    printf("speed x=%d, y=%d, z=%d\n", pimu->x_speed, pimu->y_speed, pimu->z_speed);
    printf("angle x=%f, y=%f, z=%f\n", pimu->x_angle, pimu->y_angle, pimu->z_angle);
    printf("angle_correct is %f\n", pimu->z_angle_correct);
}

void RS232_HWT905_CallBack(sint8 *buf, sint32 count)
{
    sint32 i, len, ret;
    uint8 hwt_buf[100];
    static sint8 imu_hwt905_first_time = 1;

    HwtAddStringToBuf((uint8 *)buf, count);
    //printf("collect %d bytes\n", count);

    while (1)
    {
        len = Get_HWT905Data(hwt_buf);
        if (len <= 0)
            break;

        hwt905_parse(hwt_buf,&imu_data[1]);
        //show_hwt905(&imu_data[1]);

        if(imu_hwt905_first_time == 1)
        {
            imu_hwt905_first_time = 0;
            change_yaw(imu_data[0].yaw);
            record_course_angle();
        }
        imu_received = 1;       //置IMU数据收到标志位
    }
}
