/*******************************************************************************
* �������ƣ���������
* �ļ����� CANNet.c
* �������� �� CAN���߹���
* ���ü�¼��
* ��ע�� 
* �޶���¼��
*		����		      �汾		  ����		����
*                                             
*******************************************************************************/


#include "GlobalDef.h"
#include "CANNet.h"
#include "Custom_Func.h"
//#include <string.h>
//#include "stdlib.h"
//#include <stdio.h>
//#include <math.h>
#include "Vin_Rw.h"
#include "CAN_ESK.h"
#include "CAN_APU.h"

#ifndef AECS_HW
      #pragma DATA_SEG DEFAULT
#else  
      #pragma DATA_SEG S12X_RAM
#endif



uint32 CANNet_infoIDCAN0UI;
uint8 CANNet_datMsgCAN0UB;

uint32 CANNet_infoIDCAN1UI;
uint8 CANNet_datMsgCAN1UB;

uint32 CANNet_infoIDCAN2UI;
uint8 CANNet_datMsgCAN2UB;

uint8 CANNet_idxSchedUB;

uint8 CanRec0[8]={0};
uint8 CanRec1[8]={0};

uint8 INS_Life; //�Ǳ������ź� ÿ����+1        BYTE1
uint32 INS_TD; //����ʻ��̣�2KM/bit 0~131072KM     BYTE3-4
uint8 Hand_Brake, Low_12V, Brake_Signa, Gas_Cap, ABRS_Signal, Central_Lock, Emer_Signal, Safe_Belt;
         //��ɲ��12V��ѹ�͡��ƶ�β�����������ſ�����ȫ�����������п�������   ˫������    ��ȫ������BYTE2 bit 1~8
uint8 Brake_Error_INS,Car_Door;//�ƶ������ϡ����ſ� 
uint8 INS_Err,INS_Flash_Err,INS_Bat_Low;
      //�Ǳ���ϡ�FALSH���ϡ�ʱ�ӵ�ص����� BYTE8 bit1~3

uint8 B_State,B_Mode,B_Fault_Status,Balancing_Status,B_Fault_Lockout_Status;
      //���״̬�����ģʽ������״̬����������״̬�����Ϲر�״̬
uint8 SOC,SOH;
      //������Чֵ0~125 ��λ�� %�� ƫ�ƣ� 0 �������ߣ� 0.5   �ڴ���Ϊ0~255
uint16 Pack_Res;
      //������� ������Чֵ0~65.535 ��λ�� Ohm�� ƫ�ƣ� 0 �������ߣ� 0.001 �ڴ���Ϊ0~65535 
uint16 Pack_Capacity;
      //��ص��� ������Чֵ0~125��409.5 �°�12λ�ɰ�8λ�� ��λ�� Ahr�� ƫ�ƣ� 0 �������ߣ� 0.5��0.1�°棩       
uint8 B_Message_Counter;//������� ,ÿ100ms+1, 0~14, 15=����
uint8 B_Version;//BMSͨѶЭ��汾 
      
uint16 B_Voltage,Output_Voltage;            //��ص�ѹ�������ѹ  0~800 ��λ�� V�� ƫ�ƣ� 0 ��   �����ߣ� 0.2 ��10���Ŵ���㣬0��8190 4095*2=8190��
sint16 B_Current;                           //��ص���         -500~500 ��λ�� A�� ƫ�ƣ� -500 �������ߣ� 0.02 (10���Ŵ����-5000��5000����������ʾ�Ե�س�磬��������ʾ��طŵ�
uint16 B_Charge_Current,B_Discharge_Current;//��س䡢�ŵ����   0��500 ��λ�� A�� ƫ�ƣ�0 ��    �����ߣ� 0.02���Ŵ�100������ 0��50000��
sint16 B_Power;                             //��س�Ź���     -325~325 ��λ�� kW��ƫ�ƣ� -325�� �����ߣ� 0.01 (100���Ŵ���㣩�����ʱ�ʾ�Ե�س�磬 �����ʱ�ʾ��طŵ�    
uint8 Ambient_Temperature;                  //��ذ��ڻ����¶�  -50~200 ��λ�� �棬ƫ�ƣ� -50��  �����ߣ� 1  
//uint16 B_Current_test;

uint16 Cell_Voltage_Sum;                        //�����ѹ�ܺ� 0~650 ��λ�� V�� ƫ�ƣ� 0 �������ߣ� 0.01   (100���Ŵ���㣩
uint16 Max_Cell_Voltage,Min_Cell_Voltage;       //��� ��С�����ѹ12bit  0~6.1440 ��λ�� V�� ƫ�ƣ� 0 �������ߣ� 0.0015  (1000���Ŵ���� 4095*15=61,425��
uint8 Max_Cell_Voltage_Num,Min_Cell_Voltage_num;//��� ��С�����ѹ������  0~250

uint16 Ave_Cell_Temp,Max_Cell_Temp,Min_Cell_Temp;//ƽ�� ��� ��С�����¶� -50~154 ��λ�� �棬 ƫ�ƣ� -50 �������ߣ� 0.2��10���Ŵ���� -50.0~154.0 �ڴ�ֵ500Ϊ0.0�棩
uint8 Max_Cell_Temp_Num,Min_Cell_Temp_Num;       //���  ��С�����¶ȶ�Ӧ������ 0~250


uint16 Max_B_Voltage_Limit,Min_B_Voltage_Limit;       //�����СPACK��ѹ����   0~818  ��λ�� V�� ƫ�ƣ� 0 �������ߣ� 0.2      ��10���Ŵ���㣬0��8190 4095*2=8190��
uint16 Max_Cell_Voltage_Limit,Min_Cell_Voltage_Limit;  //�����С�����ѹ����   0~6   ��λ�� V�� ƫ�ƣ� 0 �������ߣ� 0.0015    (1000���Ŵ���� 4095*15=61,425��
uint16 Max_Discharge_Current_Limit,Max_Charge_Current_Limit;//����ŵ��������  0��500 ��λ�� A�� ƫ�ƣ� 0 �������ߣ� 2       ���Ŵ�100������ 0��50000��

uint8 Max_SOC_Limit,Min_SOC_Limit;                     //�����СSOC����      0~125 ��λ�� %��  ƫ�ƣ� 0 �������ߣ� 0.5��2���Ŵ����  �ڴ�ֵ0��255��
uint16 Max_Discharge_Power_Limit,Max_Charge_Power_Limit;//���10s��ŵ繦������  0~250 ��λ�� kW�� ƫ�ƣ� 0 �������ߣ� 1
uint16 Max_Cell_Temp_Limit,Min_Cell_Temp_Limit;        //��ߡ���͵����¶����� -50~200 ��λ�� �棬 ƫ�ƣ� -50 �������ߣ� 1 ��10���Ŵ����  -50.0��200.0 �ڴ�ֵ500Ϊ0.0�棩

sint16 Motor_Speed,Motor_Torque;//���ת�١����Ť�أ���ת��Ϊ����ǰ��������Ť��Ϊ����ǰ������-15000��15000rpm, -5000��5000Nm, ƫ�ƣ� 0 �������ߣ� 1
uint16 Motor_AC_Current;   //��������� 0��1000 ��λ��A ƫ�ƣ�0  �����ߣ�0.1 ��10���Ŵ���㣬0��1000.0A��
uint8 Precharge_Allow;    //����Ԥ���־λ  0-����Ԥ��   1-����Ԥ��
uint8 FastDischarge_Fbd;  //���ٷŵ�״̬λ  0-�޷ŵ���� 1-�ŵ���
uint8 IGBT_Enable_Fbd;    //IGBT����״̬λ  0-������ 1-ʹ����
uint8 MCU_System_State;   //MCU״̬����     0-��ʼ�� 1-���������� 2-Ԥ�� 3-���������� 4-ת�ٿ��� 5-ת�ؿ��� 6-��ǿ�磨���ٷŵ磩7-������ 8-����
uint8 MCU_Life;           //MCU����  0~15   ÿ����+1

uint8 Motor_Temperature,MCU_Temperature; //����¶ȡ�MCU�¶�  -40��210�CC  ƫ�� -40 ������ 1
sint16 Torque_High_Limit,Torque_Low_Limit; //���ת�������� ������ 1  0��5000  -5000��0 
uint8 MCU_Fault_Level; //���ϼ���  0:�޹��� 1:���汨��(�����Ҫ�����ʴ���)2:����(��ת�����)3:�Ƚ����ع���(��Ҫ�ضϵ��)4:���ع���(��Ҫͣ������)
uint8 MCU_Life2;   //MCU����  0~15   ÿ����+1


uint8  DTC_Code_1,DTC_Code_2; //DTC CODE
sint16 DC_Current;//ֱ������������ֵ�� �� �������״̬Ϊ���� ����״̬Ϊ�� ���ϴ� -1000��1000A ƫ�� -1000 ������ 1 
uint16 DC_Voltage;//ֱ����ѹ  0��1000.0V  ƫ��0 ������0.1
uint8  Motor_Rotation_Count;  //���ת��ѭ������ 0~255
uint8  MCU_Life3;   //MCU����  0~15   ÿ����+1


uint8 Isolation_Life,Isolation_Status,Isolation_Level; 
//��Ե��������� 0��255������״̬ 0����1�����������ϵȼ� 0 ���� ����500ŷķ/V 1 һ������ 100��500ŷķ/V 2 �������� С��100ŷķ/V 
uint16 Isolation_Pos;//������Ե ��λK  ƫ��0  ������1
uint16 Isolation_Neg;//������Ե ��λK  ƫ��0  ������1
uint16 Isolation;//�ܾ�Ե ȡ�����н�Сһ��

//��λ������֡HC_CMD
// ����û������豸��ַ�������豸����������֡ID��֡���ݡ�ʱ������յ��±���
uint8  HC_CMD=0,User_Level,Device_Id=0,Device_Num=0,HC_Data_Id=0,HC_Data[316]=0,Time_Mark=0,GetNewMessage_HC_CMD=0;
uint8  Need_HC_Data=0;//��Ҫ���յ�����֡��
uint8  Rw_Times_Reset=0;//��ˢд������λ
//����ΪCAN����ʹ�ñ���

uint8 INS_Leds_1,INS_Leds_2;//�Ǳ��״̬                     ����2016.5.4 δ���
//�Ǳ�LEDS1 8��bit  BEEP��SOC���¶ȵ͡������ѹ�͡���Ե�͡��������ָʾ�����״ָ̬ʾ
extern uint8 Beep,Batter_Err,Soc_or_Tmp_Low,Low_Cell_Voltage,Low_Isolation,Charge_Connect,Charge_State;
//�Ǳ�LEDS2 8��bit ��չ޹��ϡ�����ģʽ������Ѳ��ģʽ��������ȡ�����ģʽ��ϵͳ���ϡ�Ready��
extern uint8 Err_Vacuum_Tank,Limit_Mode,Constant_Mode,MCU_OverHeat,FeedBack_Mode,System_Fault,Ready,Ready_Led;   



uint8 Veh_Limit_Speed;//Ѳ�������Ƴ���  0.5km/h/bit 0~127.5    ��V1.2�濪ʼ 1km/bit 0~180
uint8 Remaining_Mil;//ʣ����� 0��255km      ��V1.2�濪ʼ 2km/bit 0~510
uint8 AV_Ele;//ƽ�����    0.5kwh/100KM  0~127.5
uint8 Ins_Ele;//˲ʱ���   0.5kwh/100KM  0~127.5

uint16 VCU_Life;//VCU���� 

uint8 Vehicle_Mode,Key_Status,Charging_Driving_Mode,Vehicle_Speed,Pedal_State,Brake_State;
//��������ģʽ��Կ��λ�á���ʻ&���ģʽ�����١������г̡��ƶ��г�

uint8 VCU_Diag_Code_1,VCU_Diag_Code_2,VCU_Diag_Code_3;//����������123


uint8 VCU_BMS_CMD;//����BMS����������
//BMS���̵����������� 0��1;     BMS 3s�ϵ���ʱ0��1;    BMS���ϱ������ 0��1
uint8 BMS_Contractor_Request,BMS_Power_Down_Request,BMS_Fault_Lockout_Request;

uint8 VCU_MCU_CMD;//������MCU����������
//MCUʹ��ָ��0��1;MCU��λָ��0��1;MCU����ģʽ1��2��3
uint8 MCU_Enable,MCU_Fault_Reset,MCU_Control_Mode;
uint8 VCU_MCU_CMD_L4bit;


uint8 MCU_Live_Counter; //ѭ�������� 0~15
uint16 Demand_Limit_High_Positive,Demand_Limit_Low_Negative;//ת��ת��ָ����������12bit 0~4095 Nm / 4*(0~4095) rpm ������һ�ɷ���
sint16 Demand_Torque;//���ظ���ֵ  -5000��5000   ƫ��  -5000 ����1 ��λNm  ��Ϊǰ������ �ٶ�ģʽʱ��Ч
sint16 Demand_Speed; //�ٶȸ���ֵ  -15000��15000 ƫ�� -15000 ����1 ��λRpm ��Ϊǰ������ ת��ģʽʱ��Ч
//uint16 Torque_DisC_Power_Limit,Torque_Cha_Power_Limit;//10��š��������ת��

uint8 LostMcuCommunication,LostBMSCommunication,LostINSCommunication; 

uint16 LostMcuCom_Index,LostBMSCom_Index,LostINSCom_Index;




uint8  ABS_Active=0;//ABS�����ź�
//A9����ר��
uint8 BCD_Err_Code1=0,BCD_Err_Code2=0; //�Ǳ������  A9����ר��
extern uint16 AccPed,Breakk;
extern  uint8 bDiSatus[24];
extern  uint16 uADSatus[18];
extern  uint16 uAD4Satus[4];
extern  uint8 bLso;
extern  uint8 bHso;
extern  uint8 Error_PowOn_Delay;


extern  uint8 Can_Send_Enable;
extern  uint8 Vehco_Type_Test;
extern  uint8 Dang_L4bit;//��λ״̬   1 5 0 10 PRDN
extern  uint8 Torque_Mode;




    //���ݳ���ѡ�����߱궨ֵ
extern  uint16 * __far AccPedCD_rUW_CUR_Addr;//���ſ���
extern  uint16 * __far AccPedCD_rUW_CUR2_Addr;//���ſ���2
extern  uint16 * __far Motor_Speed_Torque_Addr;//ת��_���ת��
extern  uint16 * __far AccPed_Rip_Addr;//��������
extern  uint16 * __far Speed_Vmh10ms_Crawl_D_Addr;//D���䶯 ת��-AD����
extern  uint16 * __far Torque_Increment_Crawl_D_Addr; //D���䶯 AD����ת�ظ���ÿ��������
extern  uint16 * __far Speed_Max_Torque_Crawl_D_Addr; //D������ģʽ�������
extern  uint16 * __far Speed_Vmh10ms_General_Addr;//ͨ�����ת��-AD���� 
extern  uint16 * __far Torque_Increment_General_Addr;//ͨ��AD����ת�ظ���ÿ��������
extern  uint16 * __far Speed_Vmh10ms_D_Park_Addr;//D��פ�� ת��-AD����
extern  uint16 * __far Torque_Increment_D_Park_Addr;//D��פ��AD����ת�ظ���ÿ��������
extern  uint16 * __far Speed_Vmh10ms_Break_Addr;//�ƶ������ٶ�-AD����
extern  uint16 * __far Torque_Increment_Break_Addr;//�ƶ�����AD����ת�ظ���ÿ��������
extern  uint16 * __far Motor_Speed_Torque_Break_Addr;//�������ƶ����� 40kw 114.4
extern  uint16 * __far FeedBack_Coast_D_Torque_Addr;//D�����л��� �ٶ�-����  ����
extern  uint16 * __far FeedBack_Break_D_Torque_Addr;//D���ƶ����� �ٶ�-���� ����
extern  uint16 * __far Speed_Efficieny_Addr;//����ٶ�-ת��Ч������
     //���ݳ���ѡ��ֵ�궨ֵ
extern  uint8 Motor_Vehicle_Speed_Scale_Get;//ת��-���ٱ���
extern  uint16 Demand_Limit_High_Get;//��������
extern  uint16 Demand_Limit_Low_Get;//��������
extern  uint16 Crawl_D_Speed_Max_Get;//������״̬�����䶯���ٶ�  RPM
extern  uint8 Crawl_D_Torque_Delay_Max_Get;//�䶯���¼��  ��λ10ms
extern  uint8 Speed_Buffer_Number_Get; //10ms�����ٶȲ����������
extern  uint8 Speed_Average_Number_Get; //10msƽ�����ٻ������
extern  uint8 Torque_Buffer_Number_AutoPark_Get;  //10ms����ƽ��ת�ز���������� פ��


    #ifndef AECS_HW
      #pragma CODE_SEG DEFAULT
    #else  
      #pragma CODE_SEG SEG_VDL_CAN
    #endif


//CAN0������Ϣ������
void CANNet_RecvMsgCAN_A(MSCANDrv_infoMSGS_TYPE infoMsgS)
{
   // uint8 Index;
    //CAN��������
    CANNet_infoIDCAN0UI=infoMsgS.infoMsgIDUI;
    CANNet_datMsgCAN0UB=infoMsgS.datMsgUBA[0];
    //CAN0���շ��ദ��
    
    switch(CANNet_infoIDCAN0UI) 
    {

            
            
      case 0xC20A0A6: //INS Status 50ms  �Ǳ�״̬֡
            memcpy(CanRec0,infoMsgS.datMsgUBA,sizeof(CanRec0));
            
            
            //LostINSCom_Index
            if (((CanRec0[0]==0) && (INS_Life==255))|| (CanRec0[0]==INS_Life+1) ) //��������
            {
                LostINSCom_Index=0;
            }
            
            
            
            INS_Life=CanRec0[0];
            INS_TD=(CanRec0[2] + ((uint32)CanRec0[3])*256 + ((uint32)CanRec0[4])*65536 )    *2;
            
          //uint8 Hand_Brake, Low_12V, Brake_Signa, Gas_Cap, ABRS_Signal, Central_Lock, Emer_Signal, Safe_Belt;
          //��ɲ��12V��ѹ�͡��ƶ�β�����������ſ�����ȫ�����������п�������   ˫������    ��ȫ������BYTE2 bit 1~8
          //uint8 Brake_Error_INS,Car_Door;//�ƶ������ϡ����ſ�
          
            Hand_Brake=1&CanRec0[1];
            Low_12V= (2&CanRec0[1])>>1;
            //Brake_Signa=(4&CanRec0[1])>>2;   //�°治��
            //Gas_Cap=(8&CanRec0[1])>>3;       //�°治��

            Brake_Error_INS=(4&CanRec0[1])>>2; 
            Car_Door=(8&CanRec0[1])>>3;
            
            ABRS_Signal=(16&CanRec0[1])>>4;
            Central_Lock=(32&CanRec0[1])>>5;
            Emer_Signal=(64&CanRec0[1])>>6;
            Safe_Belt=(128&CanRec0[1])>>7;
          //INS_Err,INS_Flash_Err,INS_Bat_Low;
          //�Ǳ���ϡ�FALSH���ϡ�ʱ�ӵ�ص����� BYTE8 bit1~3
            INS_Err=1&CanRec0[7];
            INS_Flash_Err= (2&CanRec0[7])>>1;
            INS_Bat_Low=(4&CanRec0[1])>>2;
            break;
      case 0x18ffa1f3: //BCU Status 20ms   BMS״̬֡
            memcpy(CanRec0,infoMsgS.datMsgUBA,sizeof(CanRec0));
          //uint8 B_State,B_Mode,B_Fault_Status,Balancing_Status,B_Fault_Lockout_Status;
          //���״̬�����ģʽ������״̬����������״̬�����Ϲر�״̬
            B_State=0x0F&CanRec0[0];
            B_Mode=CanRec0[0]>>4;
            B_Fault_Status=0x0F&CanRec0[1];
            Balancing_Status=(0x30&CanRec0[1])>>4;
            B_Fault_Lockout_Status=(0xC0&CanRec0[1])>>6;
            SOC=CanRec0[2];
            SOH=CanRec0[3];
            if (B_Version<=10) //�ɰ�BMSЭ�� 
            {
                Pack_Capacity=CanRec0[4];  
                Pack_Res=CanRec0[5]+((uint16)CanRec0[6])*256;
                //Isolation=25000;
                

            }
            else  //�°�BMSЭ��
            {
                Pack_Res=CanRec0[4]+((uint16)CanRec0[5])*256;
                Pack_Capacity=CanRec0[6]+((uint16)(0x0F&CanRec0[7]))*256;
            }
            
            

            LostBMSCom_Index=0;
     
            
            B_Message_Counter= CanRec0[7]>>4;
            
            
            
            
            break;
      case 0x18ffa2f3: //BCU Pack Status 20ms  PACK״̬֡
            memcpy(CanRec0,infoMsgS.datMsgUBA,sizeof(CanRec0));
            B_Voltage= (CanRec0[0]+((uint16)(0x0F&CanRec0[1]))*256)*2;    //0~819.0V  
            Output_Voltage=((CanRec0[1]>>4)+((uint16)CanRec0[2])*16)*2;   //0~819.0V

                        
            //B_Current=(CanRec0[3]+CanRec0[4]*256)/5-5000; //�Ŵ�10������ -500.0��500.0 A             
            B_Current= (sint16)  (((sint32)CanRec0[3]+(sint32)CanRec0[4]*256)     /5-5000); //�Ŵ�10������ -500.0��500.0 A             
            if ((B_Current<=-5000)||(B_Current>=5000)){B_Current=0;} //������ֵ����
            
            if (B_Current<=0) //�ŵ���� 0��500.0A 
            {
                B_Discharge_Current=-1*B_Current;
                B_Charge_Current=0;
            } 
            else //������ 0��500.0A 
            {
                B_Charge_Current=B_Current;
                B_Discharge_Current=0;
            }

            //B_Power=(sint16)((CanRec0[5]+((uint16)CanRec0[6])*256)-32768);
            //B_Power=CanRec0[5]+CanRec0[6]*256;
            //B_Power=(CanRec0[5]+CanRec0[6]*256)-32500; //�Ŵ�100������  -325.00��325.00KW
           
            B_Power= (sint16)  (((sint32)CanRec0[5]+(sint32)CanRec0[6]*256)-32500); //�Ŵ�100������  -325.00��325.00KW            
              
            if ((B_Power<=-32500)||(B_Power>=32500)){B_Power=0;} //�ŵ繦�ʼ�ֵ����
            Ambient_Temperature=CanRec0[7];
            break;
      case 0x18ffa4f3: //�����ѹ����״̬ Cell Voltage Status 100ms
            memcpy(CanRec0,infoMsgS.datMsgUBA,sizeof(CanRec0));
            Cell_Voltage_Sum=CanRec0[0]+((uint16)CanRec0[1])*256;             //0~655.35V
            Max_Cell_Voltage=(CanRec0[2]+((uint16)(0x0f&CanRec0[3]))*256)*15; //0~6.1425V
            Min_Cell_Voltage=((CanRec0[3]>>4)+((uint16)CanRec0[4])*16)*15;
            Max_Cell_Voltage_Num=CanRec0[5];
            Min_Cell_Voltage_num=CanRec0[6];
            break;
      case 0x18ffa5f3: //�����¶ȼ���״̬ Cell Temperature Status 100ms
            memcpy(CanRec0,infoMsgS.datMsgUBA,sizeof(CanRec0));
            Ave_Cell_Temp=(CanRec0[0]+((uint16)CanRec0[1])*256)*2;       //-50.0~154.0�� 
            Max_Cell_Temp=(CanRec0[2]+((uint16)0x0f&CanRec0[3])*256)*2;  //-50.0~154.0�� 
            Min_Cell_Temp=((CanRec0[3]>>4)+((uint16)CanRec0[4])*16)*2;   //-50.0~154.0�� 
            Max_Cell_Temp_Num=CanRec0[5];
            Max_Cell_Temp_Num=CanRec0[6];
            break;
      case 0x18ffa6f3: //BMS ����״̬ 1 BMS Limits Status 1  100ms
            memcpy(CanRec0,infoMsgS.datMsgUBA,sizeof(CanRec0));
            Max_B_Voltage_Limit=(CanRec0[0]+(uint16)(0x0f&CanRec0[1])*256)*2;     //0~819.0V
            Min_B_Voltage_Limit=((CanRec0[1]>>4)+((uint16)CanRec0[2])*16)*2;      //0~819.0V
            Max_Cell_Voltage_Limit=(CanRec0[3]+(uint16)(0x0f&CanRec0[4])*256)*15; //0~6.1425V
            Min_Cell_Voltage_Limit=((CanRec0[4]>>4)+((uint16)CanRec0[5])*16)*15;  //0~6.1425V
            Max_Discharge_Current_Limit=((uint16)CanRec0[6])*200;                 //0~500.00A  �����ŵ��������
            Max_Charge_Current_Limit=((uint16)CanRec0[7])*200;                    //0~500.00A  �������������� 
            break;
      case 0x18ffa7f3: //BMS ����״̬ 2 BMS Limits Status 2  100ms
            memcpy(CanRec0,infoMsgS.datMsgUBA,sizeof(CanRec0));
            Max_SOC_Limit=CanRec0[0];                       
            Min_SOC_Limit=CanRec0[1];
            Max_Discharge_Power_Limit=CanRec0[2]*100;       //0~250.00KW  �ŵ�10�����ƹ���
            Max_Charge_Power_Limit=CanRec0[3]*100;          //0~250.00KW  ���/����10�����ƹ���
            Max_Cell_Temp_Limit=((uint16)CanRec0[4])*10;//-50.0~154.0�� 
            Min_Cell_Temp_Limit=((uint16)CanRec0[5])*10;//-50.0~154.0�� 
            
            
            break;
      case 0x18FFA8F3: //������Ե���� Battery Isolation Resistance   250ms  1.2����
            memcpy(CanRec0,infoMsgS.datMsgUBA,sizeof(CanRec0));
            Isolation_Life=CanRec0[0];
            Isolation_Status=CanRec0[1];
            Isolation_Level=CanRec0[2];
            Isolation_Pos=CanRec0[4]+CanRec0[5]*256;
            Isolation_Neg=CanRec0[6]+CanRec0[7]*256;
            if (Isolation_Pos >  Isolation_Neg) 
            {
                  Isolation=Isolation_Neg;
            
            } 
            else 
            {
                  Isolation=Isolation_Pos;
            }
      
            if (Isolation>25000) {Isolation=25000;}
      
            break;
      case 0x18FFA9F3: //���弫���¶ȼ��� 1 Cell Post Temperature Limits 1  1000ms  1.2����
      
            break;
      case 0x18FFAAF3: //���弫���¶ȼ��� 2 Cell Post Temperature Limits 2  1000ms  1.2����
      
            break;
      case 0x18FFABF3: //���弫���¶ȼ��� 3 Cell Post Temperature Limits 3  1000ms  1.2����
      
            break;
      case 0x18FFACF3: //��ػ�����Ϣ Battery Information  1.2���� 
            memcpy(CanRec0,infoMsgS.datMsgUBA,sizeof(CanRec0));
            B_Version=CanRec0[7];
      
            break;
      case 0x18fecaf3: //BCU ��ϱ��� BCU DM1  100ms
       
            break;
      case 0xCFF0008:  //ISG MCU STATUS 1   10ms
            memcpy(CanRec0,infoMsgS.datMsgUBA,sizeof(CanRec0));
            //Motor_Speed=(CanRec0[0]+CanRec0[1]*256)-15000;       //-15000~15000 rpm
            Motor_Speed=(sint16)  (((sint32)CanRec0[0]+(sint32)CanRec0[1]*256)  -15000);       //-15000~15000 rpm
                        
            if ((Motor_Speed<=-15000)||(Motor_Speed>=15000)) { Motor_Speed=0;} //���ת�ټ�ֵ����
            //Motor_Torque=(CanRec0[2]+CanRec0[3]*256)-5000;      //-5000-5000 Nm
            Motor_Torque=(sint16) (((sint32)CanRec0[2]+(sint32)CanRec0[3]*256)-5000);      //-5000-5000 Nm
            
            if ((Motor_Torque<=-5000)||(Motor_Torque>=5000)) {Motor_Torque=0;}  //���ת�ؼ�ֵ����
            Motor_AC_Current=CanRec0[4]+CanRec0[5]*256;   //0~1000.0A
            Precharge_Allow=(16&CanRec0[6])>>4;
            FastDischarge_Fbd=(32&CanRec0[6])>>5;
            IGBT_Enable_Fbd=(64&CanRec0[6])>>6;
            MCU_System_State=0x0f&CanRec0[7];
            
            if ((((0xf0&CanRec0[7])>>4==0) && (MCU_Life==15))|| ((0xf0&CanRec0[7])>>4==MCU_Life+1) ) //��������
            {
                LostMcuCom_Index=0;
            }
            
            
            MCU_Life=(0xf0&CanRec0[7])>>4;
            
            
            
            break;
      case 0xCFF0108:  //ISG MCU STATUS 2   20ms
            memcpy(CanRec0,infoMsgS.datMsgUBA,sizeof(CanRec0));
            Motor_Temperature=CanRec0[0];                //-40��210�� 
            MCU_Temperature=CanRec0[1];                  //-40��210�� 
            Torque_High_Limit=CanRec0[2]+CanRec0[3]*256; // 0~5000Nm
            //Torque_Low_Limit=(CanRec0[4]+CanRec0[5]*256)-5000;  //-5000~0Nm
            Torque_Low_Limit=(sint16) (((sint32)CanRec0[4]+(sint32)CanRec0[5]*256)-5000);  //-5000~0Nm
            
            if (Torque_Low_Limit<=-5000){Torque_Low_Limit=0;}    //ת�ظ����Ƽ�ֵ����
            MCU_Fault_Level=0x0f&CanRec0[6];
            MCU_Life2=(0xf0&CanRec0[7])>>4;
            break;
      case 0xCFF0208:  //ISG MCU STATUS 3   10/20ms
            memcpy(CanRec0,infoMsgS.datMsgUBA,sizeof(CanRec0));      
            DTC_Code_1=CanRec0[0];
            DTC_Code_2=CanRec0[1];            
            //DC_Current=(CanRec0[2]+CanRec0[3]*256)-1000;   //-1000��1000A
            DC_Current=(sint16)  (((sint32)CanRec0[2]+(sint32)CanRec0[3]*256)-1000);   //-1000��1000A
            
            
            if ((DC_Current<=-1000)||(DC_Current>=1000)){DC_Current=0;} //MCU ĸ�ߵ�����ֵ����
            DC_Voltage=CanRec0[4]+CanRec0[5]*256;   //0��1000.0V
            Motor_Rotation_Count=CanRec0[6];   //���ת������ 
            MCU_Life3=(0xf0&CanRec0[7])>>4;
            break;
      case 0xCFF0308:  //ISG MCU STATUS 4   50ms
       
            break;
      case 0x18F0010B:
            //ABS_Active
            memcpy(CanRec0,infoMsgS.datMsgUBA,sizeof(CanRec0));      
            //ABS_Active=(0x20&CanRec0[0])>>5; //bit 5     10 0000
            ABS_Active=(0x30&CanRec0[0])>>4; //bit 5&6     11 0000
            break;
      case 0x0C64A001:  //HC_CMD ��λ������֡ ����Ӧ����
            
            memcpy(CanRec0,infoMsgS.datMsgUBA,sizeof(CanRec0)); 
            // ����û������豸��ַ�������豸����������֡ID��֡���ݡ�ʱ������յ��±���
            //uint8  HC_CMD,User_Level,Device_Id,Device_Num,HC_Data_Id,HC_Data[316],Time_Mark,GetNewMessage_HC_CMD;            
            GetNewMessage_HC_CMD=1;
            switch(CanRec0[0]) 
            {
                case 0xA0:  //ע��֡
                    HC_CMD=     CanRec0[0] ;
                    User_Level= CanRec0[1] ;
                    Rw_Times_Reset=CanRec0[2];
                    break;
                case 0xA3:  //��ȡ֡
                    HC_CMD=     CanRec0[0] ;
                    Device_Id=  CanRec0[1] ;
                    Device_Num= CanRec0[2] ;
                    Time_Mark=  CanRec0[3] ;
                    
                    break;
                case 0xA1:  //����֡
                    HC_CMD=     CanRec0[0] ;
                    Time_Mark=  CanRec0[1] ;
                    break;
                case 0xA6:  //д��֡
                    HC_CMD=     CanRec0[0] ;
                    Device_Id=  CanRec0[1] ;
                    Device_Num= CanRec0[2] ;
                    Time_Mark=  CanRec0[3] ;               
                    
                    break;
                case 0xAF:  //ע��֡
                    HC_CMD=     CanRec0[0] ;
                    break;
                default:    //����֡  0x01~2D
                    if ((CanRec0[0]>0x00)&&(CanRec0[0]<0x2D) && (Need_HC_Data>0))
                    { //��Ч����֡
                        HC_CMD=0xFF;
                        HC_Data_Id=CanRec0[0];
                        CANNet_Get_HC_Data (CanRec0);
                    }
                    
                    
                    break;
            }
            
            
            
            
            
            
            
            break;
      default:
            break;
    }
    //�����ŵ�������Ϣ������
    if (ESK_Signal_Enable==1) {  CANNet_RecvMsgCAN_ESK (infoMsgS);}
    
}

//CAN1������Ϣ������1
void CANNet_RecvMsgCAN_B(MSCANDrv_infoMSGS_TYPE infoMsgS)
{
    //CAN��������
    CANNet_infoIDCAN1UI=infoMsgS.infoMsgIDUI;
    CANNet_datMsgCAN1UB=infoMsgS.datMsgUBA[0];
    
    //������������Ϣ������
    if (APU_Enable==1) {CANNet_RecvMsgCAN_APU(infoMsgS) ;    }
}

//CAN2������Ϣ������2
void CANNet_RecvMsgCAN_C(MSCANDrv_infoMSGS_TYPE infoMsgS)
{
    CANNet_infoIDCAN2UI=infoMsgS.infoMsgIDUI;
    CANNet_datMsgCAN2UB=infoMsgS.datMsgUBA[0];
}

//������Ϣ����
void CANNet_Manage1ms(void)
{
    uint8 VCU_BMS_CMD_Bit[8]={0},VCU_MCU_CMD_Bit[8]={0},INS_Leds_2_Bit[8]={0},INS_Leds_1_Bit[8]={0};
    //  VehCo_stNewUB ΪRunning �� Afterrun״̬ʱ������CAN����
    if ((Can_Send_Enable==1)&&((VehCo_stNewUB==_VEHCO_INDEX_STATUS_RUNNING)||(VehCo_stNewUB==_VEHCO_INDEX_STATUS_AFTERRUN)) )
    {
        
        
        MSCANDrv_infoMSGS_TYPE infoMsgS;
        switch(CANNet_idxSchedUB)
        {
             case 0:
             case 50:
                //CAN0  ��������֡ VCU_C 50ms  ������֡   ***���
                infoMsgS.infoMsgIDUI=0x08F000A0;
                infoMsgS.bExtIDUB=1;
                infoMsgS.datMsgUBA[0]=VCU_C_1[0];
                infoMsgS.datMsgUBA[1]=VCU_C_1[1];
                infoMsgS.datMsgUBA[2]=VCU_C_1[2];
                infoMsgS.datMsgUBA[3]=VCU_C_1[3];
                infoMsgS.datMsgUBA[4]=VCU_C_1[4];

                //VCUӲ������  01 ���ݰ��׿�˹ VC04A   02 �����׿�  EC36003CU001
                #ifndef AECS_HW
                    infoMsgS.datMsgUBA[5]=2;
                #else  
                    infoMsgS.datMsgUBA[5]=1; 
                #endif


                infoMsgS.datMsgUBA[6]=VCU_C_1[6];
                infoMsgS.datMsgUBA[7]=(uint8)(VCU_Life);       //VCU����
                infoMsgS.numLenUB=8;
                infoMsgS.idxMsgPriorityUB=0x01;
                MSCANDrv_SendMsgCAN_A(infoMsgS);
                VCU_Life=(uint8)((VCU_Life+1)%256); //0��255 
                break;
                
             case 1:
             case 51:
                //CAN0  ����״̬֡ VCU_S 50ms  ���٣� ����ģʽ
                //Motor_Speed -15000~15000 sint16
                //Vehicle_Speed -50��205��150��km/h
                //Motor_Vehicle_Speed_Scale_Get  //ת��-���ٱ����� ����: 114.411  D11: 36.632  ���ٹ������1��ʱ��Ӧ��10msת�����������ٶȣ�

                
                
                Vehicle_Speed=( abs(Motor_Speed) /Motor_Vehicle_Speed_Scale_Get)+50;
                
                
                //AccPed,Breakk;
                Pedal_State=(uint8)(AccPed/100);
                Brake_State=(uint8)(Breakk/100);
                
                infoMsgS.infoMsgIDUI=0x08F200A0;
                infoMsgS.bExtIDUB=1;
                infoMsgS.datMsgUBA[0]=Vehicle_Mode;         //��������ģʽ  0 1 7  **δ�����״̬
                infoMsgS.datMsgUBA[1]=Key_Status;           //Կ��λ�� ��λ
                infoMsgS.datMsgUBA[2]=Charging_Driving_Mode;//��ʻ&���ģʽ   **δ���
                infoMsgS.datMsgUBA[3]=0;                  
                infoMsgS.datMsgUBA[4]=0;                   
                infoMsgS.datMsgUBA[5]=Vehicle_Speed;        //����       1km/h /bit -50��205��180��km/h
                if (infoMsgS.datMsgUBA[5]==0) {infoMsgS.datMsgUBA[5]=50;} //����-50��ֵ����
                infoMsgS.datMsgUBA[6]=Pedal_State;          //�����г�   1%/bit  0~100%
                infoMsgS.datMsgUBA[7]=Brake_State;          //�ƶ��г�   1%/bit  0~100%
                infoMsgS.numLenUB=8;
                infoMsgS.idxMsgPriorityUB=0x02;
                MSCANDrv_SendMsgCAN_A(infoMsgS);
                
                break;



             case 2:
             case 52:
                //CAN0  ��������֡ VCU_F 50ms    ����������Ϣ
                
                
                

                
                infoMsgS.infoMsgIDUI=0x041000A0;
                infoMsgS.bExtIDUB=1;
                if (Error_PowOn_Delay>=40)
                {
                  infoMsgS.datMsgUBA[0]=VCU_Diag_Code_1;   //����������1
                  infoMsgS.datMsgUBA[1]=VCU_Diag_Code_2;   //����������2
                  infoMsgS.datMsgUBA[2]=VCU_Diag_Code_3;   //����������3
                }else
                {
                  infoMsgS.datMsgUBA[0]=0;   //����������1
                  infoMsgS.datMsgUBA[1]=0;   //����������2
                  infoMsgS.datMsgUBA[2]=0;   //����������3
                }
                
                infoMsgS.datMsgUBA[3]=0;
                infoMsgS.datMsgUBA[4]=0;
                infoMsgS.datMsgUBA[5]=0;//Warning Code1
                infoMsgS.datMsgUBA[6]=0;//Warning Code2
                infoMsgS.datMsgUBA[7]=0;//Warning Code3
                infoMsgS.numLenUB=8;
                infoMsgS.idxMsgPriorityUB=0x03;
                
               
               
                MSCANDrv_SendMsgCAN_A(infoMsgS);
             
                break;    
               
             case 3:
             case 53:
                //CAN0  �����Ǳ����֡1 VCUtoINS_C1 50ms   �Ǳ���Ƶ���Ϣ�� �������    
                //�Ǳ�LEDS1 8��bit  BEEP��SOC���¶ȵ͡������ѹ�͡���Ե�͡��������ָʾ�����״ָ̬ʾ
                //uint8 Beep,Batter_Err,Soc_or_Tmp_Low,Low_Cell_Voltage,Low_Isolation,Charge_Connect,Charge_State;
                INS_Leds_1_Bit[7]=Beep;
                INS_Leds_1_Bit[6]=Batter_Err;
                INS_Leds_1_Bit[5]=Soc_or_Tmp_Low;
                INS_Leds_1_Bit[4]=Low_Cell_Voltage;
                INS_Leds_1_Bit[3]=Low_Isolation;
                INS_Leds_1_Bit[2]=Charge_Connect;
                INS_Leds_1_Bit[1]=(Charge_State&0x2)>>1;
                INS_Leds_1_Bit[0]=Charge_State&0x1;            
                
                
                
                INS_Leds_1=SetBit8 (INS_Leds_1_Bit);
                infoMsgS.infoMsgIDUI=0x1810A6A0;
                infoMsgS.bExtIDUB=1;
                if (VehCo_stNewUB==_VEHCO_INDEX_STATUS_RUNNING) 
                {
                    infoMsgS.datMsgUBA[0]=(uint8)(SOC/2);   //SOC 1%/bit 
                } else
                {
                    infoMsgS.datMsgUBA[0]=0;
                }
                
                infoMsgS.datMsgUBA[1]=(uint8)((B_Current+4000)&0x00ff);// ��طŵ���� 0.1A/bit  ƫ��-400A ��Χ-400.0~1200.0A
                infoMsgS.datMsgUBA[2]=(uint8)((B_Current+4000)>>8);    //��8
                infoMsgS.datMsgUBA[3]=(uint8)(B_Voltage&0x00ff);  //����ܵ�ѹ 0.1V/bit 
                infoMsgS.datMsgUBA[4]=(uint8)(B_Voltage>>8);      //��8
                if (B_Version<=10) {Isolation=rand()%500+11000;}      //�ɰ�BMSЭ�����������Ե����ֵ11.0��11.5Mohm
                
                infoMsgS.datMsgUBA[5]=(uint8)(Isolation/100);          //������Ե���� 0��25.5M ohm
                infoMsgS.datMsgUBA[6]=(uint8)(Max_Cell_Temp/10);       //��������¶� -50��200 ��
                if (infoMsgS.datMsgUBA[6]==0) {infoMsgS.datMsgUBA[6]=50;}//��������¶�������-50�ȼ�ֵ����
                infoMsgS.datMsgUBA[7]=(uint8)(INS_Leds_1);             //�Ǳ��״̬1
                infoMsgS.numLenUB=8;
                infoMsgS.idxMsgPriorityUB=0x04;
                MSCANDrv_SendMsgCAN_A(infoMsgS);
                
                break;    
               
             case 4:
             case 54:
                //CAN0  �������Ǳ����֡2 VCUtoINS_C2 50ms   �Ǳ���Ƶ���Ϣ�� ���ز���ص�
                //INS_Leds_2_Bit
                //�Ǳ�LEDS2 8��bit ��չ޹��ϡ�����ģʽ������Ѳ��ģʽ��������ȡ�����ģʽ��ϵͳ���ϡ�Ready��
                //uint8 Err_Vacuum_Tank,Limit_Mode,Constant_Mode,MCU_OverHeat,FeedBack_Mode,System_Fault,Ready;  
                INS_Leds_2_Bit[7]=(Torque_Mode==4)||(Torque_Mode==8);
                INS_Leds_2_Bit[6]=Err_Vacuum_Tank;
                INS_Leds_2_Bit[5]=Limit_Mode;
                INS_Leds_2_Bit[4]=Constant_Mode;
                INS_Leds_2_Bit[3]=MCU_OverHeat;
                INS_Leds_2_Bit[2]=FeedBack_Mode;
                INS_Leds_2_Bit[1]=System_Fault;
                INS_Leds_2_Bit[0]=Ready_Led;           
                
                
                
                INS_Leds_2=SetBit8 (INS_Leds_2_Bit);
                infoMsgS.infoMsgIDUI=0x1811A6A0;
                infoMsgS.bExtIDUB=1;
                infoMsgS.datMsgUBA[0]=0;                  
                infoMsgS.datMsgUBA[1]=INS_Leds_2;          //�Ǳ��״̬2
                infoMsgS.datMsgUBA[2]=Veh_Limit_Speed;     //Ѳ�����޶����� 0��127.5km/h  0.5km/h/bit    ��V1.2�濪ʼ 1km/bit 0~180
                infoMsgS.datMsgUBA[3]=Remaining_Mil;       //ʣ����� 0��255km  ��V1.2�濪ʼ 2km/bit 0~510                      **δ���
                infoMsgS.datMsgUBA[4]=AV_Ele;              //ƽ�����    0.5kwh/100KM  0~127.5         **δ���
                infoMsgS.datMsgUBA[5]=Ins_Ele;             //˲ʱ���   0.5kwh/100KM  0~127.5          **δ���
                infoMsgS.datMsgUBA[6]=0;                 
                infoMsgS.datMsgUBA[7]=0;                  
                infoMsgS.numLenUB=8;
                infoMsgS.idxMsgPriorityUB=0x05;
                MSCANDrv_SendMsgCAN_A(infoMsgS);
                
                break;    

             case 5:
             case 25:
             case 45:
             case 65:
             case 85:
                //CAN0  ������BMS����֡ VCUtoBMS_C 20ms
                //VCU_BMS_CMD_Bit[8]
                //BMS_Contractor_Request,BMS_Power_Down_Request,BMS_Fault_Lockout_Request;
                
                VCU_BMS_CMD_Bit[0]=BMS_Contractor_Request;
                VCU_BMS_CMD_Bit[2]=BMS_Power_Down_Request;
                VCU_BMS_CMD_Bit[4]=BMS_Fault_Lockout_Request;
                
                VCU_BMS_CMD=SetBit8 (VCU_BMS_CMD_Bit);
               
                infoMsgS.infoMsgIDUI=0x18FFA0F3;
                infoMsgS.bExtIDUB=1;
                infoMsgS.datMsgUBA[0]=VCU_BMS_CMD;                     //������BMS����������
                infoMsgS.datMsgUBA[1]=(uint8)(DC_Voltage&0x00ff);      //���������ĸ�ߵ�ѹ 0��1000.0V
                infoMsgS.datMsgUBA[2]=(uint8)(DC_Voltage>>8);          //��8
                infoMsgS.datMsgUBA[3]=(uint8)(Motor_AC_Current&0x00ff);//������������ 0��1000.0A
                infoMsgS.datMsgUBA[4]=(uint8)(Motor_AC_Current>>8);    //��8
                infoMsgS.datMsgUBA[5]=0;
                infoMsgS.datMsgUBA[6]=0;
                infoMsgS.datMsgUBA[7]=0;
                infoMsgS.numLenUB=8;
                infoMsgS.idxMsgPriorityUB=0x06;
                MSCANDrv_SendMsgCAN_A(infoMsgS);
                
                break;    

             case 6:
             case 16:
             case 26:
             case 36:
             case 46:
             case 56:
             case 66:
             case 76:
             case 86:
             case 96:
                //CAN0  ������MCU����֡ VCUtoMCU_C 10ms
                //MCUʹ��ָ��0��1;MCU��λָ��0��1;MCU����ģʽ1��2��3
                //uint8 MCU_Enable,MCU_Fault_Reset,MCU_Control_Mode;
                //VCU_MCU_CMD_Bit[8]
                VCU_MCU_CMD_Bit[0]=MCU_Enable;
                VCU_MCU_CMD_Bit[1]=MCU_Fault_Reset;
                VCU_MCU_CMD_Bit[2]=MCU_Control_Mode&0x1;
                VCU_MCU_CMD_Bit[3]=MCU_Control_Mode&0x2;            
                
                VCU_MCU_CMD_Bit[4]=MCU_Live_Counter&0x1;
                VCU_MCU_CMD_Bit[5]=MCU_Live_Counter&0x2;
                VCU_MCU_CMD_Bit[6]=MCU_Live_Counter&0x4;
                VCU_MCU_CMD_Bit[7]=MCU_Live_Counter&0x8;
                
                
                VCU_MCU_CMD=SetBit8 (VCU_MCU_CMD_Bit);
                VCU_MCU_CMD_L4bit=VCU_MCU_CMD&0x0F;
                
                //VCU_MCU_CMD=(VCU_MCU_CMD&0x0F)+((MCU_Live_Counter&0x0F)<<4);
                Demand_Limit_High_Positive=Demand_Limit_High_Get;
                Demand_Limit_Low_Negative=Demand_Limit_Low_Get;
                
                infoMsgS.infoMsgIDUI=0xCFF08EF;
                infoMsgS.bExtIDUB=1;
                infoMsgS.datMsgUBA[0]=VCU_MCU_CMD; //������MCU����������
                infoMsgS.datMsgUBA[1]=(uint8)(Demand_Limit_High_Positive&0x00FF);       //�ٶ�����0~7λ
                infoMsgS.datMsgUBA[2]=(uint8)((Demand_Limit_High_Positive&0x0F00)>>8)+((Demand_Limit_Low_Negative&0x000F)<<4);   //�ٶ�����8~11λ+�ٶ�����0~3λ
                infoMsgS.datMsgUBA[3]=(uint8)((Demand_Limit_Low_Negative&0x0FF0)>>4);   //�ٶ�����4��11λ
                infoMsgS.datMsgUBA[4]=(uint8)((Demand_Torque+5000)&0x00FF);                    //���ظ���ֵ  -5000��5000
                infoMsgS.datMsgUBA[5]=(uint8)((Demand_Torque+5000)>>8);                        //��8λ
                infoMsgS.datMsgUBA[6]=(uint8)((Demand_Speed+15000)&0x00FF);                     //�ٶȸ���ֵ  -15000��15000
                infoMsgS.datMsgUBA[7]=(uint8)((Demand_Speed+15000)>>8);                         //��8λ
                infoMsgS.numLenUB=8;
                infoMsgS.idxMsgPriorityUB=0x07;
                MSCANDrv_SendMsgCAN_A(infoMsgS);
                
                MCU_Live_Counter=(uint8)((MCU_Live_Counter+1)%16); //0��15 
                break;    
            
        //����ΪA9����ר��CANЭ�� 125Kbps CAN1  
        case 7:
        case 57:    //MCS1    ���������״̬1   50ms     0X268     Motor_Speed    MCU_Temperature  Motor_Temperature
            INS_Leds_2_Bit[7]=Ready_Led;
            INS_Leds_2_Bit[6]=0;
            INS_Leds_2_Bit[5]=0;
            INS_Leds_2_Bit[4]=0;
            INS_Leds_2_Bit[3]=0;
            INS_Leds_2_Bit[2]=0;
            INS_Leds_2_Bit[1]=0;
            INS_Leds_2_Bit[0]=0;           
            INS_Leds_2=SetBit8 (INS_Leds_2_Bit);

            infoMsgS.infoMsgIDUI=0X268;
            infoMsgS.bExtIDUB=0;
            //infoMsgS.datMsgUBA[0]=(uint8)( 85*(abs(Motor_Speed/Motor_Vehicle_Speed_Scale_Get)) &0x00FF);  //���ת��
            
            
            infoMsgS.datMsgUBA[0]=(((sint32)abs(Motor_Speed))  *85/Motor_Vehicle_Speed_Scale_Get) &0x00FF; //���ת��
                        
            //infoMsgS.datMsgUBA[1]=(uint8)( 85*(abs(Motor_Speed/Motor_Vehicle_Speed_Scale_Get))     >>8);
          
            infoMsgS.datMsgUBA[1]=((((sint32)abs(Motor_Speed))  *85/Motor_Vehicle_Speed_Scale_Get) >>8) &0x00FF; //���ת��  ��8λ
                     
            infoMsgS.datMsgUBA[2]=(((sint16)MCU_Temperature)-40)*10 &0x00FF ;
            infoMsgS.datMsgUBA[3]=((((sint16)MCU_Temperature)-40)*10 >>8) &0x00FF;
            infoMsgS.datMsgUBA[4]=(((sint16)Motor_Temperature)-40)*10 &0x00FF ;
            infoMsgS.datMsgUBA[5]=((((sint16)Motor_Temperature)-40)*10 >>8) &0x00FF;
            infoMsgS.datMsgUBA[6]=INS_Leds_2;
            infoMsgS.datMsgUBA[7]=(Breakk/100)&&1;
            infoMsgS.numLenUB=8;
            infoMsgS.idxMsgPriorityUB=0x01;
            if ((Vehco_Type_Test==4)||(Vehco_Type_Test==6)) {              MSCANDrv_SendMsgCAN_B(infoMsgS);            }
      
            break;
        case 8:
        case 58:    //MCS2    ���������״̬2   50ms    0X368    B_Voltage   B_Current    Dang_L4bit
            infoMsgS.infoMsgIDUI=0X368;
            infoMsgS.bExtIDUB=0;
            infoMsgS.datMsgUBA[0]=B_Voltage&0x00FF;
            infoMsgS.datMsgUBA[1]=B_Voltage>>8;
            infoMsgS.datMsgUBA[2]=SOC/2;
            infoMsgS.datMsgUBA[3]=AccPed&0x00FF;
            infoMsgS.datMsgUBA[4]=AccPed>>8;
            infoMsgS.datMsgUBA[5]=B_Current&0x00FF;
            infoMsgS.datMsgUBA[6]=(B_Current>>8)&0x00FF;
            switch (Dang_L4bit) 
            {
                case 0:
                default:
                    infoMsgS.datMsgUBA[7]=0;
                    break;
                case 5:
                    infoMsgS.datMsgUBA[7]=2;
                    break;
                case 10:
                    infoMsgS.datMsgUBA[7]=1;
                    break;
                case 1:
                    infoMsgS.datMsgUBA[7]=3;
                    break;
            }
            
            infoMsgS.numLenUB=8;
            infoMsgS.idxMsgPriorityUB=0x02;
            if ((Vehco_Type_Test==4)||(Vehco_Type_Test==6)) {              MSCANDrv_SendMsgCAN_B(infoMsgS);            }        
            break;
        case 9:
        case 59:    //MCF     �������������BCD    50ms    0X439
            infoMsgS.infoMsgIDUI=0X439;
            infoMsgS.bExtIDUB=0;
            infoMsgS.datMsgUBA[0]=BCD_Err_Code1;
            infoMsgS.datMsgUBA[1]=BCD_Err_Code2;
            infoMsgS.datMsgUBA[2]=0;
            infoMsgS.datMsgUBA[3]=0;
            infoMsgS.datMsgUBA[4]=0;
            infoMsgS.datMsgUBA[5]=0;
            infoMsgS.datMsgUBA[6]=0;
            infoMsgS.datMsgUBA[7]=0;
            infoMsgS.numLenUB=8;
            infoMsgS.idxMsgPriorityUB=0x03;
            if ((Vehco_Type_Test==4)||(Vehco_Type_Test==6)) {              MSCANDrv_SendMsgCAN_B(infoMsgS);            }        
            break;

            default:
                break;
        }
        
        CANNet_idxSchedUB=(uint8)((CANNet_idxSchedUB+1)%100); //100msһ������ѭ��
        
        //������������Ϣ������ 1ms
        if (APU_Enable==1) {      CANNet_SendMsgCAN_APU();     }
    }
  
}


