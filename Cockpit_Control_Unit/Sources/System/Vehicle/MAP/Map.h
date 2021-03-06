/********************************************************************************
* 工程名称：整车控制器
* 文件名： Map.h
* 功能描述 ： MAP图管理模块头文件
* 引用记录：
* 备注：
* 修订记录：
*		日期		      版本		  作者		描述
*******************************************************************************/


#ifndef __MAP_H

    #define __MAP_H

    #include "TypeDef.h"


    typedef struct
    {
        uint16 xSize;
        uint16 xDist[4];
        uint16 values[4];
    }BattCD_uUW_CUR_TYPE;  //

    typedef struct
    {
        uint16 xSize;
        uint16 xDist[8];
        uint16 values[8];
    }Test_tTempUW_CUR_TYPE;  //

    typedef struct
    {
        uint16 xSize;
        uint16 ySize;
        uint16 xDist[8];
        uint16 yDist[4];
        uint16 values[8*4];
    }Test_tiTimeUW_MAP_TYPE;  //

    typedef struct
    {
        uint16 xSize;
        uint16 xDist[7];
        uint16 values[7];
    }AccPedCD_rUW_CUR_T01_TYPE;  //

    typedef struct
    {
        uint16 xSize;
        uint16 xDist[7];
        uint16 values[7];
    }AccPedCD_rUW_CUR2_T01_TYPE;  //

    typedef struct
    {
        uint16 xSize;
        uint16 xDist[11];
        uint16 values[11];
    }AccPed_Rip_T01_TYPE;  //

    typedef struct
    {
        uint16 xSize;
        uint16 xDist[7];
        uint16 values[7];
    }Breakk_rUW_CUR_T01_TYPE;  //

    typedef struct
    {
        uint16 xSize;
        uint16 xDist[11];
        sint16 values[11];
    }AccPed_Acceleration_T01_Type;  //

    typedef struct
    {
        uint16 xSize;
        uint16 xDist[11];
        sint16 values[11];
    }Break_Acceleration_T01_Type;  //

    typedef struct
    {
        uint16 xSize;
        uint16 xDist[11];
        uint16 values[11];
    }AccPed_Rip_T02_TYPE;  //

    typedef struct
    {
        uint16 xSize;
        uint16 xDist[11];
        sint16 values[11];
    }AccPed_Acceleration_T02_TYPE;  //

    typedef struct
    {
        uint16 xSize;
        uint16 xDist[11];
        sint16 values[11];
    }Break_Acceleration_T02_TYPE;  //

    typedef struct
    {
        uint16 xSize;
        uint16 xDist[7];
        uint16 values[7];
    }AccPedCD_CUR_T02_TYPE;  //

    typedef struct
    {
        uint16 xSize;
        uint16 xDist[7];
        uint16 values[7];
    }Breakk_CUR_T02_TYPE;  //

    typedef struct
    {
        uint16 xSize;
        uint16 xDist[7];
        uint16 values[7];
    }Clutch_CUR_T02_TYPE;  //

    typedef struct
    {
        uint16 xSize;
        sint16 xDist[9];
        sint16 values[9];
    }Speed_To_Pwm_Add_TYPE;  //

    typedef struct
    {
        uint16 xSize;
        sint16 xDist[13];
        uint16 values[13];
    }BackZeroForce_Angle_TYPE;  //

    typedef struct
    {
        uint16 numSizeUW;
    //底层驱动标示(勿改结构)
        uint8 DataSet_idxVerUB_CA[_KWP2000_LEN_K2REI_EMDSID];  //底层数据版本
        uint8 DataSet_idxUserVerUB_CA[16];  //用户数据版本
    //模拟量输入配置(勿改结构)
        uint8 ATDDrv_infoChanUB_CA[_ATD_NUM_CHAN];  //ATDDrv_infoChanUB_CA，16个通道的AD配置情况
        uint8 ATDDrv_idxChanSigConfUB_CA[_ATD_NUM_CHAN];  //ATDDrv_idxChanSigConfUB_CA，各个信号的通道配置
    //数字量输入配置(勿改结构)
        uint8 DIODrv_idxChanSigConfUB_CA[_DIO_NUM_CHAN];  //DIODrv_idxChanSigConfUB_CA，各个数字量输入的通道配置
        uint16 DIODrv_idxChanSwtTypeUW_CA[_DIO_NUM_CHAN];  //高低有效软件可配置数字量输入通道高低有效配置
        uint16 DIODrv_uLevelHighThresUW_C;  //高低有效软件可配通道的高电平电压门限
        uint16 DIODrv_uLevelLowThresUW_C;  //高低有效软件可配置通道的低电平电压门限
    //频率量输出配置(勿改结构)
        uint8 PWMDrv_idxScaleAUB_C;  //PWMDrv_idxScaleAUB_C, PWM模块中时钟源A的分频数
        uint8 PWMDrv_idxScaleBUB_C;  //PWMDrv_idxScaleBUB_C, PWM模块中时钟源B的分频数
        uint8 PWMDrv_idxScaleSAUB_C;  //PWMDrv_idxScaleSAUB_C, PWM模块中时钟源SA的分频数, fSA=fA/(2*N)
        uint8 PWMDrv_idxScaleSBUB_C;  //PWMDrv_idxScaleSBUB_C, PWM模块中时钟源SB的分频数, fSB=fB/(2*N)
        uint8 PWMDrv_idxChanTimerConfUB_CA[4];  //PWMDrv_idxChanTimerConfUB_CA, PWM各通道所用时钟源, X0(PWM1)与X2(PWM5)使用A或SA, X1(PWM3)和X3(PWM7)使用B或SB
        uint8 PWMDrv_stChanPrecTypeUB_CA[4];  //PWM通道的精度类型，0为要求宽频率范围,1为要求宽占空比范围
        uint8 PWMDrv_idxFuncConfUB_CA[4];  //PWMDrv_idxFuncConfUB_CA, PWM模块中各功能所对应PWM管脚配置
    //PWM型低边驱动配置(勿改结构)
        uint8 LSPWMDrv_idxChanSigConfUB_CA[_LSPWM_NUM_CHAN];  //PWM型低边驱动各任务通道配置
    //开关型高低边驱动配置(勿改结构)
        uint8 L9825Drv_swtUseUB_CA[2];  //L9825Drv_swtUseUB_CA, L9825两块芯片的使能配置
        uint8 PSWTDrv_idxChanSigConfUB_CA[_PSWT_NUM_CHAN];  //PSWTDrv_idxChanSigConfUB_CA, 高低边驱动任务号配置，0~7为低边驱动，8~15为高边驱动
    //系统控制(勿改结构)
        uint32 SysCon_stIRevShuOffMskUI_C;  //SysCon_stIRevShuOffMaskUI_C
        uint32 SysCon_stRevShuOffMskUI_C;  //SysCon_stRevShuOffMaskUI_C
        uint16 SysCon_stExtWaitMskUW_C;  //SysCon_stExtWaitMskUW_C，PostDrive状态等待结束的事件Mask
        uint16 SysCon_stExtWait2MskUW_C;  //SysCon_stExtWait2MskUW_C，PostDrive状态等待结束的事件Mask
        uint32 SysCon_tiPostDriveMaxUI_C;  //SysCon_tiPostDriveMaxUI_C，PostDrive最长时间，s
        uint32 SysCon_tiShutdownMaxUI_C;  //SysCon_tiShutdownMaxUI_C，shutdown状态最长执行时间，s
        uint32 SysCon_tiPostDriveMinUI_C;  //SysCon_tiPostDriveMinUI_C，PostDrive最短时间，s
    //通讯设置(勿改结构)
        uint16 MSCANDrv_infoBaudrateUW_CA[3];  //MSCANDrv_infoBaudrateUW_CA
        uint16 KWPCAN_tiToBMaxUW_C;  //KWPCAN_tiToBMaxUW_C,ToB 最长时间50*100ms
        uint16 KWPCAN_tiToCMaxUW_C;  //KWPCAN_tiToCMaxUW_C,ToC 最长时间50*100ms
        uint16 KWPCAN_numFCWMaxUW_C;  //KWPCAN_numFCWMaxUW_C,最多的Flow Control Wait Frame数目16
        uint8 CCP_StationIdUB_CA[10];  //
    //底层其它功能(勿改结构)
        uint16 SigTst_bEnUW_C;  //SigTst_bEnUW_C，是否直接控制9825和9362
        uint8 SigTst_bChanL9825UB_CA[_L9825_NUM_CHAN];  //SigTst_bChanL9825UB_CA，各通道开关
        uint32 SigTst_fChanL9825UI_CA[_L9825_NUM_PWMCHAN];  //SigTst_fChanL9825UI_CA，L9825各通道的PWM频率
        uint16 SigTst_rChanL9825UW_CA[_L9825_NUM_PWMCHAN];  //SigTst_rChanL9825UW_CA，L9825各通道的占空比
        uint8 SigTst_bChanBTS724UB_CA[8];  //SigTst_bChanBTS724UB_CA，高边驱动开关
    //电源管理(勿改结构)
        uint16 BattCD_uLogicOnUW_C;  //逻辑电上电电压限值
        BattCD_uUW_CUR_TYPE BattCD_uUW_CUR;  //蓄电池电压特性
    }HardwareMap_infoMap1S_TYPE;

    typedef struct
    {
    //数据版本
        uint8 VehDataSet_idxSupervisorVerUB_CA[16];  //标准数据版本号
        uint8 VehDataSet_idxUserVerUB_CA[16];  //用户数据版本号
    //通用车型基础标定
        uint8 Test_bSaveCmdUW_C;  //单值标定变量示例 触发EEPROM的存储
        Test_tTempUW_CUR_TYPE Test_tTempUW_CUR;  //曲线标定变量示例
        Test_tiTimeUW_MAP_TYPE Test_tiTimeUW_MAP;  //曲面标定变量示例
        uint8 Test_strVersionUB_CA[16];  //字符串标定变量示例
        uint16 Percent_Filter_And_All[4];  //油门滤波百分比-0总数-1增量占比-2减量占比
        uint8 VCU_C_1[7];  //整车控制帧-版本-日期-型号H渣土车
        uint8 Vehicle_Type;  //车型定义 1 实验室驾驶舱 2 钢架驾驶舱
        uint16 AccPed_Differ_Limit;  //双油门差值极限 100倍 %
        uint16 AccPed_BreakErr_Max;  //油门刹车同时踩故障 最大油门值 0~100.00%
        uint16 Error_PowOn_Delay_Max;  //上电故障检测延迟 *10ms
        uint8 Err_AccPed_Index_Max;  //油门故障检测延迟 *10ms
        uint8 LostEPSCom_Index_Max;  //EPS通讯丢失超时 *10ms
        uint8 LostVehcoCom_Index_Max;  //行车状态通讯丢失超时 *10ms
        uint8 LostVehcoStateCom_Index_Max;  //车辆状态通讯丢失超时 *10ms
        uint8 LostVehcoBreakCom_Index_Max;  //车辆制动状态通讯丢失超时 *10ms
        uint8 LostVehcoSteering_Wheel_AngleCom_Index_Max;  //模拟方向盘通讯丢失超时 *10ms
        sint16 EPS_Angle_Max;  //EPS角度命令最大值 0.1°
        sint16 EPS_Angle_Min;  //EPS角度命令最小值 0.1°
        uint16 R_Break_Min;  //远程制动j激活最小开度
        sint16 Wheel_Offset;  //方向盘编码器偏移
        sint16 EPS_Offset;  //EPS角度偏移
        uint8 DriveModeDebug;  //远程驾驶模式DEBUG激活
        uint16 D_Dang_Change_Delay_Index_Max;  //
        uint8 Break_Acceleration_Min_Step;  //制动控制最小步进
        uint8 Eps_AngleMaxSpeed;  //
        uint16 Auto_Hold_Delay_Max;  //Auto_Hold_Delay_Max
        sint16 Auto_Hold_Release_Torque;  //
    //实验室驾驶舱
        uint16 Motor_Vehicle_Speed_Scale_T01;  //车辆速比
        uint8 AccPedCD_TYPE_T01;  //油门类型 1 双线 2 单线
        uint8 Breakk_Type_T01;  //制动类型  0：单路开关量 1：双路互反开关量  2：单路模拟量 3：双路模拟量
        AccPedCD_rUW_CUR_T01_TYPE AccPedCD_rUW_CUR_T01;  //油门开度
        AccPedCD_rUW_CUR2_T01_TYPE AccPedCD_rUW_CUR2_T01;  //油门开度2
        AccPed_Rip_T01_TYPE AccPed_Rip_T01;  //油门曲线
        Breakk_rUW_CUR_T01_TYPE Breakk_rUW_CUR_T01;  //制动踏板深度
        AccPed_Acceleration_T01_Type AccPed_Acceleration_T01;  //油门开度-加速度
        Break_Acceleration_T01_Type Break_Acceleration_T01;  //制动深度-制动力给定%
    //自制驾驶舱
        uint16 Steering_Whee_Auto_Check_Pwm;  //方向盘自检PWM占空比
        uint8 Steering_Whee_Auto_Check_Still_Time_Max;  //方向盘自检堵转时间上限 10ms
        uint8 Die_Angle;  //方向盘死区角度
        uint16 Motor_Vehicle_Speed_Scale_T02;  //车辆速比
        uint8 AccPedCD_TYPE_T02;  //油门类型 1 双线 2 单线 4 CAN
        uint8 Breakk_Type_T02;  //制动类型  0：单路开关量 1：双路互反开关量  2：单路模拟量 3：双路模拟量    4:CAN
        AccPed_Rip_T02_TYPE AccPed_Rip_T02;  //油门曲线
        AccPed_Acceleration_T02_TYPE AccPed_Acceleration_T02;  //油门开度-加速度
        Break_Acceleration_T02_TYPE Break_Acceleration_T02;  //制动深度-制动力给定%
        AccPedCD_CUR_T02_TYPE AccPedCD_CUR_T02;  //油门曲线
        Breakk_CUR_T02_TYPE Breakk_CUR_T02;  //制动踏板曲线
        Clutch_CUR_T02_TYPE Clutch_CUR_T02;  //离合踏板曲线
        uint16 Level_Low;  //低门限
        uint16 Level_High;  //高门限
        uint8 Rtest_whee;  //
        sint16 Tag_Speed;  //
        Speed_To_Pwm_Add_TYPE Speed_To_Pwm_Add;  //Speed_To_Pwm_Add
        uint32 Fsetpwm;  //Fsetpwm
        BackZeroForce_Angle_TYPE BackZeroForce_Angle;  //回正力曲线
    }VehicleMap_infoMap1S_TYPE;

    //底层驱动标示(勿改结构)
    #define DataSet_idxUserVerUB_CA HardwareMap_infoMap1SP->DataSet_idxUserVerUB_CA

    //模拟量输入配置(勿改结构)
    #define ATDDrv_infoChanUB_CA HardwareMap_infoMap1SP->ATDDrv_infoChanUB_CA
    #define ATDDrv_idxChanSigConfUB_CA HardwareMap_infoMap1SP->ATDDrv_idxChanSigConfUB_CA

    //数字量输入配置(勿改结构)
    #define DIODrv_idxChanSigConfUB_CA HardwareMap_infoMap1SP->DIODrv_idxChanSigConfUB_CA
    #define DIODrv_idxChanSwtTypeUW_CA HardwareMap_infoMap1SP->DIODrv_idxChanSwtTypeUW_CA
    #define DIODrv_uLevelHighThresUW_C HardwareMap_infoMap1SP->DIODrv_uLevelHighThresUW_C
    #define DIODrv_uLevelLowThresUW_C HardwareMap_infoMap1SP->DIODrv_uLevelLowThresUW_C

    //频率量输出配置(勿改结构)
    #define PWMDrv_idxScaleAUB_C HardwareMap_infoMap1SP->PWMDrv_idxScaleAUB_C
    #define PWMDrv_idxScaleBUB_C HardwareMap_infoMap1SP->PWMDrv_idxScaleBUB_C
    #define PWMDrv_idxScaleSAUB_C HardwareMap_infoMap1SP->PWMDrv_idxScaleSAUB_C
    #define PWMDrv_idxScaleSBUB_C HardwareMap_infoMap1SP->PWMDrv_idxScaleSBUB_C
    #define PWMDrv_idxChanTimerConfUB_CA HardwareMap_infoMap1SP->PWMDrv_idxChanTimerConfUB_CA
    #define PWMDrv_stChanPrecTypeUB_CA HardwareMap_infoMap1SP->PWMDrv_stChanPrecTypeUB_CA
    #define PWMDrv_idxFuncConfUB_CA HardwareMap_infoMap1SP->PWMDrv_idxFuncConfUB_CA

    //PWM型低边驱动配置(勿改结构)
    #define LSPWMDrv_idxChanSigConfUB_CA HardwareMap_infoMap1SP->LSPWMDrv_idxChanSigConfUB_CA

    //开关型高低边驱动配置(勿改结构)
    #define L9825Drv_swtUseUB_CA HardwareMap_infoMap1SP->L9825Drv_swtUseUB_CA
    #define PSWTDrv_idxChanSigConfUB_CA HardwareMap_infoMap1SP->PSWTDrv_idxChanSigConfUB_CA

    //系统控制(勿改结构)
    #define SysCon_stIRevShuOffMskUI_C HardwareMap_infoMap1SP->SysCon_stIRevShuOffMskUI_C
    #define SysCon_stRevShuOffMskUI_C HardwareMap_infoMap1SP->SysCon_stRevShuOffMskUI_C
    #define SysCon_stExtWaitMskUW_C HardwareMap_infoMap1SP->SysCon_stExtWaitMskUW_C
    #define SysCon_stExtWait2MskUW_C HardwareMap_infoMap1SP->SysCon_stExtWait2MskUW_C
    #define SysCon_tiPostDriveMaxUI_C HardwareMap_infoMap1SP->SysCon_tiPostDriveMaxUI_C
    #define SysCon_tiShutdownMaxUI_C HardwareMap_infoMap1SP->SysCon_tiShutdownMaxUI_C
    #define SysCon_tiPostDriveMinUI_C HardwareMap_infoMap1SP->SysCon_tiPostDriveMinUI_C

    //通讯设置(勿改结构)
    #define MSCANDrv_infoBaudrateUW_CA HardwareMap_infoMap1SP->MSCANDrv_infoBaudrateUW_CA
    #define KWPCAN_tiToBMaxUW_C HardwareMap_infoMap1SP->KWPCAN_tiToBMaxUW_C
    #define KWPCAN_tiToCMaxUW_C HardwareMap_infoMap1SP->KWPCAN_tiToCMaxUW_C
    #define KWPCAN_numFCWMaxUW_C HardwareMap_infoMap1SP->KWPCAN_numFCWMaxUW_C
    #define CCP_StationIdUB_CA HardwareMap_infoMap1SP->CCP_StationIdUB_CA

    //底层其它功能(勿改结构)
    #define SigTst_bEnUW_C HardwareMap_infoMap1SP->SigTst_bEnUW_C
    #define SigTst_bChanL9825UB_CA HardwareMap_infoMap1SP->SigTst_bChanL9825UB_CA
    #define SigTst_fChanL9825UI_CA HardwareMap_infoMap1SP->SigTst_fChanL9825UI_CA
    #define SigTst_rChanL9825UW_CA HardwareMap_infoMap1SP->SigTst_rChanL9825UW_CA
    #define SigTst_bChanBTS724UB_CA HardwareMap_infoMap1SP->SigTst_bChanBTS724UB_CA

    //电源管理(勿改结构)
    #define BattCD_uLogicOnUW_C HardwareMap_infoMap1SP->BattCD_uLogicOnUW_C
    #define BattCD_uUW_CUR HardwareMap_infoMap1SP->BattCD_uUW_CUR

    //数据版本
    #define VehDataSet_idxSupervisorVerUB_CA VehicleMap_infoMap1SP->VehDataSet_idxSupervisorVerUB_CA
    #define VehDataSet_idxUserVerUB_CA VehicleMap_infoMap1SP->VehDataSet_idxUserVerUB_CA

    //通用车型基础标定
    #define Test_bSaveCmdUW_C VehicleMap_infoMap1SP->Test_bSaveCmdUW_C
    #define Test_tTempUW_CUR VehicleMap_infoMap1SP->Test_tTempUW_CUR
    #define Test_tiTimeUW_MAP VehicleMap_infoMap1SP->Test_tiTimeUW_MAP
    #define Test_strVersionUB_CA VehicleMap_infoMap1SP->Test_strVersionUB_CA
    #define Percent_Filter_And_All VehicleMap_infoMap1SP->Percent_Filter_And_All
    #define VCU_C_1 VehicleMap_infoMap1SP->VCU_C_1
    #define Vehicle_Type VehicleMap_infoMap1SP->Vehicle_Type
    #define AccPed_Differ_Limit VehicleMap_infoMap1SP->AccPed_Differ_Limit
    #define AccPed_BreakErr_Max VehicleMap_infoMap1SP->AccPed_BreakErr_Max
    #define Error_PowOn_Delay_Max VehicleMap_infoMap1SP->Error_PowOn_Delay_Max
    #define Err_AccPed_Index_Max VehicleMap_infoMap1SP->Err_AccPed_Index_Max
    #define LostEPSCom_Index_Max VehicleMap_infoMap1SP->LostEPSCom_Index_Max
    #define LostVehcoCom_Index_Max VehicleMap_infoMap1SP->LostVehcoCom_Index_Max
    #define LostVehcoStateCom_Index_Max VehicleMap_infoMap1SP->LostVehcoStateCom_Index_Max
    #define LostVehcoBreakCom_Index_Max VehicleMap_infoMap1SP->LostVehcoBreakCom_Index_Max
    #define LostVehcoSteering_Wheel_AngleCom_Index_Max VehicleMap_infoMap1SP->LostVehcoSteering_Wheel_AngleCom_Index_Max
    #define EPS_Angle_Max VehicleMap_infoMap1SP->EPS_Angle_Max
    #define EPS_Angle_Min VehicleMap_infoMap1SP->EPS_Angle_Min
    #define R_Break_Min VehicleMap_infoMap1SP->R_Break_Min
    #define Wheel_Offset VehicleMap_infoMap1SP->Wheel_Offset
    #define EPS_Offset VehicleMap_infoMap1SP->EPS_Offset
    #define DriveModeDebug VehicleMap_infoMap1SP->DriveModeDebug
    #define D_Dang_Change_Delay_Index_Max VehicleMap_infoMap1SP->D_Dang_Change_Delay_Index_Max
    #define Break_Acceleration_Min_Step VehicleMap_infoMap1SP->Break_Acceleration_Min_Step
    #define Eps_AngleMaxSpeed VehicleMap_infoMap1SP->Eps_AngleMaxSpeed
    #define Auto_Hold_Delay_Max VehicleMap_infoMap1SP->Auto_Hold_Delay_Max
    #define Auto_Hold_Release_Torque VehicleMap_infoMap1SP->Auto_Hold_Release_Torque

    //实验室驾驶舱
    #define Motor_Vehicle_Speed_Scale_T01 VehicleMap_infoMap1SP->Motor_Vehicle_Speed_Scale_T01
    #define AccPedCD_TYPE_T01 VehicleMap_infoMap1SP->AccPedCD_TYPE_T01
    #define Breakk_Type_T01 VehicleMap_infoMap1SP->Breakk_Type_T01
    #define AccPedCD_rUW_CUR_T01 VehicleMap_infoMap1SP->AccPedCD_rUW_CUR_T01
    #define AccPedCD_rUW_CUR2_T01 VehicleMap_infoMap1SP->AccPedCD_rUW_CUR2_T01
    #define AccPed_Rip_T01 VehicleMap_infoMap1SP->AccPed_Rip_T01
    #define Breakk_rUW_CUR_T01 VehicleMap_infoMap1SP->Breakk_rUW_CUR_T01
    #define AccPed_Acceleration_T01 VehicleMap_infoMap1SP->AccPed_Acceleration_T01
    #define Break_Acceleration_T01 VehicleMap_infoMap1SP->Break_Acceleration_T01

    //自制驾驶舱
    #define Steering_Whee_Auto_Check_Pwm VehicleMap_infoMap1SP->Steering_Whee_Auto_Check_Pwm
    #define Steering_Whee_Auto_Check_Still_Time_Max VehicleMap_infoMap1SP->Steering_Whee_Auto_Check_Still_Time_Max
    #define Die_Angle VehicleMap_infoMap1SP->Die_Angle
    #define Motor_Vehicle_Speed_Scale_T02 VehicleMap_infoMap1SP->Motor_Vehicle_Speed_Scale_T02
    #define AccPedCD_TYPE_T02 VehicleMap_infoMap1SP->AccPedCD_TYPE_T02
    #define Breakk_Type_T02 VehicleMap_infoMap1SP->Breakk_Type_T02
    #define AccPed_Rip_T02 VehicleMap_infoMap1SP->AccPed_Rip_T02
    #define AccPed_Acceleration_T02 VehicleMap_infoMap1SP->AccPed_Acceleration_T02
    #define Break_Acceleration_T02 VehicleMap_infoMap1SP->Break_Acceleration_T02
    #define AccPedCD_CUR_T02 VehicleMap_infoMap1SP->AccPedCD_CUR_T02
    #define Breakk_CUR_T02 VehicleMap_infoMap1SP->Breakk_CUR_T02
    #define Clutch_CUR_T02 VehicleMap_infoMap1SP->Clutch_CUR_T02
    #define Level_Low VehicleMap_infoMap1SP->Level_Low
    #define Level_High VehicleMap_infoMap1SP->Level_High
    #define Rtest_whee VehicleMap_infoMap1SP->Rtest_whee
    #define Tag_Speed VehicleMap_infoMap1SP->Tag_Speed
    #define Speed_To_Pwm_Add VehicleMap_infoMap1SP->Speed_To_Pwm_Add
    #define Fsetpwm VehicleMap_infoMap1SP->Fsetpwm
    #define BackZeroForce_Angle VehicleMap_infoMap1SP->BackZeroForce_Angle

    /*******************************************************************************************
                             MAP全局变量设置 
    ********************************************************************************************/
       
    #pragma CONST_SEG __GPAGE_SEG PARAMETERS_ROM1    
    extern volatile const HardwareMap_infoMap1S_TYPE HardwareMap_infoMap1S; //底层标定数据
    extern volatile const VehicleMap_infoMap1S_TYPE VehicleMap_infoMap1S;  //整车标定数据
    #pragma CONST_SEG DEFAULT
    
    typedef struct
    {
        uint16 dataChkSumUW;
        uint16 idxProYearUW;
        uint8 idxProMonthUB;
        uint8 idxProDayUB; //程序刷写日期
        uint8 dataReserved1UB;
        uint8 idxHourUW;
        uint8 idxMinuteUB;
        uint8 idxSecondUB; //程序刷写时间
        uint8 idxToolIDUBA[60];
    }HardwareMap_infoDataCheck1S_TYPE;
    
    #pragma CONST_SEG __GPAGE_SEG DATA_VERIFY1
        extern volatile const HardwareMap_infoDataCheck1S_TYPE HardwareMap_infoDataCheck1S;  
    #pragma CONST_SEG DEFAULT
            
                
    extern HardwareMap_infoMap1S_TYPE  * __far HardwareMap_infoMap1SP;   //指向GlabalMap的一个指针
    extern VehicleMap_infoMap1S_TYPE  * __far VehicleMap_infoMap1SP;     
    

#endif 
