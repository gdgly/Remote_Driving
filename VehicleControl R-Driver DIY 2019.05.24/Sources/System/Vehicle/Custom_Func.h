/********************************************************************************
自定义函数集合 头文件                                          
*******************************************************************************/

#ifndef __Custom_Func_H
    #define __Custom_Func_H

    #include "TypeDef.h"
    
    
    //#define _VEHCO_TIME_MONITOR_PERIOD  5
    //#define _AD_fliter_Cycles 10 //油门过滤采样次数
    #ifndef AECS_HW
      #pragma CODE_SEG DEFAULT
    #else  
      #pragma CODE_SEG VCU_CODE 
    #endif

    //车辆模式判断
    void Torque_Mode_Judge(void);   
    //速度缓冲处理
    void Motor_Speed_Buffer_FIFO (void); 
    //D档蠕动转矩计算
    void Crawl_D_Torque (void);     
    //油门转矩给定值计算   Demand_Torque
    void AccPed_Torque_Calculation (void);    
    //D自动驻坡转矩计算
    void Auto_Parking(void);
    //R自动驻坡转矩计算 
    void Auto_Parking_R(void);
    //电池功率限制转矩
    void Torque_Power_Limit(void); 
    //D档滑行回馈转矩计算
    void FeedBack_Coast (void); 
    //D档制动回馈转矩计算
    void FeedBack_Break (void); 
    //驱动电功率计算
    void Driver_Power_Func(void);
    //电机效率自动修正系数计算
    void  Efficieny_Fix(void); 
    //合并8bit为Byter
    uint8 SetBit8(uint8 *Bitt);
    //拆分byter为8个bit
    void Get8bit(uint8 *Bitt,uint8 Bytee);
    //换挡后扭矩平滑过渡处理
    void Gears_Shoft_Torque(void);
    
          
#endif    