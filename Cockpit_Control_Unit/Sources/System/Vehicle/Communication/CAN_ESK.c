/*******************************************************************************
* �������ƣ������ŵ���CANͨѶ����
* �ļ����� CAN_ESK.c
* �������� �� 
* ���ü�¼��
* ��ע�� 
* �޶���¼��
*		����		      �汾		  ����		����
*   
*******************************************************************************/
#include <string.h>
#include "GlobalDef.h"
#include "CAN_ESK.h"

    
#ifndef AECS_HW
       #pragma DATA_SEG DEFAULT
#else  
      #pragma DATA_SEG S12X_RAM
#endif

uint32 CANNet_infoID_ESK;
uint8 CanRecESK[8]={0};
uint8 LostESKCommunication=0;
uint16 LostESKCom_Index=0;
uint8 ESK_Signal=0;
    
extern uint8 ESK_Signal_Type;//��λ�ź����� 0 ��ƽ�ź� 1 CAN�����ź� 
extern uint8 ESK_Type;//���ӻ���λ����  0 ��Ƭʽ����ťʽ���㶯�źţ�  1 �ŵ���ʽ����ťʽ�������źţ�
extern uint8 ESK_Fault;//���ӻ���������
extern uint8 ESK_Live;//���ӻ����������ź� 0~15

extern uint8 ESK_SelfLock_Request; //���ӻ������ź�����  ����ʽ  ��ɲ����Ӳ�߽���
extern uint8 ESK_Inching_Request;  //���ӻ������ź�����  �㶯ʽ  ����ɲ�ź�


 
#ifndef AECS_HW
      #pragma CODE_SEG DEFAULT
#else  
      #pragma CODE_SEG VCU_CODE
      
#endif

//�����ŵ�������Ϣ������
void CANNet_RecvMsgCAN_ESK(MSCANDrv_infoMSGS_TYPE infoMsgS)
{

    // uint8 Index;
    //CAN��������
    CANNet_infoID_ESK=infoMsgS.infoMsgIDUI;

    //CAN0���շ��ദ��
    
    switch(CANNet_infoID_ESK) 
    {
      case 0x0CEEA9A0:
            //ESK �����ŵ���
            /*
            uint8 ESK_Signal_Type=0;//��λ�ź����� 0 ��ƽ�ź� 1 CAN�����ź� 
          
            */
            memcpy(CanRecESK,infoMsgS.datMsgUBA,sizeof(CanRecESK));                 
            ESK_Inching_Request=CanRecESK[0];         //���ӻ������ź�����  �㶯ʽ  ����ɲ�ź�            
            ESK_SelfLock_Request=0x0F&CanRecESK[1];   //���ӻ������ź�����  ����ʽ  ��ɲ����Ӳ�߽���
            ESK_Type=0x01&CanRecESK[2];               //���ӻ���λ����  0 ��Ƭʽ����ťʽ���㶯�źţ�  1 �ŵ���ʽ����ťʽ�������źţ�
            ESK_Fault=(0x02&CanRecESK[2])>>1;         //���ӻ���������
            
           // ESK_Signal=1;//�յ������ŵ��ź�  
           // LostESKCom_Index=0;
            
            
            //���ж�������Ϣ�ٸ�����������
            if (   (((CanRecESK[2])>>4==0) && (ESK_Live==15)) 
                ||   ((CanRecESK[2])>>4==ESK_Live+1) ) //��������
            {
                LostESKCom_Index=0;
                ESK_Signal=1;//�յ������ŵ��ź�            
            }
            ESK_Live=CanRecESK[2]>>4;                 //���ӻ����������ź� 0~15 
             
             
            break;
      default:
            break;
    }
}









