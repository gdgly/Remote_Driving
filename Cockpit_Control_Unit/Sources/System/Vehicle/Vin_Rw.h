

#ifndef __Vin_Rw_H
    #define __Vin_Rw_H

    #include "TypeDef.h"
    
    #ifndef AECS_HW
      #pragma CODE_SEG DEFAULT
    #else  
      #pragma CODE_SEG VCU_CODE 
    #endif
    //Vin码读写主程序 10ms被调用一次
    void Vin_Rw_Main(void); 
    //刷写VIN通讯功能    20ms发送一次
    void CANNet_Send_VinRwMsg(void);
    //处理接收到的数据帧
    void CANNet_Get_HC_Data (uint8 Get_HC_Data[8]);
    
    void HC_Data_Check(void);
#endif    