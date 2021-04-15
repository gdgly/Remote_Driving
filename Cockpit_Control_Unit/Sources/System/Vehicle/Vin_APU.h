

#ifndef __Vin_APU_H
    #define __Vin_APU_H

    #include "TypeDef.h"
    
    #ifndef AECS_HW
      #pragma CODE_SEG DEFAULT
    #else  
      #pragma CODE_SEG VCU_CODE 
    #endif


    void APU_Control(void);
    void APU_Debug(void);





#endif    