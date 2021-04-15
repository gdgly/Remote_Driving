

#ifndef __CAN_APU_H
    #define CAN_APU_H

    #include "TypeDef.h"
    
    #ifndef AECS_HW
      #pragma CODE_SEG DEFAULT
    #else  
      #pragma CODE_SEG VCU_CODE 
    #endif

    //増程器接收消息处理函数
    void CANNet_RecvMsgCAN_APU(MSCANDrv_infoMSGS_TYPE infoMsgS);
   //増程器发送消息处理函数 1ms
    void CANNet_SendMsgCAN_APU(void);
#endif    