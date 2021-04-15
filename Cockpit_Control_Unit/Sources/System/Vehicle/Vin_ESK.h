

#ifndef __Vin_ESK_H
    #define __Vin_ESK_H

    #include "TypeDef.h"
    
    #ifndef AECS_HW
      #pragma CODE_SEG DEFAULT
    #else  
      #pragma CODE_SEG VCU_CODE 
    #endif


//µµÎ»×´Ì¬ÅÐ¶Ï
void Vehco_Dang_Status(void);
//ÅÅµµÊ½¡¢×ÔËøÊ½»»µ²Âß¼­ 
void Vehco_Dang_SelfLock_Status(void);
 //²¦Æ¬Ê½¡¢ÐýÅ¥Ê½µã¶¯»»µ²Âß¼­
void Vehco_Dang_ESK_Inching_Request_Status(void);

#endif    