

#ifndef __CAN_APU_H
    #define CAN_APU_H

    #include "TypeDef.h"
    
    #ifndef AECS_HW
      #pragma CODE_SEG DEFAULT
    #else  
      #pragma CODE_SEG VCU_CODE 
    #endif

    //������������Ϣ������
    void CANNet_RecvMsgCAN_APU(MSCANDrv_infoMSGS_TYPE infoMsgS);
   //������������Ϣ������ 1ms
    void CANNet_SendMsgCAN_APU(void);
#endif    