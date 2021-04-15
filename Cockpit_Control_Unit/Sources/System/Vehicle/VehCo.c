/*******************************************************************************
* �������ƣ���������
* �ļ����� VehCo.c
* �������� �� ��������Э��
* ���ü�¼��
* ��ע�� 
* �޶���¼��
*		����		      �汾		  ����		����
*   
*******************************************************************************/

#include "GlobalDef.h"
#include "TypeDef.h"
#include "VehCo.h"
#include "VehCo_Type.h"
#include "CANNet.h"

//������ʷ�����趨������������
//#pragma DATA_SEG __GPAGE_SEG RUNPAR_EEPROM
#ifndef AECS_HW
      #pragma DATA_SEG __GPAGE_SEG RUNPAR_EEPROM
#else  
      #pragma DATA_SEG  NVRAM 
#endif
    uint16 VehCo_ctTestUW=0; //ʾ��EEPROM�洢����
    uint8 Vehco_State_Stop_Step;
    //Vin��ˢд����������ſ�ˢд��������ر����ˢд��������ؿ�ˢд����
    uint8 Rw_Vin_Times,Rw_Mot_Times,Rw_Bms_Times,Rw_MCU_Times;
    uint8 Vin_Code[21];
    uint8 Mot_Code[21];
    uint8 MCU_Code[21];  
    uint16 PowOn_Times;
    uint8 Flash_Times;
    uint32 INS_TD_LastSave;     //VCU�����Ǳ����?
     
       
//#pragma DATA_SEG DEFAULT
#ifndef AECS_HW
      #pragma DATA_SEG DEFAULT
#else  
      #pragma DATA_SEG S12X_RAM
#endif

//uint8  Air_Reservoirs_Over_Pressure_1=0,Air_Reservoirs_Over_Pressure_2=0;
//uint8  Air_Pressure_Ok=0;   //�ﵽ������
//uint8  Air_Pressure_Ok_OverTime=0;


uint16 Afterrun_Delay_Index;//�ػ��ӳټ���
uint8 VehCo_tiMonitorUB;
uint8 VehCo_stAfterrunUB; //�ػ�׼��״̬

uint8 AD_fliter_index;
uint32 Adtmp1;
uint32 Adtmp2;
uint16 uAD1;  //����1��ѹ
uint16 uAD2;  //����2��ѹ
uint32 Adtmp3,Adtmp4;
uint16 uAD3,uAD4;

uint16 AccPed1;
uint16 AccPed2;

uint16 AccPed;//���������ߴ�����������?
uint16 Breakk;//�ƶ�̤��״̬

uint16 AccPed_Differ;//˫���ſ��Ȳ�ֵ
uint8 Failure_Recovery_Delay[24]={0}; 
uint8 AccPed_Delay; //0���Ų����ӳ�

uint8 Err_AccPed=0,Err_AccPed_Index=0;


sint16 Percent100;
//uint16 FanStopDelay_Index=0;

uint8   RightTrunSignal_state=2;        //��ת���״�?
uint8   LeftTrunSignal_state=2;        //��ת���״�?
uint8 bLso;  
uint8 bHso;
//uint32 fSet;  //Ƶ���趨   0.01~5000.00Hz
//uint16 rSet;  //ռ�ձ��趨 0~10000
                                                                                      
uint16 Error_PowOn_Delay;//�ϵ�������ӳ�?
uint8  Fault_Detection=0; //���ϼ������?

uint8 R_Control_Switch=0; //Զ�̼�ʻ�����  

uint8 Door_Switch=0;      //Զ�̼�ʻ���ſ���
sint16 Vehco_Speed_Calc=0; //���㳵��   10��
                                        

uint16  D_Dang_Change_Delay_Index=0; 

uint8 Auto_Hold_Delay=0;  //ͣ����ɲ��ʱ��

uint8 Dang_R=0,Dang_D=0,Dang_N=0,Dang_P=0,  Want_Dang=0,  Want_Dang_last = 8;

uint8 Steering_Wheel_IN1=0,Steering_Wheel_IN2=0;  // 1��2�� ��ת ˳ʱ�� 1��2�� ��ת  00 ����  11 ����
uint16 Steering_Wheel_PWM_Out=50;      //0.01% PWM

sint16 Steering_Wheel_Angle_BaseCAN_Max=0,Steering_Wheel_Angle_BaseCAN_Min=0;  //ģ�ⷽ���������ԽǶ�
sint16 Wheel_Offset_Auto=0;              //��У׼ģ�ⷽ����0��ƫ��
uint8 Steering_Whee_Auto_Check_Mark=0;     //�������Լ�ɹ�?
uint8 Steering_Whee_Auto_Check_Step;  //0 δ����   1 ��ת��  2 ��ת���յ� 3 ��ת��  4 ��ת���յ� 5 ������   6 �ɹ�����  7ʧ��

uint8 Steering_Whee_Auto_Check_Still_Time=0;   //�����̲��Զ�תʱ��
uint8 Steering_Whee_Auto_Check_Slow_Index=0;   //�Զ����л������?

uint16 AccPed_CAN=0,Breakk_CAN=0, Clutch_CAN=0;
uint8 EPB_Switch_Want=0;   //0 �޶��� 1 ���� 2 �ͷ� 3 �н����ƶ�
uint8 EPB_Switch_LH=0,EPB_Switch_HL=0;

uint16  Steering_Whee_Check_Powon_Delay=0;
sint16 Steering_Wheel_Speed=0;
sint16 Steering_Wheel_Angle_BaseCAN_Last=0;
sint16 Pwm_Add=0;
uint16 BackZeroForce_PWM=0;

//�ӿ��ⲿ����
extern uint8  bDiSatus[24];     //DI�ڿ�����״̬
extern uint16 uADSatus[25];    //DI��ģ������ֵ
extern uint8  bDoSatus[20];     //DO�ڿ�����״̬
extern uint16 uAD4Satus[4];    
extern uint8 BattCD_bLogicOnUB; //�߼��翪��         //����ɾ��
extern uint16 BattCD_uUW;  //���ص�ѹ          //����ɾ��

//�����ⲿ��������
extern uint8 Vehco_Type_Test; //���Ͷ��� 1 ʵ���Ҽ�ʻ�� 2 �ּܼ�ʻ��
extern uint8   LostEPSCommunication;
extern uint16  LostEPSCom_Index;    
extern uint8   LostVehcoCommunication;
extern uint16  LostVehcoCom_Index;
extern uint8   LostVehcoStateCommunication;
extern uint16  LostVehcoStateCom_Index;
extern uint8   LostVehcoBreakCommunication;
extern uint16  LostVehcoBreakCom_Index;
extern uint8   LostVehcoSteering_Wheel_AngleCommunication;
extern uint16  LostVehcoSteering_Wheel_AngleCom_Index;


extern sint16 Steering_Wheel_Angle_BaseCAN ;
extern uint8  AccPed_AD255 ;
extern uint8  Breakk_AD255 ;
extern uint8  Clutch_AD255 ;
extern uint8  Steering_Wheel_Angle_Mot_Temp ;
extern uint8  Steering_Wheel_Angle_Life ; 

extern sint16 Steering_Wheel_Angle_Code; //��������ת����  -32768,360��/5000 ��תΪ��
extern uint8  Steering_Wheel_Angle_ZeroMark; //�������Ƿ񾭹�0��У��


extern uint8  EPS_Contral_Mode;//EPS����ģʽ
extern sint16 EPS_Demand_Angle;//EPS�Ƕ����� 0.1�Ȳ���   -1080.0~1080.0��
extern uint8  EPS_Angle_MaxSpeed;//EPS �����ٶ�

extern sint8 Vehco_Acceleration_Command;   //���ٶ�ָ�� -12.8~12.7
extern uint8 EPB_Command;    //������ɲָ�� 0 �޶��� 1 ���� 2 �ͷ� 3 �н����ƶ�
extern uint8 Dang_Command;   //��λָ�� 0 N 1 D 2 R 3 ����
extern uint8 Emergency_Braking;  //�����ƶ�ָ��

extern uint8 DriverMode_Command; //�Լ�ģʽ״̬ 0 �ֶ� 1 ���� 2 ���� 3 Զ��
extern uint8 Door_Command;       //���ſ������� 0 �޶��� 1 ���� 2 ���� 3 ����
extern uint8 Beep_Command;       //���ȿ���     0 �޶��� 1 ���� 2 �ر� 3 ����
extern uint8 R_DriverSystem_State;       //Զ�̼�ʻϵͳ״̬ 0 ������ 1 �������? 2 �Ͽ��� 3 �Ͽ�
extern uint8 R_DriverSystem_Level;       //Զ�̼�ʻϵͳ���ż��� 0 �����ţ����̽ӹ� 1 ������ ���˹����� 2 ���״�?����ȫ����
extern uint8 R_DriverSystem_Fault_Level; //Զ�̼�ʻϵͳ���ϼ��� 0 �޹��� 1 ��΢ ��ʾ 2 һ�����? ���� 3 ���ع��� �˳�Զ�̼�ʻ״̬
extern uint8 AC_Mode;          //������ů�յ����� 0 �ػ� 1 ���� 2 ���� 3 �Զ�
extern uint8 AC_Temperature;   //������ů�յ������¶� 0.5 ƫ��16

extern uint8 R_Break_Mode;   //Զ���ƶ�����   0x00��δ����,���ؿ��� 0x01������ƶ�������ִ��Զ������ָ�� 0x02:�ںϼ��ȡ���? 
extern uint8 R_Break_Rang;     //Զ���ƶ������� 0~100%

//***************����Ϊ�ӳ�����������********************

extern uint8 EPS_Run_Mode;//EPS���ص�ǰ����ģʽ
extern sint16 EPS_Driver_Torque;//EPS���ؼ�ʻԱ���������� -12.8~12.7Nm
extern sint16 EPS_Torque; //EPS�������? 0.3Nm/bit -38.4~38.1Nm
extern sint16 EPS_Angle; //EPS��ǰ�Ƕ� 0.1�Ȳ���   -1080.0~1080.0��
extern uint8 EPS_Temp;//EPS�������¶� 2��/bit 0~126
extern uint8 EPS_ERR_Level;    //EPS���ϼ���



extern uint8   EPB_State;                //EPB״̬
extern uint8   Dang_State;               //������λ״̬
extern sint16  Motor_Speed,Motor_Torque;   //����?�١����Ť��?
extern sint8   Vehco_Acceleration_Back; //������ǰ�ϱ����ٶ� -12.8~12.7

extern uint8   DriverMode_State;       //�Լ�ģʽ״̬
extern uint8   DoorState;              //����״̬
extern uint8   Cloud_Takeover_Cmd;  //Զ�̼�ʻ�л�����ѡ��    1�ƶ˽ӹ�  0ң�����ӹ�
extern sint16  Vehco_Speed;            //����   
extern uint8   SOC;                    //SOC ����0.5% 0~100%
extern uint8   SmartPowerCommand;       //����ϵͳ��Դָ��  00 ����  01 �ػ�
extern uint8   Change_State;           //���״�? 00 ������ 01������ 02 �����? 03 ��������Ҫ���?
extern uint8   Change_Connect_State;   //�������״�? 0 δ���� 1 ���ǹ�Ѳ���?
extern uint8   Vehco_Fault_Level;      //�������ϼ���

extern uint16  B_Voltage;    //��ص��? 0~800.0V
extern sint16  B_Current;    //��ص���? -500.0~500.0A

extern uint8   Vehco_Power_State;    //��������״̬ 00 INIT ON�� 01 READY 02 ���� 03 �����޸�ѹ

extern uint8   Isolation_Level;      //��Ե���ϵȼ� 0 ���� 1 һ�� 2 ���� 3 ���� 
extern uint8   MCU_Fault_Level;      //��ع��ϼ���?  0 ���� 1��΢ 2 һ�� 3 ����
extern uint8   BMS_Fault_Level;      //BMS���ϼ���   0 ���� 1��΢ 2 һ�� 3 ����

extern uint8   Vehco_Break_Mode;     //�ƶ�������ģʽ 0x00��δ����,���ؿ��� 0x01������ƶ�������ִ��Զ������ָ�� 0x02:�ںϼ��ȡ���?
extern uint8   Vehco_Break_Rang;     //�ƶ���״̬  0~100%




//���ݳ���ѡ�����߱궨ֵ
extern  uint16 * __far AccPedCD_rUW_CUR_Addr;   //���ſ���
extern  uint16 * __far AccPedCD_rUW_CUR2_Addr;  //���ſ���2
extern  uint16 * __far AccPed_Rip_Addr;         //��������
extern  uint16 * __far Breakk_rUW_CUR_Addr;     //�ƶ�̤�����?

extern uint16 Motor_Vehicle_Speed_Scale_Get;//ת��-���ٱ���
extern uint16 * __far   AccPed_Acceleration_Addr;  //���ſ���-���ٶ�
extern uint16 * __far   Break_Acceleration_Addr;   //�ƶ����?-�ƶ�������%

//���ݳ���ѡ��ֵ�궨ֵ
extern uint8 AccPedCD_TYPE;   //�������� 1 ˫�� 2 ����
extern uint8 Breakk_Type;     //�ƶ�����  0����·������ 1��˫·����������  2����·ģ���� 3��˫·ģ���� 
    
#ifndef AECS_HW
      #pragma CODE_SEG DEFAULT
#else  
      #pragma CODE_SEG VCU_CODE 
#endif



                              
//����������ʼ��
//��VehCo_stNewUB=Init״̬ʱ����һ��
void VehCo_Init(void)
{
    srand(2);
    Fault_Detection=0;
    Afterrun_Delay_Index=0;
    Vehco_Type_Test=Vehicle_Type;
    Vehicle_Single_Variable_Set();  //��ֵ����ӳ��
    Vehicle_Type_Set ();            //���߱�ӳ��
    
}



//��������10ms��׼��������
void VehCo_Manage10ms(void)
{
   
    Vehicle_Single_Variable_Set();  //��ֵ����ӳ��
    Vehicle_Type_Set ();            //���߱�ӳ��


        
    //�ײ�IO�ӿ�ת��
    VehCo_Interface(); 
    
    //���´������ź�
    VehCo_UpdateSig();

    
    //��ϴ������ź�?  50ms���?
    VehCo_tiMonitorUB=(uint8)((VehCo_tiMonitorUB+1)%_VEHCO_TIME_MONITOR_PERIOD);   //_VEHCO_TIME_MONITOR_PERIOD  10
    if((VehCo_tiMonitorUB==0)||(VehCo_tiMonitorUB==5))
    {
        //����״̬���?    
        VehCo_Monitor();
        if(VehCo_tiMonitorUB==0)
        {
            Steering_Wheel_Speed= Steering_Wheel_Angle_BaseCAN -Steering_Wheel_Angle_BaseCAN_Last;
            
            Steering_Wheel_Angle_BaseCAN_Last=Steering_Wheel_Angle_BaseCAN;
        
            //Speed_To_Pwm_Add
            if ((Steering_Whee_Auto_Check_Step >0) &&(Steering_Whee_Auto_Check_Step<6) )
            {
              
                Pwm_Add= Intpol_s16s16 ( (uint16 * __far)&Speed_To_Pwm_Add,abs(Steering_Wheel_Speed));
            }
        }

        
        
        
    }
    
    
    
    //�ϵ���ϼ���ӳ� 500ms 
    if (Error_PowOn_Delay <= Error_PowOn_Delay_Max ) 
    {
        Fault_Detection=0;
        Error_PowOn_Delay++;
    }else
    {
        Fault_Detection=1;
        Error_PowOn_Delay=Error_PowOn_Delay_Max;
        if (Rtest_whee==1)
        {
            Steering_Whee_Check_Powon_Delay=0;
            Steering_Whee_Auto_Check_Mark=0;
            Steering_Whee_Auto_Check_Step=0;
            Steering_Whee_Auto_Check_Slow_Index=0;
            Steering_Wheel_Angle_BaseCAN_Max=0;
            Steering_Wheel_Angle_BaseCAN_Min=0;
        }
        
        if (Steering_Whee_Check_Powon_Delay <500)
        {
            Steering_Whee_Check_Powon_Delay++;
        }else 
        {
            if (Steering_Whee_Auto_Check_Mark==0)
            {
                //ģ�ⷽ�����Զ�У��
                Steering_Whee_Auto_Check (); 
            }
        
        }
        
        
        if (Steering_Whee_Auto_Check_Mark==1)
        {
            //����������
            BackZeroForce_Angle_Calc ()  ;
          
            //�Զ�����
            Steering_Whee_Back_Zero (); 
        }
        
        
        if (AccPedCD_TYPE==4)
        {
            //CAN����ɲ������źŴ���?
            CAN_AccPed_Breakk();         
        }
        
        //���������ж�
        Vehco_Errpr_Judge();
        //��ʻģʽ�ж�
        Vehco_Driver_Mode();
        //Զ�̵�λ����
        Vehco_Dang ();   
        //�����̽Ƕȿ���
        // Steering_Wheel () ; 
        //Զ�̼��ٶȿ���
        R_Vehco_Acceleration () ;               
        //�Զ�EPB
        Auto_Hold_EPB (); 

    }
 
     //�趨DO���?
    VehCo_DOset(0);
    //���´������ź�
    VehCo_UpdateSig();    
 
    //��������״̬��
    VehCo_StateMachine();
    
    //�ػ�׼��
    VehCo_Afterrun();
    
    //ʵ�����?
    //VehCo_Test();
    
    //ʾ��    
    /**********************************************************************************************************/
    //idxIncUB=0;
             
    if((Test_bSaveCmdUW_C==1)&&(EEPROMDrv_bSaveInstantUB==0))//��ֵ�궨����ʾ��
    {
        //����EEPROM�Ĵ洢
        EEPROMDrv_bSaveInstantUB=1;
    }
 
    
    /**********************************************************************************************************/
    
}



//��������1ms��׼��������
void VehCo_Manage1ms(void)
{
    //AD_fliter_index    _AD_fliter_Cycles       uAD1Staus[10]      2·����10�βɼ� uAD1 uAD2

    #ifndef AECS_HW //�׿�
      Adtmp1=Adtmp1+ATDDrv_GetChanResult(1);
      Adtmp2=Adtmp2+ATDDrv_GetChanResult(2);
      Adtmp3=Adtmp3+ATDDrv_GetChanResult(3);
      Adtmp4=Adtmp4+ATDDrv_GetChanResult(4);
      
    #else           //���׿�˹
      Adtmp1=Adtmp1+ATDDrv_GetChanResult(_ATD_INDEX_VLT1_CHAN);
      Adtmp2=Adtmp2+ATDDrv_GetChanResult(_ATD_INDEX_VLT2_CHAN);
      Adtmp3=Adtmp3+ATDDrv_GetChanResult(_ATD_INDEX_VLT3_CHAN);
      Adtmp4=Adtmp4+ATDDrv_GetChanResult(_ATD_INDEX_VLT4_CHAN);      
    #endif



    AD_fliter_index=AD_fliter_index+1;
    if (AD_fliter_index==_AD_fliter_Cycles) 
    {
        uAD1=(uint16)(Adtmp1/_AD_fliter_Cycles);
        uAD2=(uint16)(Adtmp2/_AD_fliter_Cycles);
        uAD3=(uint16)(Adtmp3/_AD_fliter_Cycles);
        uAD4=(uint16)(Adtmp4/_AD_fliter_Cycles);
        
        Adtmp1=0;
        Adtmp2=0;
        Adtmp3=0;
        Adtmp4=0;

        AD_fliter_index=0;
    }
    //��������Ҫ���ٿ��ƣ����õ�1ms������

    if (Vehco_Type_Test==2)  //���Ƽ�ʻ��
    {
        bDoSatus[8]=Steering_Wheel_IN1;
        bDoSatus[9]=Steering_Wheel_IN2; 
             

        
        #ifndef AECS_HW    //�׿�
            //    void LSPWMDrv_Interface(uint8 idxChanUB,uint32 fSetUI,uint16 rSetUW); 
            //��������?���ĵͱ�����Ƶ�ʼ�ռ�ձȣ�Ƶ�ʾ���0.01Hz�����Ŵ�100����ռ�ձȾ���Ϊ0.01%�����Ŵ�100��

            PSWTDrv_Interface(8,bDoSatus[8]); 
            PSWTDrv_Interface(9,bDoSatus[9]);
                        
            LSPWMDrv_Interface(2,Fsetpwm,Steering_Wheel_PWM_Out); // 10k
        #else              //���׿�˹
            //    void DrvPWM_Interface(uint8 idxChanUB,uint16 rSetUW); //��������?���ĵͱ�����ռ�ձȣ�ռ�ձȾ���Ϊ0.01%�����Ŵ�100��

      

 
        #endif    
    
    }     

    
}


//CAN����ɲ������źŴ���?
void  CAN_AccPed_Breakk(void) 
{   uint16 tmp=0;
    /*
    extern uint8  AccPed_AD255 ;
    extern uint8  Breakk_AD255 ;
    extern uint8  Clutch_AD255 ;
    uint16 AccPed_CAN=0,Breakk_CAN=0, Clutch_CAN=0;
    uint8 EPB_Switch_Want=0;   //0 �޶��� 1 ���� 2 �ͷ� 3 �н����ƶ�
    uint8 EPB_Switch_LH=0,EPB_Switch_HL=0;
    */
    AccPed_CAN=Intpol_u16u16 ( (uint16 * __far)&AccPedCD_CUR_T02,AccPed_AD255);
    Breakk_CAN=Intpol_u16u16 ( (uint16 * __far)&Breakk_CUR_T02,Breakk_AD255);
    tmp=Intpol_u16u16 ( (uint16 * __far)&Clutch_CUR_T02,Clutch_AD255);

    if ( ( tmp> Level_High) &&   (Clutch_CAN<=Level_High)  )
    {
        EPB_Switch_HL=0;
        EPB_Switch_LH=1;
     
    
    }
    
    if ( ( tmp< Level_Low) &&   (Clutch_CAN>=Level_Low)  ) 
    {
        EPB_Switch_HL=1;
        EPB_Switch_LH=0;
        
    }
    
    Clutch_CAN=tmp;
}



//���²����ź�
void VehCo_UpdateSig(void)
{
    //ÿ10ms����һ�Σ����ڻ�ȡģ����������������״̬
    uint8 Index=0;
    uint16 AccPed_Tmp=0;
    uint8 Drive_Mode_Switch_Tmp=0;


    bLso=0;
    
    for(Index=1;Index<8;Index++)//7·LSO״̬  0·��������
    {
        bLso|=((bDoSatus[Index]&&1)<<Index);
    } 
    bHso=0; 
    for(Index=0;Index<8;Index++)//8·HSO״̬ 
    {
        bHso|=((bDoSatus[Index+8]&&1)<<Index);
    }   

    // 紧急制�?  
    {
       Emergency_Braking = bDiSatus[14];
    }
    
    // R_Control_Switch = bDiSatus[3];   //Զ�̼�ʻ�����
    R_Control_Switch=Cloud_Takeover_Cmd; //Զ�̼�ʻ�����
   
    //Break  �ƶ�̤��״̬���� 
    //Breakk_Type �ƶ�̤������ 0 ��·������ 1 ˫·����������  2 ��·ģ���� 3 ˫·ģ����
    switch (Breakk_Type)  
    {
        default:
        case 0:
            Breakk=bDiSatus[13]*10000;
            break;
        case 1:
            Breakk=bDiSatus[13]*10000;
            break;  
        case 2:    //2 ��·ģ����     ʵ����
            //Breakk=Intpol_u16u16(Breakk_rUW_CUR_Addr,uAD3);

            Breakk=(uint16)  ((((uint32)(Breakk))*(Percent_Filter_And_All[0]-Percent_Filter_And_All[3]) + ((uint32)(Intpol_u16u16(Breakk_rUW_CUR_Addr,uAD3)))*(Percent_Filter_And_All[3]))/Percent_Filter_And_All[0]);

            break;    
        case 3:    //3 ˫·ģ����
            Breakk=Intpol_u16u16(Breakk_rUW_CUR_Addr,uAD3);
            break;    
        case 4:     //CAN            ����
            Breakk= Breakk_CAN;
            break;

    }    


    
    switch (AccPedCD_TYPE)  //�������� 1 ˫�� 2 ���� 4 CAN
    {

        default:
            break;
        case 1:
             //uAD2   AccPedCD_rUW_CUR_Addr  ˫���ŰٷֱȲ������?
            AccPed1=Intpol_u16u16(AccPedCD_rUW_CUR_Addr,uAD1);  //0.75��4.1V
            AccPed2=Intpol_u16u16(AccPedCD_rUW_CUR2_Addr,uAD2); //0.375��2.05V
            //���̶�˫�������ݽ�����Ч�ж�!   AccPed_Differ_Limit
            AccPed_Differ=abs(AccPed1-AccPed2);

            //���Ź����ж�
           
            if ( ((AccPed_Differ>=AccPed_Differ_Limit) 
                //||(uAD1<83)||(uAD1>3685)||(uAD2<83)||(uAD2>3685) )
                )&&(Fault_Detection==1)
                
                )
            {
                if ((Err_AccPed==0)&&(Err_AccPed_Index<Err_AccPed_Index_Max))  
                {
                    Err_AccPed_Index++;
                    
                }else
                {
                    
                    Failure_Recovery_Delay[7]=200; //���Ź���2��ָ����?
                    Err_AccPed=1;
                    AccPed=0;
                    AccPed_Tmp=0;            
                }
            } 
            else 
            {   //��������
                if (Failure_Recovery_Delay[7]==0)
                {
                    Err_AccPed=0;
                    Err_AccPed_Index=0;
                    //�������ߴ���
                    AccPed_Tmp=Intpol_u16u16(AccPed_Rip_Addr,AccPed1); 
                } 
                else
                {
                    --Failure_Recovery_Delay[7];
                }
            }           
            break;

        case 4:
            AccPed1=AccPed_CAN; 
            AccPed_Tmp=Intpol_u16u16(AccPed_Rip_Addr,AccPed1);
            break;
    
    } 
 

    //����ɲ��ͬʱ�ȣ�ȡ�����ſ���
    if ((AccPed_Tmp>AccPed_BreakErr_Max)&& (Breakk> AccPed_BreakErr_Max)) 
    {
        AccPed_Tmp=0;
    }


    //�˲�����   ��ʾ
   // Percent100=Percent_Filter_And_All[0]-Percent_Filter_And_All[1]; 
   
    if (AccPed_Tmp>AccPed) 
    {   //����
        AccPed=(uint16)  ((((uint32)(AccPed))*(Percent_Filter_And_All[0]-Percent_Filter_And_All[1]) + ((uint32)(AccPed_Tmp))*(Percent_Filter_And_All[1]))/Percent_Filter_And_All[0]);
        if  ((AccPed>9900)  &&  (AccPed_Tmp >=9990) )
        {
             AccPed= AccPed_Tmp;
        }
    }else 
    {   //����
        AccPed=(uint16)  ((((uint32)(AccPed))*(Percent_Filter_And_All[0]-Percent_Filter_And_All[2]) + ((uint32)(AccPed_Tmp))*(Percent_Filter_And_All[2]))/Percent_Filter_And_All[0]);
    }
   
   
   
   
   
   
    //�ſ��Ƴ���
    
    //Door_Switch=bDiSatus[4];      //Զ�̼�ʻ���ſ���
    //uint8 Door_Command=0;       //���ſ������� 0 �޶��� 1 ���� 2 ���� 3 ����
    if ((Door_Switch==0) && (bDiSatus[4]==1) ) //������
    {
        if (DoorState==1)
        {
            Door_Command=1;
        
        }else 
        {
            Door_Command=2;
        }
    
    
    }
    if ((Door_Switch==1) && (bDiSatus[4]==0) ) //�½���
    {
        Door_Command=0;
    }
    
    Door_Switch=bDiSatus[4];
    //���㳵��    
    Vehco_Speed_Calc=(sint16) (  ((sint32) Motor_Speed) *10  /  Motor_Vehicle_Speed_Scale_Get);
    
     if(1==bDiSatus[6]) //�ҳ���״̬����
     {
          RightTrunSignal_state=1;
     } else{
          RightTrunSignal_state=2;
     }
    if(1==bDiSatus[5]) //�󳵵�״̬����
     {
          LeftTrunSignal_state=1;
     } else{
          LeftTrunSignal_state=2;
     }
       
}


void VehCo_DOset (uint8 Do_Off) 
{   uint8 Index;
    if (Do_Off==0) 
    { //���������?
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    

    } else 
    {   //�ر�����DO
        for(Index=1;Index<16;Index++) 
        {
            bDoSatus[Index]=0;
        }
    }

  
    
    
}





//����״̬���?
void VehCo_Monitor(void)
{
    //ÿ50ms���һ��?

   


    

}


//����������
void BackZeroForce_Angle_Calc (void) 
{ //������
  //EPS_Demand_Angle
  uint16  Tmp=0;
  if ((EPS_Demand_Angle < (Die_Angle/2) )&& ( EPS_Demand_Angle >  (-1 *Die_Angle/2))) 
  {
      Tmp=0;
  }else
  {
      Tmp=Intpol_s16u16((uint16 * __far)& BackZeroForce_Angle, EPS_Demand_Angle); 
  
  }
  BackZeroForce_PWM=Tmp;

}

//�Զ�����
void  Steering_Whee_Back_Zero (void) 
{
    if (EPS_Demand_Angle >Die_Angle)
    {
        Steering_Wheel_IN1=1;
        Steering_Wheel_IN2=0;
    
    }else if (EPS_Demand_Angle < -1* Die_Angle) 
        {
            Steering_Wheel_IN1=0;
            Steering_Wheel_IN2=1;
        
        }else
        {
            Steering_Wheel_IN1=0;
            Steering_Wheel_IN2=0;
        
        }
    Steering_Wheel_PWM_Out=BackZeroForce_PWM;//(uint16)((sint16)Steering_Whee_Auto_Check_Pwm  )  ;
        
        

}



//ģ�ⷽ�����Զ�У��
void Steering_Whee_Auto_Check (void) 
{
    /*
    uint8 Steering_Wheel_IN1=0,Steering_Wheel_IN2=0;  // 1��2�� ��ת ˳ʱ�� 1��2�� ��ת  00 ����  11 ����
    uint16 Steering_Wheel_PWM_Out=0;      //0.01% PWM

    sint16  Steering_Wheel_Angle_BaseCAN_Max=0,
            Steering_Wheel_Angle_BaseCAN_Min=0;  //ģ�ⷽ���������ԽǶ�
    sint16 Wheel_Offset_Auto=0;                 //��У׼ģ�ⷽ����0��ƫ��
    uint8 Steering_Whee_Auto_Check_Mark=0;     //�������Լ�ɹ�?
   
    extern uint8   LostVehcoSteering_Wheel_AngleCommunication;
    extern uint16  LostVehcoSteering_Wheel_AngleCom_Index;


    extern sint16 Steering_Wheel_Angle_BaseCAN ;
    extern uint8  AccPed_AD255 ;
    extern uint8  Breakk_AD255 ;
    extern uint8  Clutch_AD255 ;
    extern uint8  Steering_Wheel_Angle_Mot_Temp ;
    extern uint8  Steering_Wheel_Angle_Life ;     
    
    uint8 Steering_Whee_Auto_Check_Step;  //0 δ����   1 ��ת��  2 ��ת���յ� 3 ��ת��  4 ��ת���յ� 5 ������   6 �ɹ�����  7ʧ��   
    Steering_Whee_Auto_Check_Slow_Index  �Զ����л������?
    */
    
    
    if   ((Steering_Whee_Auto_Check_Mark==0) && (LostVehcoSteering_Wheel_AngleCom_Index<10))
    {
        switch (Steering_Whee_Auto_Check_Step) 
        {
            case 0:   //0 δ����
                Steering_Wheel_IN1=0;
                Steering_Wheel_IN2=1;
                Steering_Wheel_PWM_Out=(uint16)((sint16)Steering_Whee_Auto_Check_Pwm+Pwm_Add);
                Steering_Wheel_Angle_BaseCAN_Max=Steering_Wheel_Angle_BaseCAN;
                Steering_Whee_Auto_Check_Still_Time=0;
                Steering_Whee_Auto_Check_Step=1;
                
                break;
            case 1:   //1 ��ת��
                Steering_Wheel_PWM_Out=(uint16)((sint16)Steering_Whee_Auto_Check_Pwm+Pwm_Add);
                if (Steering_Wheel_Angle_BaseCAN > Steering_Wheel_Angle_BaseCAN_Max)
                {
                    Steering_Wheel_Angle_BaseCAN_Max= Steering_Wheel_Angle_BaseCAN;
                    if (Steering_Whee_Auto_Check_Still_Time>0) { Steering_Whee_Auto_Check_Still_Time--;  }
                }else
                {
                    if (Steering_Whee_Auto_Check_Still_Time < Steering_Whee_Auto_Check_Still_Time_Max)
                    {
                        Steering_Whee_Auto_Check_Still_Time++;
                    }else
                    {
                        //Steering_Wheel_PWM_Out=0;
                        Steering_Whee_Auto_Check_Step=2;
                    }
                }
          
                break;
            case 2:   // 2 ��ת���յ�
                Steering_Wheel_Angle_BaseCAN_Max= Steering_Wheel_Angle_BaseCAN;
                Steering_Wheel_IN1=1;
                Steering_Wheel_IN2=0;
                Steering_Wheel_PWM_Out=(uint16)((sint16)Steering_Whee_Auto_Check_Pwm+Pwm_Add);
                Steering_Wheel_Angle_BaseCAN_Min=Steering_Wheel_Angle_BaseCAN;
                Steering_Whee_Auto_Check_Still_Time=0;
                Steering_Whee_Auto_Check_Step=3;                
                
                               
                break;
            case 3:   //3 ��ת��
                Steering_Wheel_PWM_Out=(uint16)((sint16)Steering_Whee_Auto_Check_Pwm+Pwm_Add);
                if (Steering_Wheel_Angle_BaseCAN < Steering_Wheel_Angle_BaseCAN_Min)
                {
                    Steering_Wheel_Angle_BaseCAN_Min= Steering_Wheel_Angle_BaseCAN;
                    if (Steering_Whee_Auto_Check_Still_Time>0) { Steering_Whee_Auto_Check_Still_Time--;  }
                }else
                {
                    if (Steering_Whee_Auto_Check_Still_Time < Steering_Whee_Auto_Check_Still_Time_Max)
                    {
                        Steering_Whee_Auto_Check_Still_Time++;
                    }else
                    {
                        //Steering_Wheel_PWM_Out=0;
                        Steering_Whee_Auto_Check_Step=4;
                    }
                }          
                break;
            case 4: //4 ��ת���յ�
                Steering_Wheel_Angle_BaseCAN_Min= Steering_Wheel_Angle_BaseCAN;
                Steering_Wheel_IN1=0;
                Steering_Wheel_IN2=1;
                Steering_Wheel_PWM_Out=(uint16)((sint16)Steering_Whee_Auto_Check_Pwm+Pwm_Add);
                Steering_Whee_Auto_Check_Still_Time=0;
                Steering_Whee_Auto_Check_Step=5;                 


                Wheel_Offset_Auto=  (Steering_Wheel_Angle_BaseCAN_Min + Steering_Wheel_Angle_BaseCAN_Max) /2;
                                          
                break;
            case 5: //5 ������
                Steering_Wheel_PWM_Out=(uint16)((sint16)Steering_Whee_Auto_Check_Pwm+Pwm_Add);
                //��ת����
                if   (Steering_Wheel_Angle_BaseCAN < (Wheel_Offset_Auto-Die_Angle))
                {   
                    if (Steering_Wheel_IN1==1) {  Steering_Whee_Auto_Check_Slow_Index++;         }
                    Steering_Wheel_IN1=0;
                    Steering_Wheel_IN2=1;
                    Steering_Wheel_PWM_Out=(uint16)((sint16)Steering_Whee_Auto_Check_Pwm+Pwm_Add);
                    if   (Steering_Wheel_Angle_BaseCAN < (Wheel_Offset_Auto-Die_Angle-100)) 
                    {
                        Steering_Wheel_PWM_Out=(uint16)((sint16)Steering_Whee_Auto_Check_Pwm+Pwm_Add) ;                          
                    }
                }
                //��ת����
                if   (Steering_Wheel_Angle_BaseCAN > (Wheel_Offset_Auto+Die_Angle))
                {   
                    if (Steering_Wheel_IN1==0) {  Steering_Whee_Auto_Check_Slow_Index++;         }                
                    Steering_Wheel_IN1=1;
                    Steering_Wheel_IN2=0;
                    Steering_Wheel_PWM_Out=(uint16)((sint16)Steering_Whee_Auto_Check_Pwm+Pwm_Add);
                    if   (Steering_Wheel_Angle_BaseCAN > (Wheel_Offset_Auto+Die_Angle+100)) 
                    {
                        Steering_Wheel_PWM_Out=(uint16)((sint16)Steering_Whee_Auto_Check_Pwm+Pwm_Add) ;                          
                    }
                }         
                //�������?
                if ( (Steering_Whee_Auto_Check_Slow_Index>3)||
                    ( (Steering_Wheel_Angle_BaseCAN < (Wheel_Offset_Auto+Die_Angle))
                      &&          
                      (Steering_Wheel_Angle_BaseCAN > (Wheel_Offset_Auto-Die_Angle))  )
                   )  
                {
                    Steering_Wheel_IN1=0;
                    Steering_Wheel_IN2=0;
                    //Steering_Wheel_PWM_Out=0;
                    //�����ܽǶȲ������ж�ʧ��
                    if ((Steering_Wheel_Angle_BaseCAN_Max -Steering_Wheel_Angle_BaseCAN_Min) >800 )
                    {
                        Steering_Whee_Auto_Check_Step=6;
                    }else
                    {
                        Steering_Whee_Auto_Check_Step=7;
                    }
                    
                }
          
          
                break;
            case 6: //�ɹ����� 
                Steering_Whee_Auto_Check_Mark=1;
                 Steering_Wheel_PWM_Out=(uint16)((sint16)Steering_Whee_Auto_Check_Pwm+Pwm_Add);
                break;
        
        
            case 7: //ʧ��
                Steering_Wheel_IN1=0;
                Steering_Wheel_IN2=0;
                //Steering_Wheel_PWM_Out=0;  
                Steering_Whee_Auto_Check_Mark=0;          
            default: 
                Steering_Whee_Auto_Check_Step=7;
                break;
        
        }
    }
}



//�Զ�EPB
void Auto_Hold_EPB (void) 
{  
    //��ԭʼ�汾δ��  
 /*
    uint8 EPB_Command;    //������ɲָ�� 0 �޶��� 1 ���� 2 �ͷ� 3 �н����ƶ�
    
    uint8   EPB_State;                //EPB״̬
    sint16 Vehco_Speed_Calc=0; //���㳵��   10��
    sint16 Vehco_Speed ; //���㳵��   10��
    uint8 R_Break_Rang;     //Զ���ƶ������� 0~100%
    uint8 Auto_Hold_Delay=0;  //ͣ����ɲ��ʱ��
    uint8 Flag_IO3_HIGH;
    
    //ͣ����ɲ��δδ����ɲ1�룬�Զ�������ɲ
    if ((Vehco_Speed_Calc<5)&&(Vehco_Speed==0)&&(EPB_State!=1)&& (R_Break_Rang>0)) 
    {
        if (Auto_Hold_Delay<Auto_Hold_Delay_Max)
        {
            Auto_Hold_Delay++;
        }else
        {
            EPB_Command=1;
        }
    }

    if ((Vehco_Acceleration_Command>1)&&(EPB_State==1)&&(Motor_Torque>Auto_Hold_Release_Torque)) 
    {
        Auto_Hold_Delay=0;
        EPB_Command=2;
        
    }

    //����EPB̤��
    if (EPB_Switch_LH==1)
    {
        if (EPB_State==1) {   EPB_Command=2;     }
        if (EPB_State==2) {   EPB_Command=1;     } 
        EPB_Switch_LH=0; 
    }
    

    //����ִ�����? 
    if (EPB_Command==EPB_State) 
    {
        EPB_Command=0;
       // EPB_Switch_Want=0;
    }

*/ 


/*
   //�㶯�ź�
   uint8 Flag_IO3_HIGH = 0;   //bDiSatus[3]�ĵ�ƽΪ�߱�־λ
   uint8 Flag_EPB_Switch = 0;
      
    if(bDiSatus[3])
    {
        Flag_IO3_HIGH = 1;
    }
    else if ((Flag_IO3_HIGH = 1) && (!bDiSatus[3]))
    {
        Flag_EPB_Switch = 1;
        Flag_IO3_HIGH = 0;
    }
    else 
    {
        EPB_Command = 0;     
    }

   if(Flag_EPB_Switch && (EPB_State == 2)) 
   {
       if(Motor_Speed < 5  && Motor_Speed > -5) 
       {
           EPB_Command = 1;
       }
       else
       {
          EPB_Command = 3;
       }
       
   }
   if(Flag_EPB_Switch && (EPB_State == 1))  {EPB_Command = 2;}
   if(Flag_EPB_Switch && (EPB_State == 3))  {EPB_Command = 2;}
   if(Flag_EPB_Switch && (EPB_State == 0))  {EPB_Command = 2;}
*/

//�����ź�

   if(bDiSatus[3]) {EPB_Switch_Want = 1;}
   if(!bDiSatus[3]) {EPB_Switch_Want = 2;}

   if(EPB_Switch_Want == EPB_State)
   {
       EPB_Command = 0;
   }
   else
   {
        if(bDiSatus[3]) {EPB_Command = 1;}
        if(!bDiSatus[3]) {EPB_Command = 2;}
   }
   
}



//Զ�̼��ٶȿ���
void R_Vehco_Acceleration (void) 
{
    /*
    extern uint8 R_Break_Mode;   //Զ���ƶ�����   0x00��δ����,���ؿ��� 0x01������ƶ�������ִ��Զ������ָ�� 0x02:�ںϼ��ȡ���? 
    extern uint8 R_Break_Rang;     //Զ���ƶ������� 0~100%
    extern uint8   Vehco_Break_Mode;     //�ƶ�������ģʽ 0x00��δ����,���ؿ��� 0x01������ƶ�������ִ��Զ������ָ�� 0x02:�ںϼ��ȡ���?
    extern uint8   Vehco_Break_Rang;     //�ƶ���״̬  0~100%
    uint8 DriverMode_Command=0; //�Լ�ģʽ״̬ 0 �ֶ� 1 ���� 2 ���� 3 Զ��    DriverMode_State;       //�Լ�ģʽ״̬
    AccPed
    Breakk
    extern sint8 Vehco_Acceleration_Command;   //���ٶ�ָ�� -12.8~12.7
    uint8 Dang_Command=0;       //��λָ�� 0:N 1:D 2:R 3:����                 
    Dang_State;               //������λ״̬
    extern uint16 * __far   AccPed_Acceleration_Addr;  //���ſ���-���ٶ�
    extern uint16 * __far   Break_Acceleration_Addr;   //�ƶ����?-�ƶ�������%
    */
  
     
 
    if  ((DriveModeDebug==1)||(DriverMode_State==3) ) 
    {
        if (Breakk<R_Break_Min)
        { //δ���ƶ�
            // Vehco_Acceleration_Command=(sint8) Intpol_u16s16(AccPed_Acceleration_Addr,AccPed); 
            if((Dang_Command !=2))
            {
                Vehco_Acceleration_Command=(sint8) Intpol_u16s16(AccPed_Acceleration_Addr,AccPed) - 1;   
            }  
            R_Break_Rang=0;
            R_Break_Mode=1;
            //  if ((Dang_State==2)&&(Dang_Command==2)){ Vehco_Acceleration_Command= -1* Vehco_Acceleration_Command;            }
            if ((Dang_State==2)&&(Dang_Command==2)){ Vehco_Acceleration_Command= -1* ((sint8) Intpol_u16s16(AccPed_Acceleration_Addr,AccPed) - 1);            }
        }else
        { //���ƶ�
            Vehco_Acceleration_Command=0;
            if (abs(R_Break_Rang-(uint8)Intpol_u16s16(Break_Acceleration_Addr,Breakk)) >=  Break_Acceleration_Min_Step)
            {   //�ٷֱ��ƶ�����
                R_Break_Rang=(uint8)Intpol_u16s16((uint16 * __far)&Break_Acceleration_T01,Breakk)  ;
                // if (Vehco_Type_Test==2)
                if ((Vehco_Type_Test==2) && (Dang_Command !=2))  //����(Dang_Command !=2)����ʹ���ڹ�R�����ǳ���״̬��δ����ΪRʱ���������ٶȲ���Ϊ��
                {  //���ٶ�����
                    Vehco_Acceleration_Command=(sint8) Intpol_u16s16(Break_Acceleration_Addr,Breakk);       //���ٶ�
                }
                
            }
            R_Break_Mode=1;


            //EPB�Զ�����  �ͷŽ�ɲ
            if ((Vehco_Speed_Calc<5)&&(Vehco_Speed==0)&&(EPB_State==1)&& (Auto_Hold_Delay>=Auto_Hold_Delay_Max)) 
            {
                R_Break_Rang=0;
            }
            //����ʱ����ļ��ٶȷ���?
            //uint8 Dang_Command=0;       //��λָ�� 0:N 1:D 2:R 3:����                 
            //Dang_State;               //������λ״̬            
            // if ((Dang_State==2)&&(Dang_Command==2)){ Vehco_Acceleration_Command= -1* Vehco_Acceleration_Command;            }
            if ((Dang_State==2)&&(Dang_Command==2)){ Vehco_Acceleration_Command= (sint8) -1* Intpol_u16s16(Break_Acceleration_Addr,Breakk) ;            }
         }
    }else
    {
        Vehco_Acceleration_Command=0;
        R_Break_Rang=0;
        R_Break_Mode=0;
    
    }
    

}



// //�����̽Ƕȿ���
void  Steering_Wheel (void) 
{
    //extern sint16 Steering_Wheel_Angle_Code; //��������ת����  360��/5000 ��תΪ��
    //extern uint8  Steering_Wheel_Angle_ZeroMark; //�������Ƿ񾭹�0��У��
    //EPS_Demand_Angle;//EPS�Ƕ����� 0.1�Ȳ���   -1080.0~1080.0��
    //EPS_Angle_Max EPS_Angle_Min
    /*
    sint16 Steering_Wheel_Angle_BaseCAN ;
    sint16 Wheel_Offset_Auto=0;              //��У׼ģ�ⷽ����0��ƫ��
    uint8 Steering_Whee_Auto_Check_Mark=0;     //�������Լ�ɹ�?
    */
    switch (Vehco_Type_Test)
    {
        case 0:
        case 1:
            if (Steering_Wheel_Angle_ZeroMark==0)
            {
                EPS_Demand_Angle=0;
            }else
            {
                EPS_Demand_Angle=(sint16) (   (((sint32)Steering_Wheel_Angle_Code) *100) /139)+EPS_Offset;
                
                
                if     (EPS_Demand_Angle>=EPS_Angle_Max) { EPS_Demand_Angle=EPS_Angle_Max;    }
                if     (EPS_Demand_Angle<=EPS_Angle_Min) { EPS_Demand_Angle=EPS_Angle_Min;    }
                
            }
            break;
        case 2:
            if (Steering_Whee_Auto_Check_Mark==0) 
            {
                EPS_Demand_Angle=0;
            }else
            {
                EPS_Demand_Angle=Steering_Wheel_Angle_BaseCAN-Wheel_Offset_Auto + EPS_Offset;



                if     (EPS_Demand_Angle>=EPS_Angle_Max) { EPS_Demand_Angle=EPS_Angle_Max;    }
                if     (EPS_Demand_Angle<=EPS_Angle_Min) { EPS_Demand_Angle=EPS_Angle_Min;    }            
            }
            break;
        default:
            break;
}


 }


//Զ�̵�λ����
void Vehco_Dang (void)
{   //uint8 Dang_Command=0;       //��λָ�� 0:N 1:D 2:R 3:����                 Dang_State;               //������λ״̬
    //uint8 DriverMode_Command=0; //�Լ�ģʽ״̬ 0 �ֶ� 1 ���� 2 ���� 3 Զ��    DriverMode_State;       //�Լ�ģʽ״̬
    /*
          Dang_R=bDiSatus[15];
          Dang_N=bDiSatus[16];
          Dang_D=bDiSatus[17];       
    */
     
    switch(Vehco_Type_Test) 
    {
        
        case 0: //1 ʵ���Ҽ�ʻ��   �޻�����  ����Զ��ģʽ���Զ���D��
        default:   
            if (DriverMode_State==3)
            {
                switch (Dang_State) 
                {
                    case 0:
                        if (D_Dang_Change_Delay_Index < D_Dang_Change_Delay_Index_Max) 
                        {
                            D_Dang_Change_Delay_Index++;
                        }else 
                        {
                            Dang_Command=1;
                        }
                    
                    
                        break;
                    case 1:
                        Dang_Command=1;
                        break;
                    case 2:
                    default:
                        Dang_Command=0;
                        break;
                }
            
            }else
            {
                Dang_Command=0;
                D_Dang_Change_Delay_Index=0;
            }
            break;    
        case 1: //1 ʵ���Ҽ�ʻ��   �л����˰���
            Dang_R=bDiSatus[15];   //ʵ���� 
            Dang_N=bDiSatus[16];
            Dang_D=bDiSatus[17];         
            if (Dang_R==1) {              Want_Dang=2;              }
            if (Dang_D==1) {              Want_Dang=1;              }
            if (Dang_N==1) {              Want_Dang=0;              }
            
            if (DriverMode_State==3) 
            {
                switch (Want_Dang) 
                {
                    case 1: //��D
                        switch (Dang_State) 
                        {
                            case 0:
                                if (D_Dang_Change_Delay_Index < D_Dang_Change_Delay_Index_Max) 
                                {
                                    D_Dang_Change_Delay_Index++;
                                }else 
                                {
                                    Dang_Command=1;
                                }
                                break;
                            case 1:
                                Dang_Command=1;
                                D_Dang_Change_Delay_Index=0;
                                break;
                            case 2:
                            default:
                                Dang_Command=0;
                                D_Dang_Change_Delay_Index=0;
                                break;
                        }
                        break;
                    case 2: //��R
                         switch (Dang_State) 
                        {
                            case 0:
                                if (D_Dang_Change_Delay_Index < D_Dang_Change_Delay_Index_Max) 
                                {
                                    D_Dang_Change_Delay_Index++;
                                }else 
                                {
                                    Dang_Command=2;
                                }
                                break;
                            case 1:
                                Dang_Command=0;
                                D_Dang_Change_Delay_Index=0;
                                break;
                            case 2:
                            default:
                                Dang_Command=2;
                                D_Dang_Change_Delay_Index=0;
                                break;
                        }                        
                        
                        break;
                    case 0: //��N
                    default:
                        Dang_Command=0;
                        D_Dang_Change_Delay_Index=0;
                        break;
                }
            }
            else
            {
                Dang_Command=0;
                D_Dang_Change_Delay_Index=0;            
            }
            break;
        case 2:   //���Ƽ�ʻ��
            Dang_R=bDiSatus[15];   //ʵ���� 
            Dang_N=bDiSatus[16];
            Dang_D=bDiSatus[17];         
            if (Dang_R==1) {              Want_Dang=2;              }
            if (Dang_D==1) {              Want_Dang=1;              }
            if (Dang_N==1) {              Want_Dang=0;              }        
            if (DriverMode_State==3) 
            {
                if(( Breakk  > R_Break_Min) && (Want_Dang_last != Want_Dang) )
                    {
                        Vehco_Acceleration_Command = 0;
                        Dang_Command = Want_Dang;
                    }
                Want_Dang_last =  Want_Dang;
            }
            break;

    }
}


//��ʻģʽ�ж�
void Vehco_Driver_Mode(void)
{
    /*
    extern uint8 EPS_Run_Mode;//EPS���ص�ǰ����ģʽ
    extern uint8 EPS_ERR_Level;    //EPS���ϼ���
 
    extern uint8   Dang_State;               //������λ״̬

    extern uint8   DriverMode_State;       //�Լ�ģʽ״̬
    extern uint8   SmartPowerCommand;       //����ϵͳ��Դָ��  00 ����  01 �ػ�
    extern uint8   Change_State;           //���״�? 00 ������ 01������ 02 �����? 03 ��������Ҫ���?
    extern uint8   Change_Connect_State;   //�������״�? 0 δ���� 1 ���ǹ�Ѳ���?
    extern uint8   Vehco_Fault_Level;      //�������ϼ���

    extern uint8   Vehco_Power_State;    //��������״̬ 00 INIT ON�� 01 READY 02 ���� 03 �����޸�ѹ

    extern uint8   Isolation_Level;      //��Ե���ϵȼ� 0 ���� 1 һ�� 2 ���� 3 ���� 
    extern uint8   MCU_Fault_Level;      //��ع��ϼ���?  0 ���� 1��΢ 2 һ�� 3 ����
    extern uint8   BMS_Fault_Level;      //BMS���ϼ���   0 ���� 1��΢ 2 һ�� 3 ����
    
    extern uint8 DriverMode_Command; //�Լ�ģʽ״̬ 0 �ֶ� 1 ���� 2 ���� 3 Զ��
    extern uint8  EPS_Contral_Mode;//EPS����ģʽ
    extern uint8 R_DriverSystem_State;       //Զ�̼�ʻϵͳ״̬ 0 ������ 1 �������? 2 �Ͽ��� 3 �Ͽ�
    extern uint8 R_DriverSystem_Level;       //Զ�̼�ʻϵͳ���ż��� 0 �����ţ����̽ӹ� 1 ������ ���˹����� 2 ���״�?����ȫ����
    extern uint8 R_DriverSystem_Fault_Level; //Զ�̼�ʻϵͳ���ϼ��� 0 �޹��� 1 ��΢ ��ʾ 2 һ�����? ���� 3 ���ع��� �˳�Զ�̼�ʻ״̬    
    */
    
    //Զ�̼�ʻϵͳ״̬�ж� 0 ������ 1 �������? 2 �Ͽ��� 3 �Ͽ�
    if ((LostEPSCommunication==0)&&(LostVehcoCommunication==0)&&(LostVehcoStateCommunication==0))
    {
        if (SmartPowerCommand==0)
        {
            R_DriverSystem_State=1;  
        }
        
    
    }else 
    {
        R_DriverSystem_State=0;
    }
    //Զ�̼�ʻϵͳ���ż����ж� 0 �����ţ����̽ӹ� 1 ������ ���˹����� 2 ���״�?����ȫ����
    
    
    //Զ�̼�ʻϵͳ���ϼ��� 0 �޹��� 1 ��΢ ��ʾ 2 һ�����? ���� 3 ���ع��� �˳�Զ�̼�ʻ״̬ 
    
    //����Զ�̼�ʻģʽ�ж�
    if ( ((EPS_Run_Mode==0x00)||(EPS_Run_Mode==0x10)|| (EPS_Run_Mode==0x20))
        && (EPS_ERR_Level<2) 
        //&& ((Change_State==0)||(Change_State==3))
        && (Change_Connect_State==0)
        && (Vehco_Fault_Level<3)
        //&& ((Vehco_Power_State==1)||(Vehco_Power_State==2))
        && (Isolation_Level<2)
        && (BMS_Fault_Level<3)
        && (MCU_Fault_Level<2)
        && (LostEPSCommunication==0)&&(LostVehcoCommunication==0)&&(LostVehcoStateCommunication==0)
        && ((LostVehcoSteering_Wheel_AngleCommunication==0)||(Vehco_Type_Test!=2))
       )
    {
        if ((R_Control_Switch==1)&& (SmartPowerCommand==0))
        {        
            DriverMode_Command=3; //�Լ�ģʽ  Զ��ģʽ 
            EPS_Contral_Mode=0x20;//EPS����ģʽ
        }
    }else 
    {
        DriverMode_Command=0; 
    }
    
    //�˳�Զ��ģʽ
    if (R_Control_Switch==0) 
    {
        DriverMode_Command=0; 
    }
    
    if (SmartPowerCommand==1) 
    {
        R_DriverSystem_State=3;//�յ�Զ���µ�ָ��Ͽ�
        DriverMode_Command=0;
        EPS_Contral_Mode=0x10;
    }





}

//�����ж�
void Vehco_Errpr_Judge(void) 
{
    //uint8   LostVehcoCommunication=0;
    //uint16  LostVehcoCom_Index=0;
    LostEPSCom_Index++;
    LostVehcoCom_Index++;
    LostVehcoStateCom_Index++;
    LostVehcoBreakCom_Index++;
    LostVehcoSteering_Wheel_AngleCom_Index++;
    
    
    if (LostEPSCom_Index>=LostEPSCom_Index_Max)
    {   //EPSͨѶ��ʧ
        LostEPSCom_Index=LostEPSCom_Index_Max+1;
        LostEPSCommunication=1;
    }
    if (LostVehcoCom_Index>=LostVehcoCom_Index_Max)
    {   //�г�ͨѶ��ʧ
        LostVehcoCom_Index=LostVehcoCom_Index_Max+1;
        LostVehcoCommunication=1;
    }
    if (LostVehcoStateCom_Index>=LostVehcoStateCom_Index_Max)
    {   //����״̬ͨѶ��ʧ
        LostVehcoStateCom_Index=LostVehcoStateCom_Index_Max+1;
        LostVehcoStateCommunication=1;
    }
    if (LostVehcoBreakCom_Index>=LostVehcoBreakCom_Index_Max)
    {   //�����ƶ�״̬ͨѶ��ʧ
        LostVehcoBreakCom_Index=LostVehcoBreakCom_Index_Max+1;
        LostVehcoBreakCommunication=1;
    }
    if (LostVehcoSteering_Wheel_AngleCom_Index>=LostVehcoSteering_Wheel_AngleCom_Index_Max)
    {   //ģ�ⷽ����ͨѶ��ʧ
        LostVehcoSteering_Wheel_AngleCom_Index=LostVehcoSteering_Wheel_AngleCom_Index_Max+1;
        LostVehcoSteering_Wheel_AngleCommunication=1;
    }


}


//Կ��״̬�ж�
void Vehco_Key_Status(void) 
{
    //Կ���ж�  bDiSatus[1] ACC   bDiSatus[2] START BattCD_bLogicOnUB ON
    //��λ�ж�  bDiSatus[14] P    bDiSatus[15] R    bDiSatus[16] N  bDiSatus[17]D 
    //Key_Status 
    //Կ�� 0xF0 Bit5~8 0 lock, 1 acc, 2 on, 3 start   
    //��λ 0x0F bit 1~4 0 N ,1 P, 10 D, 5 R


    if (BattCD_bLogicOnUB==1)//ON�߼������� 
    {

    }
    else 
    {
      
    }

   

}


//�������?     10ms
void VehCo_Test(void) 

{
    //void LSPWMDrv_Interface(uint8 idxChanUB,uint32 fSetUW,uint16 rSetUW); 
    //��������?���ĵͱ�����Ƶ�ʼ�ռ�ձȣ�ռ�ձȾ���Ϊ0.01%�����Ŵ�100��  Ƶ�ʾ���Ϊ0.01Hz,�Ŵ�100��
   
    /*
    LSPWMDrv_Interface(0,fSet,rSet);
    LSPWMDrv_Interface(1,fSet,rSet);
    LSPWMDrv_Interface(2,fSet,rSet);
    LSPWMDrv_Interface(3,fSet,rSet);
    */
}   
                                  

//��������״̬��
void VehCo_StateMachine(void)
{
    //ÿ10ms����һ��
    
    //VehCo_stNewUB����4��״̬��Init, Running, Afterrun, Finish
    //����Init��Finish����״̬�Ĳ����ɵײ����?
    
    switch(VehCo_stNewUB)
    {
        case _VEHCO_INDEX_STATUS_RUNNING:
            if(0==BattCD_bLogicOnUB) //Running״̬ʱ�߼��翪�ضϿ������뵽�ػ�׼��״̬Afterrun
            {
                //Error_PowOn_Delay=0;  //���µ�������ӳ�?
                VehCo_stNewUB=_VEHCO_INDEX_STATUS_AFTERRUN;
            }
            break;
            
        case _VEHCO_INDEX_STATUS_AFTERRUN:
 
                 
            
            if(1==BattCD_bLogicOnUB)  //Afterrun״̬ʱ�߼��翪�رպϣ����½�������״̬
            {

                VehCo_Init(); //���³�ʼ��
                
                VehCo_stNewUB=_VEHCO_INDEX_STATUS_RUNNING; //�ָ�����״̬
            }
            else
            {
               // Error_PowOn_Delay=0;  //���µ�������ӳ�?
                if( VehCo_stAfterrunUB==_VEHCO_AFTERRUN_STATUS_FINISH)//����ػ�׼�������Ѿ���ɣ�����ͣ��״̬Finish
                {
                    //�ػ�׼��������ɣ�����ͣ��״�?
                    bDoSatus[0]=0;//�Ͽ����̵���
                    
                    //DCDC_Enable=0;//DCDCʧ��
                    
                    VehCo_DOset(1);
                    VehCo_UpdateSig(); 
                    //Can_Send_Enable=0;//��ֹ����CAN����
                    VehCo_stNewUB=_VEHCO_INDEX_STATUS_FINISH;
                }
            }
            break;
            
        default:
            break;
    }
}

//ͣ��׼��״̬����
void VehCo_Afterrun(void)
{
    //extern uint8 SOC;
    switch(VehCo_stAfterrunUB)
    {
        case _VEHCO_AFTERRUN_STATUS_INIT:     //��ʼ������VehCo_Init�Ὣ����ΪINIT
            if(VehCo_stNewUB==_VEHCO_INDEX_STATUS_AFTERRUN)   //�ػ�׼��״̬Afterrun
            {
              
                    VehCo_stAfterrunUB=_VEHCO_AFTERRUN_STATUS_BEGIN; //��ʼ�ػ�׼������
                    Afterrun_Delay_Index=0;
                    //Vehco_State=6;
                    //Delay_Index_LostMcuShutDown=0;
                   // On_LostInRuning=0;
           
            }
            break;
            
        case _VEHCO_AFTERRUN_STATUS_BEGIN:   //    ��ʼ�ػ�׼������
            //�ڴ˴����ӹػ�׼��״̬�µĲ���
            
            
            
            
                

            
            //������ɺ����״̬��ת
            if (Afterrun_Delay_Index>=300)  //�ػ���ʱ���� 3��  ����ŵ����
            {
                //����EEPROM���� 
                PowOn_Times++;


                



                //EEPROMDrv_bSaveInstantUB=1;
                VehCo_stAfterrunUB=_VEHCO_AFTERRUN_STATUS_FINISH;
            } 
            else 
            {
                Afterrun_Delay_Index++;
            }
            break;
            
        case _VEHCO_AFTERRUN_STATUS_FINISH:
            break;
            
        default:
            break;
    }
}





