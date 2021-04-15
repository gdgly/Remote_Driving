/*******************************************************************************
* �������ƣ����������Ƴ���
* �ļ����� Vin_APU.c
* �������� �� 
* ���ü�¼��
* ��ע�� 
* �޶���¼��
*		����		      �汾		  ����		����
*   
*******************************************************************************/
#include <string.h>
#include "GlobalDef.h"
#include "Vin_APU.h"

    
#ifndef AECS_HW
       #pragma DATA_SEG DEFAULT
#else  
      #pragma DATA_SEG S12X_RAM
#endif





 //����������ָ��,ů�翪������,��е�յ���������,
extern uint8  APU_CMD,APU_Warm_CMD,Mechanical_AC_CMD;
 //������Ŀ�귢�繦������
extern uint16 APU_Demand_Power; 
 //Ŀ�깤��ת��  
extern uint16 APU_Demand_Speed;

 //APU��ʼ��״̬,OBDָʾ��,��е�յ�״̬,������������־λ,����������ѹ������,�������ŵ���ɱ�־λ,��������ǰ״̬,���������ϵȼ�      
extern uint8  APU_Staus,APU_OBD_LED,Mechanical_AC,APU_Start_Staus,Engine_Oil_Pressure_Alarm,APU_Discharged_Flag,APU_Mode,APU_Fault_Status;
 //������ת��,���������ת��
extern uint16 APU_Speed,APU_Torque;
extern sint16 APU_Power;//�������������
 //������ĸ�ߵ�ѹ����������ǰת�������
extern uint16 APU_DC_Voltage,APU_Power_Limit;
 //������ĸ�ߵ�������������������
extern sint16 APU_DC_Current,APU_AC_Current;

 //    ����������ѹ��,        ����������Һλ,  ������˲ʱȼ��������, ������λ�ÿ���ʵ��ֵ
extern uint16 Engine_Inlet_Pressure, Engine_Oil_Level,Fuel_Consumption_Rate,Engine_Throttle_Rate;
 //    �����������¶�,          ��������ȴҺ�¶�,          �������¶�,     ������������GCU�¶� 
extern sint16 Engine_Inlet_Temperature,Engine_Coolant_Temperature,APU_Temperature,GCU_Temperature;
 //   ������������              ������������
extern uint8 Engine_Fault_Word[8], APU_Fault_Word[8];
 //   APU�����ź�
extern uint8 APU_Life;


 
#ifndef AECS_HW
      #pragma CODE_SEG DEFAULT
#else  
      #pragma CODE_SEG VCU_CODE
      
#endif

//���������Ƴ���
void APU_Control() 
{
    if (APU_DebugMode==1)    
    {  
        APU_Debug();    
    }else 
    {
        APU_CMD=0;    
        APU_Warm_CMD=0;
        Mechanical_AC_CMD=0;
        APU_Demand_Power=0;
        APU_Demand_Speed=0;
        
    }

}
//���������Գ���
void APU_Debug() 
{
    APU_CMD=APU_Debug_CMD;
    APU_Demand_Power=APU_Debug_Demand_Power;
    APU_Demand_Speed=APU_Debug_Demand_Speed;

}





































