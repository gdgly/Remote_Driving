/*******************************************************************************
* �������ƣ���е�������ŵ��˴������
* �ļ����� Vin_ESK.c
* �������� �� 
* ���ü�¼��
* ��ע�� 
* �޶���¼��
*		����		      �汾		  ����		����
*   
*******************************************************************************/
#include <string.h>
#include "GlobalDef.h"
#include "Vin_ESK.h"

    
#ifndef AECS_HW
       #pragma DATA_SEG DEFAULT
#else  
      #pragma DATA_SEG S12X_RAM
#endif




uint8 ESK_Signal_Type=0;//��λ�ź����� 0 ��ƽ�ź� 1 CAN�����ź� 
uint8 ESK_Type;//���ӻ���λ����  0 ��Ƭʽ����ťʽ���㶯�źţ�  1 �ŵ���ʽ����ťʽ�������źţ�
uint8 ESK_Fault;//���ӻ���������
uint8 ESK_Live;//���ӻ����������ź� 0~15

uint8 ESK_SelfLock_Request; //���ӻ������ź�����  ����ʽ  ��ɲ����Ӳ�߽���
uint8 ESK_Inching_Request;  //���ӻ������ź�����  �㶯ʽ  ����ɲ�ź�

uint8 Dang_S,Dang_E,Dang_EE,Dang_C;
uint8 Breakk_Please;//���ƶ�̤������


extern uint8 Dang_P,Dang_R,Dang_N,Dang_D; //��λ���뿪��״̬
extern uint8 Dang_L4bit;//��λ״̬   1 5 0 10 PRDN
extern uint8 bDiSatus[24];     //DI�ڿ�����״̬
extern uint16 Breakk;//�ƶ�̤��״̬
extern uint16 AccPed;//���������ߴ���������ֵ
extern sint16 Motor_Speed;
extern uint16 Motor_Vehicle_Speed_Scale_Get;//ת��-���ٱ���
extern uint8 LostESKCommunication;
extern uint16 LostESKCom_Index;
extern uint8 ESK_Signal; //�յ����ӵ�λ�ź�

extern sint8  Gears_GetPow; //������ȡ������״̬  0 δ���  1 ��� 3 �쳣
extern uint8  Gears_Install;

 
#ifndef AECS_HW
      #pragma CODE_SEG DEFAULT
#else  
      #pragma CODE_SEG VCU_CODE
      
#endif


//��λ״̬�ж� 10ms
void Vehco_Dang_Status(void) 
{   
    
    if (ESK_Signal==1)  //�յ�CANͨѶʹ�ñ궨���ͣ�����ʹ�õ�ƽʽ�ź�����
    {
        ESK_Signal_Type=ESK_Signal_Enable;
    }else 
    {
        ESK_Signal_Type=0;
    }

    switch(ESK_Signal_Type) 
    { //�����ź����;�����λ�����ź� ,������ �㶯ʽ

      case 1: //CAN�ź�
          if (ESK_Type==1)
          {   //1 �ŵ���ʽ����ťʽ�������źţ�
              Dang_P=bDiSatus[14];   //��ɲ�ź�����Ӳ�ߵ�ƽ����
              switch(ESK_SelfLock_Request) 
              {
                  default:
                  case 0:
                      Dang_S=0; Dang_E=0; Dang_EE=0;  Dang_R=0; Dang_N=1; Dang_D=0,Dang_C=0;
                      break;
                  case 2:
                      Dang_S=1; Dang_E=0; Dang_EE=0;  Dang_R=0; Dang_N=0; Dang_D=1,Dang_C=0;//   S E E+ �Ƿ���D
                        break;
                  case 3:
                      Dang_S=0; Dang_E=1; Dang_EE=0;  Dang_R=0; Dang_N=0; Dang_D=1,Dang_C=0;//  E
                      break;
                  case 4:
                      Dang_S=0; Dang_E=0; Dang_EE=1;  Dang_R=0; Dang_N=0; Dang_D=1,Dang_C=0;//  E+
                      break;
                  case 5:
                      Dang_S=0; Dang_E=0; Dang_EE=0;  Dang_R=1; Dang_N=0; Dang_D=0,Dang_C=0;
                      break;
                  case 10:
                      Dang_S=0; Dang_E=0; Dang_EE=0;  Dang_R=0; Dang_N=0; Dang_D=1,Dang_C=0;
                      break;
                 case 11:
                      Dang_S=0; Dang_E=0; Dang_EE=0;  Dang_R=0; Dang_N=0; Dang_D=1,Dang_C=1;//  C
                      break;
              }
          }else 
          {   //ESK_Type!=1 ��Ƭʽ����ťʽ���㶯�ź�)    δ��
              //uint8 ESK_Inching_Request;  //���ӻ������ź�����  �㶯ʽ  ����ɲ�ź�     bit 0001,1111  ���� P,-2,-1,+1,+2
              Dang_S=0; Dang_E=0; Dang_EE=0;  Dang_R=0; Dang_N=0; Dang_D=0; Dang_P=0; Dang_C=0;
              
              
              
              
          }


          LostESKCom_Index++;

          if (LostESKCom_Index>500)   //ESK ͨѶ��ʱ5S
          {
              LostESKCom_Index=501;
              LostESKCommunication=1;
          } else 
          {
              LostESKCommunication=0;
          }

          //���ϴ���  ����ʽ  �㶯δ��   
          if ((ESK_Fault==1)||(LostESKCommunication ==1))
          {
              Dang_S=0; Dang_E=0; Dang_EE=0;  Dang_R=0; Dang_N=0; Dang_D=0; Dang_P=0;
          }
          break;
      default:
      case 0: //��ƽʽ�ź�
          Dang_P=bDiSatus[14];    //��ɲ�ź���Ӳ�ߵ�ƽ����
          Dang_R=bDiSatus[15];
          Dang_N=bDiSatus[16];
          Dang_D=bDiSatus[17];                
          Dang_S=0;
          Dang_E=0;
          Dang_EE=0;
          Dang_C=0;
          break;
    }
    
    // ESK_Signal_Type;//��λ�ź�����    0 ��ƽ�ź�    1 CAN�����ź� 
    // ESK_Type;       //���ӻ���λ����  0 ��Ƭʽ����ťʽ���㶯�źţ�  1 �ŵ���ʽ����ťʽ�������źţ�   
   
    if ((ESK_Signal_Type==0)                           //����0 ��ƽ�ź�
        || ((ESK_Signal_Type==1)&&(ESK_Type==1))  )    //����1 CAN�����ź�&&�������źţ�   

    {   //�ŵ�ʽ������ʽ�����߼�
        Vehco_Dang_SelfLock_Status();
    }else 
    {   //��Ƭʽ����ťʽ�㶯�����߼�    δ��
        Vehco_Dang_ESK_Inching_Request_Status();
    }
}
 //��Ƭʽ����ťʽ�㶯�����߼�
void Vehco_Dang_ESK_Inching_Request_Status(void) 
{//uint8 ESK_Inching_Request;  //���ӻ������ź�����  �㶯ʽ  ����ɲ�ź�     bit 0001,1111  ���� P,-2,-1,+1,+2




}

//�ŵ�ʽ������ʽ�����߼�
void Vehco_Dang_SelfLock_Status(void)
{   
    //��δͣ��ǰ������������Ч������Ϊ-100~100�����Ч�� Motor_Speed
    
    switch(Dang_D+(Dang_N<<1)+(Dang_R<<2)+(Dang_P<<3))  // PRND 0000
    {
        case 0:
        case 2:
            Dang_L4bit=0;  //N��  0010 0000
            Breakk_Please=0;
            break;
        case 1:
        case 9:
            if (Dang_L4bit!=10) 
            {
                if (   ((Breakk>=10)||(Motor_Speed>=10*Motor_Vehicle_Speed_Scale_Get))
                    && (AccPed<=100)&&(Motor_Speed>=(sint16) -2* ((sint16)Motor_Vehicle_Speed_Scale_Get)))  //�����Źҵ���Ч�������ź�λ��Ч  ���ƶ����ٴ���10��λ��Ч
                {
                    Dang_L4bit=10; //D��  0001 1001
                    Breakk_Please=0 ;

                } 
                else 
                {
                    Dang_L4bit=0; //�ô����������󱨾�
                    if ((Breakk<100)||(AccPed>100))
                    {
                      Breakk_Please=1 ;
                    }
                }
            }
            break;
        case 4:
        case 12:
            if (Dang_L4bit!=5)
            {
                if ((Breakk>=10)&&(AccPed<=100)&&(Motor_Speed<=2*(sint16)Motor_Vehicle_Speed_Scale_Get)) //�����Źҵ���Ч�������ź�λ��Ч   ���ƶ���λ��Ч
                {
                    Dang_L4bit=5;  //R��  0100 1100
                    Breakk_Please=0;
                } 
                else
                {
                    Dang_L4bit=0;   //�ô����������󱨾�
                    if ((Breakk<100)||(AccPed>100))
                    {
                      Breakk_Please=1 ;
                    }
                }
            }
            break;
        case 8:
        case 10:
            Dang_L4bit=1;  //P��  1000 1010 
            Breakk_Please=0; 
            break;
         default:
            //�������Ϊ���� ���ÿյ�
            Dang_L4bit=0;
            break;   
    }
    //ȡ��������ʱ��λ�������յ�
    if ((Gears_GetPow==1)&&(Gears_Install==1))
    {
        Dang_L4bit=Dang_P;
    }


}
















