

#ifndef __Vin_Rw_H
    #define __Vin_Rw_H

    #include "TypeDef.h"
    
    #ifndef AECS_HW
      #pragma CODE_SEG DEFAULT
    #else  
      #pragma CODE_SEG VCU_CODE 
    #endif
    //Vin���д������ 10ms������һ��
    void Vin_Rw_Main(void); 
    //ˢдVINͨѶ����    20ms����һ��
    void CANNet_Send_VinRwMsg(void);
    //������յ�������֡
    void CANNet_Get_HC_Data (uint8 Get_HC_Data[8]);
    
    void HC_Data_Check(void);
#endif    