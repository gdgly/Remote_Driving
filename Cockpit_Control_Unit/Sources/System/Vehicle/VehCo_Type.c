/*******************************************************************************
* 工程名称：整车管理
* 文件名： VehCo_Type.c
* 功能描述 ：车辆类型定义
* 引用记录：
* 备注： 
* 修订记录：
*		日期		      版本		  作者		描述
*   
*******************************************************************************/

#include "GlobalDef.h"
#include "TypeDef.h"

//#include "VehCo.h"
//#include "Custom_Func.h"
//#include "Vehco_Logic.h"
//#include "VehCo_Type.h"


#ifndef AECS_HW
      #pragma DATA_SEG DEFAULT
#else  
      #pragma DATA_SEG S12X_RAM
#endif

/*
uint16 * __far   Motor_Speed_Torque_Addr;//=(uint16 * __far)&Motor_Speed_Torque; //转速_最大转矩
uint16 * __far   Speed_Vmh10ms_Crawl_D_Addr;//=(uint16 * __far)&Speed_Vmh10ms_Crawl_D; //D档蠕动 转速-AD曲线
uint16 * __far   Torque_Increment_Crawl_D_Addr;//=(uint16 * __far)&Torque_Increment_Crawl_D;  //D档蠕动 AD修正转矩给定每周期增量
uint16 * __far   Speed_Max_Torque_Crawl_D_Addr;//=(uint16 * __far)&Speed_Max_Torque_Crawl_D;  //D档蠕行模式最大力矩
uint16 * __far   Speed_Vmh10ms_General_Addr;//=(uint16 * __far)&Speed_Vmh10ms_General;        //通用相对转速-AD曲线 
uint16 * __far   Torque_Increment_General_Addr;//=(uint16 * __far)&Torque_Increment_General;  //通用AD修正转矩给定每周期增量
uint16 * __far   Speed_Vmh10ms_D_Park_Addr;//=(uint16 * __far)&Speed_Vmh10ms_D_Park;          //D档驻坡 转速-AD曲线
uint16 * __far   Torque_Increment_D_Park_Addr;//=(uint16 * __far)&Torque_Increment_D_Park;    //D档驻坡AD修正转矩给定每周期增量
uint16 * __far   Speed_Vmh10ms_Break_Addr;//=(uint16 * __far)&Speed_Vmh10ms_Break;            //制动回馈速度-AD曲线
uint16 * __far   Torque_Increment_Break_Addr;//=(uint16 * __far)&Torque_Increment_Break;      //制动回馈AD修正转矩给定每周期增量
uint16 * __far   Motor_Speed_Torque_Break_Addr;//=(uint16 * __far)&Motor_Speed_Torque_Break;  //电机最大制动力矩 40kw 114.4
uint16 * __far   FeedBack_Coast_D_Torque_Addr;//=(uint16 * __far)&FeedBack_Coast_D_Torque;    //D档滑行回馈 速度-力矩  定点
uint16 * __far   FeedBack_Break_D_Torque_Addr;//=(uint16 * __far)&FeedBack_Break_D_Torque;    //D档制动回馈 速度-力矩 定点
uint16 * __far   Speed_Efficieny_Addr;//=(uint16 * __far)&Speed_Efficieny;                    //电机速度-转换效率曲线
uint16 * __far   Speed_Efficieny_FeedBack_Addr;//=(uint16 * __far)&Speed_Efficieny_FeedBack_M03;  //电机速度-转换效率曲线 发电
uint16 * __far   Speed_Max_Torque_Crawl_D_Break_Addr;//=(uint16 * __far)&Speed_Max_Torque_Crawl_D_Break;  //D档蠕行制动状态下最大驱动力矩
uint16 * __far   AutoPark_Motor_Speed_Torque_Addr;//=(uint16 * __far)&AutoPark_Motor_Speed_Torque; //驻坡最大力矩

uint16 Motor_Vehicle_Speed_Scale_Get;//转速-车速比例
uint16 Demand_Limit_High_Get;//车速上限
uint16 Demand_Limit_Low_Get;//车速下限
uint16 Crawl_D_Speed_Max_Get;//从其他状态进入蠕动的速度  RPM
uint8 Crawl_D_Torque_Delay_Max_Get;//蠕动更新间隔  单位10ms
uint8 Crawl_D_Torque_Delay_Start_Max_Get; //起步蠕动更新间隔  单位10ms
uint8 Speed_Buffer_Number_Get; //10ms车辆速度采样缓冲次数
uint8 Speed_Average_Number_Get; //10ms平均车速缓冲次数
uint8 Torque_Buffer_Number_AutoPark_Get;  //10ms车辆平均转矩采样缓冲次数 驻坡
uint8 D_Park_Start_Speed_Get;//驻坡触发转速
sint16 Auto_Park_Speed_Acc_Max_Get; //激活D驻坡最大退行转速累加
uint16 Auto_Park_Speed_Times_Max_Get;//激活D驻坡最大退行增速时间 单位10ms

sint16 Auto_Torque_Min_Get=0,Auto_Torque_Max_Get=0; //自动转矩最小正值 自动转矩最大负值
uint16 R_Vhms_Average_Ratio_Get,D_Vhms_Average_Ratio_Get;//Vhms平均值缓冲比例


sint16 Air_Pump_Speed_DrapDown_Get=0;
sint16 Air_Pump_Torque_DrapDown_Get=0;
uint8 Air_Start_Press_Rate_Get=0;
uint8 Air_Over_Reset_Get=0;
uint16 Air_Pressure_Delay_Max_Get=0;
uint16 Air_Pump_WorkTime_Get=0;
uint16 Air_Pump_Fan_Delay_Get=0;
uint16 Air_Pump_Taget_Pressure_Get=0;
sint16 Air_Pump_Speed_Limit_Get=0;
uint16 Air_Pump_Torque_Limit_Get=0;

sint16 Oil_Pump_Speed_Limit_Get=0;
uint16 Oil_Pump_Torque_Limit_Get=0;

uint16 * __far   Oil_Pump_Speed_VSpeed_Addr=(uint16 * __far)&Oil_Pump_Speed_VSpeed_M12;;



uint8 FanType=0;//风扇类型 0 单速 1 双速    D07-750
uint8 ACType=0;//空调类型 0 简单 1 复杂

uint8 MaxGears=0;//最高档位  D15：2  渣土车：8

uint16 Motor_Vehicle_Speed_Scale_Gears_Out_Get=1; //变速箱输出后速比




uint16 AV_Ele_Calc_Min=100; //最小百公里平均电耗 10.0KWh/100Km

extern uint8 Gears_State; //变速器当前档位
extern uint8 Shift_Gears; //变速箱换档状态
extern uint8  Oil_Pump_Install; //油泵控制器适配状态 0 未装 1 安装
extern uint8  Air_Pump_Install; //气泵控制器适配状态 
extern uint8  Gears_Install; //变速箱安装 状态
extern uint8 Fan_Pwm_Install;
*/

uint16 * __far   AccPedCD_rUW_CUR_Addr;//=(uint16 * __far)&AccPedCD_rUW_CUR;  //油门开度
uint16 * __far   AccPedCD_rUW_CUR2_Addr;//=(uint16 * __far)&AccPedCD_rUW_CUR2;  //油门开度2
uint16 * __far   AccPed_Rip_Addr;//=(uint16 * __far)&AccPed_Rip;                 //油门曲线
uint16 * __far   Breakk_rUW_CUR_Addr;//=(uint16 * __far)&Breakk_rUW_CUR;  //制动踏板深度

uint16 * __far   AccPed_Acceleration_Addr;  //油门开度-加速度
uint16 * __far   Break_Acceleration_Addr;   //制动深度-制动力给定%




uint16 Motor_Vehicle_Speed_Scale_Get;//转速-车速比例
uint8 Vehco_Type_Test=0;
uint8 AccPedCD_TYPE=1;   //油门类型 1 双线 2 单线
uint8 Breakk_Type=2;     //制动类型  0：单路开关量 1：双路互反开关量  2：单路模拟量 3：双路模拟量


#ifndef AECS_HW
  #pragma CODE_SEG DEFAULT
#else  
  #pragma CODE_SEG VCU_CODE 
#endif

void  Vehicle_Single_Variable_Set(void) 
{      //单值变量映射    
      //车型定义 1 实验室驾驶舱 2 自制驾驶舱
      
    switch(Vehco_Type_Test) 
    {
        
        case 1:
        default:
            Motor_Vehicle_Speed_Scale_Get=Motor_Vehicle_Speed_Scale_T01;
            AccPedCD_TYPE=AccPedCD_TYPE_T01;  //油门类型 1 双线 2 单线   
            Breakk_Type=Breakk_Type_T01;      //制动类型  0：单路开关量 1：双路互反开关量  2：单路模拟量 3：双路模拟量 
            break;  
        case 2:  //2 自制驾驶舱
            Motor_Vehicle_Speed_Scale_Get=Motor_Vehicle_Speed_Scale_T02;
            AccPedCD_TYPE=AccPedCD_TYPE_T02;  //油门类型 1 双线 2 单线    4 CAN
            Breakk_Type=Breakk_Type_T02;      //制动类型  0：单路开关量 1：双路互反开关量  2：单路模拟量 3：双路模拟量    4:CAN     
            break;        
          
    }
}



void  Vehicle_Type_Set (void) 
{             //曲线表映射
/*uint16  aa;
sint16 bb;
bb=55;*/
    //switch(Vehicle_Type) 
    switch (Vehco_Type_Test)
    {
        
        case 1: //1 实验室驾驶舱
        default:
            AccPedCD_rUW_CUR_Addr =(uint16 * __far)&AccPedCD_rUW_CUR_T01;   //油门开度1
            AccPedCD_rUW_CUR2_Addr=(uint16 * __far)&AccPedCD_rUW_CUR2_T01;  //油门开度2
            AccPed_Rip_Addr       =(uint16 * __far)&AccPed_Rip_T01;         //油门曲线
            Breakk_rUW_CUR_Addr   =(uint16 * __far)&Breakk_rUW_CUR_T01;     //制动踏板深度

            AccPed_Acceleration_Addr       =(uint16 * __far)&AccPed_Acceleration_T01;         //油门曲线
            Break_Acceleration_Addr       =(uint16 * __far)&Break_Acceleration_T01;         //制动曲线
 

            
            break;
        case 2:  //2 自制驾驶舱
            
            AccPedCD_rUW_CUR_Addr =(uint16 * __far)&AccPedCD_rUW_CUR_T01;   //油门开度1        //TYPE4不用
            AccPedCD_rUW_CUR2_Addr=(uint16 * __far)&AccPedCD_rUW_CUR2_T01;  //油门开度2        //TYPE4不用
            AccPed_Rip_Addr       =(uint16 * __far)&AccPed_Rip_T02;         //油门曲线
            Breakk_rUW_CUR_Addr   =(uint16 * __far)&Breakk_rUW_CUR_T01;     //制动踏板深度     //TYPE4不用

            AccPed_Acceleration_Addr       =(uint16 * __far)&AccPed_Acceleration_T02;         //油门曲线
            Break_Acceleration_Addr       =(uint16 * __far)&Break_Acceleration_T02;         //制动曲线           
           
           
            break;
                            
    }
}



