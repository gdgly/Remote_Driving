/**
* \file       can_msg_process.h
* \author     yangkun
* \date       2019/09/08
* \version    Version 1.0
* \brief
* \note       Copyright (c) 2000-2020
* \remarks    certification header definition
******************************************************************************/

#ifndef _CAN_MSG_PROCESS_H
#define _CAN_MSG_PROCESS_H

#ifdef __cplusplus
extern "C"
{
#endif

#include "../COMM/macro_def.h"

#define REMOTE_POW_EPB_FLAG			1<<0
#define REMOTE_POW_GEAR_FLAG		1<<1
#define REMOTE_POW_BREAK_FLAG 		1<<2
#define REMOTE_POW_ACC_FLAG			1<<3
#define REMOTE_POW_RPM_FLAG			1<<4
#define REMOTE_POW_TORQUE_FLAG		1<<5
#define REMOTE_POW_FULL_FLAG		0x3F

typedef struct tRemoteVehiclePowState
{
	WORD32 dwCanID;
	BYTE ucEPBState;
	BYTE ucGearState;
	BYTE ucBreakPercentage;
	BYTE ucAccelerate;
	WORD16 wRPM;
	WORD16 wTorque;
	WORD32 dwSegmentFlag;
	PTHREAD_MUTEX_T pMutex;
}T_REMOTE_VEHICLE_POW_STATE;


#define REMOTE_LAMP_DRIVEMODE_FLAG		1<<0
#define REMOTE_LAMP_DOORSTATE_FLAG		1<<1
#define REMOTE_LAMP_DRIVELAMP_FLAG		1<<2
#define REMOTE_LAMP_LEFTLAMP_FLAG		1<<3
#define REMOTE_LAMP_RIGHTLAMP_FLAG		1<<4
#define REMOTE_LAMP_WARNINGLAMP_FLAG	1<<5
#define REMOTE_LAMP_POSLAMP_FLAG		1<<6
#define REMOTE_LAMP_HEADLAMP_FLAG		1<<7
#define REMOTE_LAMP_BACKFOGLAMP_FLAG	1<<8
#define REMOTE_LAMP_LEFTFOGLAMP_FLAG	1<<9
#define REMOTE_LAMP_RIGHTFOGLAMP_FLAG	1<<10
#define REMOTE_LAMP_TOPLAMP_FLAG		1<<11
#define REMOTE_LAMP_AMBIENTLAMP_FLAG	1<<12
#define REMOTE_LAMP_SPEED_FLAG			1<<13
#define REMOTE_LAMP_SOC_FLAG			1<<14
#define REMOTE_LAMP_TOTOALMILES_FLAG	1<<15
#define REMOTE_LAMP_FULL_FLAG			0xFFFF

typedef struct tRemoteVehicleLampState
{
	WORD32 dwCanID;
	BYTE ucDriveMode;
	BYTE ucDoorState;
	BYTE ucDriveLamp;
	BYTE ucLeftTurnLamp;
	BYTE ucRightTurnLamp;
	BYTE ucWarningLamp;
	BYTE ucPosLamp;
	BYTE ucHeadLamp;
	BYTE ucBackFogLamp;
	BYTE ucLeftFogLamp;
	BYTE ucRightFogLamp;
	BYTE ucTopLamp;
	BYTE ucAmbientLamp;
	BYTE ucSpeed;
	BYTE ucSOC;
	BYTE ucKeepAlive;
	WORD16 wTotalMiles;
	WORD32 dwSegmentFlag;
	PTHREAD_MUTEX_T pMutex;
}T_REMOTE_VEHICLE_LAMP_STATE;

#define REMOTE_ELECTRIC_VOLTAGE_FLAG	1<<0
#define REMOTE_ELECTRIC_CURRENT_FLAG	1<<1
#define REMOTE_ELECTRIC_FULL_FLAG		0x03

typedef struct tRemoteVehicleElecState
{
	WORD32 dwCanID;
	WORD16 wMotiveVoltage;
	WORD16 wMotiveCurrent;
	WORD32 dwSegmentFlag;
	PTHREAD_MUTEX_T pMutex;
}T_REMOTE_VEHICLE_ELEC_STATE;

#define REMOTE_BAT_MACHINERYTEMP_FLAG	1<<0
#define REMOTE_BAT_CONTROLTEMP_FLAG		1<<1
#define REMOTE_BAT_FULL_FLAG			0x03

typedef struct tRemoteVehicleBatState
{
	WORD32 dwCanID;
	BYTE ucElectricMachineryTemp;
	BYTE ucElectricCtrlTemp;
	BYTE ucPad[2];
	WORD32 dwSegmentFlag;
	PTHREAD_MUTEX_T pMutex;
}T_REMOTE_VEHICLE_BAT_STATE;

#define REMOTE_EPSCTRL_MODE_FLAG		1<<0
#define REMOTE_EPSCTRL_KEEPALIVE_FLAG	1<<1
#define REMOTE_EPSCTRL_ANGLESPEED_FLAG	1<<2
#define REMOTE_EPSCTRL_STEERANGLE_FLAG	1<<3
#define REMOTE_EPSCTRL_FULL_FLAG		0x0F

typedef struct tRemoteEPSCtrlCmd
{
	WORD32 dwCanID;
	BYTE ucEPSCtrlMode;
	BYTE ucKeepAlive;
	BYTE ucAngleSpeed;
	BYTE ucPad;
	WORD16 wSteerAngle;
	WORD32 dwSegmentFlag;
	PTHREAD_MUTEX_T pMutex;
}T_REMOTE_VEHICLE_EPS_CTRLCMD;

#define REMOTE_SPEED_ACC_FLAG			1<<0
#define REMOTE_SPEED_AEB_FLAG			1<<1
#define REMOTE_SPEED_EPB_FLAG			1<<2
#define REMOTE_SPEED_GEAR_FLAG			1<<3
#define REMOTE_SPEED_KEEPALIVE_FLAG		1<<4
#define REMOTE_SPEED_EMERBREAK_FLAG		1<<5
#define REMOTE_SPEED_FULL_FLAG			0x3F

typedef struct tRemoteSpeedCmd
{
	WORD32 dwCanID;
	BYTE ucAccelerate;
	BYTE ucAEBCollideLevel;
	BYTE ucEPBCmd;
	BYTE ucGear;
	BYTE ucKeepAlive;
	BYTE ucEmergencyBreak;
	BYTE ucPad[2];
	WORD32 dwSegmentFlag;
	PTHREAD_MUTEX_T pMutex;
}T_REMOTE_VEHICLE_SPEED_CMD;

#define REMOTE_CTRL_DRIVEMODE_FLAG				1<<0
#define REMOTE_CTRL_DOOR_FLAG					1<<1
#define REMOTE_CTRL_HORN_FLAG					1<<2
#define REMOTE_CTRL_DRIVELAMP_FLAG				1<<3
#define REMOTE_CTRL_LEFTLAMP_FLAG				1<<4
#define REMOTE_CTRL_RIGHTLAMP_FLAG				1<<5
#define REMOTE_CTRL_WARNINGLAMP_FLAG			1<<6
#define REMOTE_CTRL_POSLAMP_FLAG				1<<7
#define REMOTE_CTRL_HEADLAMP_FLAG				1<<8
#define REMOTE_CTRL_BACKFOGLAMP_FLAG			1<<9
#define REMOTE_CTRL_LEFTFOGLAMP_FLAG			1<<10
#define REMOTE_CTRL_RIGHTFOGLAMP_FLAG			1<<11
#define REMOTE_CTRL_TOPLAMP_FLAG				1<<12
#define REMOTE_CTRL_AMBIENTLAMP_FLAG			1<<13
#define REMOTE_CTRL_REMOTESTATE_FLAG			1<<14
#define REMOTE_CTRL_REMOTERELIABILITY_FLAG		1<<15
#define REMOTE_CTRL_REMOTEFAULT_FLAG			1<<16
#define REMOTE_CTRL_AIRCONDITION_FLAG			1<<17
#define REMOTE_CTRL_AIRTEMP_FLAG				1<<18
#define REMOTE_CTRL_KEEPALIVE_FLAG				1<<19
#define REMOTE_CTRL_FULL_FLAG					0xFFFFF

typedef struct tRemoteCtrlCmd
{
	WORD32 dwCanID;
	BYTE ucDriveMode;
	BYTE ucDoorCmd;
	BYTE ucHornCmd;
	BYTE ucDriveLampCmd;
	BYTE ucLeftLampCmd;
	BYTE ucRightLampCmd;
	BYTE ucWarningLampCmd;
	BYTE ucPosLampCmd;
	BYTE ucHeadLampCmd;
	BYTE ucBackFogLampCmd;
	BYTE ucLeftFogLampCmd;
	BYTE ucRightFogLampCmd;
	BYTE ucTopLampCmd;
	BYTE ucAmbientLampCmd;
	BYTE ucRemoteCtrlState;
	BYTE ucRemoteReliability;
	BYTE ucRemoteFaultLevel;
	BYTE ucRemoteAirconditionCmd;
	BYTE ucRemoteAirTempCmd;
	BYTE ucKeepAlive;
	WORD32 dwSegmentFlag;
	PTHREAD_MUTEX_T pMutex;
}T_REMOTE_VEHICLE_CTRL_CMD;

#define TRUCK_POW_STATE_FLAG		1<<0
#define TRUCK_POW_LOCK_FLAG			1<<1
#define TRUCK_POW_SELFCHECK_FLAG	1<<2
#define TRUCK_POW_CLUTCH_FLAG		1<<3
#define TRUCK_POW_GEAR_FLAG			1<<4
#define TRUCK_POW_ERRSTATUS_FLAG	1<<5
#define TRUCK_POW_DRIVEMODE_FLAG	1<<6
#define TRUCK_POW_SPEED_FLAG		1<<7
#define TRUCK_POW_FULL_FLAG			0xFF

typedef struct tTruckPowState
{
	BYTE ucState;
	BYTE ucLockStatus;
	BYTE ucSelfCheckStatus;
	BYTE ucClutchstatus;
	BYTE ucGear;
	BYTE ucErrorStatus;
	BYTE ucDriveMode;
	BYTE ucPad;
	WORD16 wSpeed;
	WORD32 dwSegmentFlag;
	PTHREAD_MUTEX_T pMutex;
}T_TRUCK_POW_STATE;

#define TRUCK_ENDU_VERSION_FLAG			1<<0
#define TRUCK_ENDU_ACCELERATE_FLAG		1<<1
#define TRUCK_ENDU_BREAK_FLAG			1<<2
#define TRUCK_ENDU_CONSUM_FLAG			1<<3
#define TRUCK_ENDU_ENDURANCE_FLAG		1<<4
#define TRUCK_ENDU_TORQUE_FLAG			1<<5
#define TRUCK_ENDU_FULL_FLAG			0x3F


typedef struct tTruckEnduranceState
{
	BYTE ucVersion;
	BYTE ucAcc;
	BYTE ucBreak;
	BYTE ucConsum;
	WORD16 wEndurance;
	WORD16 wTorque;
	WORD32 dwSegmentFlag;
	PTHREAD_MUTEX_T pMutex;
}T_TRUCK_ENDURANCE_STATE;


#define TRUCK_ACC_ACC_FLAG		1<<0
#define TRUCK_ACC_FULL_FLAG		0x01

typedef struct tTruckAccelerateState
{
	WORD16 wAccelerate;
	BYTE ucPad[2];
	WORD32 dwSegmentFlag;
	PTHREAD_MUTEX_T pMutex;
}T_TRUCK_ACC_STATE;

#define TRUCK_ELEC_MACHTEMP_FLAG		1<<0
#define TRUCK_ELEC_CTRLTEMP_FLAG		1<<1
#define TRUCK_ELEC_MCUSTATUS_FLAG		1<<2
#define TRUCK_ELEC_MCUSELFCHECK_FLAG	1<<3
#define TRUCK_ELEC_ERRORSTATE_FLAG		1<<4
#define TRUCK_ELEC_VOLTAGE_FLAG			1<<5
#define TRUCK_ELEC_FULL_FLAG			0x3F

typedef struct tTruckElecState
{
	BYTE ucElecMachineryTemp;
	BYTE ucElecCtrlTemp;
	BYTE ucMCUStaus;
	BYTE ucMCUSelfCheckStatus;
	BYTE ucErrorState;
	BYTE ucPad[5];
	WORD16 wVoltage;
	WORD32 dwSegmentFlag;
	PTHREAD_MUTEX_T pMutex;
}T_TRUCK_ELEC_STATE;

#define TRUCK_ELEC_SPEED_FLAG	1<<0
#define TRUCK_ELEC_TORQUE_FLAG	1<<1
#define TRUCK_ELEC_CURRENT_FLAG	1<<2
#define TRUCK_ELEC2_FULL_FLAG	0x07

typedef struct tTruckEleccState2
{
	WORD16 wMachinerySpeed;
	WORD16 wTorque;
	WORD16 wMachCurrent;
	WORD16 wPad;
	WORD32 dwSegmentFlag;
	PTHREAD_MUTEX_T pMutex;
}T_TRUCK_ELEC_STATE2;

typedef struct tTruckTotalMile
{
	WORD32 dwTotalMiles;
}T_TRUCK_TOTALMILE;

#define TRUCK_LAMP_RIGHT_FLAG		1<<0
#define TRUCK_LAMP_LEFT_FLAG		1<<1
#define TRUCK_LAMP_POS_FLAG			1<<2
#define TRUCK_LAMP_BACKDOOR_FLAG	1<<3
#define TRUCK_LAMP_FRONTDOOR_FLAG	1<<4
#define TRUCK_LAMP_MIDDOOR_FLAG		1<<5
#define TRUCK_LAMP_HANDBREAK_FLAG	1<<6
#define TRUCK_LAMP_BACKFOG_FLAG		1<<7
#define TRUCK_LAMP_FRONTFOG_FLAG	1<<8
#define TRUCK_LAMP_NEAR_HEAD_FLAG	1<<9
#define TRUCK_LAMP_FAR_HEAD_FLAG	1<<10
#define TRUCK_LAMP_BREAK_FLAG		1<<11
#define TRUCK_LAMP_FULL_FLAG		0x0FFF

typedef struct tTruckLampState
{
	BYTE ucRightLamp;
	BYTE ucLeftLamp;
	BYTE ucPosLamp;
	BYTE ucBackDoorStatus;
	BYTE ucFrontDoorStatus;
	BYTE ucMidDoorStatus;
	BYTE ucHandBreakStatus;
	BYTE ucBackFogLamp;
	BYTE ucFrontFogLamp;
	BYTE ucNearHeadLamp;
	BYTE ucFarHeadLamp;
	BYTE ucBreakLamp;
	
}T_TRUCK_LAMP_STATE;

SWORD32 send_pass_msg2cloud(WORD32 dwInPass,WORD32 dwUpPass,WORD32 dwDownPass);
SWORD32 send_gps_msg2cloud(WORD32 dwLongtitude,WORD32 dwLatitude);
SWORD32 send_can_msg2client(BYTE *pCanPkt,WORD32 dwPktLen);
SWORD32 server_init();

#ifdef __cplusplus
}
#endif

#endif
