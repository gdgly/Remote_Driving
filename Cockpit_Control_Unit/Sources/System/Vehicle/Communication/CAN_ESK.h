

#ifndef __CAN_ESK_H
    #define __CAN_ESK_H

    #include "TypeDef.h"
    
    #ifndef AECS_HW
      #pragma CODE_SEG DEFAULT
    #else  
      #pragma CODE_SEG VCU_CODE 
    #endif

    //�����ŵ�������Ϣ������
    void CANNet_RecvMsgCAN_ESK(MSCANDrv_infoMSGS_TYPE infoMsgS);

#endif    