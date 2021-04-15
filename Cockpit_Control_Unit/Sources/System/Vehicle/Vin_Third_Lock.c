/*******************************************************************************
* �������ƣ��������ն����������ٹ���
* �ļ����� Vin_Third_Lock.c
* �������� �� 
* ���ü�¼��
* ��ע�� 
* �޶���¼��
*		����		      �汾		  ����		����
*   
*******************************************************************************/
#include <string.h>
#include "GlobalDef.h"
#include "Vin_Third_Lock.h"

    
#ifndef AECS_HW
       #pragma DATA_SEG DEFAULT
#else  
      #pragma DATA_SEG S12X_RAM
#endif

uint8 Third_Lock_PowOn_Delay=0; //�����������ϵ�3���ж�����״̬��ʱ��   100ms +1


uint8 Third_Lock_GetNewMsg_HongQuan=0;
uint8 Third_Lock_GetNewMsg_MaiJiaMai=0;
uint8 Third_Lock_RandKey[5] ={0} ;
uint8 Third_Lock_Com_Index =0;
uint8 Third_Lock_Life =0;

uint8 Third_Lock_Command=0;             //�ն�����CANָ��

uint8 Third_Lock_Active=0;              //��������
uint8 Third_Lock_Mode_Active=0;         //��Ч����ģʽ
uint8 Third_Lock_LimitSpeed_Active=0;   //��Ч�����ٶ� 


extern uint8 Third_Lock_Mode;        //EEPROM     ����ģʽ
extern uint8 Third_Lock_LimitSpeed;  //EEPROM     �����ٶ�
extern uint8 Third_Lock_HongQuan_Active;  //���ݺ�Ȫ�ն˼���״̬
extern uint8 Third_Lock_MaiJiaMai_Active;  //�����������ն˼���״̬



extern uint16 AccPed,Breakk;
extern uint8 Dang_L4bit;
extern sint16 Motor_Speed;
extern uint16 Motor_Vehicle_Speed_Scale_Get;   //�൵����ΪUINT16



#ifndef AECS_HW
      #pragma CODE_SEG DEFAULT
#else  
      #pragma CODE_SEG VCU_CODE
      
#endif

//������  �ϵ�3��������������ָ���������������10�� ��ʱ������ָ����㰲ȫ�����½������ָ��  (����Ϊ0����λ�յ���P���������ţ���ɲ��)

//��Ȫ    �ϵ�3��������������ָ�������������ֹ��ʻ�� ��ʱ������ָ����㰲ȫ�����½������ָ��(����Ϊ0����λ�յ���P���������ţ���ɲ��)
//        ��ʱ��⡢ִ������ָ� ��ʱ���������ָ����㰲ȫ�����½������ָ�� (����Ϊ0����λ�յ���P���������ţ���ɲ��)




//������������
void Third_Lock_Vehicle(void) 
{

    //�ϵ�3��������������ָ��
    if (Third_Lock_PowOn_Delay<=Third_Lock_PowOn_Delay_Max) 
    {
        Third_Lock_PowOn_Delay++;
        Third_Lock_Com_Index++;
        //�����ն˼���  ִ�������������ж�
        if ((Third_Lock_HongQuan_Active==1)||(Third_Lock_MaiJiaMai_Active==1)) 
        {
            if (Third_Lock_GetNewMsg_HongQuan==1) //�յ����ݺ�Ȫָ��
            {   Third_Lock_GetNewMsg_HongQuan=0;
                switch (Third_Lock_Command)
                {
                    default:
                        break;
                    case 1:   //����
                        Third_Lock_Mode=1;
                        if (Third_Lock_LimitSpeed<15) 
                        {
                            Third_Lock_LimitSpeed=15;
                        }
                        if (Third_Lock_LimitSpeed>120)
                        {
                            Third_Lock_LimitSpeed=120;
                        }
                        break;
                    case 2:   //����
                        Third_Lock_Mode=2;
                        Third_Lock_LimitSpeed=0;
                        break;
                    case 4:  //����
                        Third_Lock_Mode=0;
                        Third_Lock_LimitSpeed=0;                      
                        break;    
                }
            }
            if (Third_Lock_GetNewMsg_MaiJiaMai==1) //�յ�������ָ��
            {   Third_Lock_GetNewMsg_MaiJiaMai=0;
                switch (Third_Lock_Command)
                {
                    default:
                        break;
                    case 1:   //����
                        Third_Lock_Mode=0;
                        Third_Lock_LimitSpeed=0;
                        break;
                    case 2:   //���� ����10
                        Third_Lock_Mode=1;
                        Third_Lock_LimitSpeed=10;
                        break;
                }
            }
            //��ʧ�ն�ͨѶ 2.8��      ��������
            if (Third_Lock_Com_Index>=Third_Lock_PowOn_Delay_Max-2) 
            {
                if (Third_Lock_HongQuan_Active==1) 
                {
                    Third_Lock_Mode=2;
                    Third_Lock_LimitSpeed=0;
                    
                    Third_Lock_Active=1; 
                    Third_Lock_Mode_Active=Third_Lock_Mode;
                    Third_Lock_LimitSpeed_Active=0; 
                
                }
                if (Third_Lock_MaiJiaMai_Active==1) 
                {
                    Third_Lock_Mode=1;
                    Third_Lock_LimitSpeed=10;                

                    Third_Lock_Active=1; 
                    Third_Lock_Mode_Active=Third_Lock_Mode;
                    Third_Lock_LimitSpeed_Active=0; 

                }
            }
            //�����ж�   Breakk
            //������  �ϵ�3��������������ָ���������������10�� ��ʱ������ָ����㰲ȫ�����½������ָ��  (����Ϊ0����λ�յ���P���������ţ���ɲ��)

            //��Ȫ    �ϵ�3��������������ָ�������������ֹ��ʻ�� ��ʱ������ָ����㰲ȫ�����½������ָ��(����Ϊ0����λ�յ���P���������ţ���ɲ��)
            //        ��ʱ��⡢ִ������ָ� ��ʱ���������ָ����㰲ȫ�����½������ָ�� (����Ϊ0����λ�յ���P���������ţ���ɲ��)            
            
            //�ϵ�ǰ3�룬���㰲ȫ������ִ������������ָ��
            if ((Breakk>100)&&(AccPed<100)&&((Dang_L4bit==0)||(Dang_L4bit==1)) ) 
            {
                switch (Third_Lock_Mode) 
                {
                    default:
                        break;
                    case 0: //����   ͣ������
                        if (abs(Motor_Speed)/Motor_Vehicle_Speed_Scale_Get<=1) 
                        {
                            Third_Lock_Mode_Active=Third_Lock_Mode;
                            Third_Lock_LimitSpeed_Active=Third_Lock_LimitSpeed;
                            Third_Lock_Active=0;                        
                        }
                        break;
                    case 1: //����
                        Third_Lock_Active=1;
                        Third_Lock_Mode_Active=Third_Lock_Mode;
                        Third_Lock_LimitSpeed_Active=Third_Lock_LimitSpeed;
                        break;
                    case 2: //����
                        Third_Lock_Active=1; 
                        Third_Lock_Mode_Active=Third_Lock_Mode;
                        Third_Lock_LimitSpeed_Active=0; 
                        break;
                }
            
            }


        }        
    }else 
    {   //3���Ժ�
        //�����ն˼���  ִ�������������ж�
        if ((Third_Lock_HongQuan_Active==1)||(Third_Lock_MaiJiaMai_Active==1)) 
        {
            if (Third_Lock_GetNewMsg_HongQuan==1) //�յ����ݺ�Ȫָ��
            {   Third_Lock_GetNewMsg_HongQuan=0;
                switch (Third_Lock_Command)
                {
                    default:
                        break;
                    case 1:   //����
                        Third_Lock_Mode=1;
                        if (Third_Lock_LimitSpeed<15) 
                        {
                            Third_Lock_LimitSpeed=15;
                        }
                        if (Third_Lock_LimitSpeed>120)
                        {
                            Third_Lock_LimitSpeed=120;
                        }
                        break;
                    case 2:   //����
                        Third_Lock_Mode=2;
                        Third_Lock_LimitSpeed=0;
                        break;
                    case 4:  //����
                        Third_Lock_Mode=0;
                        Third_Lock_LimitSpeed=0;                      
                        break;    
                }
            }
            if (Third_Lock_GetNewMsg_MaiJiaMai==1) //�յ�������ָ��
            {   Third_Lock_GetNewMsg_MaiJiaMai=0;
                switch (Third_Lock_Command)
                {
                    default:
                        break;
                    
                    case 1:   //����
                        Third_Lock_Mode=0;
                        Third_Lock_LimitSpeed=0;
                        break;
                    /*    
                    case 2:   //����10   3��󲻽�������ָ��
                        Third_Lock_Mode=1;
                        Third_Lock_LimitSpeed=10;
                        break;
                    */
                }
            }

            //�����ж� 
            
            //�ϵ�3������㰲ȫ������ִ������������ָ��
            if ((Breakk>100)&&(AccPed<100)&&((Dang_L4bit==0)||(Dang_L4bit==1)||(Dang_L4bit==10)) ) 
            {
                switch (Third_Lock_Mode) 
                {
                    default:
                        break;
                    case 0: //����    ͣ������
                        if ((abs(Motor_Speed)/Motor_Vehicle_Speed_Scale_Get<=1)&&((Dang_L4bit==0)||(Dang_L4bit==1)) )
                        {
                            Third_Lock_Active=0; 
                            Third_Lock_Mode_Active=Third_Lock_Mode;
                            Third_Lock_LimitSpeed_Active=0;                                                   
                        }
                        break;
                    case 1: //����      ��Ȫִ������ָ�������ǰ���Ѿ�����������ָ��
                        
                        Third_Lock_Active=1;
                        Third_Lock_Mode_Active=Third_Lock_Mode;
                        Third_Lock_LimitSpeed_Active=Third_Lock_LimitSpeed;                        
                        break;
                    case 2: //����
                        //Third_Lock_Active=1;  //3���ִ������ָ��
                        //Third_Lock_Mode_Active=Third_Lock_Mode;
                        //Third_Lock_LimitSpeed_Active=Third_Lock_LimitSpeed;
                        break;
                }
            
            }


        }
    }
    if (Third_Lock_Enable==0)
    {
        Third_Lock_Mode=0;
        Third_Lock_LimitSpeed=0;
        Third_Lock_HongQuan_Active=0;
        Third_Lock_MaiJiaMai_Active=0;
        
        Third_Lock_Active=0;
        Third_Lock_Mode_Active=0;
        Third_Lock_LimitSpeed_Active=0;
        
    }
    
    
    
}
















