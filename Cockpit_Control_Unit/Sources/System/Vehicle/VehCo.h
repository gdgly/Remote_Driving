/*******************************************************************************
* 工程名称：整车管理
* 文件名： VehCo.h
* 功能描述 ： 整车管理协调头文件
* 引用记录：
* 备注： 
* 修订记录：
*		日期		      版本		  作者		描述
*                                             
*******************************************************************************/

#ifndef __VEHCO_H
    #define __VEHCO_H

    #include "TypeDef.h"
    
    
    #define _VEHCO_TIME_MONITOR_PERIOD  10
    #define _AD_fliter_Cycles 10 //油门过滤采样次数
    
    #define _VEHCO_AFTERRUN_STATUS_INIT     0
    #define _VEHCO_AFTERRUN_STATUS_BEGIN    1
    #define _VEHCO_AFTERRUN_STATUS_FINISH   2    
    //所有历史数据需定义在下述段中
    //#pragma DATA_SEG __GPAGE_SEG RUNPAR_EEPROM

    #ifndef AECS_HW
      #pragma DATA_SEG __GPAGE_SEG RUNPAR_EEPROM
    #else  
      #pragma DATA_SEG  NVRAM 
    #endif
        extern uint16 VehCo_ctTestUW;
        extern uint8 Vehco_State_Stop_Step;



    #ifndef AECS_HW
      #pragma CODE_SEG DEFAULT
    #else  
      #pragma CODE_SEG VCU_CODE 
    #endif
    //整车管理初始化
    void VehCo_Init(void);
    
    //基础管理循环
    void VehCo_Manage10ms(void);
    void VehCo_Manage1ms(void);
    
    //具体操作
    void VehCo_UpdateSig(void);
    void VehCo_Monitor(void);
    
    //状态管理
    void VehCo_StateMachine(void);
    void VehCo_Afterrun(void);
    
    //void Vehco_AC_Control(void);
    //void VehCo_Test(void);
    void Vehco_Key_Status(void);
    void VehCo_DOset (uint8 Do_off) ;

    //CAN油门刹车离合信号处理
    void  CAN_AccPed_Breakk(void) ;
    //自动EPB
    void Auto_Hold_EPB (void) ;

    //自动回正
    void  Steering_Whee_Back_Zero (void); 
    //回正力计算
    void BackZeroForce_Angle_Calc (void) ;
    
    //模拟方向盘自动校正
    void Steering_Whee_Auto_Check (void); 

    //远程加速度控制
    void R_Vehco_Acceleration (void) ;
    
    //方向盘角度控制
    void  Steering_Wheel (void); 
    
    //远程档位控制
    void Vehco_Dang (void);
    
    //驾驶模式判断
    void Vehco_Driver_Mode(void);
    //故障判断
    void Vehco_Errpr_Judge(void);     
     
#endif    