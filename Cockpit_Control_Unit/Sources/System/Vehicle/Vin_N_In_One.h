

#ifndef __Vin_N_In_One
    #define __Vin_N_In_One

    #include "TypeDef.h"
    
    #ifndef AECS_HW
      #pragma CODE_SEG DEFAULT
    #else  
      #pragma CODE_SEG VCU_CODE 
    #endif

/*
//��λ״̬�ж�
void Vehco_Dang_Status(void);
//�ŵ�ʽ������ʽ�����߼� 
void Vehco_Dang_SelfLock_Status(void);
 //��Ƭʽ����ťʽ�㶯�����߼�
void Vehco_Dang_ESK_Inching_Request_Status(void);
*/
uint8 Power_OK(void);

void Oil_Pump_Control(void); 

void Air_Pump_Control(void); 

#endif    