

#ifndef __Vin_ESK_H
    #define __Vin_ESK_H

    #include "TypeDef.h"
    
    #ifndef AECS_HW
      #pragma CODE_SEG DEFAULT
    #else  
      #pragma CODE_SEG VCU_CODE 
    #endif


//��λ״̬�ж�
void Vehco_Dang_Status(void);
//�ŵ�ʽ������ʽ�����߼� 
void Vehco_Dang_SelfLock_Status(void);
 //��Ƭʽ����ťʽ�㶯�����߼�
void Vehco_Dang_ESK_Inching_Request_Status(void);

#endif    