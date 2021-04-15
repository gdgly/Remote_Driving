/*******************************************************************************
* �������ƣ������ͱá����ÿ��Ƴ���
* �ļ����� Vin_N_In_One.c
* �������� �� 
* ���ü�¼��
* ��ע�� 
* �޶���¼��
*		����		      �汾		  ����		����
*   
*******************************************************************************/
#include <string.h>
#include "GlobalDef.h"
#include "Vin_N_In_One.h"

    
#ifndef AECS_HW
       #pragma DATA_SEG DEFAULT
#else  
      #pragma DATA_SEG S12X_RAM
#endif



/*
uint8 ESK_Signal_Type=0;//��λ�ź����� 0 ��ƽ�ź� 1 CAN�����ź� 
uint8 ESK_Type;//���ӻ���λ����  0 ��Ƭʽ����ťʽ���㶯�źţ�  1 �ŵ���ʽ����ťʽ�������źţ�
uint8 ESK_Fault;//���ӻ���������
uint8 ESK_Live;//���ӻ����������ź� 0~15

uint8 ESK_SelfLock_Request; //���ӻ������ź�����  ����ʽ  ��ɲ����Ӳ�߽���
uint8 ESK_Inching_Request;  //���ӻ������ź�����  �㶯ʽ  ����ɲ�ź�

uint8 Dang_S,Dang_E,Dang_EE;
uint8 Breakk_Please;//���ƶ�̤������

*/

/*
0���ػ� 1��ON�� 2������� 3��BMS�ϵ���� ���ʹ��ָ�� 4�����READY
5��ON��ʧ �Զ������� 6���µ羯�� 7�������ŵ�ָ�� 8�������ŵ��� 9���ŵ���� ȥʹ��ָ��
10: �ػ�׼����ɣ��¿��Ƶ�

12: 1������  13��2������ 14��3������ ???
*/
uint8 Air_Pump_OverHeated=0;  //���ù���
sint16 Air_Pump_Fan_Delay_Index=0; //���÷��ȹر��ӳ�  0.1�벽�� 
uint16 Air_Reservoirs_Pressure_Low_Index=0; //���ù�����ѹ�Թ��ͼ��� 0.1��
sint16 Air_Pump_WorkTime_Index=0;//     0.1S
uint8 Air_Pump_Cool=0;

sint16 Air_Pump_Torque_Last;//���õ��ǰ����ת�� 0.01N.m  -100~100
sint16 Air_Pump_Speed_Last;
sint16 Air_Pressure_Delay=0;


uint8 Air_Pump_Still=0,Oil_Pump_Still=0; // ��ת��־

uint8 Oil_Pump_Still_OverTime=0; //�ͱö�ת��ʱ

uint8 Air_Reservoirs_Pressure_Last_1=0,Air_Reservoirs_Pressure_Last_2=0;

extern uint8 Air_Reservoirs_Over_Pressure_1,Air_Reservoirs_Over_Pressure_2;
extern uint8  Air_Pressure_Ok;   //�ﵽ������
extern uint8  Air_Pressure_Ok_OverTime;

extern uint8 Vehco_State,Ready,Vehicle_Speed;
                                          
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
 
 

extern uint16 Oil_Pump_DTC; //�ͱÿ��������ϴ���
extern uint16 Oil_Pump_DC_Voltage;//�ͱ�ĸ�ߵ�ѹ     0.1V
extern uint16 Oil_Pump_DC_Current;//�ͱ�ĸ�ߵ���     0.1A
extern sint16 Oil_Pump_Temp;//�ͱÿ������¶�   -40~150
extern uint8 Oil_Pump_Fault;//�ͱù���״̬ 0 ����  1 ����

extern sint16 Oil_Pump_Speed;//�ͱõ��ת��   -6000~6000rpm
extern sint16 Oil_Pump_Torque;//�ͱõ��ת�� 0.01N.m  -100~100

extern uint8  Oil_Pump_Cmd_Mode;//�ͱ�����ģʽָ��  0 ֹͣ 1 ת��ģʽ 2 ת��ģʽ
extern uint8  Oil_Pump_Fault_Reset; //�ͱù��ϸ�λ����
extern sint16 Oil_Pump_Cmd_Torque;//�ͱ�ת��ָ��   0.01N.m -100~100
extern sint16 Oil_Pump_Cmd_Speed; //�ͱ�ת��ָ��   -6000~6000 rpm


extern uint16 Air_Pump_DTC; //���ÿ��������ϴ���
extern uint16 Air_Pump_DC_Voltage;//����ĸ�ߵ�ѹ     0.1V
extern uint16 Air_Pump_DC_Current;//����ĸ�ߵ���     0.1A
extern sint16 Air_Pump_Temp;//���ÿ������¶�   -40~150
extern uint8 Air_Pump_Fault;//���ù���״̬ 0 ����  1 ����

extern sint16 Air_Pump_Speed;//���õ��ת��   -6000~6000rpm
extern sint16 Air_Pump_Torque;//���õ��ת�� 0.01N.m  -100~100

extern uint8  Air_Pump_Cmd_Mode;//��������ģʽָ��  0 ֹͣ 1 ת��ģʽ 2 ת��ģʽ
extern uint8  Air_Pump_Fault_Reset; //���ù��ϸ�λ����
extern sint16 Air_Pump_Cmd_Torque;//����ת��ָ��   0.01N.m -100~100
extern sint16 Air_Pump_Cmd_Speed; //����ת��ָ��   -6000~6000 rpm

extern uint8  Oil_Pump_Install; //�ͱÿ���������״̬ 0 δװ 1 ��װ
extern uint8  Air_Pump_Install; //���ÿ���������״̬ 0 δװ 1 ��װ
extern uint8  Gears_Install; //�����䰲װ ״̬
extern uint8  Fan_Pwm_Install;
extern uint8  Air_Pump_Fan;//����ɢ�ȷ���  �ر�ʱ�ӳ�180��

extern uint8 Air_Reservoirs_Pressure_1,Air_Reservoirs_Pressure_2;//����Ͱѹ�� 0.00~2.50Mpa
extern uint8 LostBMSCommunication,LostINSCommunication,Air_Pump_Communication,Oil_Pump_Communication;

extern uint16  Min_B_Voltage_Limit,B_Voltage;//0~819.0V  
extern uint8   B_Fault_Status, B_Version;//BMSͨѶЭ��汾 

extern sint16 Air_Pump_Speed_DrapDown_Get;
extern sint16 Air_Pump_Torque_DrapDown_Get;
extern uint8 Air_Start_Press_Rate_Get;
extern uint8 Air_Over_Reset_Get;
extern uint16 Air_Pressure_Delay_Max_Get;
extern uint16 Air_Pump_WorkTime_Get;
extern uint16 Air_Pump_Fan_Delay_Get;
extern uint16 Air_Pump_Taget_Pressure_Get;
extern sint16 Air_Pump_Speed_Limit_Get;
extern uint16 Air_Pump_Torque_Limit_Get;

extern sint16 Oil_Pump_Speed_Limit_Get;
extern uint16 Oil_Pump_Torque_Limit_Get;

extern uint16 * __far   Oil_Pump_Speed_VSpeed_Addr;

 
#ifndef AECS_HW
      #pragma CODE_SEG DEFAULT
#else  
      #pragma CODE_SEG VCU_CODE
      
#endif

//����ж�
uint8 Power_OK(void) //BMSͨѶ��������2�����Ϲ���
{
    
    uint8 Power=0;
    if (
        (LostBMSCommunication==0)
        //&&((B_Fault_Status<=2) ||  ((B_Fault_Status<=1)&&(B_Version<18))   )       //����й����ͱ�ҲҪ���ֹ���
        &&(B_Voltage>=Min_B_Voltage_Limit)
        &&(B_Voltage!=0)
        &&((VehCo_stNewUB==_VEHCO_INDEX_STATUS_RUNNING)||(VehCo_stNewUB==_VEHCO_INDEX_STATUS_AFTERRUN))
        ) 
    { Power=1; }else{ Power=0;}
  
    return (Power);
    
}



//�ͱÿ��Ƴ���  //100ms�������
void Oil_Pump_Control(void) 
{
    uint8 Power=0,Oil_Pump_Cmd_Mode_Tmp=0,Oil_Pump_Fault_Reset_Tmp=0;
    sint16 Oil_Pump_Cmd_Torque_Tmp=0;
    sint16 Oil_Pump_Cmd_Speed_Tmp=0;
    sint16 Oil_Pump_Speed_Get=0;
    
    Power=Power_OK();

    //��Դ��ͨѶ�������Ѿ��ϸ�ѹ���г���
    if (
       (Power==1)&&(Oil_Pump_DC_Voltage/9>(B_Voltage/10) )&& (Oil_Pump_Communication==0)
       //&&((Vehco_State==4)||(Vehco_State==5)||(Vehicle_Speed>50))
       &&(Oil_Pump_Fault==0) 
       &&(Oil_Pump_Debug==0)
       &&(Oil_Pump_Still_OverTime==0)
        ) 
        
    {
        Oil_Pump_Cmd_Mode_Tmp=2;                      //2 ת��ģʽ
        Oil_Pump_Cmd_Torque_Tmp=Oil_Pump_Torque_Limit_Get;//����ת��8.68
        Oil_Pump_Fault_Reset_Tmp=0;
        
        //��ȡ��ǰ���ٶ�Ӧ�ͱ�ת��
        //AccPed2=Intpol_u16u16(AccPedCD_rUW_CUR2_Addr,uAD2); //0.375��2.05V
 
        Oil_Pump_Speed_Get= Intpol_s16s16(Oil_Pump_Speed_VSpeed_Addr, Vehicle_Speed-50) ;



       //���ٿ���
       if (Oil_Pump_Cmd_Speed<Oil_Pump_Speed_Get)
       {
          Oil_Pump_Cmd_Speed_Tmp=Oil_Pump_Cmd_Speed+Oil_Pump_Speed_Step[0];
          
          Oil_Pump_Cmd_Speed_Tmp=(Oil_Pump_Cmd_Speed_Tmp>Oil_Pump_Speed_Get) 
                                  ? Oil_Pump_Speed_Get:Oil_Pump_Cmd_Speed_Tmp;
       }else 
       {
          Oil_Pump_Cmd_Speed_Tmp=Oil_Pump_Speed_Get;
       }
       //���ٿ���
       if (Oil_Pump_Cmd_Speed>Oil_Pump_Speed_Get) 
       {
          Oil_Pump_Cmd_Speed_Tmp=Oil_Pump_Cmd_Speed-Oil_Pump_Speed_Step[1];
          
          Oil_Pump_Cmd_Speed_Tmp=(Oil_Pump_Cmd_Speed_Tmp<Oil_Pump_Speed_Get) 
                                  ? Oil_Pump_Speed_Get:Oil_Pump_Cmd_Speed_Tmp;
       }else
       {
          Oil_Pump_Cmd_Speed=Oil_Pump_Speed_Get;
       }
       //Oil_Pump_Still ��ת����ת�ж�
       
       
       
    }
    else
    {
        Oil_Pump_Cmd_Mode_Tmp   =0;
        Oil_Pump_Cmd_Torque_Tmp =0;
        Oil_Pump_Cmd_Speed_Tmp  =0;            
        //�ͱø�λ����  ��Դ���� �ͱ�ͨѶ����
        if ((Oil_Pump_Fault==1)&&(Power==1)&&(Oil_Pump_Communication==0))
        {
            Oil_Pump_Fault_Reset_Tmp=1;
           
        }else 
        {
            Oil_Pump_Fault_Reset_Tmp=0;
        }
    }

    
    
    if (Oil_Pump_Debug==1)
    {
        Oil_Pump_Cmd_Mode   =Oil_Pump_Cmd_Mode_Debug;
        Oil_Pump_Fault_Reset=Oil_Pump_Fault_Reset_Debug;
        Oil_Pump_Cmd_Torque =Oil_Pump_Cmd_Torque_Debug;
        Oil_Pump_Cmd_Speed  =Oil_Pump_Cmd_Speed_Debug;
        
    }else 
    {
        Oil_Pump_Cmd_Mode   =Oil_Pump_Cmd_Mode_Tmp;
        Oil_Pump_Fault_Reset=Oil_Pump_Fault_Reset_Tmp;
        Oil_Pump_Cmd_Torque =Oil_Pump_Cmd_Torque_Tmp;
        Oil_Pump_Cmd_Speed  =Oil_Pump_Cmd_Speed_Tmp;    
    }
    //�����ͱ�ת��
    if  (abs(Oil_Pump_Cmd_Speed)>Oil_Pump_Speed_Limit_Get)
    
    { 
        Oil_Pump_Cmd_Speed=(Oil_Pump_Cmd_Speed<0)?
                           -1 * Oil_Pump_Speed_Limit_Get:Oil_Pump_Speed_Limit_Get;  
    }

}


//���ÿ��Ƴ���  //100ms�������
void Air_Pump_Control(void) 
{
    //Air_Reservoirs_Pressure_1 Air_Pump_Taget_Pressure_Get   Air_Pump_Speed_Limit_Get
    
    uint8 Power=0,Air_Pump_Cmd_Mode_Tmp=0,Air_Pump_Fault_Reset_Tmp=0;
    sint16 Air_Pump_Cmd_Torque_Tmp=0;
    sint16 Air_Pump_Cmd_Speed_Tmp=0;
    
    
    Power=Power_OK();
    
    //��Դ��ͨѶ�������Ѿ��ϸ�ѹ���г���   Air_Pump_Fan  Air_Reservoirs_Pressure_Low_Index
    if (
     (Power==1)&&(Air_Pump_DC_Voltage/9>(B_Voltage/10) )&& (Air_Pump_Communication==0)
     //&&((Ready==1)||(Vehicle_Speed>50))
     &&(Air_Pump_Fault==0)&&(Air_Pump_OverHeated==0)
     //&&(Air_Pump_WorkTime_Index<Air_Pump_WorkTime_Get*2)
     &&(Air_Pump_Cool==0) 
     &&(Air_Pump_Debug==0)
     &&(Air_Pressure_Ok_OverTime==0)
     &&(BattCD_bLogicOnUB==1)
                                             
      )
    {
 
        Air_Pump_Cmd_Mode_Tmp=2;//�ٶ�ģʽ
        Air_Pump_Cmd_Torque_Tmp=Air_Pump_Torque_Limit_Get;//����ת��
        Air_Pump_Fault_Reset_Tmp=0;

        //С��  0.85��Ŀ��ѹ������
        if ((Air_Reservoirs_Pressure_1< (uint8)((uint16)Air_Pump_Taget_Pressure_Get)*Air_Start_Press_Rate_Get/100) 
            || (Air_Reservoirs_Pressure_2< (uint8)((uint16)Air_Pump_Taget_Pressure_Get)*Air_Start_Press_Rate_Get/100)  )
        {
            //���ٿ���             
            if (Air_Pump_Cmd_Speed<Air_Pump_Speed_Limit_Get)
            {
                Air_Pump_Cmd_Speed_Tmp=Air_Pump_Cmd_Speed+Air_Pump_Speed_Step[0]; 
                Air_Pump_Cmd_Speed_Tmp=(Air_Pump_Cmd_Speed_Tmp>Air_Pump_Speed_Limit_Get) 
                                       ? Air_Pump_Speed_Limit_Get:Air_Pump_Cmd_Speed_Tmp;                 
            }else
            {
                Air_Pump_Cmd_Speed_Tmp=Air_Pump_Speed_Limit_Get;
            }
                    
        }
        else
        {
      
            if (Air_Pump_Cmd_Mode==2)
            {
                Air_Pump_Cmd_Speed_Tmp=Air_Pump_Speed_Limit_Get;
            }
            
        }
        
        //�ﵽ���ת�أ���ת�ض���4N.m       
        if  ( (Air_Reservoirs_Pressure_1> (uint8)Air_Pump_Taget_Pressure_Get/3) 
              && (Air_Reservoirs_Pressure_2> (uint8)Air_Pump_Taget_Pressure_Get/3)
              && (Air_Pressure_Ok==0)&&(Air_Pump_Speed >=Air_Pump_Speed_Limit_Get )&&  ((Air_Pump_Torque<=(Air_Pump_Torque_Limit_Get/2)  ) ||((Air_Pump_Torque_Last-Air_Pump_Torque)>Air_Pump_Torque_DrapDown_Get))   )
            
          
        {
            Air_Pressure_Ok=1;
            Air_Pressure_Delay=0;
            Air_Reservoirs_Over_Pressure_1   =Air_Reservoirs_Pressure_1;
            Air_Reservoirs_Over_Pressure_2   =Air_Reservoirs_Pressure_2;

        }
        //�����������߼�����ת     ��תAir_Pump_Speed_DrapDown_Get      Air_Pump_Torque_Limit_Get
        
        if (
            (Air_Pump_Cmd_Speed >=Air_Pump_Speed_Limit_Get )&&
            ((Air_Pump_Speed_Last-Air_Pump_Speed)>Air_Pump_Speed_DrapDown_Get)
              &&((Air_Reservoirs_Pressure_1 <Air_Pump_Taget_Pressure_Get) ||(Air_Reservoirs_Pressure_2 <Air_Pump_Taget_Pressure_Get))) 
        {
            Air_Pump_Still=1;
            //Air_Pressure_Delay=0;
            Air_Reservoirs_Over_Pressure_1   =Air_Reservoirs_Pressure_1;
            Air_Reservoirs_Over_Pressure_2   =Air_Reservoirs_Pressure_2;       
        }
        
        if ( ((Air_Pressure_Ok==1)||(Air_Pump_Still==1))  &&(Air_Pressure_Delay<Air_Pressure_Delay_Max_Get))
        {
            Air_Pressure_Delay++;
                    
        }
        if (Air_Pressure_Delay>=Air_Pressure_Delay_Max_Get)
        //Air_Pressure_Ok_OverTime�ﵽ����ʱ�� ͣ��
        {
            Air_Pressure_Ok_OverTime=1;     
            Air_Reservoirs_Over_Pressure_1   =Air_Reservoirs_Pressure_1;
            Air_Reservoirs_Over_Pressure_2   =Air_Reservoirs_Pressure_2;

        }
        
        
        
        if (Air_Pump_WorkTime_Index%5 ==0) //ÿ��0.5�����һ��
        {
            Air_Pump_Torque_Last= Air_Pump_Torque;
            Air_Pump_Speed_Last=Air_Pump_Speed;
        }
        
        
        
        
        //�ﵽĿ��ѹ������ 
        if ((Air_Reservoirs_Pressure_1 >Air_Pump_Taget_Pressure_Get) &&(Air_Reservoirs_Pressure_2 >Air_Pump_Taget_Pressure_Get) )
        {
            //���ٿ���
            if (Air_Pump_Cmd_Speed>0)
            {
                Air_Pump_Cmd_Speed_Tmp=Air_Pump_Cmd_Speed-Air_Pump_Speed_Step[1]; 
                Air_Pump_Cmd_Speed_Tmp=(Air_Pump_Cmd_Speed_Tmp>0)
                                       ?Air_Pump_Cmd_Speed_Tmp:0;                  
            }         
        }

        
        //�ٶ�Ϊ0 �ر�����
        if (Air_Pump_Cmd_Speed_Tmp==0) 
        {
            Air_Pump_Cmd_Mode_Tmp   =0;
            Air_Pump_Cmd_Torque_Tmp =0;
        }
              
         
        //uint16 Air_Reservoirs_Pressure_Low_Index=0; 
        //���ù�����ѹ��<0.1MPa ,���� 0.1�벽��    
        //���ù��� ��ѹ����                    20��ػ�����   2018.10.23 ��Ϊ2/3���ٳ�ʱ���������
        if ((Air_Pump_Cmd_Mode_Tmp!=0)&&
            ((Air_Reservoirs_Pressure_1<10)&&(Air_Reservoirs_Pressure_2<10))
            ||((Air_Reservoirs_Pressure_Last_2*3 <Air_Pump_Taget_Pressure_Get*2) &&(Air_Reservoirs_Pressure_Last_1*3 <Air_Pump_Taget_Pressure_Get*2)  &&(Air_Reservoirs_Pressure_Last_1==Air_Reservoirs_Pressure_1)&&(Air_Reservoirs_Pressure_Last_2==Air_Reservoirs_Pressure_2) )
            
            ) 
        
        
        {
            Air_Reservoirs_Pressure_Low_Index++;
            if (Air_Reservoirs_Pressure_Low_Index>=200) 
            {
               // Air_Pump_Cmd_Mode_Tmp   =0;
               // Air_Pump_Cmd_Torque_Tmp =0;
               // Air_Pump_Cmd_Speed_Tmp  =Air_Pump_Speed_Limit_Get*3/2;
               // Air_Pump_WorkTime_Index =200; 
                Air_Reservoirs_Pressure_Low_Index=200;
                
            }
        }else
        {
            Air_Reservoirs_Pressure_Low_Index=0;
        }
        if  (Air_Pump_Cmd_Mode_Tmp!=0)
        {
            Air_Reservoirs_Pressure_Last_1=Air_Reservoirs_Pressure_1;
            Air_Reservoirs_Pressure_Last_2=Air_Reservoirs_Pressure_2;
        }
        
        //���ù�����ʱ
        if (Air_Pump_Cmd_Mode_Tmp!=0) 
        {
            Air_Pump_WorkTime_Index++;
            if (Air_Pump_WorkTime_Index>=Air_Pump_WorkTime_Get) 
            { Air_Pump_Cool=1;            }
        }
        //����ͣ����ʱ
        if ((Air_Pump_Cmd_Mode_Tmp==0)&&(Air_Pump_WorkTime_Index>0))
        {
            //���       
            Air_Pump_WorkTime_Index--;
            Air_Pump_WorkTime_Index--;
            Air_Pump_WorkTime_Index--;
            Air_Pump_WorkTime_Index--;
            Air_Pump_WorkTime_Index--;
            Air_Pump_WorkTime_Index--;
            Air_Pump_WorkTime_Index--;
            Air_Pump_WorkTime_Index--;            
        }      
        if  (Air_Pump_WorkTime_Index<=0)
        {    
            Air_Pump_WorkTime_Index=0;  
            Air_Pump_Cool=0;        
        }                 
               
        
     } 
    else 
    {
        Air_Pump_Cmd_Mode_Tmp   =0;
        Air_Pump_Cmd_Torque_Tmp =0;
        Air_Pump_Cmd_Speed_Tmp  =0;            
        //���ø�λ����  ��Դ���� ����ͨѶ����
        if ((Air_Pump_Fault==1)&&(Power==1)&&(Air_Pump_Communication==0))
        {
            Air_Pump_Fault_Reset_Tmp=1;

            
        }else 
        {
            Air_Pump_Fault_Reset_Tmp=0;
        }
        /*
         Air_Pressure_Ok_OverTime=1;     
         Air_Reservoirs_Over_Pressure_1   =Air_Reservoirs_Pressure_1;
         Air_Reservoirs_Over_Pressure_2   =Air_Reservoirs_Pressure_2;
        */
        //������������������
        if ((Air_Reservoirs_Over_Pressure_2!=0)&&(Air_Reservoirs_Over_Pressure_1!=0)
            &&(Air_Reservoirs_Pressure_1!=0)&&(Air_Reservoirs_Pressure_2!=0)
            &&((Air_Reservoirs_Pressure_1<(uint8) (((uint16) Air_Reservoirs_Over_Pressure_1)*Air_Over_Reset_Get/100 )  )
            ||(Air_Reservoirs_Pressure_2<(uint8) (((uint16) Air_Reservoirs_Over_Pressure_2)*Air_Over_Reset_Get/100 )  ))
            )                                                                                    
            {
                Air_Pressure_Ok_OverTime=0;
                Air_Reservoirs_Over_Pressure_1=0;//(uint8) Air_Pump_Taget_Pressure_Get;
                Air_Reservoirs_Over_Pressure_2=0;//(uint8) Air_Pump_Taget_Pressure_Get;
                Air_Pressure_Delay=0;  
                Air_Pressure_Ok=0;
                Air_Pump_Still=0; 
                    
            }
        
        //����ͣ����ʱ
        if ((Air_Pump_Cmd_Mode_Tmp==0)&&(Air_Pump_WorkTime_Index>0))
        {   
            Air_Pump_WorkTime_Index--;
            Air_Pump_WorkTime_Index--;
            Air_Pump_WorkTime_Index--;
            Air_Pump_WorkTime_Index--;
            Air_Pump_WorkTime_Index--;
            Air_Pump_WorkTime_Index--;
            Air_Pump_WorkTime_Index--;
            Air_Pump_WorkTime_Index--;            
            
        }
        if  (Air_Pump_WorkTime_Index<=0)
        {    
            Air_Pump_WorkTime_Index=0;  
            Air_Pump_Cool=0;        
        }                
    }
    
    if (Air_Pump_Debug==1)
    {
        Air_Pump_Cmd_Mode   =Air_Pump_Cmd_Mode_Debug;
        Air_Pump_Fault_Reset=Air_Pump_Fault_Reset_Debug;
        Air_Pump_Cmd_Torque =Air_Pump_Cmd_Torque_Debug;
        Air_Pump_Cmd_Speed  =Air_Pump_Cmd_Speed_Debug;
        
    }else 
    {   //  �Ǳ�ͨѶ���ϻ�ʱ�����ѹ�����ý��������������ģʽ��������ѹ���������ƣ�   
        if ((LostINSCommunication==1)||(Air_Reservoirs_Pressure_Low_Index>=200))
        {
            Air_Pump_Cmd_Mode   =2;
            Air_Pump_Fault_Reset=0;
            Air_Pump_Cmd_Torque =Air_Pump_Torque_Limit_Get;
            Air_Pump_Cmd_Speed  =Air_Pump_Speed_Limit_Get*2/3;  
                        
        }else
        {
            Air_Pump_Cmd_Mode   =Air_Pump_Cmd_Mode_Tmp;
            Air_Pump_Fault_Reset=Air_Pump_Fault_Reset_Tmp;
            Air_Pump_Cmd_Torque =Air_Pump_Cmd_Torque_Tmp;
            Air_Pump_Cmd_Speed  =Air_Pump_Cmd_Speed_Tmp;         
        }
 
        
          
    }
    
    if (Air_Pump_Cmd_Mode!=0){ Air_Pump_Fan_Delay_Index=Air_Pump_Fan_Delay_Get; }
    
   //���ȿ���
    if ((Air_Pump_Cmd_Mode!=0)||(Air_Pump_OverHeated==1)||(Air_Pump_Fan_Delay_Index>0))
    {
        Air_Pump_Fan=1;
        if (Air_Pump_Cmd_Mode==0)
        {   
            Air_Pump_Fan_Delay_Index--;           
        }
    }else 
    {
        Air_Pump_Fan=0;
    }
    
}












