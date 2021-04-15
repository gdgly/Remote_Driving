/*******************************************************************************
* �������ƣ������߼�
* �ļ����� Vehco_Logic.c
* �������� �� ��������Э��
* ���ü�¼��
* ��ע�� 
* �޶���¼��
*		����		      �汾		  ����		����
*   
*******************************************************************************/

#include "GlobalDef.h"
#include "VehCo.h"
#include "Custom_Func.h"
#include "Vehco_Logic.h"
#ifndef AECS_HW
      #pragma DATA_SEG __GPAGE_SEG RUNPAR_EEPROM
#else  
      #pragma DATA_SEG S12X_RAM
#endif

    extern uint8 Lock_Vehicle_Mode;   //����״̬      0 δ���� 1 1������      2 2������ 
    extern uint8 Heartbeat_Lock_Mode; //��������״̬  0 δ���� 1 ������������ 2 �������� 
    
#ifndef AECS_HW
      #pragma DATA_SEG DEFAULT
#else  
      #pragma DATA_SEG S12X_RAM
#endif

/*
0���ػ� 1��ON�� 2������� 3��BMS�ϵ���� ���ʹ��ָ�� 4�����READY
5��ON��ʧ �Զ������� 6���µ羯�� 7�������ŵ�ָ�� 8�������ŵ��� 9���ŵ���� ȥʹ��ָ��
10: �ػ�׼����ɣ��¿��Ƶ�

12: 1������  13��2������ 14��3������ ???
*/
uint8 Vehco_State;
//extern uint8 Vehco_State_Stop_Step;
uint8 Vehco_State_Debug;

uint8 MCU_Fault_Reset_Times;//MCU��λ����

uint16 Delay_Index=0,Delay_Index_Tmp=0;
//��չ޹��ϡ�EPS���ϡ��յ����ϡ�DC���ϡ�������ع���
uint8 Err_Vacuum_Tank,Err_Eps,Err_Air_Conditioning,Err_DCDC,Err_BatterLow;


//�Ǳ�LEDS1 8��bit  BEEP��SOC�͡������ѹ�͡���Ե�͡��������ָʾ�����״ָ̬ʾ
uint8 Beep,Batter_Err,Soc_or_Tmp_Low,Low_Cell_Voltage,Low_Isolation,Charge_Connect,Charge_State;
//�Ǳ�LEDS2 8��bit ����ģʽ������Ѳ��ģʽ��������ȡ�����ģʽ��ϵͳ���ϡ�Ready��
uint8 Limit_Mode,Constant_Mode,MCU_OverHeat,FeedBack_Mode,System_Fault,Ready,Ready_Led;

uint8 Motor_OverHeat=0;//��� ��ع���
uint8 Motor_OverHeat_High=0;// ��� ���2������
uint8 Beep_Index,Ready_Index,Km100_Index;
uint16 Err_Dcdc_Delay=0; //ON������ӳ��ж�12V��ѹ����12.5V ��DCDC���� 
uint16 Err_Dcdc_Di_Delay=0;
uint8 Breakk_Please_Start;//�����ƶ�̤������
uint8 Dang_NP_Please_Start;//����ÿյ�����
uint8 Brake_Shoe_Warning;//�ƶ���Ƭ��������
uint8 Tire_Pressure_Waring=0; //̥ѹ��������

uint16 Err_BatterLow_Delay=0; //������ص�ѹ�����ӳ�
uint16 Err_Vacuum_Tank_Delay=0;


//sint16 Ins_Ele_10[10]={0};    //˲ʱ���10ms�ۼ�    0.0~1000.0KWh/100Km
//sint16 Ins_Ele_100[10]={0};   //˲ʱ���100ms�ۼ�   0.0~1000.0KWh/100Km
sint32 Ins_Ele_1000=0;        //˲ʱ���1000ms      0.0~1000.0KWh/100Km
 

uint16 Electric_Consume_Calculation_Times=0;//�ܺļ���ʱ�� 0~1000 (999)

uint32 Vehicle_Speed_Add=1;     //���ٻ����ۼ�ֵ  0.1Km/h
sint32 Electric_Consume_Add=0;  //�繦�ʻ����ۼ�ֵ  0.01KW
uint32 Vehicle_Speed_Avg_Add=1;     // ƽ�� ���ٻ����ۼ�ֵ  0.1Km/h
sint32 Electric_Consume_Avg_Add=0;  //ƽ�� �繦�ʻ����ۼ�ֵ  0.01KW

sint32 AV_Ele_Calc=0; //ƽ����ļ���ֵ
uint16 Electric_Consume_Calculation_Avg_Times=0; //ƽ���ܺļ���ʱ�� 0~3600(2599)

uint16 Ins_Ele_NoCalc_Times=0;  //˲ʱ���δ�������  *1s
uint16 AV_Ele_NoCalc_Times=0;   //ƽ�����δ�������  10min

sint32 Remaining_Mil_Calc=0;  //ʣ�����  ����ֵ 0~65535KM

extern uint8 Failure_Recovery_Delay[24];
extern uint16 Output_Voltage,B_Voltage,Min_B_Voltage_Limit;//��ѹ  �����ѹ��ֵ
extern sint16 B_Current; //����
extern uint8 BattCD_bLogicOnUB; //�߼��翪��         //����ɾ��
extern uint16 BattCD_uUW;  //���ص�ѹ          //����ɾ��
extern uint16 uAD4Satus[4];
extern uint8 Car_Door;//���ſ�

extern uint8 bDiSatus[24];
extern uint16 Breakk;
extern uint8 MCU_Fault_Level, FastDischarge_Fbd,Precharge_Allow,MCU_System_State,LostMcuCommunication;
extern uint8 LostBMSCommunication,B_Fault_Status,B_State,B_Mode;
//BMS���̵����������� 0��1;     BMS 3s�ϵ���ʱ0��1;    BMS���ϱ������ 0��1
extern uint8 BMS_Contractor_Request,BMS_Power_Down_Request,BMS_Fault_Lockout_Request;
//MCUʹ��ָ��0��1;MCU��λָ��0��1;MCU����ģʽ1��2��3
extern uint8 MCU_Enable,MCU_Fault_Reset,MCU_Control_Mode;
//��ص������12V��Դ
extern uint8 BMS_MCU_12V_Enable,VCU_Diag_Code_1,VCU_Diag_Code_2,VCU_Diag_Code_3; 
extern uint16 DC_Voltage;

extern uint8 LostINSCommunication;
extern uint16 LostMcuCom_Index,LostBMSCom_Index,LostINSCom_Index;
extern uint8 ACPTC_No_LowSoc,ACPTC_No_LowSoc_Once;
extern uint8 SOC,Min_SOC_Limit,Isolation_Level,Dang_L4bit,Dang_P;
extern uint16 Min_Cell_Voltage,Min_Cell_Voltage_Limit,Min_Cell_Temp,Min_Cell_Temp_Limit;
extern sint16 Demand_Torque,Motor_Torque,Motor_Speed;  
extern sint16 Motor_Speed_Average;  
extern uint8  DTC_Code_1,DTC_Code_2; //MCU DTC CODE
extern uint8  VCU_Warning_Code_1,VCU_Warning_Code_2,VCU_Warning_Code_3;//������������123 
extern uint8 BMS_DTC_Code[8];//�����ϱ���
extern uint8 Err_AccPed; //����̤�����
extern uint8 Err_Breakk; //�ƶ�̤�����
extern uint16 AccPed;
extern uint16 Max_Cell_Temp_Limit,Max_Cell_Temp;
extern uint8 DCDC_Enable,Fan_Enable,Dang_R_Enable;
extern uint8 Motor_Temperature,MCU_Temperature;
extern uint8 Torque_Mode;

extern uint8 BCD_Err_Code1,BCD_Err_Code2; //�Ǳ������  A9����ר��
extern uint8 Vehco_Type_Test;
extern uint8 B_Version;//BMSͨѶЭ��汾
extern uint8 Vehicle_Mode;//�Ǳ���ģʽ 0 1 2   7
extern uint8 Vehicle_Type_Identify_Err;  //����ͺŲ�ƥ�����
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
extern  uint16 Motor_Vehicle_Speed_Scale_Get;//ת��-���ٱ���
extern  uint16 Demand_Limit_High_Get;//��������
extern  uint16 Demand_Limit_Low_Get;//��������
extern  uint16 Crawl_D_Speed_Max_Get;//������״̬�����䶯���ٶ�  RPM
extern  uint8 Crawl_D_Torque_Delay_Max_Get;//�䶯���¼��  ��λ10ms
extern  uint8 Speed_Buffer_Number_Get; //10ms�����ٶȲ����������
extern  uint8 Speed_Average_Number_Get; //10msƽ�����ٻ������
extern  uint8 Torque_Buffer_Number_AutoPark_Get;  //10ms����ƽ��ת�ز���������� פ��

//̥ѹ���

extern uint16 Tire_Pressure[8]; //��̥ѹ��ֵ 0~1397.0Kpa 10��
extern sint16 Tire_Temp[8];     //��̥�¶�ֵ -273.0~1735.0�� 10��
extern uint8  Tire_Sensor[8];   //���ִ�����״̬
extern uint8  Tire_Pressure_Test[8]; //ѹ�������
extern uint8  Tire_Pressure_Active; //̥ѹ��������Ծ 


//Զ������
extern uint8 Lock_Command,Heartbeat_Command,Lock_Life,Lock_Box_Mark; //�ն�Զ���������������������ն�����ֵ���ն�������ʶ
extern uint8 Lock_Box_RandKey;//�ն�����ܳ�
extern uint32 Lock_Box_SN;//�����ն˱��
extern uint16 Lock_Box_Com_Index;
extern uint8 LostLockBoxCommunication;
extern uint8 GetNewMsg_LockBox[3];
extern uint8 Lock_Vehicle_Active; //����״̬����
           

//����������ר��
extern uint8 Third_Lock_Active;              //��������
extern uint8 Third_Lock_Mode_Active;         //��Ч����ģʽ
extern uint8 Third_Lock_LimitSpeed_Active;   //��Ч�����ٶ� 

//��ļ���
extern uint8 Remaining_Mil;//ʣ����� 0��255km      ��V1.2�濪ʼ 2km/bit 0~510
extern uint8 AV_Ele;//ƽ�����    0.5kwh/100KM  0~127.5
extern uint8 Ins_Ele;//˲ʱ���   0.5kwh/100KM  0~127.5
extern uint16 Rated_Energy; //����� 0~409.5kwh
extern uint16 AV_Ele_Calc_Min;// ��С�ٹ���ƽ����� 10.0KWh/100Km

//����
extern uint8  Oil_Pump_Install; //�ͱÿ���������״̬ 0 δװ 1 ��װ
extern uint8  Air_Pump_Install; //���ÿ���������״̬ 
extern uint8  Fan_Pwm_Install;
extern uint8  Air_Pump_OverHeated;
extern uint16 Fan_Pwm_Rate;     //D15 ����PWM����ֵ 0~100.00%
extern uint8  Air_Reservoirs_Pressure_1,Air_Reservoirs_Pressure_2;//����Ͱѹ�� 0.00~2.50Mpa
extern uint8  Air_Pump_Fault,Oil_Pump_Fault;//�ͱù���״̬ 0 ����  1 ����
extern uint8  Oil_Pump_Still_OverTime; //�ͱö�ת ��ת��ʱ
extern uint16 Air_Reservoirs_Pressure_Low_Index;
extern uint8  Gears_Install;
extern uint8 Shift_Gears; 

//��绥��
extern uint8   No_Ins_Charge_CAN;//����ڼ��ֹINS����֡����

    
#ifndef AECS_HW
  #pragma CODE_SEG DEFAULT
#else  
  #pragma CODE_SEG VCU_CODE 
#endif

//�������µ��߼�
void Vehco_Logic(void) 
{
   Vehco_State_Debug=Vehco_State_Stop_Step; 
    
    switch(Vehco_State) 
    {
      case 0:
          Vehco_State_Stop_Step=0; 
          break;
      case 11:  //MCU 1��2������  ��λ��
          MCU_Fault_Reset=1;
          MCU_Enable=0;
          Delay_Index++;
          if (MCU_Fault_Level==0) 
          {
              MCU_Fault_Reset=0;
              MCU_Enable=0;
              Vehco_State=1;  
              Vehicle_Mode=0;         
          }
          if (Delay_Index>100)//1�볬ʱ
          {
              Vehco_State_Stop_Step=11;
              MCU_Fault_Reset=0;
              MCU_Enable=0;
              Vehco_State=1;              
          }
      
      
          break;    
      case 12: //��ر������ȴ�����µ�


          break;
      case 1: //����ON�� �ȴ�STAR�ź�
          //START�ź� & ���ϵͳ���� ��READY
          Ready=0;
          Ready_Led=0;
          //Vehicle_Mode=0; 
          if (((MCU_Fault_Level==1)||(MCU_Fault_Level==2))&&(MCU_Fault_Reset_Times<3)) //MCU���θ�λ  
          {
              Vehco_State=11;
              Delay_Index=0; 
              MCU_Fault_Reset_Times++;   
              Vehicle_Mode=7;
          }
          
          //uint8 Breakk_Please_Start;//�����ƶ�̤������
          //uint8 Dang_NP_Please_Start;//����ÿյ�����           
          if ((Dang_L4bit==0)||(Dang_L4bit==1)  )
          {
              Dang_NP_Please_Start=0;
              Failure_Recovery_Delay[5]=0;          
          }
          if ((Breakk>100)&&(AccPed<=100)) 
          {
              Breakk_Please_Start=0;
              Failure_Recovery_Delay[4]=0;          
          }
          
              //��λΪN��P�ſ��Ե��//���Ӳ��ܲ�����
          if ( (Vehicle_Type_Identify_Err==0)&&((Dang_L4bit==0)||(Dang_L4bit==1)) && (bDiSatus[2]==1)&& (Breakk>100)&&(AccPed<=100)&&(Err_AccPed==0)&&
              ((MCU_Fault_Level==0)||(MCU_Fault_Level==1))&&(No_Ins_Charge_CAN==0)&&(FastDischarge_Fbd==0)&&(Precharge_Allow==1)&&  ((MCU_System_State==1)||(MCU_System_State==3)||(MCU_System_State==7)   )    &&(LostMcuCommunication==0))
          
          {   //���ϵͳ����
              if ((LostBMSCommunication==0)&&   ((B_Fault_Status<=2)||  ((B_Fault_Status<=1)&&(B_Version<18))   )
              
              &&(((B_State==0)&&((B_Mode==0)||(B_Mode==1)))  ||  (B_State==1)&& ( (B_Mode==2)||(B_Mode==1) ) ||(B_State==4)&& ( (B_Mode==2))  ))
              {
                  BMS_Contractor_Request=1; 
                  Delay_Index=0;              
              
              }
          
          }else 
          { //���ʧ������
              if (bDiSatus[2]==1)//š��START
              {
                  //����ÿյ�����    
                  if ((Dang_L4bit==5)||(Dang_L4bit==10))
                  { 
                      Dang_NP_Please_Start=1;
                      Failure_Recovery_Delay[5]=200; 
                  }
                  //�����ƶ�̤������
                  if ((Breakk<100)||(AccPed>100)) 
                  { 
                      Breakk_Please_Start=1;
                      Failure_Recovery_Delay[4]=200; 
                  }
              }else 
              {   //�ɿ�START��ʼ��ʱ
                  //
                  if (Failure_Recovery_Delay[5]==0) 
                  {
                      Dang_NP_Please_Start=0;
                  }else 
                  {
                      --Failure_Recovery_Delay[5];
                  }
                  if (Failure_Recovery_Delay[4]==0) 
                  {
                      Breakk_Please_Start=0;
                  }else 
                  {
                      --Failure_Recovery_Delay[4];
                  }                          
                  //
              }
              
          }
          
          if  (BMS_Contractor_Request==1) 
          {
              
              if (Delay_Index>100)//1�볬ʱ  
              {
                  BMS_Contractor_Request=0;
                  Vehco_State_Stop_Step=1;
                  Delay_Index=0;
              } 
              else
              {
                  Delay_Index++;
              }
              
              if ((B_State==2)&&  ((B_Mode==2)||(B_Mode==0))    ||(  (B_State==3)&&(B_Mode==3)   )      ) 
              {
                  Vehco_State=2;//����Ԥ��״̬
                  Delay_Index=0;
              }
          }
          break;
      case 2://Ԥ��׶�       18.12.05 ��Ϊ9��
          if  (Delay_Index>900)//5�볬ʱ
          {
              BMS_Contractor_Request=0;
              Vehco_State_Stop_Step=2;
              Vehco_State=1;
              Delay_Index=0;          
          }
          else 
          {
              Delay_Index++;
          }
          //B_Voltage  Output_Voltage //0~819.0V
          //DC_Voltage   //0��1000.0V
          //  ((MCU_System_State==3)||   (MCU_System_State==1)  )  ) //A9 ����MCU����
          if ((B_State==3)&&(B_Mode==3)
              &&(MCU_System_State==3)
              && (abs(B_Voltage-DC_Voltage)<300)
              &&(Delay_Index>50)      )
          {
              Vehco_State=3;    
              Delay_Index_Tmp=0;
              Delay_Index=0;
          }
          break;
      case 3://BMS�ϵ���� �ӳٺ��͵��ʹ��ָ��
          if (Delay_Index_Tmp>10) //���ʹ����ʱ100ms
          {
              MCU_Enable=1;
              MCU_Control_Mode=2;
              Delay_Index_Tmp=11;
              //Delay_Index=0;
              
          }else 
          {
              Delay_Index_Tmp++;
          }

          if (MCU_Enable==1) 
          {
              if (Delay_Index>200)  //�����ʹ��ָ���ȴ������Ӧ  ����ʱ��2s
              {
                  BMS_Contractor_Request=0;
                  Vehco_State_Stop_Step=3;
                  Vehco_State=1;
                  MCU_Enable=0;
                  Delay_Index=0;
                  Delay_Index_Tmp=0;
              } else
              {
                  Delay_Index++;
              }
              if  (MCU_System_State==5)//((MCU_System_State==5)||(MCU_System_State==3))     //����ĳЩ�������5
              {
                  MCU_Fault_Reset_Times=0;
                  Vehco_State=4;
                  Ready=1;
                  Ready_Led=1;
                  Vehicle_Mode=1;
                  Delay_Index=0; 
                  Delay_Index_Tmp=0;  
              }
          
          
          }
          break;
      case 4: //�������READY״̬  �Ǳ���ʾ��READY
          //�˴��Ƿ����й����ж�  ��ͬ����Ĺ������벻ͬ��״̬
          //���ع���   B_Version
          //if ((MCU_Fault_Level>1)||(B_Fault_Status>1)||(LostMcuCommunication==1)||(LostBMSCommunication==1)) //�������2    ��ع���4 ���ͨѶ���� ���ͨѶ����
          
          if ((MCU_Fault_Level>2)||    
             ((B_Fault_Status>1)&&(B_Version<18))  || (B_Fault_Status>2)    //BMS 1.8����ǰ2�������µ磬1.8�濪ʼ3�������µ�
             ||(LostMcuCommunication==1)||(LostBMSCommunication==1)) //�������3    ��ع���2/3 ���ͨѶ���� ���ͨѶ����
          {
                  Ready=0;
                  Ready_Led=0; 
                  Vehicle_Mode=7;
                  MCU_Enable=0;          //��ͣ��� 100ms��ͣ���
                  //BMS_Contractor_Request=0;
                  //Vehco_State_Stop_Step=4;
                  //Vehco_State=1;
          }
          if (Vehicle_Mode==7) 
          {
              if ((Delay_Index_Tmp>10)||(MCU_System_State!=5)) //100ms�������˳�����״̬
              {
                  BMS_Contractor_Request=0;  //ͣ���
                  Vehco_State_Stop_Step=4;
                  Vehco_State=1;  
                  Delay_Index_Tmp=0;            
              
              }else 
              {
                  Delay_Index_Tmp++;
              }
          
          
          }
          
      
      
      
          break;
      case 5://���ٳ��� 
          //��ʧON�ź� �ٶȹ��ߣ���Ҫ���� ��Vehco.c  VehCo_Afterrun���ж�
          //On_LostInRuning
          //����D������������ ��R ����״̬ ��Custom_Func.c�д���
          
          //��ʱӦ�����Ǳ���  Ready ���̽�����˸ ����
          
          break;
      case 6: //BMS�µ�ָ��   �Ǳ���ʾ��READY
          //��ʧON�źź� �ٶȹ��ͣ������¸�ѹ ��Vehco.c  VehCo_Afterrun���ж�
          BMS_Power_Down_Request=0;
          BMS_Contractor_Request=0;
          Ready=0;
          Ready_Led=0;
          Vehicle_Mode=0;
          MCU_Enable=0;
          if (BMS_Contractor_Request==0) 
          {
              
              if (Delay_Index>200)      //2s�����޷��µ� �����н��������ŵ� ״̬10ʱ�Ƿ�Ҫ����DCV<36 ?
              {
                  Vehco_State=9;
                  Delay_Index=0;
                  Vehco_State_Stop_Step=6;
              } 
              else 
              {
                  Delay_Index++;
              }
              
          
              if  ((B_State==0)&&((B_Mode==0)||(B_Mode==1))||(   ((B_State==1)||(B_State==4))    &&  ((B_Mode==2)||(B_Mode==1))  ))  //BS=0&BM=0,1 OR BS=1&BM=2
              { //��ѹ����Ѿ��µ磬���Խ����������ŵ�

                  if (DC_Voltage>=500)
                  {
                     
                     
                     //if (Vehco_Type_Test==9)
                     if (FastDischarge_Enable==0)  //�����������ŵ�
                     
                     {
                         Vehco_State=8 ;   //8�����Ų�֧�������ŵ�;
                         Delay_Index=0; 
                     }else{
                         Vehco_State=7;
                         Delay_Index=0; 
                     }
                  } else 
                  {
                      Vehco_State=9;
                      Delay_Index=0;
                  
                  }
              }
          }
          break;
      case 7: //�����ŵ�ָ��
          MCU_Enable=1;
          MCU_Control_Mode=3;
          
          if ((MCU_Control_Mode==3)&&(MCU_Enable==1)) 
          {
              if (Delay_Index>100) //1s�����޷����������ŵ�״̬ �˳������ŵ�  ״̬10ʱ�Ƿ�Ҫ����DCV<36 ?
              {
                  Vehco_State=9;
                  Delay_Index=0;
                  Vehco_State_Stop_Step=7;              
                  
              }
              else 
              {
                  Delay_Index++;
              }


              if ((FastDischarge_Fbd==1)&&(MCU_System_State==6)) 
              {
                  Vehco_State=8;
                  Delay_Index=0;
              }
          }
          break;
      case 8: //�����ŵ����

          if (Delay_Index>300) //3s�����޷��Ÿɾ���  �˳������ŵ�  ״̬10ʱ�Ƿ�Ҫ����DCV<36 ?
          {
              Vehco_State=9;
              Delay_Index=0;
              Vehco_State_Stop_Step=8;                  
                  
          }
          else 
          {
              Delay_Index++;
          }          
          if ((MCU_System_State==7)||(FastDischarge_Fbd==0)) //�ŵ���� 
          {   
              Vehco_State=9;
              Delay_Index=0;                       
          }          

          break;
      case 9: //�����ŵ���� ���ȥʹ��
          MCU_Enable=0;
          MCU_Control_Mode=2;          
      
          if ((MCU_System_State==1)||(MCU_System_State==8)) 
          {
              Vehco_State=10;
              Delay_Index=0;               
          }
           if (Delay_Index>100)//1S �������Ӧ
           {  
              Vehco_State=10;
              Delay_Index=0;
              Vehco_State_Stop_Step=9;
           }
           else
           {
              Delay_Index++;
           }
          
          break;
      case 10: //�¸�����
          //MCU�ػ�������ɺ�,�ر�MCU������12V
          BMS_MCU_12V_Enable=0;
          VehCo_DOset(1);
          break;
      default:
          break;
    }




}


//���������жϴ��� 
void Vehco_Error (void) 
{   //10ms
    uint8 VCU_Diag_Code_1_bit[8]={0},VCU_Diag_Code_2_bit[8]={0};
    uint8 VCU_Diag_Code_3_bit[8]={0};
    uint8 VCU_Warning_Code_1_bit[8]={0},VCU_Warning_Code_2_bit[8]={0},VCU_Warning_Code_3_bit[8]={0};
    

    //uint8 Err_Vacuum_Tank,Err_Eps,Err_Air_Conditioning,Err_DCDC,Err_BatterLow;
    //uint8 BMS_DTC_Code[8];//�����ϱ���
    //uint8 VCU_Warning_Code_1,VCU_Warning_Code_2,VCU_Warning_Code_3;//������������123 
    //uint8 Brake_Shoe_Warning �ƶ���Ƭ��������
    extern uint8 Limit_P;
    extern uint8 Breakk_Please;//���ƶ�̤������
    extern uint8 Safe_Belt;//��ȫ������״̬
    
    
    
    //���������ж�
    Vehco_Errpr_Judge();
    //����������
    Beep_5050();
    
    
    //�������ϴ�������
    VCU_Diag_Code_1_bit[7]=Err_AccPed;                                 //����̤�崫��������
    VCU_Diag_Code_1_bit[6]=Err_Breakk; //�ƶ�̤�����     //�ƶ�̤�崫��������   
    VCU_Diag_Code_1_bit[5]=LostMcuCommunication;         //�����������
    VCU_Diag_Code_1_bit[4]=LostBMSCommunication;         //BMS ��������
    VCU_Diag_Code_1_bit[3]=((MCU_Fault_Level>1)||(Vehicle_Type_Identify_Err==1)  );    //�������  ���Ǳ����ô˴��뱨����ͺŲ�ƥ��
    VCU_Diag_Code_1_bit[2]=(B_Fault_Status>2);           //������ع���
    VCU_Diag_Code_1_bit[1]=MCU_OverHeat;                               //�������
    VCU_Diag_Code_1_bit[0]=(Max_Cell_Temp>Max_Cell_Temp_Limit);        // ��ع���
    VCU_Diag_Code_1=SetBit8(VCU_Diag_Code_1_bit);
    
    
    VCU_Diag_Code_2_bit[6]=Err_Vacuum_Tank;            //��ձù���
    //VCU_Diag_Code_2_bit[5]=            //��ѹ�̵������� ճ�� ʧ�� 
    VCU_Diag_Code_2_bit[4]=LostINSCommunication;       //�Ǳ���������
    //VCU_Diag_Code_2_bit[2]=           //��ѹ������·    
    VCU_Diag_Code_2_bit[0]=(Isolation_Level>0);       //��Ե�������
    VCU_Diag_Code_2=SetBit8(VCU_Diag_Code_2_bit);

    VCU_Diag_Code_3_bit[5]=Vehicle_Type_Identify_Err;//����ͺŲ�ƥ�����
    //VCU_Diag_Code_3_bit[4]=    // ����¶��쳣���뼴��ͣʻ�³���ɢ�� 
    VCU_Diag_Code_3_bit[3]=Err_BatterLow;          //��������쳣
    VCU_Diag_Code_3_bit[2]=Err_DCDC;               //DC ����
    VCU_Diag_Code_3_bit[1]=Err_Air_Conditioning;   //�յ�����
    VCU_Diag_Code_3_bit[0]=Err_Eps;                //EPS ����
    VCU_Diag_Code_3=SetBit8(VCU_Diag_Code_3_bit);
    
    //uint8 Breakk_Please_Start;//�����ƶ�̤������
    //uint8 Dang_NP_Please_Start;//����ÿյ�����    
    VCU_Warning_Code_1_bit[6]=Limit_P;
    VCU_Warning_Code_1_bit[3]=Tire_Pressure_Waring;//̥ѹ����
    VCU_Warning_Code_1_bit[2]=(Safe_Belt==0)&&(Motor_Speed/Motor_Vehicle_Speed_Scale_Get>10);
    VCU_Warning_Code_1_bit[1]=Dang_NP_Please_Start;
    VCU_Warning_Code_1_bit[0]=Breakk_Please||Breakk_Please_Start;

    VCU_Warning_Code_1=SetBit8(VCU_Warning_Code_1_bit);
    
    VCU_Warning_Code_2_bit[5]=Brake_Shoe_Warning;
    //VCU_Warning_Code_2_bit[4]=Car_Door && Ready_Led && (Safe_Belt==0); //�������ϵ磬�����뿪��ʻ��
    
    VCU_Warning_Code_2=SetBit8(VCU_Warning_Code_2_bit);
    
    VCU_Warning_Code_3=SetBit8(VCU_Warning_Code_3_bit);
    
}

void Vehco_Errpr_Judge(void)
{
  
    uint8 Err_DCDC_dE8=0;
    uint8 Err_DCDC_Bi=0;
    uint16 Fan_Pwm_Rate_Temp=0;
    
    Err_Eps=(bDiSatus[11] &&(Oil_Pump_Install==0)) || (((Oil_Pump_Fault==1)||(Oil_Pump_Still_OverTime==1)) &&(Oil_Pump_Install==1));
    //Err_Air_Conditioning=?;

    
    if (BattCD_uUW<Err_BatterLow_Limit)          //��ص�ѹ����10.5V
    {
        Err_BatterLow_Delay++;
        if (Err_BatterLow_Delay>800)         //�ӳ�8��
        {
            Err_BatterLow=1;
            Err_BatterLow_Delay=900;
        }
    }else
    {
        Err_BatterLow=0;
        Err_BatterLow_Delay=0;    
    }
        
    //Err_Vacuum_Tank=bDiSatus[8];
    //Air_Pump_OverHeated=((bDiSatus[8]==0)&&(Air_Pump_Install==1)&&(Vehco_Type_Test!=16)&&(Vehco_Type_Test!=18)); //���ù��ȱ���  ���߱���   H������ D35C �������¿���
    
    if (((Air_Reservoirs_Pressure_Low_Index==200)||(bDiSatus[8]==1))&&(Air_Pump_Install==0))                             //��չޱ���
    {
        Err_Vacuum_Tank_Delay++;
        if (Err_Vacuum_Tank_Delay>300)    //�ӳ�3��
        {
            Err_Vacuum_Tank=1;
            Err_Vacuum_Tank_Delay=400;
        }
    }else 
    {
        if ((Air_Pump_Install==1)
            &&(((Air_Reservoirs_Pressure_1<Air_Pump_Low_Pressure)||(Air_Reservoirs_Pressure_2<Air_Pump_Low_Pressure))||(Air_Pump_Fault==1))
            
            
            )
        {
            Err_Vacuum_Tank_Delay++;
            if (Err_Vacuum_Tank_Delay>200)    //�ӳ�2��
            {
                Err_Vacuum_Tank=1;
                Err_Vacuum_Tank_Delay=400;
            }     
        }else
        {
            Err_Vacuum_Tank_Delay=0;
            Err_Vacuum_Tank=0;
        }
    }
  
  
  
    //Fan_Enable  Motor_Temperature  MCU_Temperature -40��210��  Motor_OverHeat_High
  
    //Brake_Shoe_Warning ;//�ƶ���Ƭ�������� ��̬�ӵأ��ڲ�2.74K����5V  uAD4Satus[0] 0~4095 0~5V
    Brake_Shoe_Warning= Brake_Shoe_Check && (uAD4Satus[0]>2048);   
  
    /*
    extern uint16 Tire_Pressure[8]; //��̥ѹ��ֵ 0~1397.0Kpa 10��
    extern sint16 Tire_Temp[8];     //��̥�¶�ֵ -273.0~1735.0�� 10��
    extern uint8  Tire_Sensor[8];   //���ִ�����״̬
    extern uint8  Tire_Pressure_Test[8]; //ѹ�������    
    uint8 Tire_Pressure_Waring; //̥ѹ��������
    */
    
    
    if (Tire_Pressure_Active==1)
    {
        switch(Vehco_Type_Test) 
        {
              case 9:
                  if ((Tire_Pressure[0]<Tire_Pressure_Low_Level_M10) || (Tire_Pressure[1]<Tire_Pressure_Low_Level_M10)) 
                  {
                      Tire_Pressure_Waring=1;
                  }else 
                  {
                      Tire_Pressure_Waring=0;
                  }
                  break;
              default:
                  Tire_Pressure_Waring=0;
                  break;
        }
    }
    if ((Motor_Temperature>=Motor_Temperature_L1)||(MCU_Temperature>=MCU_Temperature_L1))  {Motor_OverHeat=1;}  // ���80 ������ 50 ��������
    if ((Motor_Temperature>=Motor_Temperature_L2)||(MCU_Temperature>=MCU_Temperature_L2))  {Motor_OverHeat_High=1;}  //���100 ������ 70  2������
   
    if ((Motor_Temperature<=Motor_Temperature_L1-10)&&(MCU_Temperature<=MCU_Temperature_L1-4))  {Motor_OverHeat=0;Motor_OverHeat_High=0;}   // ���70 ������ 46 �رշ���
   
   
    //Fan_Pwm_Rate
    //˹̩�����  500 Hz 6~25 (15)% ͣ�� 35~55% ��������  55~95% 25~100ת��
    
    if (Motor_Temperature>=Motor_Temperature_L1)  //140~120---->1~21
    {
        Fan_Pwm_Rate_Temp=Fan_Pwm_Rate_Temp+(1+Motor_Temperature-Motor_Temperature_L1); 
    }
    if (MCU_Temperature>=MCU_Temperature_L1)    //90~100------>2~22
    {
        Fan_Pwm_Rate_Temp=Fan_Pwm_Rate_Temp+2*(1+MCU_Temperature-MCU_Temperature_L1); 
    }
    //���ݵ���¶Ⱥ͵���¶ȼ������PWM
    Fan_Pwm_Rate=Intpol_u16u16((uint16 * __far)&Fan_Rate_Temp, Fan_Pwm_Rate_Temp); 

   
    if (DCDC_Enable==1) 
    {
        //1������ 2��D11\D12-600Nm 3��D07-750   4��A9 ��غ�150Nm   5��D11\D12 ��غ�700Nm 6��A9����180Nm 7��D10���� 500Nm  8��D09 400Nm ��غ�  9��D11L8�� 1200Nm����           
        if  (
            ((BattCD_uUW<Err_BatterLow_Limit+200)&& (  (Vehco_Type_Test==1)||(Vehco_Type_Test==2)||(Vehco_Type_Test==5)   )     )       //ON����DCDC��ѹ
            || 
            ((BattCD_uUW<Err_BatterLow_Limit+200)&& 
              //((Vehco_Type_Test==4)||(Vehco_Type_Test==3)||(Vehco_Type_Test==7)||(Vehco_Type_Test==8)||(Vehco_Type_Test==6)) 
              (  (Vehco_Type_Test!=1)&&(Vehco_Type_Test!=2)&&(Vehco_Type_Test!=5)   )      //ON����DCDC��ѹ
              &&(Vehco_State==4))
            ) 
        {
           
           
            Err_Dcdc_Delay++;
        }else 
        {
            Err_Dcdc_Delay=0;
        }
        
        if ((Err_Dcdc_Delay>2000))
        {
            Err_Dcdc_Delay=2100;
            Err_DCDC_dE8=1; //DCDC�ϵ�20���12V��ѹ����12.5V ��DCDC����
        }else 
        {
            Err_DCDC_dE8=0;
        }
      
        
  
    }else{Err_Dcdc_Delay=0;}
    //Err_Dcdc_Di_Delay    Err_DCDC_Bi  �յ�DCDC����Ӳ���ź�8���DCDC����
    if ((bDiSatus[12])&&(Vehco_State==4)) 
    {
       Err_Dcdc_Di_Delay++;
       if (Err_Dcdc_Di_Delay>800) 
       {
          Err_DCDC_Bi=1;
          Err_Dcdc_Di_Delay=900;
          
       } else 
       {
          Err_DCDC_Bi=0;
       }
    } 
    else 
    {
        Err_Dcdc_Di_Delay=0;
        Err_DCDC_Bi=0;
    }
    
    
    
    
    
    
    
    Err_DCDC=(Err_DCDC_Bi||Err_DCDC_dE8);
  
  
    if (BMS_MCU_12V_Enable==1) {LostMcuCom_Index++;LostBMSCom_Index++;LostINSCom_Index++;}
  
    if (LostMcuCom_Index>500)   //MCU ͨѶ��ʱ5S
    {
        LostMcuCom_Index=501;
        LostMcuCommunication=1;
    } else 
    {
        LostMcuCommunication=0;
    }
    if (LostBMSCom_Index>500)   //BMS ͨѶ��ʱ5S
    {
        LostBMSCom_Index=501;
        LostBMSCommunication=1;
    } else 
    {
        LostBMSCommunication=0;
    }
    if (LostINSCom_Index>1000)   //INS ͨѶ��ʱ5S
    {  
        LostINSCom_Index=1001;
        LostINSCommunication=1;
    } else 
    {
        LostINSCommunication=0;
    }  
    //�Ǳ�LEDS1 8��bit  BEEP��SOC�͡������ѹ�͡���Ե�͡��������ָʾ�����״ָ̬ʾ
    //uint8 Beep,Batter_Err,Soc_or_Tmp_Low,Low_Cell_Voltage,Low_Isolation,Charge_Connect,Charge_State;
  


  
    Batter_Err=(B_Fault_Status>0);
    
    
    if ((ACPTC_No_LowSoc_Once==0)&&(Soc_or_Tmp_Low==0)&&(((SOC<=Min_SOC_Limit)&&(SOC!=0)) || ((Min_Cell_Temp<=Min_Cell_Temp_Limit)&&(Min_Cell_Temp!=0))==1))  
    {
        ACPTC_No_LowSoc=0;  //SOC�״ι���ʱ����ؿյ�����
        ACPTC_No_LowSoc_Once=1;
    }
    
    
    
    Soc_or_Tmp_Low=((SOC<=Min_SOC_Limit)&&(SOC!=0)) || ((Min_Cell_Temp<=Min_Cell_Temp_Limit)&&(Min_Cell_Temp!=0));

    Charge_State=  (3*Soc_or_Tmp_Low); //������ 11������
    
    Low_Cell_Voltage=( ((Min_Cell_Voltage<=Min_Cell_Voltage_Limit)&&(Min_Cell_Voltage!=0)) ||
                       ((B_Voltage<=Min_B_Voltage_Limit)&&(Min_B_Voltage_Limit!=0))         );  
                     //��������ѹ����   B_Voltage 0~819.0V    Min_B_Voltage_Limit
    
    
    Low_Isolation=(Isolation_Level>0);
  
    //�Ǳ�LEDS2 8��bit ����ģʽ������Ѳ��ģʽ��������ȡ�����ģʽ��ϵͳ���ϡ�Ready��
    //  uint8 Limit_Mode,Constant_Mode,MCU_OverHeat,FeedBack_Mode,System_Fault,Ready;    
    //MCU_OverHeat=(((DTC_Code_1&0x8)>>3)||((DTC_Code_1&0x10)>>4));
    MCU_OverHeat=(((DTC_Code_1&0x8)>>3)||((DTC_Code_1&0x10)>>4)||((DTC_Code_2&0x8)>>3)||((DTC_Code_2&0x10)>>4));  //2018.3.1 ���ӵ�����¾���
    if (((Torque_Mode==2)||(Torque_Mode==3)||(Torque_Mode==9))&&(Demand_Torque<0))  //��������ģʽ��
    {FeedBack_Mode=1;}
    else 
    {FeedBack_Mode=0;}
    
}

void Beep_5050 (void) 
{ //����������
     uint8 beep1=0,beep2=0,beep3=0;
    //P�� 500ms��  500msͣ
    if ((Dang_L4bit==10)&&(Dang_P==1)&&(Motor_Speed/Motor_Vehicle_Speed_Scale_Get>10))
    {
        if  (Beep_Index<50) 
        {
            beep1=1;
        }else 
        {
            beep1=0; 
        }
        Beep_Index=(uint8)((Beep_Index+1)%100);
    }
    else
    {
       Beep_Index=0;
       beep1=0;
    }
    //��ʻ�ж�ON����  250ms�� 250msͣ Ready������˸   ���������������ϸ�ѹ
    //Ready_Index
    if ( (Vehco_State==5)
        ||  ((Third_Lock_Active==1)&&(Vehco_State==4))   
        ||(((Lock_Vehicle_Enable==1)||(Heartbeat_Lock_Vehicle_Enable==1)) &&(Lock_Vehicle_Active==1)&&(Vehco_State==4)) )
    {
        if(Ready_Index<25) 
        {
            beep2=1;
            Ready_Led=0;
            
        }else 
        {
            beep2=0;
            Ready_Led=1;
        }
        Ready_Index=(uint8)((Ready_Index+1)%50);
    }
    else
    {
        Ready_Index=0;
        beep2=0;   
        Ready_Led=Ready;     
    }
    // 100km/h����    500ms��  500msͣ
    if ( ((Gears_Install==0)||( Shift_Gears==0  ))&&(AccPed_Torque_100_Enable==1)&& (Dang_L4bit==10)&&(Motor_Speed/Motor_Vehicle_Speed_Scale_Get>(D100_Max_Speed-3))) //D������100 
    {
        if  (Km100_Index<50) 
        {
            beep3=1;
        }else 
        {
            beep3=0; 
        }
        Km100_Index=(uint8)((Km100_Index+1)%100);    
    }else 
    {
       Km100_Index=0;
       beep3=0;
    }
    
    
    if ((beep1==1)||(beep2==1)||(beep3==1)) 
    {
        Beep=1;  
    }else 
    {
        Beep=0;
    }

}


void Lock_Vehicle_Active_Init(void)
{
   //ֻ���ڸ��ϵ����Ѿ��յ�����ָ��ſ��Լ�������״̬
    if (Lock_Vehicle_Mode==255)
    {
        Lock_Vehicle_Mode=0;
    }
    if (Heartbeat_Lock_Mode==255)
    {
        Heartbeat_Lock_Mode=0;
    }
      
    if (
        ((Lock_Vehicle_Enable==1)&& ((Lock_Vehicle_Mode==1)||(Lock_Vehicle_Mode==2)))
        ||((Heartbeat_Lock_Vehicle_Enable==1)&& ((Heartbeat_Lock_Mode==2)))
        
        ) 
    {
        Lock_Vehicle_Active=1;//����״̬����
         
    }
    //����ָ����ʧ����������״̬����          �����������ҵ�λ�ÿյ���P��            
    if ((Heartbeat_Lock_Mode!=2)&&(Lock_Vehicle_Mode!=1)&&(Lock_Vehicle_Mode!=2)&&(AccPed<50)&&((Dang_L4bit==0)||(Dang_L4bit==1))) 
    {
        Lock_Vehicle_Active=0;
    }
}


void Lock_Box_Vehicle(void) //Զ����������  100ms����һ��
{   
    /*
    extern uint8 Lock_Command,Heartbeat_Command,Lock_Life,Lock_Box_Mark; //�ն�Զ���������������������ն�����ֵ���ն�������ʶ
    extern uint8 Lock_Box_RandKey;//�ն�����ܳ�
    extern uint32 Lock_Box_SN;//�����ն˱��
    extern uint16 Lock_Box_Com_Index;
    extern uint8 LostLockBoxCommunication;
    extern uint8 GetNewMsg_LockBox[3];
    extern uint8 Lock_Vehicle_Active; //����״̬����
    extern uint8 Lock_Vehicle_Mode;   //����״̬      0 δ���� 1 1������      2 2������ 
    extern uint8 Heartbeat_Lock_Mode; //��������״̬  0 δ���� 1 ������������ 2 ��������    
    */
    //Lock_Level_1_Max_Speed 1�����������
    
    MSCANDrv_infoMSGS_TYPE infoMsgS;
    uint8 SendMsg_BCC_Code=0,Bcc_Index=0;

    if (GetNewMsg_LockBox[0]==1)       //�ն�Զ���������� 0=��Ч 1=1������ָ�� 2=2������ָ�� 3=����  4=����ָ��
    {  //Զ������Ӧ��
        GetNewMsg_LockBox[0]=0;//��1֡��1֡
        infoMsgS.infoMsgIDUI=0x1865F0A0;
        infoMsgS.bExtIDUB=1;
        infoMsgS.datMsgUBA[0]=Lock_Command;
        infoMsgS.datMsgUBA[1]=0;
        infoMsgS.datMsgUBA[2]=(uint8)((Lock_Box_RandKey+Lock_Command*2)/256);
        infoMsgS.datMsgUBA[3]=0;
        infoMsgS.datMsgUBA[4]=0;
        infoMsgS.datMsgUBA[5]=0;
        infoMsgS.datMsgUBA[6]=0;
        
        //SendMsg_BCC_Code  ���У��
        SendMsg_BCC_Code=0;
        for (Bcc_Index=0;Bcc_Index<7;Bcc_Index++) 
        {
        	  SendMsg_BCC_Code^=infoMsgS.datMsgUBA[Bcc_Index]	;	
        } 
        
        infoMsgS.datMsgUBA[7]=SendMsg_BCC_Code;       
        infoMsgS.numLenUB=8;
        infoMsgS.idxMsgPriorityUB=0x02;
        MSCANDrv_SendMsgCAN_A(infoMsgS); 
       
    }
    if (GetNewMsg_LockBox[1]==1)       //������������ 0=��Ч 1=��������ָ�� 2=���� 3=����  4=����ָ��
    {  //��������Ӧ��
        GetNewMsg_LockBox[1]=0;//��1֡��1֡
        infoMsgS.infoMsgIDUI=0x1867F0A0;
        infoMsgS.bExtIDUB=1;
        infoMsgS.datMsgUBA[0]=Heartbeat_Command;
        infoMsgS.datMsgUBA[1]=0;
        infoMsgS.datMsgUBA[2]=(uint8)((Lock_Box_RandKey+Heartbeat_Command*2)/256);
        infoMsgS.datMsgUBA[3]=0;
        infoMsgS.datMsgUBA[4]=0;
        infoMsgS.datMsgUBA[5]=0;
        infoMsgS.datMsgUBA[6]=0;
        
        //SendMsg_BCC_Code  ���У��
        SendMsg_BCC_Code=0;
        for (Bcc_Index=0;Bcc_Index<7;Bcc_Index++) 
        {
        	  SendMsg_BCC_Code^=infoMsgS.datMsgUBA[Bcc_Index]	;	
        } 
        
        infoMsgS.datMsgUBA[7]=SendMsg_BCC_Code;       
        infoMsgS.numLenUB=8;
        infoMsgS.idxMsgPriorityUB=0x02;
        MSCANDrv_SendMsgCAN_A(infoMsgS);
        
    }
    
    if (AccPed<50)
    {
        switch (Lock_Command) 
        {
            case 1:
                Lock_Vehicle_Mode=1;
                break;
            case 2:
                Lock_Vehicle_Mode=2;
                break;
            case 4:
                Lock_Vehicle_Mode=0;
                break;
            default:
                break;
        }
        
        
        switch (Heartbeat_Command) 
        {
            case 1:
                if (Heartbeat_Lock_Mode!=2){ Heartbeat_Lock_Mode=1;  }
                break;
            case 4:
                Heartbeat_Lock_Mode=0;
                break;
            default:
                break;
        }
    
    }
    

    
    //����Ѽ���������������
    if (Heartbeat_Lock_Mode==1)
    {   
        Lock_Box_Com_Index++;    //100ms +1 
        if (Lock_Box_Com_Index>Lock_Box_Com_Index_Max)  //10������ʱ
        {
            Heartbeat_Lock_Mode=2;
            LostLockBoxCommunication=1;
        }
    }

    if ((Heartbeat_Lock_Mode==2)&&(Heartbeat_Lock_Vehicle_Enable==1)&&(Motor_Speed/Motor_Vehicle_Speed_Scale_Get<20)) 
    {
        Lock_Vehicle_Active=1;     //���ն˳��ٵ���������
    }

    //�յ��ն�����������BCCУ��ͨ��
    if (GetNewMsg_LockBox[2]==1)
    {
        GetNewMsg_LockBox[2]=0;
        if ((Lock_Box_Com_Index<Lock_Box_Com_Index_Max)&&(Heartbeat_Lock_Mode==2)) 
        {
            Heartbeat_Lock_Mode=1;//�ָ�����������״̬
            LostLockBoxCommunication=0;
        }
    }
    
    
    
}

//�������ļ���    10ms����            2018.11.06 ��д
void Electric_Consume_Calculation(uint8 Pow_Down) 
{

    // sint16 Ins_Ele_1000;      //˲ʱ���1000ms      0.0~1000.0KWh/100Km 0.1ǧ��ʱÿ�ٹ���     (0~10000Wh/Km)��ʱÿ����
    // Electric_Consume_Calculation_Times 0~999
    //uint16 B_Voltage��sint16 B_Current   0.1V 0.1A
    // Vehicle_Speed=( abs(Motor_Speed_Average) /Motor_Vehicle_Speed_Scale_Get)+50;
    
    //uint8 Times_10=0;
    //uint8 Times_100=0; 
    //uint8 i=0;
    //sint32  Ins_Ele_Tmp=0,Ins_Ele_Tmp_Mathc=0;
    uint32  Vehicle_Speed_Tmp=0;  //0.00 km/h  ˲ʱ���� 
    
    //uint32 Vehicle_Speed_Add=1;     //���ٻ����ۼ�ֵ    0.1Km/h
    //sint32 Electric_Consume_Add=0;  //�繦�ʻ����ۼ�ֵ    0.01KW
    //uint32 Vehicle_Speed_Avg_Add=1;     // ƽ�� ���ٻ����ۼ�ֵ  0.1Km/h
    //sint32 Electric_Consume_Avg_Add=0;  //ƽ�� �繦�ʻ����ۼ�ֵ  0.01KW
    /*
    sint32 AV_Ele_Calc=0; //ƽ����ļ���ֵ
    uint16 Electric_Consume_Calculation_Avg_Times=0; //ƽ���ܺļ���ʱ�� 0~3600(2599)    
    uint16 Ins_Ele_NoCalc_Times=0;  //˲ʱ���δ�������  *1s
    uint16 AV_Ele_NoCalc_Times=0;   //ƽ�����δ�������  10min
    
          AV_Ele_Calc_Max_Times  600 // ƽ����ļ���ʱ�� ��λ����
    uint8 Remaining_Mil;//ʣ����� 0��255km      ��V1.2�濪ʼ 2km/bit 0~510
    uint16 Rated_Energy; //����� 0~409.5kwh
    uint8 SOC; 0~255    0.5%
    */
    
    //��ֹ��Сֵ ����Ϊ0

    
    if ( ((abs (Motor_Speed_Average))==0) || (Motor_Vehicle_Speed_Scale_Get==0) )
    {       
        Vehicle_Speed_Tmp=0; //��С0.0Km/h     ˲ʱ���� 
        //Ins_Ele_Tmp=0;

    }else 
    {
        Vehicle_Speed_Tmp=((uint32)(abs(Motor_Speed_Average)) * 10   /Motor_Vehicle_Speed_Scale_Get)   ;
        //��ǰ˲ʱ�ͺ�
        //Ins_Ele_Tmp=  (((sint32)Output_Voltage) * B_Current    / Vehicle_Speed_Tmp)/10;       
    }
   
    Vehicle_Speed_Add   = Vehicle_Speed_Add + Vehicle_Speed_Tmp ; 
    
    //Electric_Consume_Add= Electric_Consume_Add + ((sint32)Output_Voltage) * B_Current;
    Electric_Consume_Add= Electric_Consume_Add + ((sint32)Output_Voltage) * B_Current/1000;  //500.0V*1.0A=500.00W =0.50KW
    //1�����
    Electric_Consume_Calculation_Times=(Electric_Consume_Calculation_Times+1)  %100;   //0~99
    if (Electric_Consume_Calculation_Times==0) //�ﵽ1�룬����˲ʱ���
    {   
         //1���ڳ�������ʻ��600��δ��ʻ���ܺĴﵽ����������,����㲢���ݡ����� ;   
        if ( (Vehicle_Speed_Add>1500)    //1.5km/h ÿ�����
          || (Ins_Ele_NoCalc_Times>=  AV_Ele_Calc_Max_Times-1) 
          || (Electric_Consume_Add>=  2000000000/AV_Ele_Calc_Max_Times ) 
          || (Electric_Consume_Add<= -2000000000/AV_Ele_Calc_Max_Times ) )       
        
        {
            
            
            Vehicle_Speed_Avg_Add     = Vehicle_Speed_Avg_Add + Vehicle_Speed_Add-1;
            Electric_Consume_Avg_Add  = Electric_Consume_Avg_Add  + Electric_Consume_Add;

            //����ʻ��ˢ���Ǳ�            
            if (Vehicle_Speed_Add>600)
            {   
                //��ƽ�����
                Ins_Ele_1000=  ( Electric_Consume_Add / (Vehicle_Speed_Add/100)  );
                
                //�Ǳ���ʾ��ʽ��  
                if (Ins_Ele_1000>1275) //��ֵ���� 127.5 kwh/100km
                {
                    Ins_Ele=255;
                }else                 
                {
                    if (Ins_Ele_1000<0)
                    {
                        Ins_Ele=0;
                    }else
                    {
                        Ins_Ele=(uint8) (Ins_Ele_1000/5);
                    }
                }
            }

            Vehicle_Speed_Add=1;
            Electric_Consume_Add=0;
            Ins_Ele_NoCalc_Times=0;

        }else
        {   //����ʻ��������ۼӲ����� ������
            Ins_Ele_NoCalc_Times++;
        }
        
        
        
        
        Electric_Consume_Calculation_Avg_Times=(Electric_Consume_Calculation_Avg_Times+1)  %AV_Ele_Calc_Max_Times; //0~599  10���� AV_Ele_Calc_Max_Times=600
        //�ﵽ10���� ����ƽ�����
        if (Electric_Consume_Calculation_Avg_Times==0)
        {
            //��������ʻ��1Сʱδ��ʻ���ܺĴﵽ����������,����㲢���ݡ�����
            if  ((Vehicle_Speed_Avg_Add>1500*AV_Ele_Calc_Max_Times)      //1.5km/h  10���ӻ���   250��
              ||(AV_Ele_NoCalc_Times>= (3600/AV_Ele_Calc_Max_Times)-1)
              ||(Electric_Consume_Avg_Add >=2000000000) 
              ||(Electric_Consume_Avg_Add<= -2000000000) )
            
            {   

                //����ʻ��ˢ���Ǳ�  ˢ��ƽ���ܺ�
                if (Vehicle_Speed_Avg_Add>1500*AV_Ele_Calc_Max_Times)
                {
                    //10%����
                    //AV_Ele_Calc=9*(AV_Ele_Calc/10) +   ( Electric_Consume_Add  / (Vehicle_Speed_Add/100)  ) /10 ;
                    AV_Ele_Calc= Electric_Consume_Add  / (Vehicle_Speed_Add/100)   ;
                    //�Ǳ���ʾ��ʽ��
                    if (AV_Ele_Calc>1275) //��ֵ���� 127.5
                    {
                        AV_Ele=255;
                    }else                 
                    {
                        if ((AV_Ele_Calc<0)||( AV_Ele_Calc<((sint32)AV_Ele_Calc_Min))  )
                        {
                            //AV_Ele=(uint8)AV_Ele_Calc_Min/5;
                            AV_Ele=(uint8)((uint16)AV_Ele*9/10+   ( AV_Ele_Calc_Min/5)/10 );
                        }else
                        {
                            AV_Ele=(uint8)((uint16)AV_Ele*9/10+   ( AV_Ele_Calc/5)/10 );
                        }
                    }
                    Vehicle_Speed_Avg_Add=1;
                }
                //������ݣ������ۼ�
                Electric_Consume_Avg_Add=0;
                AV_Ele_NoCalc_Times=0;
            }else
            {
                AV_Ele_NoCalc_Times++;
            }
            
            //ʣ����̼���
            /*
            uint8 Remaining_Mil;//ʣ����� 0��255km      ��V1.2�濪ʼ 2km/bit 0~510
            uint16 Rated_Energy; //����� 0~409.5kwh
            uint8 SOC; 0~255    0.5%
            AV_Ele_Calc  0.0~1000.0KWh/100Km 0.1ǧ��ʱÿ�ٹ���
            */            
            if ((SOC>0)&&(Rated_Energy>0)&&(AV_Ele>0)  )
            {

               Remaining_Mil_Calc=  100* ( ((sint32) Rated_Energy*SOC/200) )/ AV_Ele ;

               
              
               if ((Remaining_Mil_Calc > 510)||(Remaining_Mil_Calc<0)  )
               {
                  Remaining_Mil=255;
               }else
               {
                  Remaining_Mil= (uint8) (Remaining_Mil_Calc /2);               
               }
                
                           
            }else
            {
                Remaining_Mil=0;
            }
            
            
                    
        }
    }
   
    /*   
   
    Times_10=Electric_Consume_Calculation_Times%10; //0~9
    Times_100=(Electric_Consume_Calculation_Times/10) %10; //0~9

    Ins_Ele_10[Times_10]=(sint16)Ins_Ele_Tmp;

    if (Times_10==9) //100ms  
    {
      
      Ins_Ele_Tmp_Mathc=0;
      for (i=0;i<10;i++) 
      {
          Ins_Ele_Tmp_Mathc=Ins_Ele_Tmp_Mathc+ Ins_Ele_10[i];
      }
      //100msƽ��
      Ins_Ele_100[Times_100]=(sint16) (Ins_Ele_Tmp_Mathc/10);
     
      if (Times_100==9) //1000ms
      {
          Ins_Ele_Tmp_Mathc=0;      
          for (i=0;i<10;i++) 
          {
              Ins_Ele_Tmp_Mathc=Ins_Ele_Tmp_Mathc+ Ins_Ele_100[i];
          }         
          //1sƽ��
          Ins_Ele_1000=(sint16)(Ins_Ele_Tmp_Mathc/10);
      }
      
    } 
    */
    //�ĵ�ǰ�����
    //Electric_Consume_Calculation_Times=(Electric_Consume_Calculation_Times+1)  %100;   //0~99 
    
    // �µ�ǿ�Ƽ������ڴ洢EEPROM
    if ((Pow_Down==1)&&(Vehicle_Speed_Avg_Add>1500*AV_Ele_Calc_Max_Times))
    {
        //����ʻ��ˢ���Ǳ�  ˢ��ƽ���ܺ�
                
        AV_Ele_Calc=   Electric_Consume_Add  / (Vehicle_Speed_Add/100)     ;
        
        //�Ǳ���ʾ��ʽ��
        if (AV_Ele_Calc>1275) //��ֵ���� 127.5
        {
            AV_Ele=255;
        }else                 
        {
            if ((AV_Ele_Calc<0)||( AV_Ele_Calc<((sint32)AV_Ele_Calc_Min))  )
            {
                AV_Ele=(uint8)((uint16)AV_Ele*9/10+   ( AV_Ele_Calc_Min/5)/10 );
            }else
            {
                AV_Ele=(uint8)((uint16)AV_Ele*9/10+   ( AV_Ele_Calc/5)/10 );
            }
        }        
        //������ݣ������ۼ�
        Vehicle_Speed_Avg_Add=1;
        Electric_Consume_Avg_Add=0;
        AV_Ele_NoCalc_Times=0;
    }
}









