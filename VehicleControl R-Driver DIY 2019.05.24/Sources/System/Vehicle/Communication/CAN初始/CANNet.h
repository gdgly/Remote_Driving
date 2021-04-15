#ifndef __CANNET_H
    #define __CANNET_H
    #include "HW_select.h"
    #include "TypeDef.h"
    
    
    #ifndef AECS_HW
      #pragma CONST_SEG DEFAULT
    #else  
      #pragma CODE_SEG SEG_VDL_CAN
    #endif
    
    //接收CAN消息函数接口
    void CANNet_RecvMsgCAN0(MSCANDrv_infoMSGS_TYPE infoMsgS);
    void CANNet_RecvMsgCAN1(MSCANDrv_infoMSGS_TYPE infoMsgS);
    void CANNet_RecvMsgCAN2(MSCANDrv_infoMSGS_TYPE infoMsgS);
    
    //发送消息管理
    void CANNet_Manage1ms(void);
    
    
#endif