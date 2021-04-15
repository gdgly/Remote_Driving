

#ifndef __Vin_Interface_conversion
    #define __Vin_Interface_conversion

    #include "TypeDef.h"
    
    #ifndef AECS_HW
      #pragma CODE_SEG DEFAULT
    #else  
      #pragma CODE_SEG VCU_CODE 
    #endif

    void VehCo_Interface(void); 
    void MSCANDrv_SendMsgCAN_A(MSCANDrv_infoMSGS_TYPE infoMsgS);
    void MSCANDrv_SendMsgCAN_B(MSCANDrv_infoMSGS_TYPE infoMsgS);
    void MSCANDrv_SendMsgCAN_C(MSCANDrv_infoMSGS_TYPE infoMsgS);

    //接收CAN消息函数接口
    void CANNet_RecvMsgCAN0(MSCANDrv_infoMSGS_TYPE infoMsgS);
    void CANNet_RecvMsgCAN1(MSCANDrv_infoMSGS_TYPE infoMsgS);
    void CANNet_RecvMsgCAN2(MSCANDrv_infoMSGS_TYPE infoMsgS);
    
#endif    