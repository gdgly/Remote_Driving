#ifndef _CAN_PACK_H_
#define _CAN_PACK_H_

#include <stdio.h>
#include <stdlib.h>
#include "./NET/CloudComm.h"

// 0x1806A0B0 行车状态1
int VehicleState1(uint8 *data);
// 0x18040AB0
int DrivingState(uint8 *data);

// 0x1802A0B0 整车控制器 EPS 状态 
int VCUEPSState(uint8 *data);

// 0x1812A0B0 行车状态4
int VehicleState4(uint8 *data);
#endif
