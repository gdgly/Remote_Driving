/*******************************************************************************
* �������ƣ���������
* �ļ����� VehCo_Type.c
* �������� ���������Ͷ���
* ���ü�¼��
* ��ע�� 
* �޶���¼��
*		����		      �汾		  ����		����
*   
*******************************************************************************/

#include "GlobalDef.h"
#include "TypeDef.h"

//#include "VehCo.h"
//#include "Custom_Func.h"
//#include "Vehco_Logic.h"
//#include "VehCo_Type.h"


#ifndef AECS_HW
      #pragma DATA_SEG DEFAULT
#else  
      #pragma DATA_SEG S12X_RAM
#endif

/*
uint16 * __far   Motor_Speed_Torque_Addr;//=(uint16 * __far)&Motor_Speed_Torque; //ת��_���ת��
uint16 * __far   Speed_Vmh10ms_Crawl_D_Addr;//=(uint16 * __far)&Speed_Vmh10ms_Crawl_D; //D���䶯 ת��-AD����
uint16 * __far   Torque_Increment_Crawl_D_Addr;//=(uint16 * __far)&Torque_Increment_Crawl_D;  //D���䶯 AD����ת�ظ���ÿ��������
uint16 * __far   Speed_Max_Torque_Crawl_D_Addr;//=(uint16 * __far)&Speed_Max_Torque_Crawl_D;  //D������ģʽ�������
uint16 * __far   Speed_Vmh10ms_General_Addr;//=(uint16 * __far)&Speed_Vmh10ms_General;        //ͨ�����ת��-AD���� 
uint16 * __far   Torque_Increment_General_Addr;//=(uint16 * __far)&Torque_Increment_General;  //ͨ��AD����ת�ظ���ÿ��������
uint16 * __far   Speed_Vmh10ms_D_Park_Addr;//=(uint16 * __far)&Speed_Vmh10ms_D_Park;          //D��פ�� ת��-AD����
uint16 * __far   Torque_Increment_D_Park_Addr;//=(uint16 * __far)&Torque_Increment_D_Park;    //D��פ��AD����ת�ظ���ÿ��������
uint16 * __far   Speed_Vmh10ms_Break_Addr;//=(uint16 * __far)&Speed_Vmh10ms_Break;            //�ƶ������ٶ�-AD����
uint16 * __far   Torque_Increment_Break_Addr;//=(uint16 * __far)&Torque_Increment_Break;      //�ƶ�����AD����ת�ظ���ÿ��������
uint16 * __far   Motor_Speed_Torque_Break_Addr;//=(uint16 * __far)&Motor_Speed_Torque_Break;  //�������ƶ����� 40kw 114.4
uint16 * __far   FeedBack_Coast_D_Torque_Addr;//=(uint16 * __far)&FeedBack_Coast_D_Torque;    //D�����л��� �ٶ�-����  ����
uint16 * __far   FeedBack_Break_D_Torque_Addr;//=(uint16 * __far)&FeedBack_Break_D_Torque;    //D���ƶ����� �ٶ�-���� ����
uint16 * __far   Speed_Efficieny_Addr;//=(uint16 * __far)&Speed_Efficieny;                    //����ٶ�-ת��Ч������
uint16 * __far   Speed_Efficieny_FeedBack_Addr;//=(uint16 * __far)&Speed_Efficieny_FeedBack_M03;  //����ٶ�-ת��Ч������ ����
uint16 * __far   Speed_Max_Torque_Crawl_D_Break_Addr;//=(uint16 * __far)&Speed_Max_Torque_Crawl_D_Break;  //D�������ƶ�״̬�������������
uint16 * __far   AutoPark_Motor_Speed_Torque_Addr;//=(uint16 * __far)&AutoPark_Motor_Speed_Torque; //פ���������

uint16 Motor_Vehicle_Speed_Scale_Get;//ת��-���ٱ���
uint16 Demand_Limit_High_Get;//��������
uint16 Demand_Limit_Low_Get;//��������
uint16 Crawl_D_Speed_Max_Get;//������״̬�����䶯���ٶ�  RPM
uint8 Crawl_D_Torque_Delay_Max_Get;//�䶯���¼��  ��λ10ms
uint8 Crawl_D_Torque_Delay_Start_Max_Get; //���䶯���¼��  ��λ10ms
uint8 Speed_Buffer_Number_Get; //10ms�����ٶȲ����������
uint8 Speed_Average_Number_Get; //10msƽ�����ٻ������
uint8 Torque_Buffer_Number_AutoPark_Get;  //10ms����ƽ��ת�ز���������� פ��
uint8 D_Park_Start_Speed_Get;//פ�´���ת��
sint16 Auto_Park_Speed_Acc_Max_Get; //����Dפ���������ת���ۼ�
uint16 Auto_Park_Speed_Times_Max_Get;//����Dפ�������������ʱ�� ��λ10ms

sint16 Auto_Torque_Min_Get=0,Auto_Torque_Max_Get=0; //�Զ�ת����С��ֵ �Զ�ת�����ֵ
uint16 R_Vhms_Average_Ratio_Get,D_Vhms_Average_Ratio_Get;//Vhmsƽ��ֵ�������


sint16 Air_Pump_Speed_DrapDown_Get=0;
sint16 Air_Pump_Torque_DrapDown_Get=0;
uint8 Air_Start_Press_Rate_Get=0;
uint8 Air_Over_Reset_Get=0;
uint16 Air_Pressure_Delay_Max_Get=0;
uint16 Air_Pump_WorkTime_Get=0;
uint16 Air_Pump_Fan_Delay_Get=0;
uint16 Air_Pump_Taget_Pressure_Get=0;
sint16 Air_Pump_Speed_Limit_Get=0;
uint16 Air_Pump_Torque_Limit_Get=0;

sint16 Oil_Pump_Speed_Limit_Get=0;
uint16 Oil_Pump_Torque_Limit_Get=0;

uint16 * __far   Oil_Pump_Speed_VSpeed_Addr=(uint16 * __far)&Oil_Pump_Speed_VSpeed_M12;;



uint8 FanType=0;//�������� 0 ���� 1 ˫��    D07-750
uint8 ACType=0;//�յ����� 0 �� 1 ����

uint8 MaxGears=0;//��ߵ�λ  D15��2  ��������8

uint16 Motor_Vehicle_Speed_Scale_Gears_Out_Get=1; //������������ٱ�




uint16 AV_Ele_Calc_Min=100; //��С�ٹ���ƽ����� 10.0KWh/100Km

extern uint8 Gears_State; //��������ǰ��λ
extern uint8 Shift_Gears; //�����任��״̬
extern uint8  Oil_Pump_Install; //�ͱÿ���������״̬ 0 δװ 1 ��װ
extern uint8  Air_Pump_Install; //���ÿ���������״̬ 
extern uint8  Gears_Install; //�����䰲װ ״̬
extern uint8 Fan_Pwm_Install;
*/

uint16 * __far   AccPedCD_rUW_CUR_Addr;//=(uint16 * __far)&AccPedCD_rUW_CUR;  //���ſ���
uint16 * __far   AccPedCD_rUW_CUR2_Addr;//=(uint16 * __far)&AccPedCD_rUW_CUR2;  //���ſ���2
uint16 * __far   AccPed_Rip_Addr;//=(uint16 * __far)&AccPed_Rip;                 //��������
uint16 * __far   Breakk_rUW_CUR_Addr;//=(uint16 * __far)&Breakk_rUW_CUR;  //�ƶ�̤�����

uint16 * __far   AccPed_Acceleration_Addr;  //���ſ���-���ٶ�
uint16 * __far   Break_Acceleration_Addr;   //�ƶ����-�ƶ�������%




uint16 Motor_Vehicle_Speed_Scale_Get;//ת��-���ٱ���
uint8 Vehco_Type_Test=0;
uint8 AccPedCD_TYPE=1;   //�������� 1 ˫�� 2 ����
uint8 Breakk_Type=2;     //�ƶ�����  0����·������ 1��˫·����������  2����·ģ���� 3��˫·ģ����


#ifndef AECS_HW
  #pragma CODE_SEG DEFAULT
#else  
  #pragma CODE_SEG VCU_CODE 
#endif

void  Vehicle_Single_Variable_Set(void) 
{      //��ֵ����ӳ��    
      //���Ͷ��� 1 ʵ���Ҽ�ʻ�� 2 ���Ƽ�ʻ��
      
    switch(Vehco_Type_Test) 
    {
        
        case 1:
        default:
            Motor_Vehicle_Speed_Scale_Get=Motor_Vehicle_Speed_Scale_T01;
            AccPedCD_TYPE=AccPedCD_TYPE_T01;  //�������� 1 ˫�� 2 ����   
            Breakk_Type=Breakk_Type_T01;      //�ƶ�����  0����·������ 1��˫·����������  2����·ģ���� 3��˫·ģ���� 
            break;  
        case 2:  //2 ���Ƽ�ʻ��
            Motor_Vehicle_Speed_Scale_Get=Motor_Vehicle_Speed_Scale_T02;
            AccPedCD_TYPE=AccPedCD_TYPE_T02;  //�������� 1 ˫�� 2 ����    4 CAN
            Breakk_Type=Breakk_Type_T02;      //�ƶ�����  0����·������ 1��˫·����������  2����·ģ���� 3��˫·ģ����    4:CAN     
            break;        
          
    }
}



void  Vehicle_Type_Set (void) 
{             //���߱�ӳ��
/*uint16  aa;
sint16 bb;
bb=55;*/
    //switch(Vehicle_Type) 
    switch (Vehco_Type_Test)
    {
        
        case 1: //1 ʵ���Ҽ�ʻ��
        default:
            AccPedCD_rUW_CUR_Addr =(uint16 * __far)&AccPedCD_rUW_CUR_T01;   //���ſ���1
            AccPedCD_rUW_CUR2_Addr=(uint16 * __far)&AccPedCD_rUW_CUR2_T01;  //���ſ���2
            AccPed_Rip_Addr       =(uint16 * __far)&AccPed_Rip_T01;         //��������
            Breakk_rUW_CUR_Addr   =(uint16 * __far)&Breakk_rUW_CUR_T01;     //�ƶ�̤�����

            AccPed_Acceleration_Addr       =(uint16 * __far)&AccPed_Acceleration_T01;         //��������
            Break_Acceleration_Addr       =(uint16 * __far)&Break_Acceleration_T01;         //�ƶ�����
 

            
            break;
        case 2:  //2 ���Ƽ�ʻ��
            
            AccPedCD_rUW_CUR_Addr =(uint16 * __far)&AccPedCD_rUW_CUR_T01;   //���ſ���1        //TYPE4����
            AccPedCD_rUW_CUR2_Addr=(uint16 * __far)&AccPedCD_rUW_CUR2_T01;  //���ſ���2        //TYPE4����
            AccPed_Rip_Addr       =(uint16 * __far)&AccPed_Rip_T02;         //��������
            Breakk_rUW_CUR_Addr   =(uint16 * __far)&Breakk_rUW_CUR_T01;     //�ƶ�̤�����     //TYPE4����

            AccPed_Acceleration_Addr       =(uint16 * __far)&AccPed_Acceleration_T02;         //��������
            Break_Acceleration_Addr       =(uint16 * __far)&Break_Acceleration_T02;         //�ƶ�����           
           
           
            break;
                            
    }
}



