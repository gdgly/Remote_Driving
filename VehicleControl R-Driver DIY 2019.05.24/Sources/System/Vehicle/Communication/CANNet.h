/*******************************************************************************
* 工程名称：整车管理
* 文件名： CANNet.h
* 功能描述 ： CAN总线管理头文件
* 引用记录：
* 备注： 
* 修订记录：
*		日期		      版本		  作者		描述
*                                           
*******************************************************************************/

#ifndef __CANNET_H
    #define __CANNET_H
    #include "HW_select.h"
    #include "TypeDef.h"
  // #include "stdlib.h"
   #include <stdio.h>
       
    #ifndef AECS_HW
      #pragma CODE_SEG DEFAULT
    #else  
      #pragma CODE_SEG SEG_VDL_CAN
    #endif
    
    //接收CAN消息函数接口
    void CANNet_RecvMsgCAN_A(MSCANDrv_infoMSGS_TYPE infoMsgS);
    void CANNet_RecvMsgCAN_B(MSCANDrv_infoMSGS_TYPE infoMsgS);
    void CANNet_RecvMsgCAN_C(MSCANDrv_infoMSGS_TYPE infoMsgS);
    
    //发送消息管理
    void CANNet_Manage1ms(void);
    //合并8bit为Byter
    uint8 SetBit8(uint8 *Bitt);
    //拆分byter为8个bit
    void Get8bit(uint8 *Bitt,uint8 Bytee);
        
    
#endif