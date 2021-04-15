/*******************************************************************************
* �������ƣ���������
* �ļ����� CANNet.h
* �������� �� CAN���߹���ͷ�ļ�
* ���ü�¼��
* ��ע�� 
* �޶���¼��
*		����		      �汾		  ����		����
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
    
    //����CAN��Ϣ�����ӿ�
    void CANNet_RecvMsgCAN_A(MSCANDrv_infoMSGS_TYPE infoMsgS);
    void CANNet_RecvMsgCAN_B(MSCANDrv_infoMSGS_TYPE infoMsgS);
    void CANNet_RecvMsgCAN_C(MSCANDrv_infoMSGS_TYPE infoMsgS);
    
    //������Ϣ����
    void CANNet_Manage1ms(void);
    
    
#endif