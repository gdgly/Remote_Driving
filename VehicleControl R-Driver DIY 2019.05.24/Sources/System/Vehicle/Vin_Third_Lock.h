

#ifndef __Vin_Third_Lock
    #define __Vin_Third_Lock

    #include "TypeDef.h"
    
    #define Third_Lock_PowOn_Delay_Max     30   //�ϵ�������ָ���ӳ�  ���3��
    
    #ifndef AECS_HW
      #pragma CODE_SEG DEFAULT
    #else  
      #pragma CODE_SEG VCU_CODE 
    #endif


    void Third_Lock_Vehicle(void);
    

#endif    