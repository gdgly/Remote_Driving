/*******************************************************************************
* 工程名称：増程器控制程序
* 文件名： Vin_APU.c
* 功能描述 ： 
* 引用记录：
* 备注： 
* 修订记录：
*		日期		      版本		  作者		描述
*   
*******************************************************************************/
#include <string.h>
#include "GlobalDef.h"
#include "Vin_APU.h"

    
#ifndef AECS_HW
       #pragma DATA_SEG DEFAULT
#else  
      #pragma DATA_SEG S12X_RAM
#endif





 //増程器工作指令,暖风开启请求,机械空调开启请求,
extern uint8  APU_CMD,APU_Warm_CMD,Mechanical_AC_CMD;
 //増程器目标发电功率命令
extern uint16 APU_Demand_Power; 
 //目标工作转速  
extern uint16 APU_Demand_Speed;

 //APU初始化状态,OBD指示灯,机械空调状态,増程器启动标志位,发动机机油压力报警,増程器放电完成标志位,増程器当前状态,増程器故障等级      
extern uint8  APU_Staus,APU_OBD_LED,Mechanical_AC,APU_Start_Staus,Engine_Oil_Pressure_Alarm,APU_Discharged_Flag,APU_Mode,APU_Fault_Status;
 //増程器转速,増程器输出转矩
extern uint16 APU_Speed,APU_Torque;
extern sint16 APU_Power;//増程器输出功率
 //増程器母线电压，増程器当前转速最大功率
extern uint16 APU_DC_Voltage,APU_Power_Limit;
 //増程器母线电流，増程器交流电流
extern sint16 APU_DC_Current,APU_AC_Current;

 //    发动机进气压力,        发动机油箱液位,  发动机瞬时燃油消耗率, 节气门位置开度实际值
extern uint16 Engine_Inlet_Pressure, Engine_Oil_Level,Fuel_Consumption_Rate,Engine_Throttle_Rate;
 //    发动机进气温度,          发动机冷却液温度,          増程器温度,     増程器控制器GCU温度 
extern sint16 Engine_Inlet_Temperature,Engine_Coolant_Temperature,APU_Temperature,GCU_Temperature;
 //   发动机故障字              増程器故障字
extern uint8 Engine_Fault_Word[8], APU_Fault_Word[8];
 //   APU生命信号
extern uint8 APU_Life;


 
#ifndef AECS_HW
      #pragma CODE_SEG DEFAULT
#else  
      #pragma CODE_SEG VCU_CODE
      
#endif

//増程器控制程序
void APU_Control() 
{
    if (APU_DebugMode==1)    
    {  
        APU_Debug();    
    }else 
    {
        APU_CMD=0;    
        APU_Warm_CMD=0;
        Mechanical_AC_CMD=0;
        APU_Demand_Power=0;
        APU_Demand_Speed=0;
        
    }

}
//増程器调试程序
void APU_Debug() 
{
    APU_CMD=APU_Debug_CMD;
    APU_Demand_Power=APU_Debug_Demand_Power;
    APU_Demand_Speed=APU_Debug_Demand_Speed;

}





































