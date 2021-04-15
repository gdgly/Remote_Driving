/*******************************************************************************
* 工程名称：整车管理
* 文件名： VehCo.c
* 功能描述 ： 整车管理协调
* 引用记录：
* 备注： 
* 修订记录：
*		日期		      版本		  作者		描述
*   
*******************************************************************************/

#include "GlobalDef.h"
#include "TypeDef.h"
#include "VehCo.h"
#include "VehCo_Type.h"
#include "CANNet.h"

//所有历史数据需定义在下述段中
//#pragma DATA_SEG __GPAGE_SEG RUNPAR_EEPROM
#ifndef AECS_HW
#pragma DATA_SEG __GPAGE_SEG RUNPAR_EEPROM
#else
#pragma DATA_SEG NVRAM
#endif
uint16 VehCo_ctTestUW = 0; //示例EEPROM存储变量
uint8 Vehco_State_Stop_Step;
//Vin可刷写次数、电机号可刷写次数、电池编码可刷写次数、电控可刷写次数
uint8 Rw_Vin_Times, Rw_Mot_Times, Rw_Bms_Times, Rw_MCU_Times;
uint8 Vin_Code[21];
uint8 Mot_Code[21];
uint8 MCU_Code[21];
uint16 PowOn_Times;
uint8 Flash_Times;
uint32 INS_TD_LastSave; //VCU记忆仪表里程

//#pragma DATA_SEG DEFAULT
#ifndef AECS_HW
#pragma DATA_SEG DEFAULT
#else
#pragma DATA_SEG S12X_RAM
#endif

//uint8  Air_Reservoirs_Over_Pressure_1=0,Air_Reservoirs_Over_Pressure_2=0;
//uint8  Air_Pressure_Ok=0;   //达到排气点
//uint8  Air_Pressure_Ok_OverTime=0;

uint16 Afterrun_Delay_Index; //关机延迟计数
uint8 VehCo_tiMonitorUB;
uint8 VehCo_stAfterrunUB; //关机准备状态

uint8 AD_fliter_index;
uint32 Adtmp1;
uint32 Adtmp2;
uint16 uAD1; //油门1电压
uint16 uAD2; //油门2电压
uint32 Adtmp3, Adtmp4;
uint16 uAD3, uAD4;

uint16 AccPed1;
uint16 AccPed2;

uint16 AccPed; //经油门曲线处理后的油门值
uint16 Breakk; //制动踏板状态

uint16 AccPed_Differ; //双油门开度差值
uint8 Failure_Recovery_Delay[24] = {0};
uint8 AccPed_Delay; //0油门踩下延迟

uint8 Err_AccPed = 0, Err_AccPed_Index = 0;

sint16 Percent100;
//uint16 FanStopDelay_Index=0;

uint8 RightTrunSignal_state = 2; //右转向灯状态
uint8 LeftTrunSignal_state = 2;  //左转向灯状态
uint8 bLso;
uint8 bHso;
//uint32 fSet;  //频率设定   0.01~5000.00Hz
//uint16 rSet;  //占空比设定 0~10000

uint16 Error_PowOn_Delay;  //上电检测故障延迟
uint8 Fault_Detection = 0; //故障检测许可

uint8 R_Control_Switch = 0; //远程驾驶激活开关
uint8 Door_Switch = 0;      //远程驾驶仓门开关

sint16 Vehco_Speed_Calc = 0; //计算车速   10倍

uint16 D_Dang_Change_Delay_Index = 0;

uint8 Auto_Hold_Delay = 0; //停车踩刹车时长

uint8 Dang_R = 0, Dang_D = 0, Dang_N = 0, Dang_P = 0, Want_Dang = 0;

uint8 Steering_Wheel_IN1 = 0, Steering_Wheel_IN2 = 0; // 1低2高 正转 顺时针 1高2低 反转  00 阻尼  11 悬空
uint16 Steering_Wheel_PWM_Out = 50;                   //0.01% PWM

sint16 Steering_Wheel_Angle_BaseCAN_Max = 0, Steering_Wheel_Angle_BaseCAN_Min = 0; //模拟方向盘最大测试角度
sint16 Wheel_Offset_Auto = 0;                                                      //自校准模拟方向盘0点偏移
uint8 Steering_Whee_Auto_Check_Mark = 0;                                           //方向盘自检成功
uint8 Steering_Whee_Auto_Check_Step;                                               //0 未进行   1 左转中  2 左转到终点 3 右转中  4 右转到终点 5 回中中   6 成功回正  7失败

uint8 Steering_Whee_Auto_Check_Still_Time = 0; //方向盘测试堵转时间
uint8 Steering_Whee_Auto_Check_Slow_Index = 0; //自动回中换向次数

uint16 AccPed_CAN = 0, Breakk_CAN = 0, Clutch_CAN = 0;
uint8 EPB_Switch_Want = 0; //0 无动作 1 拉起 2 释放 3 行进中制动
uint8 EPB_Switch_LH = 0, EPB_Switch_HL = 0;

uint16 Steering_Whee_Check_Powon_Delay = 0;
sint16 Steering_Wheel_Speed = 0;
sint16 Steering_Wheel_Angle_BaseCAN_Last = 0;
sint16 Pwm_Add = 0;
uint16 BackZeroForce_PWM = 0;

//接口外部引用
extern uint8 bDiSatus[24];  //DI口开关量状态
extern uint16 uADSatus[25]; //DI口模拟量数值
extern uint8 bDoSatus[20];  //DO口开关量状态
extern uint16 uAD4Satus[4];
extern uint8 BattCD_bLogicOnUB; //逻辑电开关         //不可删除
extern uint16 BattCD_uUW;       //蓄电池电压          //不可删除

//其他外部变量引用
extern uint8 Vehco_Type_Test; //车型定义 1 实验室驾驶舱 2 钢架驾驶舱
extern uint8 LostEPSCommunication;
extern uint16 LostEPSCom_Index;
extern uint8 LostVehcoCommunication;
extern uint16 LostVehcoCom_Index;
extern uint8 LostVehcoStateCommunication;
extern uint16 LostVehcoStateCom_Index;
extern uint8 LostVehcoBreakCommunication;
extern uint16 LostVehcoBreakCom_Index;
extern uint8 LostVehcoSteering_Wheel_AngleCommunication;
extern uint16 LostVehcoSteering_Wheel_AngleCom_Index;

extern sint16 Steering_Wheel_Angle_BaseCAN;
extern uint8 AccPed_AD255;
extern uint8 Breakk_AD255;
extern uint8 Clutch_AD255;
extern uint8 Steering_Wheel_Angle_Mot_Temp;
extern uint8 Steering_Wheel_Angle_Life;

extern sint16 Steering_Wheel_Angle_Code;    //方向盘旋转编码  -32768,360°/5000 右转为正
extern uint8 Steering_Wheel_Angle_ZeroMark; //方向盘是否经过0点校正

extern uint8 EPS_Contral_Mode;   //EPS控制模式
extern sint16 EPS_Demand_Angle;  //EPS角度命令 0.1度步进   -1080.0~1080.0°
extern uint8 EPS_Angle_MaxSpeed; //EPS 最大角速度

extern sint8 Vehco_Acceleration_Command; //加速度指令 -12.8~12.7
extern uint8 EPB_Command;                //电子手刹指令 0 无动作 1 拉起 2 释放 3 行进中制动
extern uint8 Dang_Command;               //档位指令 0 N 1 D 2 R 3 保留
extern uint8 Emergency_Braking;          //紧急制动指令

extern uint8 DriverMode_Command;         //自驾模式状态 0 手动 1 保留 2 保留 3 远程
extern uint8 Door_Command;               //车门控制命令 0 无动作 1 开门 2 关门 3 保留
extern uint8 Beep_Command;               //喇叭控制     0 无动作 1 开启 2 关闭 3 保留
extern uint8 R_DriverSystem_State;       //远程驾驶系统状态 0 连接中 1 连接完成 2 断开中 3 断开
extern uint8 R_DriverSystem_Level;       //远程驾驶系统可信级别 0 不可信，立刻接管 1 低质量 ，人工监视 2 最佳状态，完全控制
extern uint8 R_DriverSystem_Fault_Level; //远程驾驶系统故障级别 0 无故障 1 轻微 提示 2 一般故障 跛行 3 严重故障 退出远程驾驶状态
extern uint8 AC_Mode;                    //车辆冷暖空调控制 0 关机 1 制冷 2 制热 3 自动
extern uint8 AC_Temperature;             //车辆冷暖空调设置温度 0.5 偏移16

extern uint8 R_Break_Mode; //远程制动激活   0x00：未激活,本地控制 0x01：激活，制动控制器执行远程网关指令 0x02:融合激活，取最大
extern uint8 R_Break_Rang; //远程制动力请求 0~100%

//***************以下为从车辆接收数据********************

extern uint8 EPS_Run_Mode;       //EPS返回当前控制模式
extern sint16 EPS_Driver_Torque; //EPS返回驾驶员方向盘力矩 -12.8~12.7Nm
extern sint16 EPS_Torque;        //EPS输出力矩 0.3Nm/bit -38.4~38.1Nm
extern sint16 EPS_Angle;         //EPS当前角度 0.1度步进   -1080.0~1080.0°
extern uint8 EPS_Temp;           //EPS控制器温度 2℃/bit 0~126
extern uint8 EPS_ERR_Level;      //EPS故障级别

extern uint8 EPB_State;                  //EPB状态
extern uint8 Dang_State;                 //车辆档位状态
extern sint16 Motor_Speed, Motor_Torque; //电机转速、电机扭矩
extern sint8 Vehco_Acceleration_Back;    //车辆当前上报加速度 -12.8~12.7

extern uint8 DriverMode_State;     //自驾模式状态
extern uint8 DoorState;            //车门状态
extern sint16 Vehco_Speed;         //车速
extern uint8 SOC;                  //SOC 精度0.5% 0~100%
extern uint8 SmartPowerCommand;    //智能系统电源指令  00 启动  01 关机
extern uint8 Change_State;         //充电状态 00 无需充电 01充电完成 02 充电中 03 电量低需要充电
extern uint8 Change_Connect_State; //充电连接状态 0 未连接 1 充电枪已插入
extern uint8 Vehco_Fault_Level;    //整车故障级别

extern uint16 B_Voltage; //电池电压 0~800.0V
extern sint16 B_Current; //电池电流 -500.0~500.0A

extern uint8 Vehco_Power_State; //整车动力状态 00 INIT ON档 01 READY 02 跛行 03 故障无高压

extern uint8 Isolation_Level; //绝缘故障等级 0 正常 1 一般 2 严重 3 保留
extern uint8 MCU_Fault_Level; //电控故障级别  0 正常 1轻微 2 一般 3 严重
extern uint8 BMS_Fault_Level; //BMS故障级别   0 正常 1轻微 2 一般 3 严重

extern uint8 Vehco_Break_Mode; //制动控制器模式 0x00：未激活,本地控制 0x01：激活，制动控制器执行远程网关指令 0x02:融合激活，取最大
extern uint8 Vehco_Break_Rang; //制动力状态  0~100%

//根据车型选择曲线标定值
extern uint16 *__far AccPedCD_rUW_CUR_Addr;  //油门开度
extern uint16 *__far AccPedCD_rUW_CUR2_Addr; //油门开度2
extern uint16 *__far AccPed_Rip_Addr;        //油门曲线
extern uint16 *__far Breakk_rUW_CUR_Addr;    //制动踏板深度

extern uint16 Motor_Vehicle_Speed_Scale_Get;   //转速-车速比例
extern uint16 *__far AccPed_Acceleration_Addr; //油门开度-加速度
extern uint16 *__far Break_Acceleration_Addr;  //制动深度-制动力给定%

//根据车型选择单值标定值
extern uint8 AccPedCD_TYPE; //油门类型 1 双线 2 单线
extern uint8 Breakk_Type;   //制动类型  0：单路开关量 1：双路互反开关量  2：单路模拟量 3：双路模拟量

#ifndef AECS_HW
#pragma CODE_SEG DEFAULT
#else
#pragma CODE_SEG VCU_CODE
#endif

//整车管理初始化
//在VehCo_stNewUB=Init状态时调用一次
void VehCo_Init(void)
{
  srand(2);
  Fault_Detection = 0;
  Afterrun_Delay_Index = 0;
  Vehco_Type_Test = Vehicle_Type;
  Vehicle_Single_Variable_Set(); //单值变量映射
  Vehicle_Type_Set();            //曲线表映射
}

//整车管理10ms基准运行任务
void VehCo_Manage10ms(void)
{

  Vehicle_Single_Variable_Set(); //单值变量映射
  Vehicle_Type_Set();            //曲线表映射

  //底层IO接口转换
  VehCo_Interface();

  //更新传感器信号
  VehCo_UpdateSig();

  //诊断传感器信号  50ms间隔
  VehCo_tiMonitorUB = (uint8)((VehCo_tiMonitorUB + 1) % _VEHCO_TIME_MONITOR_PERIOD); //_VEHCO_TIME_MONITOR_PERIOD  10
  if ((VehCo_tiMonitorUB == 0) || (VehCo_tiMonitorUB == 5))
  {
    //部件状态监控
    VehCo_Monitor();
    if (VehCo_tiMonitorUB == 0)
    {
      Steering_Wheel_Speed = Steering_Wheel_Angle_BaseCAN - Steering_Wheel_Angle_BaseCAN_Last;

      Steering_Wheel_Angle_BaseCAN_Last = Steering_Wheel_Angle_BaseCAN;

      //Speed_To_Pwm_Add
      if ((Steering_Whee_Auto_Check_Step > 0) && (Steering_Whee_Auto_Check_Step < 6))
      {

        Pwm_Add = Intpol_s16s16((uint16 * __far) & Speed_To_Pwm_Add, abs(Steering_Wheel_Speed));
      }
    }
  }

  //上电故障检测延迟 500ms
  if (Error_PowOn_Delay <= Error_PowOn_Delay_Max)
  {
    Fault_Detection = 0;
    Error_PowOn_Delay++;
  }
  else
  {
    Fault_Detection = 1;
    Error_PowOn_Delay = Error_PowOn_Delay_Max;
    if (Rtest_whee == 1)
    {
      Steering_Whee_Check_Powon_Delay = 0;
      Steering_Whee_Auto_Check_Mark = 0;
      Steering_Whee_Auto_Check_Step = 0;
      Steering_Whee_Auto_Check_Slow_Index = 0;
      Steering_Wheel_Angle_BaseCAN_Max = 0;
      Steering_Wheel_Angle_BaseCAN_Min = 0;
    }

    if (Steering_Whee_Check_Powon_Delay < 500)
    {
      Steering_Whee_Check_Powon_Delay++;
    }
    else
    {
      if (Steering_Whee_Auto_Check_Mark == 0)
      {
        //模拟方向盘自动校正
        Steering_Whee_Auto_Check();
      }
    }

    if (Steering_Whee_Auto_Check_Mark == 1)
    {
      //回正力计算
      BackZeroForce_Angle_Calc();

      //自动回正
      Steering_Whee_Back_Zero();
    }

    if (AccPedCD_TYPE == 4)
    {
      //CAN油门刹车离合信号处理
      CAN_AccPed_Breakk();
    }

    //车辆故障判断
    Vehco_Errpr_Judge();
    //驾驶模式判断
    Vehco_Driver_Mode();
    //远程档位控制
    Vehco_Dang();
    //方向盘角度控制
    Steering_Wheel();
    //远程加速度控制
    R_Vehco_Acceleration();
    //自动EPB
    Auto_Hold_EPB();
  }

  //设定DO输出
  VehCo_DOset(0);
  //更新传感器信号
  VehCo_UpdateSig();

  //整车控制状态机
  VehCo_StateMachine();

  //关机准备
  VehCo_Afterrun();

  //实验代码
  //VehCo_Test();

  //示例
  /**********************************************************************************************************/
  //idxIncUB=0;

  if ((Test_bSaveCmdUW_C == 1) && (EEPROMDrv_bSaveInstantUB == 0)) //单值标定变量示例
  {
    //触发EEPROM的存储
    EEPROMDrv_bSaveInstantUB = 1;
  }

  /**********************************************************************************************************/
}

//整车管理1ms基准运行任务
void VehCo_Manage1ms(void)
{
  //AD_fliter_index    _AD_fliter_Cycles       uAD1Staus[10]      2路油门10次采集 uAD1 uAD2

#ifndef AECS_HW //易控
  Adtmp1 = Adtmp1 + ATDDrv_GetChanResult(1);
  Adtmp2 = Adtmp2 + ATDDrv_GetChanResult(2);
  Adtmp3 = Adtmp3 + ATDDrv_GetChanResult(3);
  Adtmp4 = Adtmp4 + ATDDrv_GetChanResult(4);

#else //奥易克斯
  Adtmp1 = Adtmp1 + ATDDrv_GetChanResult(_ATD_INDEX_VLT1_CHAN);
  Adtmp2 = Adtmp2 + ATDDrv_GetChanResult(_ATD_INDEX_VLT2_CHAN);
  Adtmp3 = Adtmp3 + ATDDrv_GetChanResult(_ATD_INDEX_VLT3_CHAN);
  Adtmp4 = Adtmp4 + ATDDrv_GetChanResult(_ATD_INDEX_VLT4_CHAN);
#endif

  AD_fliter_index = AD_fliter_index + 1;
  if (AD_fliter_index == _AD_fliter_Cycles)
  {
    uAD1 = (uint16)(Adtmp1 / _AD_fliter_Cycles);
    uAD2 = (uint16)(Adtmp2 / _AD_fliter_Cycles);
    uAD3 = (uint16)(Adtmp3 / _AD_fliter_Cycles);
    uAD4 = (uint16)(Adtmp4 / _AD_fliter_Cycles);

    Adtmp1 = 0;
    Adtmp2 = 0;
    Adtmp3 = 0;
    Adtmp4 = 0;

    AD_fliter_index = 0;
  }
  //方向盘需要快速控制，放置到1ms进程中

  if (Vehco_Type_Test == 2) //自制驾驶舱
  {
    bDoSatus[8] = Steering_Wheel_IN1;
    bDoSatus[9] = Steering_Wheel_IN2;

#ifndef AECS_HW //易控
    //    void LSPWMDrv_Interface(uint8 idxChanUB,uint32 fSetUI,uint16 rSetUW);
    //输出上述通道的低边驱动频率及占空比，频率精度0.01Hz，即放大100倍，占空比精度为0.01%，即放大100倍

    PSWTDrv_Interface(8, bDoSatus[8]);
    PSWTDrv_Interface(9, bDoSatus[9]);

    LSPWMDrv_Interface(2, Fsetpwm, Steering_Wheel_PWM_Out); // 10k
#else                                                       //奥易克斯
    //    void DrvPWM_Interface(uint8 idxChanUB,uint16 rSetUW); //输出上述通道的低边驱动占空比，占空比精度为0.01%，即放大100倍

#endif
  }
}

//CAN油门刹车离合信号处理
void CAN_AccPed_Breakk(void)
{
  uint16 tmp = 0;
  /*
    extern uint8  AccPed_AD255 ;
    extern uint8  Breakk_AD255 ;
    extern uint8  Clutch_AD255 ;
    uint16 AccPed_CAN=0,Breakk_CAN=0, Clutch_CAN=0;
    uint8 EPB_Switch_Want=0;   //0 无动作 1 拉起 2 释放 3 行进中制动
    uint8 EPB_Switch_LH=0,EPB_Switch_HL=0;
    */
  AccPed_CAN = Intpol_u16u16((uint16 * __far) & AccPedCD_CUR_T02, AccPed_AD255);
  Breakk_CAN = Intpol_u16u16((uint16 * __far) & Breakk_CUR_T02, Breakk_AD255);
  tmp = Intpol_u16u16((uint16 * __far) & Clutch_CUR_T02, Clutch_AD255);

  if ((tmp > Level_High) && (Clutch_CAN <= Level_High))
  {
    EPB_Switch_HL = 0;
    EPB_Switch_LH = 1;
  }

  if ((tmp < Level_Low) && (Clutch_CAN >= Level_Low))
  {
    EPB_Switch_HL = 1;
    EPB_Switch_LH = 0;
  }

  Clutch_CAN = tmp;
}

//更新部件信号
void VehCo_UpdateSig(void)
{
  //每10ms更新一次，用于获取模拟量和数字量输入状态
  uint8 Index = 0;
  uint16 AccPed_Tmp = 0;
  uint8 Drive_Mode_Switch_Tmp = 0;

  bLso = 0;

  for (Index = 1; Index < 8; Index++) //7路LSO状态  0路不做操作
  {
    bLso |= ((bDoSatus[Index] && 1) << Index);
  }
  bHso = 0;
  for (Index = 0; Index < 8; Index++) //8路HSO状态
  {
    bHso |= ((bDoSatus[Index + 8] && 1) << Index);
  }

  R_Control_Switch = bDiSatus[3]; //远程驾驶激活开关

  //Break  制动踏板状态计算
  //Breakk_Type 制动踏板类型 0 单路开关量 1 双路互反开关量  2 单路模拟量 3 双路模拟量
  switch (Breakk_Type)
  {
  default:
  case 0:
    Breakk = bDiSatus[13] * 10000;
    break;
  case 1:
    Breakk = bDiSatus[13] * 10000;
    break;
  case 2: //2 单路模拟量     实验室
    //Breakk=Intpol_u16u16(Breakk_rUW_CUR_Addr,uAD3);

    Breakk = (uint16)((((uint32)(Breakk)) * (Percent_Filter_And_All[0] - Percent_Filter_And_All[3]) + ((uint32)(Intpol_u16u16(Breakk_rUW_CUR_Addr, uAD3))) * (Percent_Filter_And_All[3])) / Percent_Filter_And_All[0]);

    break;
  case 3: //3 双路模拟量
    Breakk = Intpol_u16u16(Breakk_rUW_CUR_Addr, uAD3);
    break;
  case 4: //CAN            自制
    Breakk = Breakk_CAN;
    break;
  }

  switch (AccPedCD_TYPE) //油门类型 1 双线 2 单线 4 CAN
  {

  default:
    break;
  case 1:
    //uAD2   AccPedCD_rUW_CUR_Addr  双油门百分比查表计算
    AccPed1 = Intpol_u16u16(AccPedCD_rUW_CUR_Addr, uAD1);  //0.75～4.1V
    AccPed2 = Intpol_u16u16(AccPedCD_rUW_CUR2_Addr, uAD2); //0.375～2.05V
    //立刻对双油门数据进行有效判断!   AccPed_Differ_Limit
    AccPed_Differ = abs(AccPed1 - AccPed2);

    //油门故障判断

    if (((AccPed_Differ >= AccPed_Differ_Limit)
         //||(uAD1<83)||(uAD1>3685)||(uAD2<83)||(uAD2>3685) )
         ) &&
        (Fault_Detection == 1)

    )
    {
      if ((Err_AccPed == 0) && (Err_AccPed_Index < Err_AccPed_Index_Max))
      {
        Err_AccPed_Index++;
      }
      else
      {

        Failure_Recovery_Delay[7] = 200; //油门故障2秒恢复延时
        Err_AccPed = 1;
        AccPed = 0;
        AccPed_Tmp = 0;
      }
    }
    else
    { //油门正常
      if (Failure_Recovery_Delay[7] == 0)
      {
        Err_AccPed = 0;
        Err_AccPed_Index = 0;
        //油门曲线处理
        AccPed_Tmp = Intpol_u16u16(AccPed_Rip_Addr, AccPed1);
      }
      else
      {
        --Failure_Recovery_Delay[7];
      }
    }
    break;

  case 4:
    AccPed1 = AccPed_CAN;
    AccPed_Tmp = Intpol_u16u16(AccPed_Rip_Addr, AccPed1);
    break;
  }

  //油门刹车同时踩，取消油门开度
  if ((AccPed_Tmp > AccPed_BreakErr_Max) && (Breakk > AccPed_BreakErr_Max))
  {
    AccPed_Tmp = 0;
  }

  //滤波增量   演示
  // Percent100=Percent_Filter_And_All[0]-Percent_Filter_And_All[1];

  if (AccPed_Tmp > AccPed)
  { //加油
    AccPed = (uint16)((((uint32)(AccPed)) * (Percent_Filter_And_All[0] - Percent_Filter_And_All[1]) + ((uint32)(AccPed_Tmp)) * (Percent_Filter_And_All[1])) / Percent_Filter_And_All[0]);
    if ((AccPed > 9900) && (AccPed_Tmp >= 9990))
    {
      AccPed = AccPed_Tmp;
    }
  }
  else
  { //减油
    AccPed = (uint16)((((uint32)(AccPed)) * (Percent_Filter_And_All[0] - Percent_Filter_And_All[2]) + ((uint32)(AccPed_Tmp)) * (Percent_Filter_And_All[2])) / Percent_Filter_And_All[0]);
  }

  //门控制程序

  //Door_Switch=bDiSatus[4];      //远程驾驶仓门开关
  //uint8 Door_Command=0;       //车门控制命令 0 无动作 1 开门 2 关门 3 保留
  if ((Door_Switch == 0) && (bDiSatus[4] == 1)) //上升沿
  {
    if (DoorState == 1)
    {
      Door_Command = 1;
    }
    else
    {
      Door_Command = 2;
    }
  }
  if ((Door_Switch == 1) && (bDiSatus[4] == 0)) //下降沿
  {
    Door_Command = 0;
  }

  Door_Switch = bDiSatus[4];
  //计算车速
  Vehco_Speed_Calc = (sint16)(((sint32)Motor_Speed) * 10 / Motor_Vehicle_Speed_Scale_Get);
  if (1 == bDiSatus[6]) //右车灯状态开关
  {
    RightTrunSignal_state = 1;
  }
  else
  {
    RightTrunSignal_state = 2;
  }
  if (1 == bDiSatus[5]) //左车灯状态开关
  {
    LeftTrunSignal_state = 1;
  }
  else
  {
    LeftTrunSignal_state = 2;
  }
}

void VehCo_DOset(uint8 Do_Off)
{
  uint8 Index;
  if (Do_Off == 0)
  { //开关量输出
  }
  else
  { //关闭所有DO
    for (Index = 1; Index < 16; Index++)
    {
      bDoSatus[Index] = 0;
    }
  }
}

//部件状态监控
void VehCo_Monitor(void)
{
  //每50ms监控一次
}

//回正力计算
void BackZeroForce_Angle_Calc(void)
{ //回正力
  //EPS_Demand_Angle
  uint16 Tmp = 0;
  if ((EPS_Demand_Angle < (Die_Angle / 2)) && (EPS_Demand_Angle > (-1 * Die_Angle / 2)))
  {
    Tmp = 0;
  }
  else
  {
    Tmp = Intpol_s16u16((uint16 * __far) & BackZeroForce_Angle, EPS_Demand_Angle);
  }
  BackZeroForce_PWM = Tmp;
}

//自动回正
void Steering_Whee_Back_Zero(void)
{
  if (EPS_Demand_Angle > Die_Angle)
  {
    Steering_Wheel_IN1 = 1;
    Steering_Wheel_IN2 = 0;
  }
  else if (EPS_Demand_Angle < -1 * Die_Angle)
  {
    Steering_Wheel_IN1 = 0;
    Steering_Wheel_IN2 = 1;
  }
  else
  {
    Steering_Wheel_IN1 = 0;
    Steering_Wheel_IN2 = 0;
  }
  Steering_Wheel_PWM_Out = BackZeroForce_PWM; //(uint16)((sint16)Steering_Whee_Auto_Check_Pwm  )  ;
}

//模拟方向盘自动校正
void Steering_Whee_Auto_Check(void)
{
  /*
    uint8 Steering_Wheel_IN1=0,Steering_Wheel_IN2=0;  // 1低2高 正转 顺时针 1高2低 反转  00 阻尼  11 悬空
    uint16 Steering_Wheel_PWM_Out=0;      //0.01% PWM

    sint16  Steering_Wheel_Angle_BaseCAN_Max=0,
            Steering_Wheel_Angle_BaseCAN_Min=0;  //模拟方向盘最大测试角度
    sint16 Wheel_Offset_Auto=0;                 //自校准模拟方向盘0点偏移
    uint8 Steering_Whee_Auto_Check_Mark=0;     //方向盘自检成功
   
    extern uint8   LostVehcoSteering_Wheel_AngleCommunication;
    extern uint16  LostVehcoSteering_Wheel_AngleCom_Index;


    extern sint16 Steering_Wheel_Angle_BaseCAN ;
    extern uint8  AccPed_AD255 ;
    extern uint8  Breakk_AD255 ;
    extern uint8  Clutch_AD255 ;
    extern uint8  Steering_Wheel_Angle_Mot_Temp ;
    extern uint8  Steering_Wheel_Angle_Life ;     
    
    uint8 Steering_Whee_Auto_Check_Step;  //0 未进行   1 左转中  2 左转到终点 3 右转中  4 右转到终点 5 回中中   6 成功回正  7失败   
    Steering_Whee_Auto_Check_Slow_Index  自动回中换向次数
    */

  if ((Steering_Whee_Auto_Check_Mark == 0) && (LostVehcoSteering_Wheel_AngleCom_Index < 10))
  {
    switch (Steering_Whee_Auto_Check_Step)
    {
    case 0: //0 未进行
      Steering_Wheel_IN1 = 0;
      Steering_Wheel_IN2 = 1;
      Steering_Wheel_PWM_Out = (uint16)((sint16)Steering_Whee_Auto_Check_Pwm + Pwm_Add);
      Steering_Wheel_Angle_BaseCAN_Max = Steering_Wheel_Angle_BaseCAN;
      Steering_Whee_Auto_Check_Still_Time = 0;
      Steering_Whee_Auto_Check_Step = 1;

      break;
    case 1: //1 左转中
      Steering_Wheel_PWM_Out = (uint16)((sint16)Steering_Whee_Auto_Check_Pwm + Pwm_Add);
      if (Steering_Wheel_Angle_BaseCAN > Steering_Wheel_Angle_BaseCAN_Max)
      {
        Steering_Wheel_Angle_BaseCAN_Max = Steering_Wheel_Angle_BaseCAN;
        if (Steering_Whee_Auto_Check_Still_Time > 0)
        {
          Steering_Whee_Auto_Check_Still_Time--;
        }
      }
      else
      {
        if (Steering_Whee_Auto_Check_Still_Time < Steering_Whee_Auto_Check_Still_Time_Max)
        {
          Steering_Whee_Auto_Check_Still_Time++;
        }
        else
        {
          //Steering_Wheel_PWM_Out=0;
          Steering_Whee_Auto_Check_Step = 2;
        }
      }

      break;
    case 2: // 2 左转到终点
      Steering_Wheel_Angle_BaseCAN_Max = Steering_Wheel_Angle_BaseCAN;
      Steering_Wheel_IN1 = 1;
      Steering_Wheel_IN2 = 0;
      Steering_Wheel_PWM_Out = (uint16)((sint16)Steering_Whee_Auto_Check_Pwm + Pwm_Add);
      Steering_Wheel_Angle_BaseCAN_Min = Steering_Wheel_Angle_BaseCAN;
      Steering_Whee_Auto_Check_Still_Time = 0;
      Steering_Whee_Auto_Check_Step = 3;

      break;
    case 3: //3 右转中
      Steering_Wheel_PWM_Out = (uint16)((sint16)Steering_Whee_Auto_Check_Pwm + Pwm_Add);
      if (Steering_Wheel_Angle_BaseCAN < Steering_Wheel_Angle_BaseCAN_Min)
      {
        Steering_Wheel_Angle_BaseCAN_Min = Steering_Wheel_Angle_BaseCAN;
        if (Steering_Whee_Auto_Check_Still_Time > 0)
        {
          Steering_Whee_Auto_Check_Still_Time--;
        }
      }
      else
      {
        if (Steering_Whee_Auto_Check_Still_Time < Steering_Whee_Auto_Check_Still_Time_Max)
        {
          Steering_Whee_Auto_Check_Still_Time++;
        }
        else
        {
          //Steering_Wheel_PWM_Out=0;
          Steering_Whee_Auto_Check_Step = 4;
        }
      }
      break;
    case 4: //4 右转到终点
      Steering_Wheel_Angle_BaseCAN_Min = Steering_Wheel_Angle_BaseCAN;
      Steering_Wheel_IN1 = 0;
      Steering_Wheel_IN2 = 1;
      Steering_Wheel_PWM_Out = (uint16)((sint16)Steering_Whee_Auto_Check_Pwm + Pwm_Add);
      Steering_Whee_Auto_Check_Still_Time = 0;
      Steering_Whee_Auto_Check_Step = 5;

      Wheel_Offset_Auto = (Steering_Wheel_Angle_BaseCAN_Min + Steering_Wheel_Angle_BaseCAN_Max) / 2;

      break;
    case 5: //5 回正中
      Steering_Wheel_PWM_Out = (uint16)((sint16)Steering_Whee_Auto_Check_Pwm + Pwm_Add);
      //正转回中
      if (Steering_Wheel_Angle_BaseCAN < (Wheel_Offset_Auto - Die_Angle))
      {
        if (Steering_Wheel_IN1 == 1)
        {
          Steering_Whee_Auto_Check_Slow_Index++;
        }
        Steering_Wheel_IN1 = 0;
        Steering_Wheel_IN2 = 1;
        Steering_Wheel_PWM_Out = (uint16)((sint16)Steering_Whee_Auto_Check_Pwm + Pwm_Add);
        if (Steering_Wheel_Angle_BaseCAN < (Wheel_Offset_Auto - Die_Angle - 100))
        {
          Steering_Wheel_PWM_Out = (uint16)((sint16)Steering_Whee_Auto_Check_Pwm + Pwm_Add);
        }
      }
      //逆转回中
      if (Steering_Wheel_Angle_BaseCAN > (Wheel_Offset_Auto + Die_Angle))
      {
        if (Steering_Wheel_IN1 == 0)
        {
          Steering_Whee_Auto_Check_Slow_Index++;
        }
        Steering_Wheel_IN1 = 1;
        Steering_Wheel_IN2 = 0;
        Steering_Wheel_PWM_Out = (uint16)((sint16)Steering_Whee_Auto_Check_Pwm + Pwm_Add);
        if (Steering_Wheel_Angle_BaseCAN > (Wheel_Offset_Auto + Die_Angle + 100))
        {
          Steering_Wheel_PWM_Out = (uint16)((sint16)Steering_Whee_Auto_Check_Pwm + Pwm_Add);
        }
      }
      //回中完成
      if ((Steering_Whee_Auto_Check_Slow_Index > 3) ||
          ((Steering_Wheel_Angle_BaseCAN < (Wheel_Offset_Auto + Die_Angle)) &&
           (Steering_Wheel_Angle_BaseCAN > (Wheel_Offset_Auto - Die_Angle))))
      {
        Steering_Wheel_IN1 = 0;
        Steering_Wheel_IN2 = 0;
        //Steering_Wheel_PWM_Out=0;
        //往复总角度不足则判定失败
        if ((Steering_Wheel_Angle_BaseCAN_Max - Steering_Wheel_Angle_BaseCAN_Min) > 800)
        {
          Steering_Whee_Auto_Check_Step = 6;
        }
        else
        {
          Steering_Whee_Auto_Check_Step = 7;
        }
      }

      break;
    case 6: //成功回正
      Steering_Whee_Auto_Check_Mark = 1;
      Steering_Wheel_PWM_Out = (uint16)((sint16)Steering_Whee_Auto_Check_Pwm + Pwm_Add);
      break;

    case 7: //失败
      Steering_Wheel_IN1 = 0;
      Steering_Wheel_IN2 = 0;
      //Steering_Wheel_PWM_Out=0;
      Steering_Whee_Auto_Check_Mark = 0;
    default:
      Steering_Whee_Auto_Check_Step = 7;
      break;
    }
  }
}

//自动EPB
void Auto_Hold_EPB(void)
{ /*
    uint8 EPB_Command;    //电子手刹指令 0 无动作 1 拉起 2 释放 3 行进中制动
    
    uint8   EPB_State;                //EPB状态
    sint16 Vehco_Speed_Calc=0; //计算车速   10倍
    sint16 Vehco_Speed ; //计算车速   10倍
    uint8 R_Break_Rang;     //远程制动力请求 0~100%
    uint8 Auto_Hold_Delay=0;  //停车踩刹车时长
    */

  //停车踩刹车未未拉手刹1秒，自动拉起手刹
  /*
    if ((Vehco_Speed_Calc<5)&&(Vehco_Speed==0)&&(EPB_State!=1)&& (R_Break_Rang>0)) 
    {
        if (Auto_Hold_Delay<Auto_Hold_Delay_Max)
        {
            Auto_Hold_Delay++;
        }else
        {
            EPB_Command=1;
        }
    }
    */

  if ((Vehco_Acceleration_Command > 1) && (EPB_State == 1) && (Motor_Torque > Auto_Hold_Release_Torque))
  {
    Auto_Hold_Delay = 0;
    EPB_Command = 2;
  }

  //踩下EPB踏板
  if (EPB_Switch_LH == 1)
  {
    if (EPB_State == 1)
    {
      EPB_Command = 2;
    }
    if (EPB_State == 2)
    {
      EPB_Command = 1;
    }
    EPB_Switch_LH = 0;
  }

  //命令执行完毕
  if (EPB_Command == EPB_State)
  {
    EPB_Command = 0;
    // EPB_Switch_Want=0;
  }
}

//远程加速度控制
void R_Vehco_Acceleration(void)
{
  /*
    extern uint8 R_Break_Mode;   //远程制动激活   0x00：未激活,本地控制 0x01：激活，制动控制器执行远程网关指令 0x02:融合激活，取最大 
    extern uint8 R_Break_Rang;     //远程制动力请求 0~100%
    extern uint8   Vehco_Break_Mode;     //制动控制器模式 0x00：未激活,本地控制 0x01：激活，制动控制器执行远程网关指令 0x02:融合激活，取最大
    extern uint8   Vehco_Break_Rang;     //制动力状态  0~100%
    uint8 DriverMode_Command=0; //自驾模式状态 0 手动 1 保留 2 保留 3 远程    DriverMode_State;       //自驾模式状态
    AccPed
    Breakk
    extern sint8 Vehco_Acceleration_Command;   //加速度指令 -12.8~12.7
    uint8 Dang_Command=0;       //档位指令 0:N 1:D 2:R 3:保留                 
    Dang_State;               //车辆档位状态
    extern uint16 * __far   AccPed_Acceleration_Addr;  //油门开度-加速度
    extern uint16 * __far   Break_Acceleration_Addr;   //制动深度-制动力给定%
    */

  // if  ((DriveModeDebug==1)||(DriverMode_State==3) )
  // {
  if (Breakk < R_Break_Min)
  { //未踩制动
    Vehco_Acceleration_Command = (sint8)Intpol_u16s16(AccPed_Acceleration_Addr, AccPed);
    R_Break_Rang = 0;
    R_Break_Mode = 1;
    if ((Dang_State == 2) && (Dang_Command == 2))
    {
      Vehco_Acceleration_Command = -1 * Vehco_Acceleration_Command;
    }
  }
  else
  { //踩制动
    Vehco_Acceleration_Command = 0;
    if (abs(R_Break_Rang - (uint8)Intpol_u16s16(Break_Acceleration_Addr, Breakk)) >= Break_Acceleration_Min_Step)

    { //百分比制动请求
      R_Break_Rang = (uint8)Intpol_u16s16((uint16 * __far) & Break_Acceleration_T01, Breakk);
      if (Vehco_Type_Test == 2)
      { //加速度请求
        Vehco_Acceleration_Command = (sint8)Intpol_u16s16(Break_Acceleration_Addr, Breakk);
      }
    }
    R_Break_Mode = 1;

    //EPB自动拉起  释放脚刹
    if ((Vehco_Speed_Calc < 5) && (Vehco_Speed == 0) && (EPB_State == 1) && (Auto_Hold_Delay >= Auto_Hold_Delay_Max))
    {
      R_Break_Rang = 0;
    }
    //倒挡时需更改加速度方向
    //uint8 Dang_Command=0;       //档位指令 0:N 1:D 2:R 3:保留
    //Dang_State;               //车辆档位状态
    if ((Dang_State == 2) && (Dang_Command == 2))
    {
      Vehco_Acceleration_Command = -1 * Vehco_Acceleration_Command;
    }
    // if (Dang_R) { Vehco_Acceleration_Command= -1* Vehco_Acceleration_Command; }
  }
  // }else
  // {
  //     Vehco_Acceleration_Command=0;
  //     R_Break_Rang=0;
  //     R_Break_Mode=0;

  // }
}

//方向盘角度控制
// void Steering_Wheel(void)
// {
//   //extern sint16 Steering_Wheel_Angle_Code; //方向盘旋转编码  360°/5000 右转为正
//   //extern uint8  Steering_Wheel_Angle_ZeroMark; //方向盘是否经过0点校正
//   //EPS_Demand_Angle;//EPS角度命令 0.1度步进   -1080.0~1080.0°
//   //EPS_Angle_Max EPS_Angle_Min
//   /*
//     sint16 Steering_Wheel_Angle_BaseCAN ;
//     sint16 Wheel_Offset_Auto=0;              //自校准模拟方向盘0点偏移
//     uint8 Steering_Whee_Auto_Check_Mark=0;     //方向盘自检成功
//     */
//   switch (Vehco_Type_Test)
//   {
//   case 0:
//   case 1:
//     //EPS_Demand_Angle=
//     if (Steering_Wheel_Angle_ZeroMark == 0)
//     {
//       EPS_Demand_Angle = 0;
//     }
//     else
//     {
//       EPS_Demand_Angle = (sint16)((((sint32)Steering_Wheel_Angle_Code) * 100) / 139) + EPS_Offset;

//       if (EPS_Demand_Angle >= EPS_Angle_Max)
//       {
//         EPS_Demand_Angle = EPS_Angle_Max;
//       }
//       if (EPS_Demand_Angle <= EPS_Angle_Min)
//       {
//         EPS_Demand_Angle = EPS_Angle_Min;
//       }
//     }
//     break;
//   case 2:
//     if (Steering_Whee_Auto_Check_Mark == 0)
//     {
//       EPS_Demand_Angle = 0;
//     }
//     else
//     {
//       EPS_Demand_Angle = Steering_Wheel_Angle_BaseCAN - Wheel_Offset_Auto + EPS_Offset;

//       if (EPS_Demand_Angle >= EPS_Angle_Max)
//       {
//         EPS_Demand_Angle = EPS_Angle_Max;
//       }
//       if (EPS_Demand_Angle <= EPS_Angle_Min)
//       {
//         EPS_Demand_Angle = EPS_Angle_Min;
//       }
//     }
//     break;
//   default:
//     break;
//   }
// }

//远程档位控制
void Vehco_Dang(void)
{ //uint8 Dang_Command=0;       //档位指令 0:N 1:D 2:R 3:保留                 Dang_State;               //车辆档位状态
  //uint8 DriverMode_Command=0; //自驾模式状态 0 手动 1 保留 2 保留 3 远程    DriverMode_State;       //自驾模式状态
  /*
          Dang_R=bDiSatus[15];
          Dang_N=bDiSatus[16];
          Dang_D=bDiSatus[17];       
    */

  switch (Vehco_Type_Test)
  {

  case 0: //1 实验室驾驶舱   无换挡杆  进入远程模式后自动进D档
  default:
    if (DriverMode_State == 3)
    {
      switch (Dang_State)
      {
      case 0:
        if (D_Dang_Change_Delay_Index < D_Dang_Change_Delay_Index_Max)
        {
          D_Dang_Change_Delay_Index++;
        }
        else
        {
          Dang_Command = 1;
        }

        break;
      case 1:
        Dang_Command = 1;
        break;
      case 2:
      default:
        Dang_Command = 0;
        break;
      }
    }
    else
    {
      Dang_Command = 0;
      D_Dang_Change_Delay_Index = 0;
    }
    break;
  case 1:                  //1 实验室驾驶舱   有换挡杆按键
    Dang_R = bDiSatus[15]; //实验室
    Dang_N = bDiSatus[16];
    Dang_D = bDiSatus[17];
    if (Dang_R == 1)
    {
      Want_Dang = 2;
    }
    if (Dang_D == 1)
    {
      Want_Dang = 1;
    }
    if (Dang_N == 1)
    {
      Want_Dang = 0;
    }

    // if (DriverMode_State==3)
    // {
    switch (Want_Dang)
    {
    case 1: //进D
      switch (Dang_State)
      {
      case 0:
        if (D_Dang_Change_Delay_Index < D_Dang_Change_Delay_Index_Max)
        {
          D_Dang_Change_Delay_Index++;
        }
        else
        {
          Dang_Command = 1;
        }
        break;
      case 1:
        Dang_Command = 1;
        D_Dang_Change_Delay_Index = 0;
        break;
      case 2:
      default:
        Dang_Command = 0;
        D_Dang_Change_Delay_Index = 0;
        break;
      }
      break;
    case 2: //进R
      switch (Dang_State)
      {
      case 0:
        if (D_Dang_Change_Delay_Index < D_Dang_Change_Delay_Index_Max)
        {
          D_Dang_Change_Delay_Index++;
        }
        else
        {
          Dang_Command = 2;
        }
        break;
      case 1:
        Dang_Command = 0;
        D_Dang_Change_Delay_Index = 0;
        break;
      case 2:
      default:
        Dang_Command = 2;
        D_Dang_Change_Delay_Index = 0;
        break;
      }

      break;
    case 0: //进N
    default:
      Dang_Command = 0;
      D_Dang_Change_Delay_Index = 0;
      break;
    }
    // }
    // else
    // {
    //     Dang_Command=0;
    //     D_Dang_Change_Delay_Index=0;
    // }
    break;
  case 2:                  //自制驾驶舱
    Dang_R = bDiSatus[15]; //实验室
    Dang_N = bDiSatus[16];
    Dang_D = bDiSatus[17];
    if (Dang_R == 1)
    {
      Want_Dang = 2;
    }
    if (Dang_D == 1)
    {
      Want_Dang = 1;
    }
    if (Dang_N == 1)
    {
      Want_Dang = 0;
    }
    if (DriverMode_State == 3)
    {
      Dang_Command = Want_Dang;
    }
    break;
  }
}

//驾驶模式判断
void Vehco_Driver_Mode(void)
{
  /*
    extern uint8 EPS_Run_Mode;//EPS返回当前控制模式
    extern uint8 EPS_ERR_Level;    //EPS故障级别
 
    extern uint8   Dang_State;               //车辆档位状态

    extern uint8   DriverMode_State;       //自驾模式状态
    extern uint8   SmartPowerCommand;       //智能系统电源指令  00 启动  01 关机
    extern uint8   Change_State;           //充电状态 00 无需充电 01充电完成 02 充电中 03 电量低需要充电
    extern uint8   Change_Connect_State;   //充电连接状态 0 未连接 1 充电枪已插入
    extern uint8   Vehco_Fault_Level;      //整车故障级别

    extern uint8   Vehco_Power_State;    //整车动力状态 00 INIT ON档 01 READY 02 跛行 03 故障无高压

    extern uint8   Isolation_Level;      //绝缘故障等级 0 正常 1 一般 2 严重 3 保留 
    extern uint8   MCU_Fault_Level;      //电控故障级别  0 正常 1轻微 2 一般 3 严重
    extern uint8   BMS_Fault_Level;      //BMS故障级别   0 正常 1轻微 2 一般 3 严重
    
    extern uint8 DriverMode_Command; //自驾模式状态 0 手动 1 保留 2 保留 3 远程
    extern uint8  EPS_Contral_Mode;//EPS控制模式
    extern uint8 R_DriverSystem_State;       //远程驾驶系统状态 0 连接中 1 连接完成 2 断开中 3 断开
    extern uint8 R_DriverSystem_Level;       //远程驾驶系统可信级别 0 不可信，立刻接管 1 低质量 ，人工监视 2 最佳状态，完全控制
    extern uint8 R_DriverSystem_Fault_Level; //远程驾驶系统故障级别 0 无故障 1 轻微 提示 2 一般故障 跛行 3 严重故障 退出远程驾驶状态    
    */

  //远程驾驶系统状态判断 0 连接中 1 连接完成 2 断开中 3 断开
  if ((LostEPSCommunication == 0) && (LostVehcoCommunication == 0) && (LostVehcoStateCommunication == 0))
  {
    if (SmartPowerCommand == 0)
    {
      R_DriverSystem_State = 1;
    }
  }
  else
  {
    R_DriverSystem_State = 0;
  }
  //远程驾驶系统可信级别判断 0 不可信，立刻接管 1 低质量 ，人工监视 2 最佳状态，完全控制

  //远程驾驶系统故障级别 0 无故障 1 轻微 提示 2 一般故障 跛行 3 严重故障 退出远程驾驶状态

  //进入远程驾驶模式判断
  if (((EPS_Run_Mode == 0x00) || (EPS_Run_Mode == 0x10) || (EPS_Run_Mode == 0x20)) && (EPS_ERR_Level < 2)
      //&& ((Change_State==0)||(Change_State==3))
      && (Change_Connect_State == 0) && (Vehco_Fault_Level < 3)
      //&& ((Vehco_Power_State==1)||(Vehco_Power_State==2))
      && (Isolation_Level < 2) && (BMS_Fault_Level < 3) && (MCU_Fault_Level < 2) && (LostEPSCommunication == 0) && (LostVehcoCommunication == 0) && (LostVehcoStateCommunication == 0) && ((LostVehcoSteering_Wheel_AngleCommunication == 0) || (Vehco_Type_Test != 2)))
  {
    if ((R_Control_Switch == 1) && (SmartPowerCommand == 0) && (EPB_State == 2))
    {
      DriverMode_Command = 3;  //自驾模式  远程模式
      EPS_Contral_Mode = 0x20; //EPS控制模式
    }
  }
  else
  {
    DriverMode_Command = 0;
  }

  //退出远程模式
  if (R_Control_Switch == 0)
  {
    DriverMode_Command = 0;
  }

  if (SmartPowerCommand == 1)
  {
    R_DriverSystem_State = 3; //收到远程下电指令，断开
    DriverMode_Command = 0;
    EPS_Contral_Mode = 0x10;
  }
}

//故障判断
void Vehco_Errpr_Judge(void)
{
  //uint8   LostVehcoCommunication=0;
  //uint16  LostVehcoCom_Index=0;
  LostEPSCom_Index++;
  LostVehcoCom_Index++;
  LostVehcoStateCom_Index++;
  LostVehcoBreakCom_Index++;
  LostVehcoSteering_Wheel_AngleCom_Index++;

  if (LostEPSCom_Index >= LostEPSCom_Index_Max)
  { //EPS通讯丢失
    LostEPSCom_Index = LostEPSCom_Index_Max + 1;
    LostEPSCommunication = 1;
  }
  if (LostVehcoCom_Index >= LostVehcoCom_Index_Max)
  { //行车通讯丢失
    LostVehcoCom_Index = LostVehcoCom_Index_Max + 1;
    LostVehcoCommunication = 1;
  }
  if (LostVehcoStateCom_Index >= LostVehcoStateCom_Index_Max)
  { //车辆状态通讯丢失
    LostVehcoStateCom_Index = LostVehcoStateCom_Index_Max + 1;
    LostVehcoStateCommunication = 1;
  }
  if (LostVehcoBreakCom_Index >= LostVehcoBreakCom_Index_Max)
  { //车辆制动状态通讯丢失
    LostVehcoBreakCom_Index = LostVehcoBreakCom_Index_Max + 1;
    LostVehcoBreakCommunication = 1;
  }
  if (LostVehcoSteering_Wheel_AngleCom_Index >= LostVehcoSteering_Wheel_AngleCom_Index_Max)
  { //模拟方向盘通讯丢失
    LostVehcoSteering_Wheel_AngleCom_Index = LostVehcoSteering_Wheel_AngleCom_Index_Max + 1;
    LostVehcoSteering_Wheel_AngleCommunication = 1;
  }
}

//钥匙状态判断
void Vehco_Key_Status(void)
{
  //钥匙判断  bDiSatus[1] ACC   bDiSatus[2] START BattCD_bLogicOnUB ON
  //档位判断  bDiSatus[14] P    bDiSatus[15] R    bDiSatus[16] N  bDiSatus[17]D
  //Key_Status
  //钥匙 0xF0 Bit5~8 0 lock, 1 acc, 2 on, 3 start
  //档位 0x0F bit 1~4 0 N ,1 P, 10 D, 5 R

  if (BattCD_bLogicOnUB == 1) //ON逻辑电输入
  {
  }
  else
  {
  }
}

//试验代码     10ms
void VehCo_Test(void)

{
  //void LSPWMDrv_Interface(uint8 idxChanUB,uint32 fSetUW,uint16 rSetUW);
  //输出上述通道的低边驱动频率及占空比，占空比精度为0.01%，即放大100倍  频率精度为0.01Hz,放大100倍

  /*
    LSPWMDrv_Interface(0,fSet,rSet);
    LSPWMDrv_Interface(1,fSet,rSet);
    LSPWMDrv_Interface(2,fSet,rSet);
    LSPWMDrv_Interface(3,fSet,rSet);
    */
}

//整车管理状态机
void VehCo_StateMachine(void)
{
  //每10ms运行一次

  //VehCo_stNewUB包含4个状态：Init, Running, Afterrun, Finish
  //其中Init和Finish两个状态的操作由底层管理

  switch (VehCo_stNewUB)
  {
  case _VEHCO_INDEX_STATUS_RUNNING:
    if (0 == BattCD_bLogicOnUB) //Running状态时逻辑电开关断开，进入到关机准备状态Afterrun
    {
      //Error_PowOn_Delay=0;  //上下电检测故障延迟
      VehCo_stNewUB = _VEHCO_INDEX_STATUS_AFTERRUN;
    }
    break;

  case _VEHCO_INDEX_STATUS_AFTERRUN:

    if (1 == BattCD_bLogicOnUB) //Afterrun状态时逻辑电开关闭合，重新进入运行状态
    {

      VehCo_Init(); //重新初始化

      VehCo_stNewUB = _VEHCO_INDEX_STATUS_RUNNING; //恢复运行状态
    }
    else
    {
      // Error_PowOn_Delay=0;  //上下电检测故障延迟
      if (VehCo_stAfterrunUB == _VEHCO_AFTERRUN_STATUS_FINISH) //如果关机准备操作已经完成，进入停机状态Finish
      {
        //关机准备操作完成，进入停机状态
        bDoSatus[0] = 0; //断开主继电器

        //DCDC_Enable=0;//DCDC失能

        VehCo_DOset(1);
        VehCo_UpdateSig();
        //Can_Send_Enable=0;//禁止发送CAN报文
        VehCo_stNewUB = _VEHCO_INDEX_STATUS_FINISH;
      }
    }
    break;

  default:
    break;
  }
}

//停机准备状态操作
void VehCo_Afterrun(void)
{
  //extern uint8 SOC;
  switch (VehCo_stAfterrunUB)
  {
  case _VEHCO_AFTERRUN_STATUS_INIT:                    //初始化程序VehCo_Init会将此置为INIT
    if (VehCo_stNewUB == _VEHCO_INDEX_STATUS_AFTERRUN) //关机准备状态Afterrun
    {

      VehCo_stAfterrunUB = _VEHCO_AFTERRUN_STATUS_BEGIN; //开始关机准备工作
      Afterrun_Delay_Index = 0;
      //Vehco_State=6;
      //Delay_Index_LostMcuShutDown=0;
      // On_LostInRuning=0;
    }
    break;

  case _VEHCO_AFTERRUN_STATUS_BEGIN: //    开始关机准备工作
    //在此处添加关机准备状态下的操作

    //操作完成后进行状态跳转
    if (Afterrun_Delay_Index >= 300) //关机延时计数 3秒  电机放电完成
    {
      //更新EEPROM数据
      PowOn_Times++;

      //EEPROMDrv_bSaveInstantUB=1;
      VehCo_stAfterrunUB = _VEHCO_AFTERRUN_STATUS_FINISH;
    }
    else
    {
      Afterrun_Delay_Index++;
    }
    break;

  case _VEHCO_AFTERRUN_STATUS_FINISH:
    break;

  default:
    break;
  }
}
