/*******************************************************************************
* �������ƣ��Զ��庯������
* �ļ����� Custom_Func.c
* �������� �� ������״̬ת�ؼ���
* ���ü�¼��
* ��ע�� 
* �޶���¼��
*		����		      �汾		  ����		����
*   
*******************************************************************************/
#include "GlobalDef.h"
#include "Custom_Func.h"

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

sint16 Motor_Speed_Buffer[80]={0}, Motor_Speed_Delta,Vmh_Delta_Target,Vmh_Delta_Vehco;//ת�ٻ��塢ת�ٲĿ��ADֵ������ADֵ

sint16 Vmh_Delta_Target_Crawl_D,Torque_Increment_Crawl_D_Get;
sint16 Vmh_Delta_Target_FeedBack_Break,Torque_Increment_FeedBack_Break;
sint16 Vmh_Delta_Target_Parking_D,Torque_Increment_Parking_D;
sint16 Vmh_Delta_Target_Parking_R,Torque_Increment_Parking_R;



sint16 Motor_Torque_Buffer[80]={0};
sint16 Motor_Speed_Average_Buffer[80]={0};
sint16 Motor_Speed_Average;//ƽ��ת��
sint16 Motor_Torque_Average;//ƽ��ת��
sint16 Torque_Increment;//ת������
sint16 Vmh_Delta_Vehco_Average_R=0,Vmh_Delta_Vehco_Average_D=0;//ƽ������ADֵ
sint32 Vmh_Delta_Vehco_Average_D_500=0,Vmh_Delta_Vehco_Average_R_500=0;  ////ƽ������ADֵ500���м�ֵ
uint16  Crawl_D_Max_Torque;//����Ť�ط�ֵ
sint16 Crawl_D_Torque_New;//,Crawl_D_Torque_Buffer; //�䶯ת��
sint16 AccPed_Torque;//�������Ŷ�Ӧת��
sint16 Auto_Parking_D_Torque,Auto_Parking_R_Torque;//פ��ת��  
uint16 AccPed_Torque_Buffer;//����ת�ػ���
uint16 AccPed_Slope_Delay_2key=0;
uint8 AccPed_Torque_ModeDelay=0; //����ģʽ����0Nm����
uint8 AccPed_Slope_Delay_Num=0; //�������Ÿ���б������ 0~250

//D�����л���
sint16 FeedBack_Torque_Coast;  //���л���ת��
sint16 FeedBack_Torque_Coast_Buffer;  //���л���ת��  10��
sint16 FeedBack_Torque_Coast_Buffer_Quit; 
uint8 FeedBack_Coast_ModeDelay;
//uint8 FeedBack_Diff;//��Ч�ٷֱ�
uint8 FeedBack_Slope_Coast_Enable;
uint8 FeedBack_Slope_Enable;//������ʼб�¿���
uint16 FeedBack_Coast_QuitDelay;



//D���ƶ�����
sint16 FeedBack_Torque_Break; //�ƶ�����ת��
sint16 FeedBack_Torque_Break_Buffer;// �ƶ�����ת�� 10��
sint16 FeedBack_Torque_Break_Buffer_Quit;
uint8 FeedBack_Break_ModeDelay=0;

//sint16 FeedBack_Break_Power;  //�ƶ������繦�� 100��
uint16 FeedBack_Break_QuitDelay; //�ƶ������˳�б���ӳ�

//D�䶯
uint16 Crawl_D_ModeDelay;



//��������
sint16 Driver_Power;//����繦��  100��
sint16 Break_Torque_Limit=0;//������������
sint16 Bms_Max_Torque_Break; //BMS���ƻ���ת��
sint16 Motor_Max_Torque_Break;//������ƻ���ת��

sint16 Driver_Torque_Limit=0;//������������
sint16 Bms_Max_Torque_Driver; //BMS��������ת��
sint16 Motor_Max_Torque_Driver;//�����������ת��
sint32 Bms_Max_Torque_Break_Tmp=0;  //BMS�����ƶ�ת�ؼ���ֵ
sint32 Bms_Max_Torque_Driver_Tmp=0; //BMS��������ת�ؼ���ֵ
uint16 Motor_Efficieny_Fix; //���Ч���Զ�����ֵ
uint16 Motor_Efficieny_Pow_Limit=0;//���������Ч��
//פ��
sint16 Auto_Park_Speed_Accumulation;//����ת���ۼ�
uint16 Auto_Park_Speed_Times;//��������ʱ���ۼ�
//����
uint8 Limit_P=0;
extern uint8 Pow_Down_Enable;//�����޹��ʱ�־
extern uint8 MCU_Fault_Level;

extern uint8 bDiSatus[24];
extern sint16 Motor_Speed,Motor_Torque,Demand_Torque;
extern uint16 AccPed, Breakk;//���š��ƶ�̤��״̬
extern uint8 Dang_L4bit,Dang_P;//��λ״̬   1 5 0 10 PRDN
extern uint8 Err_AccPed;//���Ź���
extern uint8 Vehco_State;
extern uint8 Veh_Limit_Speed;//Ѳ�������Ƴ���  0.5km/h/bit 0~127.5    ��V1.2�濪ʼ 1km/bit 0~180
//�Ǳ�LEDS2 8��bit ��չ޹��ϡ�����ģʽ������Ѳ��ģʽ��������ȡ�����ģʽ��ϵͳ���ϡ�Ready��
extern uint8 Err_Vacuum_Tank,Limit_Mode,Constant_Mode,MCU_OverHeat,FeedBack_Mode,System_Fault,Ready;
extern uint8 SOC;
extern uint16 B_Voltage,Output_Voltage;            //��ص�ѹ�������ѹ  0~800 ��λ�� V�� ƫ�ƣ� 0 ��   �����ߣ� 0.2 ��10���Ŵ���㣬0��8190 4095*2=8190��
//extern sint16 B_Current;                           //��ص���         -500~500 ��λ�� A�� ƫ�ƣ� -500 �������ߣ� 0.02 (10���Ŵ����-5000��5000����������ʾ�Ե�س�磬��������ʾ��طŵ�
//extern uint16 B_Charge_Current,B_Discharge_Current;//��س䡢�ŵ����   0��500 ��λ�� A�� ƫ�ƣ�0 ��    �����ߣ� 0.02���Ŵ�100������ 0��50000��
extern sint16 B_Power;                             //��س�Ź���     -325~325 ��λ�� kW��ƫ�ƣ� -325�� �����ߣ� 0.01 (100���Ŵ���㣩�����ʱ�ʾ�Ե�س�磬 �����ʱ�ʾ��طŵ�    

//********************************************************************
    //����ģʽ��ʶ  0 ���ɻ���  
    //1 D���䶯  2 D�����л��� 3 D���ƶ�����  4 D���µ�פ�� 
    //5 R���䶯  6 R�����л��� 7 R���ƶ�����  8 R���µ�פ��
    //9 ǰ��(���Ÿ������أ�10 ���� (���Ÿ������أ�
extern uint8 Torque_Mode;

extern uint16 Max_Charge_Current_Limit,Max_Charge_Power_Limit,B_Voltage;
extern uint16 Max_Discharge_Current_Limit,Max_Discharge_Power_Limit;
extern sint16 Torque_Low_Limit,Torque_High_Limit;

extern uint8  ABS_Active;//ABS�����ź�
extern uint8 Err_Eps,Oil_Pump_Install,Oil_Pump_Fault;
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
extern  uint16 * __far Speed_Max_Torque_Crawl_D_Break_Addr;  //D�������ƶ�״̬�������������
extern  uint16 * __far AutoPark_Motor_Speed_Torque_Addr;   //פ���������

     //���ݳ���ѡ��ֵ�궨ֵ
extern  uint16 Motor_Vehicle_Speed_Scale_Get;//ת��-���ٱ���
extern  uint16 Demand_Limit_High_Get;//��������
extern  uint16 Demand_Limit_Low_Get;//��������
extern  uint16 Crawl_D_Speed_Max_Get;//������״̬�����䶯���ٶ�  RPM
extern  uint8 Crawl_D_Torque_Delay_Max_Get;//�䶯���¼��  ��λ10ms
extern  uint8 Speed_Buffer_Number_Get; //10ms�����ٶȲ����������
extern  uint8 Speed_Average_Number_Get; //10msƽ�����ٻ������
extern  uint8 Torque_Buffer_Number_AutoPark_Get;  //10ms����ƽ��ת�ز���������� פ��
extern  uint8 D_Park_Start_Speed_Get;//פ�´���ת��
extern  sint16 Auto_Park_Speed_Acc_Max_Get; //����Dפ���������ת���ۼ�
extern  uint16 Auto_Park_Speed_Times_Max_Get;//����Dפ�������������ʱ�� ��λ10ms
extern  uint8 Vehco_Type_Test;
extern  sint16 Auto_Torque_Min_Get,Auto_Torque_Max_Get; //�Զ�ת����С��ֵ �Զ�ת�����ֵ
extern  uint16 R_Vhms_Average_Ratio_Get,D_Vhms_Average_Ratio_Get;//Vhmsƽ��ֵ�������

//Զ������
extern uint8 Lock_Vehicle_Active; //����״̬����
 

//����������ר��
extern uint8 Third_Lock_Active;              //��������
extern uint8 Third_Lock_Mode_Active;         //��Ч����ģʽ
extern uint8 Third_Lock_LimitSpeed_Active;   //��Ч�����ٶ�

//��绥��
extern uint8   No_Ins_Charge_CAN;//����ڼ��ֹINS����֡����


//����
extern uint8 Shift_Gears,Gears_Install,Shift_Gears_Enable,Gears_State;
extern uint16 Motor_Vehicle_Speed_Scale_Gears_Out_Get ;
extern sint16 Gears_Speed;
extern uint8 Gears_Shoft_Torque_State,Gears_Shoft_Torque_Delay;  //��������״̬������������ʱ
extern sint32 Gears_Torque_Buffer; //��������Ť��  0.1

    #ifndef AECS_HW
      #pragma CODE_SEG DEFAULT
    #else  
      #pragma CODE_SEG VCU_CODE 
    #endif

//����ģʽ�ж�
void Torque_Mode_Judge(void) 
{
  
    
    
    switch(Dang_L4bit)  //���ݵ�λ
    {
        case 10://D
          
            //���л���
            if ((Motor_Speed>(sint16)Motor_Vehicle_Speed_Scale_Get*7)&&(AccPed<50)&&(Breakk==0)&&(Limit_P==0))
            {   
                FeedBack_Coast_QuitDelay=10;
                switch (Torque_Mode)
                {
                    case 0:
                    case 9: 
                    case 1:   //�����С����źͻ��н��뻬�л���
                        FeedBack_Coast_ModeDelay=0;         //0 Nm����
                        FeedBack_Slope_Coast_Enable=1; //б�½���
                        FeedBack_Torque_Coast=0;
                        FeedBack_Torque_Coast_Buffer=0;
                        Torque_Mode=2; //D���л���
                       
                        break;
                    case 3:
                        //���ƶ����뻬�л���
                        if    (((Gears_Install==1)&&(FeedBack_Torque_Break>=-1*(Intpol_s16s16(FeedBack_Coast_D_Torque_Addr,Gears_Speed))))
                              ||((Gears_Install==0)&&(FeedBack_Torque_Break>=-1*(Intpol_s16s16(FeedBack_Coast_D_Torque_Addr,Motor_Speed_Average)))) )  //���ƶ��������뻬�л���
                        {
                            //FeedBack_Coast_ModeDelay=FeedBack_Break_ModeDelay;//����0 Nm
                            
                            FeedBack_Coast_ModeDelay=FeedBack_Coast_ModeDelay_Max;  //����0 Nm
                            
                            FeedBack_Slope_Coast_Enable=1; //noб�½���
                            //FeedBack_Torque_Coast=Motor_Torque;
                            //FeedBack_Torque_Coast_Buffer=Motor_Torque*10;
                            FeedBack_Torque_Coast=FeedBack_Torque_Break;
                            FeedBack_Torque_Coast_Buffer=FeedBack_Torque_Break*10;
                            FeedBack_Break_QuitDelay=0;
                            Torque_Mode=2; //D���л���
                        }
                        break;
                    default:
                       Torque_Mode=2; //D���л���
                      
                       break;
                 }
            }
            
            //�ƶ�����
            if ((Motor_Speed>(sint16)Motor_Vehicle_Speed_Scale_Get*7)&&(AccPed<FeedBack_Break_Max_AccPed)&&(Breakk!=0))   
            {
                FeedBack_Break_QuitDelay=10;
                switch (Torque_Mode)
                {
                    case 9:
                    case 0: 
                    case 1://�����š����кͻ��н����ƶ�����
                        FeedBack_Break_ModeDelay=0;  //0 Nm����
                        FeedBack_Slope_Enable=1; //б�½���
                        FeedBack_Torque_Break=0;
                        FeedBack_Torque_Break_Buffer=0;
                        Torque_Mode=3; //D�ƶ�����
        
                        
                       
                        break;
                    default:
                        Torque_Mode=3; //D�ƶ�����
                       
                        break;
                    case 2: //�ӻ��л��������ƶ�����
                        FeedBack_Break_ModeDelay=FeedBack_Coast_ModeDelay;//����0 Nm
                        FeedBack_Slope_Enable=1; //б�½���
                        FeedBack_Torque_Break=FeedBack_Torque_Coast;
                        FeedBack_Torque_Break_Buffer=FeedBack_Torque_Coast*10;
                        Torque_Mode=3; //D�ƶ�����
     
                        break;
                }    
            }

            
            //D�µ�פ��
            if (  (Auto_Park_Speed_Accumulation<Auto_Park_Speed_Acc_Max_Get)   
                &&(Auto_Park_Speed_Times>=Auto_Park_Speed_Times_Max_Get)
                &&(Breakk<=100)   
                &&(Auto_Parking_D_Torque_Enable==1)
                &&(Motor_Speed<-1*D_Park_Start_Speed_Get)
                &&(Vmh_Delta_Vehco<-2) 
                &&(Dang_P==0))
            {
                Torque_Mode=4; //D�µ�פ��
                FeedBack_Break_QuitDelay=0;
                FeedBack_Coast_QuitDelay=0;   
            }


            //D�䶯 
            if (   (   ((Gears_Install==0)&&(Motor_Speed<Crawl_D_Speed_Max_Get))
                ||    ((Gears_Install==1)&&(Motor_Speed<Crawl_D_Speed_Max_Get)
                        //&&(Motor_Speed/Motor_Vehicle_Speed_Scale_Get <5)
                         )  )
                
                &&(AccPed<100)&&(Dang_P==0)) 
            {
                switch(Torque_Mode) 
                {
                    case 4: //��פ���л���D�䶯
                        if (((Crawl_D_Torque_New>Auto_Parking_D_Torque)&&(Vmh_Delta_Vehco>=0))||(Auto_Parking_D_Torque_Enable==0)   )
                        {
                            Torque_Mode=1;
                           
                        }
                        break;
                    case 3: //���ƶ������л���D�䶯 
                        if (((Breakk==0)&&(Motor_Torque==0))||(FeedBack_Break_QuitDelay==0)) 
                        {
                            Crawl_D_ModeDelay=0;
                            Torque_Mode=1;
                                                 
                        }                       
                        break;
                    case 2: //�ӻ��л����л���D�䶯 
                        Crawl_D_ModeDelay=0;
                        Torque_Mode=1;
                       
                        break;
                    case 9:   //������D�л���D�䶯  ����Ҫ������ʱ
                        //Crawl_D_ModeDelay=0;
                        Torque_Mode=1;
                       
                        break;
                    case 0: //�����ɻ����л���D�䶯
                        if (Motor_Torque==0)
                        {
                            Crawl_D_ModeDelay=0;
                            Torque_Mode=1;
                           
                        }
                        break;
                    default:
                        Torque_Mode=1;  //D�䶯
                          
                        break; 
                }
            } 


            
            if (AccPed>=50)  //���Ÿ���ģʽ 
            {
                /*
                AccPed_Torque_ModeDelay  //����ģʽ����0Nm����
                AccPed_Slope_Delay_Num   //���Ž���б�»���
                */
                switch (Torque_Mode) 
                {
                  default:
                      Torque_Mode=9;
                      break;
                  case 9: 
                      if (AccPed==0) //����ģʽ���ɹ�����
                      {                       //��0��б��
                        //  AccPed_Torque_ModeDelay=AccPed_ModeDelay;
                          AccPed_Slope_Delay_Num=0;
                          AccPed_Torque_Buffer=0;
                          AccPed_Torque=0;                        
                      }
                      break;
                  case 4:  //��פ���л���ǰ��  ��0��б��
                      if (((AccPed_Torque>Auto_Parking_D_Torque)&&(Vmh_Delta_Vehco>=0))||(Auto_Parking_D_Torque_Enable==0)||(Dang_P==1)  )
                      {
                          Torque_Mode=9;
                       //   AccPed_Torque_ModeDelay=AccPed_ModeDelay;
                       //   AccPed_Slope_Delay_Num=AccPed_Slope_Delay;
                          AccPed_Torque_Buffer=AccPed_Torque*10;
                      }
                      break;
                  case 1: //���䶯�л���ǰ��  ��0��б��
                      if (AccPed_Torque>=Crawl_D_Torque_New) 
                      {
                          Torque_Mode=9;
                        //  AccPed_Torque_ModeDelay=AccPed_ModeDelay;
                        //  AccPed_Slope_Delay_Num=AccPed_Slope_Delay;
                          AccPed_Torque_Buffer=AccPed_Torque*10;
                      }
                      break;
                  case 0: //�ӻ����л���ǰ��  ��0��б��
                      Torque_Mode=9;
                    //  AccPed_Torque_ModeDelay=AccPed_ModeDelay;
                      AccPed_Slope_Delay_Num=0;
                      AccPed_Torque_Buffer=0;                  
                      AccPed_Torque=0; 
                      break;
                  case 2:   //�ӻ��л����л���ǰ��  ��0��б��
                      if ((Demand_Torque==0)||(FeedBack_Coast_QuitDelay==0)) 
                      {
                          Torque_Mode=9;
                          AccPed_Torque_ModeDelay=0;
                          AccPed_Slope_Delay_Num=0;  
                          AccPed_Torque_Buffer=0;
                          AccPed_Torque=0;                     
                      }
                      break;
                  case 3:   //���ƶ������л���ǰ��  ��0��б��
                       if ((Demand_Torque==0)||(FeedBack_Break_QuitDelay==0)) 
                       {
                          Torque_Mode=9;
                          AccPed_Torque_ModeDelay=0;
                          AccPed_Slope_Delay_Num=0;                      
                          AccPed_Torque_Buffer=0;
                          AccPed_Torque=0;     
                       }
                  
                      break;
                }
            } else 
            {
                if (Torque_Mode==9) //����ģʽ��������  *������ɲ�޷����뻬�л���
                {
                
                
                }
            
            }
            
            if (Vehco_State==5)//��ʻ�ж�ʧON�ź�
            {
                Torque_Mode=0; //����
               
                FeedBack_Break_QuitDelay=0;
                FeedBack_Coast_QuitDelay=0;
            }
            if ((MCU_Fault_Level==2)&&(Vehco_State==4)) // ���2�����ϣ�0ת�ػ���
            {
                Torque_Mode=0; //����
               
                FeedBack_Break_QuitDelay=0;
                FeedBack_Coast_QuitDelay=0;

            }
            
            break;
        case 5://R
            

            if (AccPed>100)   
            {
                if (Torque_Mode==8) 
                {
                    if (((AccPed_Torque<Auto_Parking_R_Torque)&&(Vmh_Delta_Vehco<=0))||(Auto_Parking_R_Torque_Enable==0)  )
                    {
                         Torque_Mode=10;//��פ���л�������
                    }
                }
                else
                {
                    Torque_Mode=10;//����
                }
            }else 
            {
                Torque_Mode=10;
            }
            
            if ((Motor_Speed>50)&&(Vmh_Delta_Vehco>10)&&(Auto_Parking_R_Torque_Enable==1) )
            {
                Torque_Mode=8; //R�µ�פ��    
            }
            
            if (Vehco_State==5)//��ʻ�ж�ʧON�ź�
            {
                Torque_Mode=0; //����
            }            
          
            if ((MCU_Fault_Level==2)&&(Vehco_State==4)) // ���2�����ϣ�0ת�ػ���  
            
            {          
                Torque_Mode = 0; //����
               
                FeedBack_Break_QuitDelay=0;
                FeedBack_Coast_QuitDelay=0;

            }   
            break;
            
         
        case 0:  //N
        case 1:  //P
        default:
            Torque_Mode=0; //����
            break;      
    }

    
    //���С�������������פ���ڼ��ֹ����
    switch(Torque_Mode) //���ݳ���ģʽѡ��ת�ش������
    {
        case 1: //D�䶯   
            Demand_Torque=Crawl_D_Torque_New;
            /*if ((Shift_Gears==0)&& (Gears_State!=0)&&(Gears_State!=15))
            {
                Shift_Gears_Enable=0;
            }else
            {
               Shift_Gears_Enable=1;
            } */
            Shift_Gears_Enable=1;
            break;      
        case 2: //D���л���
            Demand_Torque=FeedBack_Torque_Coast;
            Shift_Gears_Enable=1;
            break;      
        case 3: //D�ƶ�����   
            Demand_Torque=FeedBack_Torque_Break;
            Shift_Gears_Enable=1;
            break;      
        case 4: //D�µ�פ�� 
            Demand_Torque=Auto_Parking_D_Torque;
            if ((Shift_Gears==0)&& (Gears_State!=0) &&(Gears_State!=15))
            {
                Shift_Gears_Enable=0;
            }else
            {
               Shift_Gears_Enable=1;
            }
            break;      
        case 10: //����
            Demand_Torque=AccPed_Torque;
            if ((Shift_Gears==0)&&( ( (Gears_State!=0)&&(Vehco_Type_Test!=16)   )  ||(  (Gears_State==15)&&(Vehco_Type_Test==16) )    ) ) 
            {
                Shift_Gears_Enable=0;
            }else 
            {
                Shift_Gears_Enable=1;
            }
            break; 
        case 9:  //ǰ��    
            Demand_Torque=AccPed_Torque;
            Shift_Gears_Enable=!Limit_Mode;
            
            //Shift_Gears_Enable=1;
            break; 
        case 5: //R�䶯
            Demand_Torque=0;
            /*
            if ((Shift_Gears==0)&&( ( (Gears_State!=0)&&(Vehco_Type_Test!=16)   )  ||(  (Gears_State==15)&&(Vehco_Type_Test==16) )    ) ) 
            {
                Shift_Gears_Enable=0;
            }else 
            {
                Shift_Gears_Enable=1;
            }
            */
            Shift_Gears_Enable=1;
            break;      
        case 8: //R�µ�פ��
            Demand_Torque=Auto_Parking_R_Torque;
            if ((Shift_Gears==0)&&( ( (Gears_State!=0)&&(Vehco_Type_Test!=16)   )  ||(  (Gears_State==15)&&(Vehco_Type_Test==16) )    ) ) 
            {
                Shift_Gears_Enable=0;
            }else 
            {
                Shift_Gears_Enable=1;
            }
            break;                           
     

        case 0: //����      ��0ʱ��б�½�0
        default:
            Demand_Torque=0;
            Shift_Gears_Enable=1;
            break;
    }
    
    if   (Shift_Gears_Enable_Debug!=2 )  {  Shift_Gears_Enable= Shift_Gears_Enable_Debug;    }


    //������Ť��ƽ�����ɴ���
    Gears_Shoft_Torque();

    //�൵��յ���ֹŤ�����
    if ((Gears_State==0)&&(Gears_Install==1)) 
    {
        Demand_Torque=0;
    }
    
    //��ع�������ת��
    Torque_Power_Limit();
    //����ڼ��ֹ��ʻ
    if (No_Ins_Charge_CAN==1)
    {
        Demand_Torque=0;
        if (Vehco_State==4)
        {                                      
            Vehco_State=1;
        }
    }
}

//������Ť��ƽ�����ɴ���
void Gears_Shoft_Torque (void) 
{   /*
    uint8 Gears_Shoft_Torque_State=0,Gears_Shoft_Torque_Delay=0;  //��������״̬������������ʱ
    sint32 Gears_Torque_Buffer=0; //��������Ť��  0.1
    */
    if ((Gears_Shoft_Torque_State==1)&&(Gears_Shoft_Torque_Delay< AccPed_Slope_Delay))
    {
        if((Demand_Torque>0) && (Gears_Torque_Buffer>0)  &&  ((sint32)Demand_Torque*10 >Gears_Torque_Buffer  ) ) 
        {
            Gears_Torque_Buffer=Gears_Torque_Buffer+ AccPed_Torque_Diff_Add;
            Gears_Shoft_Torque_Delay++;
            Demand_Torque= (sint16)(Gears_Torque_Buffer/10);            
            
        }else if ((Demand_Torque<0) && (Gears_Torque_Buffer<0) &&((sint32)Demand_Torque*10<Gears_Torque_Buffer  )   ) 
        {
            Gears_Torque_Buffer=Gears_Torque_Buffer - AccPed_Torque_Diff_Add;
            Gears_Shoft_Torque_Delay++;
            Demand_Torque= (sint16)(Gears_Torque_Buffer/10);            
            
        }else 
        {   //Ť�ص���
            Gears_Torque_Buffer= (sint32)Demand_Torque*10;
            Gears_Shoft_Torque_State=0;
        }

    }else
    {   //Gears_Shoft_Torque_Delayб��ʱ���ܵ��� Ť�ػ�����  5% �ݽ�  ��ʱGears_Shoft_Torque_State=1
        if (Gears_Shoft_Torque_State==1) 
        {
            if((Demand_Torque>0) && (Gears_Torque_Buffer>0)  &&  ((sint32)Demand_Torque*10 >Gears_Torque_Buffer  ) ) 
            {
                Gears_Torque_Buffer=Gears_Torque_Buffer+Demand_Torque/2;
                
            }else if ((Demand_Torque<0) && (Gears_Torque_Buffer<0) &&((sint32)Demand_Torque*10<Gears_Torque_Buffer  )   ) 
            {
                Gears_Torque_Buffer=Gears_Torque_Buffer-Demand_Torque/2;
            }else 
            {   //Ť�ص���
                Gears_Torque_Buffer= (sint32)Demand_Torque*10;
                Gears_Shoft_Torque_State=0;
            }
            Demand_Torque=   (sint16)(Gears_Torque_Buffer/10); 
               
        }else
        {
            Gears_Shoft_Torque_Delay=AccPed_Slope_Delay+1;
            Gears_Shoft_Torque_State=0;
            //Gears_Torque_Buffer=0;          
        }
    
    }
    
}


 //����ٶȻ��崦��
void Motor_Speed_Buffer_FIFO (void) 
{
    sint16 Vmh_Delta_Vehco_Tmp=0;
    sint32 Total_Speed=0;
    //Motor_Speed_Buffer[0]                   ���ϵ��ٶ�  
    //Motor_Speed_Buffer[Speed_Buffer_Number_Get] ���µ��ٶ� 
    //Speed_Buffer_Number_AutoPark    Motor_Speed_Delta_AutoPark
    //sint16 Motor_Torque_Buffer[50]={0};
    //sint16 Motor_Torque_Average;//ƽ��ת��
    //Torque_Buffer_Number_AutoPark_Get
    //Speed_Average_Number_Get
    //Motor_Speed_Average
    //Motor_Torque_Average
    //R_Vhms_Average_Ratio_M03 R��Vhmsƽ��ֵ�������
    //D_Vhms_Average_Ratio_M03 D��Vhmsƽ��ֵ�������
    //Vmh_Delta_Vehco_Average_R,Vmh_Delta_Vehco_Average_D;//ƽ������ADֵ
    
    uint8 Index=0;
    sint32 Motor_Speed_Average_Tmp=0,Motor_Torque_Average_Tmp=0;
  
    for(Index=79;Index>0;Index--)  //���ת�ٻ��� FIFO ����    80���ٶ� 0~79  0 ����  79��� 
    {
         Motor_Speed_Buffer[Index]=Motor_Speed_Buffer[Index-1];
    }
    Motor_Speed_Buffer[0]=Motor_Speed;
    Motor_Speed_Delta=Motor_Speed_Buffer[0]-Motor_Speed_Buffer[Speed_Buffer_Number_Get];  //�����ڼ�ת�ٲ�
    /*
    for (Index=1;Index<(Speed_Buffer_Number_Get+1);Index++)   
    {
        Total_Speed=Total_Speed+Motor_Speed_Buffer[Index];
    }
    */
    //Motor_Speed_Delta=Motor_Speed_Buffer[0]-(sint16)(Total_Speed/Speed_Buffer_Number_Get);  //�����ڼ�ת�ٲ�
    
    
    //Motor_Speed_Average=0;   //��ȡƽ�����ת��
    for(Index=0;Index<Speed_Average_Number_Get;Index++) 
    {
        Motor_Speed_Average_Tmp=Motor_Speed_Average_Tmp+Motor_Speed_Buffer[Index];
    }
    Motor_Speed_Average=(sint16)(Motor_Speed_Average_Tmp/Speed_Average_Number_Get);

    
    for(Index=79;Index>0;Index--)  //���ƽ��ת�ٻ��� FIFO ����    80���ٶ� 0~79  0 ����  79��� 
    {
         Motor_Speed_Average_Buffer[Index]=Motor_Speed_Average_Buffer[Index-1];
    }
    Motor_Speed_Average_Buffer[0]=Motor_Speed_Average;
    
    if (Motor_Speed_Buffer_Type==1)//��ʽ�����ڼ�ת�ٲ�  ʹ��ƽ��ת����ת��
    {
        Motor_Speed_Delta=Motor_Speed_Average_Buffer[0]-Motor_Speed_Average_Buffer[Speed_Buffer_Number_Get];  
    }
   
    
    /*  ƽ��ת���ò��� ע��
    for(Index=Torque_Buffer_Number_AutoPark_Get;Index>0;Index--)  //���ת�ػ��� FIFO ���� 1~Torque_Buffer_Number_AutoPark_Get  0����  
    {
         Motor_Torque_Buffer[Index]=Motor_Torque_Buffer[Index-1];
         Motor_Torque_Average_Tmp=Motor_Torque_Average_Tmp+Motor_Torque_Buffer[Index];
    }
    Motor_Torque_Buffer[0]=Motor_Torque;
    //���ת��ƽ��ֵ����
    Motor_Torque_Average=(sint16)((Motor_Torque_Average_Tmp+Motor_Torque_Buffer[0])/(Torque_Buffer_Number_AutoPark_Get+1));
    */
    
       //δ������δװ������
    if ((Shift_Gears==0)||(Gears_Install==0))
    {
        Vmh_Delta_Vehco_Tmp=(sint16)((((sint32)Motor_Speed_Delta)*(1000/Speed_Buffer_Number_Get))/(sint16)Motor_Vehicle_Speed_Scale_Get); //����ADֵ  mh/10ms 
      
        if (Vmh_Delta_Vehco_Filter_Type==0)   //Vmh_Delta_Vehco�������� 0 ��ʽ82 ��37���� 1 �޹���
        {
            if (Vmh_Delta_Vehco_Tmp >Vmh_Delta_Vehco) 
            {
                  Vmh_Delta_Vehco=(Vmh_Delta_Vehco*8+Vmh_Delta_Vehco_Tmp*2)/10;
            }else
            
            {
                Vmh_Delta_Vehco=(Vmh_Delta_Vehco*3+Vmh_Delta_Vehco_Tmp*7)/10; 
            }
        }else 
        {
            Vmh_Delta_Vehco=Vmh_Delta_Vehco_Tmp;
        }


        //Vmh_Delta_Vehco_Average_R Vmh_Delta_Vehco_Average_D  R_Vhms_Average_Ratio_M03 D_Vhms_Average_Ratio_M03
        //R_Vhms_Average_Ratio_Get  B=B* (A-1)/A + C*1/A     
        //D��ƽ������ADֵ
        /*
        Vmh_Delta_Vehco_Average_D= 
              (Vmh_Delta_Vehco_Average_D * (D_Vhms_Average_Ratio_Get-1))/D_Vhms_Average_Ratio_Get
              + Vmh_Delta_Vehco/D_Vhms_Average_Ratio_Get;          
        */
        //Vmh_Delta_Vehco_Average_D_500

        Vmh_Delta_Vehco_Average_D_500=
              (Vmh_Delta_Vehco_Average_D_500 * (D_Vhms_Average_Ratio_Get-1)+ (sint32)Vmh_Delta_Vehco*500 )
              / D_Vhms_Average_Ratio_Get;
              
        Vmh_Delta_Vehco_Average_D= (sint16)(Vmh_Delta_Vehco_Average_D_500/500);

        //Vmh_Delta_Vehco_Average_D=(sint16) (((sint32)Vmh_Delta_Vehco_Average_D * (D_Vhms_Average_Ratio_Get-1) + Vmh_Delta_Vehco)/D_Vhms_Average_Ratio_Get);  





        //R��ƽ������ADֵ
        /*
        Vmh_Delta_Vehco_Average_R= 
              (Vmh_Delta_Vehco_Average_R * (R_Vhms_Average_Ratio_Get-1))/R_Vhms_Average_Ratio_Get
              + Vmh_Delta_Vehco/R_Vhms_Average_Ratio_Get;
        */
        // Vmh_Delta_Vehco_Average_R=(sint16)  (((sint32)Vmh_Delta_Vehco_Average_R * (R_Vhms_Average_Ratio_Get-1) + Vmh_Delta_Vehco)/R_Vhms_Average_Ratio_Get);

        Vmh_Delta_Vehco_Average_R_500=
              (Vmh_Delta_Vehco_Average_R_500 * (R_Vhms_Average_Ratio_Get-1)+ (sint32)Vmh_Delta_Vehco*500 )
              / R_Vhms_Average_Ratio_Get;
              
        Vmh_Delta_Vehco_Average_R= (sint16)(Vmh_Delta_Vehco_Average_R_500/500);
      
      
      
      }else 
    { //�����ڼ� ��������ٶ�
      

    } 

    
 
    
       



    
}


                                       
//D�����л���ת�ؼ���
void FeedBack_Coast (void) 
{
    sint32 FeedBack_Torque_Coast_Tmp=0;
    
    if ((AccPed>=50) || (SOC>199)||(Ready==0)||(Motor_Speed<=0)||(Vehco_State!=4)  ) 
    {
        FeedBack_Torque_Coast_Tmp=0;
    }else 
    {
        if (Gears_Install==0)   //ֱ��
        {
            FeedBack_Torque_Coast_Tmp=-1*(Intpol_s16s16(FeedBack_Coast_D_Torque_Addr,Motor_Speed_Average));
        }else                 //�൵
        {   
            /*
            //����������ٱ�
            Motor_Vehicle_Speed_Scale_Gears_Out_Get 
            Motor_Vehicle_Speed_Scale_Get       
            Gears_Speed
            �궨ֵΪ�����Ť��
            */
            FeedBack_Torque_Coast_Tmp=-1*(Intpol_s16s16(FeedBack_Coast_D_Torque_Addr,   Gears_Speed  ));
            FeedBack_Torque_Coast_Tmp=  FeedBack_Torque_Coast_Tmp * (sint16)Motor_Vehicle_Speed_Scale_Gears_Out_Get / (sint16)Motor_Vehicle_Speed_Scale_Get;
            
        }

    }
    

    
    if (FeedBack_Torque_Coast_Enable==1) 
    {
         // if (FeedBack_Coast_ModeDelay<(Speed_Buffer_Number_Get)/5 +4)      //���л��� 0 Nm ��ʱ����
         if (FeedBack_Coast_ModeDelay<FeedBack_Coast_ModeDelay_Max)      //���л��� 0 Nm ��ʱ����
         
         {
             // FeedBack_Diff=0;
              FeedBack_Torque_Coast=0;
              FeedBack_Coast_ModeDelay++;
         }else 
         {
             //����ʱ��������б�´���    
              if (FeedBack_Slope_Coast_Enable==1)
              {
                
              
                  if (FeedBack_Torque_Coast>FeedBack_Torque_Coast_Tmp) 
                  {
                      if (Gears_Install==0)   //ֱ��
                      {
                          FeedBack_Torque_Coast_Buffer=FeedBack_Torque_Coast_Buffer-FeedBack_Coast_Diff_Add * FeedBack_Diff_Rate; 
                      }else     //�൵
                      {
                          FeedBack_Torque_Coast_Buffer=FeedBack_Torque_Coast_Buffer-FeedBack_Coast_Diff_Add * FeedBack_Diff_Rate * (sint16)Motor_Vehicle_Speed_Scale_Gears_Out_Get / (sint16)Motor_Vehicle_Speed_Scale_Get; 
                      }
                      FeedBack_Torque_Coast=FeedBack_Torque_Coast_Buffer/10;
                    
                 } 
                  else 
                 {
                      FeedBack_Slope_Coast_Enable=0;              
                 }
              
              }
              else 
              {
                    
                 FeedBack_Torque_Coast=(sint16)FeedBack_Torque_Coast_Tmp;
                 FeedBack_Torque_Coast_Buffer=0;
              }
         }
       
          //FeedBack_Coast_QuitDelay
         if (((Torque_Mode==2)&&(AccPed>=50))||(ABS_Active>0))  //������̤��  ����б�½�0      ���˳�   FeedBack_Coast_QuitDelay��10��ʼ--
         {
             //�˳����л���б���ӳ�
             if ((Motor_Torque<-2)&&(FeedBack_Coast_QuitDelay>0)&&(Motor_Speed>(sint16)Motor_Vehicle_Speed_Scale_Get)) 
             {
                 //FeedBack_Torque_Coast_Quir_Add
                 //FeedBack_Torque_Coast=(Motor_Torque*7)/10;
                 if (Gears_Install==0)   //ֱ��
                 {
                    FeedBack_Torque_Coast_Buffer_Quit=FeedBack_Torque_Coast_Buffer_Quit + FeedBack_Torque_Coast_Quir_Add * FeedBack_Diff_Rate;
                 }else                    //�൵
                 {
                    FeedBack_Torque_Coast_Buffer_Quit=FeedBack_Torque_Coast_Buffer_Quit + FeedBack_Torque_Coast_Quir_Add * FeedBack_Diff_Rate* (sint16)Motor_Vehicle_Speed_Scale_Gears_Out_Get / (sint16)Motor_Vehicle_Speed_Scale_Get; 
                 }
                 
                 
                 FeedBack_Torque_Coast=FeedBack_Torque_Coast_Buffer_Quit/10;
                 FeedBack_Coast_QuitDelay--;   // ÿ�μ�1   �ƶ��˳����10����
                 Torque_Mode=2;
            
             }else 
             {
                  FeedBack_Torque_Coast=0;
                  FeedBack_Coast_QuitDelay=0;
             }
         }
         else 
         {
              FeedBack_Torque_Coast_Buffer_Quit= FeedBack_Torque_Coast*10;
         }
         //�ƶ����ز�Ϊ��
         if (FeedBack_Torque_Coast>0){  FeedBack_Torque_Coast=0;  }
      
    }

    else 
    {
         FeedBack_Torque_Coast=0;
    }
}

//D���ƶ�����ת�ؼ���
void FeedBack_Break (void) 
{ //Ҫ���� BMS���ƹ���  ����ƶ��������ߵȵ�
  // Vmh_Delta_Target_FeedBack_Break  
  // Torque_Increment_FeedBack_Break
  // Speed_Vmh10ms_Break_Addr
  // Torque_Increment_Break_Addr
  // FeedBack_Torque_Break
  // FeedBack_Break_ModeDelay
  // Motor_Max_Torque_Break
  
 
  
  sint32 FeedBack_Torque_Break_Tmp=0;
  sint16 Motor_Max_Torque_Break_Table=0;
  sint16 Bms_MaxCharge_Power=0;
  sint16 Bms_MaxCharge_Power_Curr=0;
  //uint16 Motor_Efficieny=0;
  //sint32 FeedBack_Break_Power_Tmp=0;
  
  if ((SOC>199)||(Torque_Mode!=3)||(Breakk==0)||(Ready==0)||(Motor_Speed<=0)||(Vehco_State!=4)) 
  {
      FeedBack_Torque_Break=0;    
      //FeedBack_Break_Power=0;      
  } 
  else 
  {
     /*  //AD��
      Vmh_Delta_Target_FeedBack_Break=Intpol_s16s16(Speed_Vmh10ms_Break_Addr,Motor_Speed);
      Torque_Increment_FeedBack_Break=Intpol_s16s16(Torque_Increment_Break_Addr,Vmh_Delta_Target_FeedBack_Break-Vmh_Delta_Vehco);
      FeedBack_Torque_Break_Tmp=Motor_Torque+Torque_Increment_FeedBack_Break;
      FeedBack_Torque_Break=FeedBack_Torque_Break_Tmp;
     */  //��ֵ���
      //FeedBack_Torque_Coast_Tmp=-1*(Intpol_s16s16(FeedBack_Coast_D_Torque_Addr,Motor_Speed));
      
      
      //FeedBack_Torque_Break_Tmp=-1*(Intpol_s16s16(FeedBack_Break_D_Torque_Addr,Motor_Speed_Average));
      if (Gears_Install==0)   //ֱ��
      {
          FeedBack_Torque_Break_Tmp=-1*(Intpol_s16s16(FeedBack_Break_D_Torque_Addr,Motor_Speed_Average));
      }else                 //�൵
      {   
          /*
          //����������ٱ�
          Motor_Vehicle_Speed_Scale_Gears_Out_Get 
          Motor_Vehicle_Speed_Scale_Get       
          Gears_Speed
          �궨ֵΪ�����Ť��
          */
          FeedBack_Torque_Break_Tmp=-1*(Intpol_s16s16(FeedBack_Break_D_Torque_Addr,   Gears_Speed  ));
          FeedBack_Torque_Break_Tmp=  FeedBack_Torque_Break_Tmp * (sint32)Motor_Vehicle_Speed_Scale_Gears_Out_Get / (sint32)Motor_Vehicle_Speed_Scale_Get;
          
      }     
     
     
     
     // FeedBack_Torque_Break=FeedBack_Torque_Break_Tmp;  
      //�����繦�ʼ���    ���210.00KW
      //FeedBack_Break_Power_Tmp= ((sint32)Motor_Torque)*(abs(Motor_Speed)+1);
      //FeedBack_Break_Power=(sint16)((FeedBack_Break_Power_Tmp*(Motor_Efficieny/100))/9550);      
  }
  
  
  
    
  if (FeedBack_Torque_Break_Enable==1) 
  {
      //if (FeedBack_Break_ModeDelay<(Speed_Buffer_Number_Get)/5 +4)      //�����ƶ����� 0 Nm ��ʱ����
      if (FeedBack_Break_ModeDelay<FeedBack_Break_ModeDelay_Max)      //�����ƶ����� 0 Nm ��ʱ����
      
      
      
      {
         // FeedBack_Diff=0;
          FeedBack_Torque_Break=0;
          FeedBack_Break_ModeDelay++;
      }else 
      {
          

          
          //����ʱ��������б�´���    
          if ((Breakk>100)&&((sint32)FeedBack_Torque_Break>FeedBack_Torque_Break_Tmp)&&(FeedBack_Slope_Enable==1)&& (Torque_Mode==3)) 
          {
              if (Gears_Install==0)   //ֱ��
              {
                  FeedBack_Torque_Break_Buffer=FeedBack_Torque_Break_Buffer-FeedBack_Break_Diff_Add * FeedBack_Diff_Rate;    
              }else                  //�൵
              {
                  FeedBack_Torque_Break_Buffer=FeedBack_Torque_Break_Buffer-FeedBack_Break_Diff_Add * FeedBack_Diff_Rate* (sint16)Motor_Vehicle_Speed_Scale_Gears_Out_Get / (sint16)Motor_Vehicle_Speed_Scale_Get;    
              }
              
              FeedBack_Torque_Break=FeedBack_Torque_Break_Buffer/10;
              
          } 
          else 
          {   //б����� �����ƶ�����
              FeedBack_Torque_Break=(sint16)FeedBack_Torque_Break_Tmp;
              FeedBack_Torque_Break_Buffer=0;
              FeedBack_Slope_Enable=0;              
          }
      }
     



      //FeedBack_Break_QuitDelay
      if (   ((Torque_Mode==3)&&(Breakk<=100))||(ABS_Active>0))  //���ƶ�̤��  ����б�½�0      ���˳�   FeedBack_Break_QuitDelay��10��ʼ--
      {
         if ((Motor_Torque<-2)&&(FeedBack_Break_QuitDelay>0)&&(Motor_Speed>(sint16)Motor_Vehicle_Speed_Scale_Get)) 
         {
            //FeedBack_Torque_Break=(Motor_Torque*9)/10;
            if (Gears_Install==0)   //ֱ��
            {
                FeedBack_Torque_Break_Buffer_Quit=FeedBack_Torque_Break_Buffer_Quit + FeedBack_Torque_Break_Quir_Add * FeedBack_Diff_Rate;
            }else                   //�൵
            {
                FeedBack_Torque_Break_Buffer_Quit=FeedBack_Torque_Break_Buffer_Quit + FeedBack_Torque_Break_Quir_Add * FeedBack_Diff_Rate* (sint16)Motor_Vehicle_Speed_Scale_Gears_Out_Get /(sint16) Motor_Vehicle_Speed_Scale_Get;
            }
            
            FeedBack_Torque_Break=FeedBack_Torque_Break_Buffer_Quit/10;            
            
            FeedBack_Break_QuitDelay--;   // ÿ�μ�1   �ƶ��˳����10����
            Torque_Mode=3;
        
         } else 
         {
            //FeedBack_Torque_Break=0;
            FeedBack_Break_QuitDelay=0;                                                  
            if (ABS_Active>0){  FeedBack_Torque_Break=0; }
           
         }
      }else
      {
          FeedBack_Torque_Break_Buffer_Quit= FeedBack_Torque_Break*10;
      
      }
      
      
      
      //�ƶ����ز�Ϊ��
      if (FeedBack_Torque_Break>0){  FeedBack_Torque_Break=0;  }
  } 
  else
  {
      FeedBack_Torque_Break=0;
  }
 
}



//D���䶯ת�ؼ���
void Crawl_D_Torque(void) 
{
     uint16  Crawl_D_Max_Torque_Table=0;
   
    //Vmh_Delta_Target  Ŀ��ADֵ  mh/10ms
    //Vmh_Delta_Vehco   ����ADֵ  mh/10ms 
    //Torque_Increment  ת������  0.1Nm
    //Motor_Vehicle_Speed_Scale_Gears_Out_Get
    //Gears_Install

    if ((Limit_Mode==1)||(Torque_Mode!=1)&&(Torque_Mode!=4)  || (Crawl_D_Torque_Enable==0)||(Motor_Speed/(sint16)Motor_Vehicle_Speed_Scale_Get>15)|| ((sint32)( Motor_Speed/Motor_Vehicle_Speed_Scale_Get)<-10)||(Dang_P==1)||(Ready==0)||(Vehco_State!=4)) //�ٶȳ���-10~15km/h P������ �䶯ת�ع�0 
    {
        //Crawl_D_Torque_New=0;
        Crawl_D_Torque_New=2; //��������ģʽ������ת��Ϊ1
    } 
    else 
    {
        if (Gears_Install==0 )
        {       //����
            Vmh_Delta_Target_Crawl_D=Intpol_s16s16(Speed_Vmh10ms_Crawl_D_Addr,Motor_Speed);
        }else   //�൵  �õ��ת�ٷ��ƴ�����ת�ٻ�ȡ ����Ŀ��
        {
            Vmh_Delta_Target_Crawl_D=Intpol_s16s16(Speed_Vmh10ms_Crawl_D_Addr,  
            (sint16)(((sint32)Motor_Speed)*Motor_Vehicle_Speed_Scale_Gears_Out_Get/Motor_Vehicle_Speed_Scale_Get)   );
        }
        
        Torque_Increment_Crawl_D_Get=Intpol_s16s16(Torque_Increment_Crawl_D_Addr,Vmh_Delta_Target_Crawl_D-Vmh_Delta_Vehco);
        
        if (Breakk>0)
        {

            //�ƶ����������Ť�ط�ֵ
             Crawl_D_Max_Torque_Table=Intpol_s16u16(Speed_Max_Torque_Crawl_D_Break_Addr,Motor_Speed_Average);
            
            
            
        } 
        else 
        {
            //���ƶ����������Ť�ط�ֵ
            Crawl_D_Max_Torque_Table=Intpol_s16u16(Speed_Max_Torque_Crawl_D_Addr,Motor_Speed_Average);
            
    
        }
        // �������20%����
        Crawl_D_Max_Torque=(Crawl_D_Max_Torque*30+Crawl_D_Max_Torque_Table*20)/50;
        
        //Crawl_D_Torque_Buffer==Demand_Torque*10; 10������
        Crawl_D_Torque_New=Demand_Torque+Torque_Increment_Crawl_D_Get;
        
        
        if (Crawl_D_Torque_New<1) {
            Crawl_D_Torque_New=1;
        }
    
        if (Crawl_D_ModeDelay < (Speed_Buffer_Number_Get)/Crawl_D_Torque_Delay_Max_Get +2)  //��ʼ�����䶯��ʱ����
        {
           Crawl_D_Torque_New=1;//�����ڼ�ת�غ㶨����ֵ
           Crawl_D_ModeDelay++;
        }    
        
        if  ( (((Lock_Vehicle_Mode==2)&&(Lock_Vehicle_Enable==1))
            ||((Heartbeat_Lock_Mode==2)&&(Heartbeat_Lock_Vehicle_Enable==1))) 
            &&(Lock_Vehicle_Active==1) )  //2��Զ������ ���ն�����
        {
            
            Veh_Limit_Speed=0;
            Crawl_D_Torque_New=0;
        }
        
        
 
        if (Crawl_D_Torque_New>=Crawl_D_Max_Torque) //�������Ť��
        {
            Crawl_D_Torque_New=Crawl_D_Max_Torque;
        }
    }
}

//D�Զ�פ��ת�ؼ���
void Auto_Parking(void) 
{    
    sint16 AccPed100_Torque=0,Vmh_Delta_Target_Tmp=0;
    

    //Auto_Park_Speed_Accumulation   ��������ת���ۼ� 
    //Auto_Park_Speed_Times          ��������ʱ���ۼ� Motor_Speed_Buffer[0]  Auto_Park_Speed_Times_Max
    if ( (Dang_L4bit==10)  && (Motor_Speed_Buffer[0]<0) 
    //&& (Breakk==0) 
    ) 
    {
        if (Motor_Speed_Buffer[0]<Motor_Speed_Buffer[1])
        {
            Auto_Park_Speed_Times++;
            Auto_Park_Speed_Accumulation=Auto_Park_Speed_Accumulation+Motor_Speed;
            if (Auto_Park_Speed_Accumulation<-32000)
            {
                Auto_Park_Speed_Accumulation=-32000;
            }
            if (Auto_Park_Speed_Times>1000) 
            {
                Auto_Park_Speed_Times=1000;
            }
        }else 
        {
            Auto_Park_Speed_Times=0;
            Auto_Park_Speed_Accumulation=0;  
        }
    }
    else 
    {
        Auto_Park_Speed_Times=0;
        Auto_Park_Speed_Accumulation=0; 
    }
    
    
    
    
    if ((Motor_Speed>((sint16)Motor_Vehicle_Speed_Scale_Get*5))
    ||((sint32)Motor_Speed/(sint32)Motor_Vehicle_Speed_Scale_Get<-10)
    ||(Dang_P==1)||(Ready==0)||(Vehco_State!=4)) //�ٶȳ���5km/h�˳�����
    {
        //Auto_Parking_D_Torque=0;
        Auto_Parking_D_Torque=1; //����ɲ�ڼ���С����ת��Ϊ1//����б�½�0
    } 
    else 
    {
        
        Vmh_Delta_Target_Tmp=Intpol_s16s16(Speed_Vmh10ms_D_Park_Addr,Motor_Speed_Average);
        if (Vmh_Delta_Target_Tmp>Vmh_Delta_Target_Parking_D)   //פ��Ŀ��ADֵ�仯б�´���
        {
            Vmh_Delta_Target_Parking_D=(Vmh_Delta_Target_Parking_D*2+ Vmh_Delta_Target_Tmp*8)/10;
        } else 
        {
            Vmh_Delta_Target_Parking_D=(Vmh_Delta_Target_Parking_D*7+ Vmh_Delta_Target_Tmp*3)/10;
        }
        
        Torque_Increment_Parking_D=Intpol_s16s16(Torque_Increment_D_Park_Addr,Vmh_Delta_Target_Parking_D-Vmh_Delta_Vehco_Average_D);
        
        //Auto_Parking_D_Torque=Motor_Torque+Torque_Increment_Parking_D; //��������ת����Ϊ����ֵ
        Auto_Parking_D_Torque=Demand_Torque+Torque_Increment_Parking_D; //���������ת����Ϊ����ֵ
        //Auto_Parking_D_Torque=Motor_Torque_Average+Torque_Increment_Parking_D; //��ƽ��ת����Ϊ����ֵ
        
        
        //AutoPark_Motor_Speed_Torque_Addr
        //AccPed100_Torque=Intpol_s16u16(Motor_Speed_Torque_Addr,Motor_Speed); //�ٶ������������ת��
        
        //AccPed100_Torque=Intpol_s16u16(AutoPark_Motor_Speed_Torque_Addr,Motor_Speed); //�������ת��
        AccPed100_Torque=Intpol_s16u16(AutoPark_Motor_Speed_Torque_Addr,Motor_Speed_Average); //�������ת��
        
        if (Auto_Parking_D_Torque>AccPed100_Torque) 
        { 
            Auto_Parking_D_Torque=AccPed100_Torque;
        }
        if (Auto_Parking_D_Torque<=0) {Auto_Parking_D_Torque=0;}
        if (Auto_Parking_D_Torque_Enable==0)
        {Auto_Parking_D_Torque=0;
        }
    }
    
    
    
}
//R�Զ�פ��ת�ؼ���
void Auto_Parking_R(void) 
{   
    sint16 AccPed100_Torque=0,Vmh_Delta_Target_Tmp=0;
    if ((Motor_Speed/(sint16)Motor_Vehicle_Speed_Scale_Get<-5)||(Motor_Speed/(sint16)Motor_Vehicle_Speed_Scale_Get>10)||(Dang_P==1)||(Ready==0)||(Vehco_State!=4)) //�ٶȳ���-5km/h�˳�����
    {
        Auto_Parking_R_Torque=0;
    } 
    else 
    {
        Vmh_Delta_Target_Tmp=Intpol_s16s16(Speed_Vmh10ms_D_Park_Addr,-1*Motor_Speed);
       
       if (Vmh_Delta_Target_Tmp<Vmh_Delta_Target_Parking_R)   //פ��Ŀ��ADֵ�仯б�´���
        {
            Vmh_Delta_Target_Parking_R=(Vmh_Delta_Target_Parking_R*2+ Vmh_Delta_Target_Tmp*8)/10;
        } else 
        {
            Vmh_Delta_Target_Parking_R=(Vmh_Delta_Target_Parking_R*7+ Vmh_Delta_Target_Tmp*3)/10;
        }
       
       
       
       
        Torque_Increment_Parking_R=Intpol_s16s16(Torque_Increment_D_Park_Addr,Vmh_Delta_Target_Parking_R-(-1*Vmh_Delta_Vehco));
       
        Auto_Parking_R_Torque=Demand_Torque-Torque_Increment_Parking_R; //���������ת����Ϊ����ֵ       
        //Auto_Parking_R_Torque=Motor_Torque_Average-Torque_Increment_Parking_R; 
        AccPed100_Torque=-1*(Intpol_s16u16(Motor_Speed_Torque_Addr,Motor_Speed));
        if (Auto_Parking_R_Torque<AccPed100_Torque) 
        { 
            Auto_Parking_R_Torque=AccPed100_Torque;
        }
        if (Auto_Parking_R_Torque>-1) { Auto_Parking_R_Torque=-1;}
        
        if (Auto_Parking_R_Torque_Enable==0) {
        Auto_Parking_R_Torque=0;
        }
    }
}




//����ת�ظ���ֵ���� 
void AccPed_Torque_Calculation(void)
{   //Pow_Down_Enable �������ƹ��ʱ�־ 
    uint16 AccPed100_Torque=0,AccPed_Torque_Check_Table=0 ;
    sint16 R_Motor_Speed=0,R_Motor_Torque=0;//R�����ת��ת��
    sint16 R_Auto_Torque=0;// R�����ټ�����������
    sint16 Auto_Torque=0;
    uint8 Limit_Mode_zi[7]={0};
    sint32 LimtRpm=0;
    
    //Motor_Speed_Torque_Addr ת��_���ת��_75KW  ����114.4
    //ʹ��ƽ���ٶȲ�ѯ 
    AccPed100_Torque=Intpol_s16u16(Motor_Speed_Torque_Addr,Motor_Speed_Average);//100%���ſ���ת��
    AccPed_Torque_Check_Table=(uint16)((AccPed*((uint32)AccPed100_Torque))/10000);  //(sint16)((1000*((uint32)AccPed))/10000) �����ſ��ȱ�������ת��
    
    //�Ŵ�10�����������
    /*
    AccPed_Torque_ModeDelay=0; //����ģʽ����0-1Nm����
    AccPed_Torque_ModeDelay=AccPed_ModeDelay;
    AccPed_Slope_Delay
    */
    
    
    if (AccPed_Torque_ModeDelay<AccPed_ModeDelay)      //���Ÿ��� 1 Nm ��ʱ����
    {
            
        AccPed_Torque_Buffer=10;
        AccPed_Torque_ModeDelay++;
    }else 
    {
        //����ʱ���Ÿ�������б�´���    
        if (( AccPed_Slope_Delay_Num<AccPed_Slope_Delay ) && (AccPed_Torque_Buffer<AccPed_Torque_Check_Table*10)   )
        {
            AccPed_Torque_Buffer=AccPed_Torque_Buffer+AccPed_Torque_Diff_Add; 
            AccPed_Slope_Delay_Num++;
            AccPed_Slope_Delay_2key=1; 
        } 
        else 
        {
            if( (AccPed_Torque_Buffer<AccPed_Torque_Check_Table*10) && (AccPed_Slope_Delay_2key==1)     )
            {   //�˳��̶�б�º�������С�����Ÿ���ֵ ��5%����
                AccPed_Torque_Buffer=AccPed_Torque_Buffer + AccPed_Torque_Check_Table/2;
            
            } 
            else 
           {
               AccPed_Torque_Buffer=AccPed_Torque_Check_Table*10;
               AccPed_Slope_Delay_Num=AccPed_Slope_Delay;      
               AccPed_Slope_Delay_2key=0;    
          }
        }
    }
    
   
    
    
    
    switch(Dang_L4bit)  //���ݵ�λ����ת�ط���
    {
        case 10:
            AccPed_Torque=(sint16)AccPed_Torque_Buffer/10;// D 
            break;
        case 5:
            AccPed_Torque=-1*(sint16)AccPed_Torque_Buffer/10;//R
            break;
        case 0:
        case 1:
            AccPed_Torque=0;// N P
        default:
            break;
    }
    //��������뽵���ʴ��� ?                                         
    
    
    if ((Ready==0)||(Vehco_State!=4)){AccPed_Torque=0;} //��δREADY  ������Ч
    
    
    //����3��������������
    
    Veh_Limit_Speed=0;
    if (((Gears_Install==0)||( Shift_Gears==0  ))&&(bDiSatus[7]==0 ) &&       //����100���
       (AccPed_Torque_100_Enable==1)&& (Dang_L4bit==10)&&(Motor_Speed/(sint16)Motor_Vehicle_Speed_Scale_Get>(D100_Max_Speed-10))) //D������100 
    {
        Vmh_Delta_Target=Intpol_s16s16(Speed_Vmh10ms_General_Addr,Motor_Speed_Average-D100_Max_Speed*(sint16)Motor_Vehicle_Speed_Scale_Get); //Ŀ�공�� 100*Motor_Vehicle_Speed_Scale_Get
        Torque_Increment=Intpol_s16s16(Torque_Increment_General_Addr,Vmh_Delta_Target-Vmh_Delta_Vehco_Average_D);
        
        Auto_Torque=Demand_Torque+Torque_Increment;
        
        if (AccPed_Torque>Auto_Torque) //������Ÿ���ת�ش�������ת�أ���ʹ������ת��
        {
            AccPed_Torque=Auto_Torque;
            Limit_Mode_zi[2]=1;
            Veh_Limit_Speed=D100_Max_Speed;
        }
        if (AccPed_Torque<Auto_Torque_Min_Get){ AccPed_Torque=Auto_Torque_Min_Get;}
    }
    if ( (Err_AccPed ==1)&&(Dang_L4bit==10)&&(Motor_Speed/(sint16)Motor_Vehicle_Speed_Scale_Get>0)) //���Ź���ʱ����30  
    {
        Vmh_Delta_Target=Intpol_s16s16(Speed_Vmh10ms_General_Addr,Motor_Speed_Average-Err_AccPed_Max_Speed*(sint16)Motor_Vehicle_Speed_Scale_Get); //Ŀ�공�� 30*Motor_Vehicle_Speed_Scale_Get
        Torque_Increment=Intpol_s16s16(Torque_Increment_General_Addr,Vmh_Delta_Target-Vmh_Delta_Vehco_Average_D);
       
        Auto_Torque=Demand_Torque+Torque_Increment;

        if (AccPed_Torque>Auto_Torque) //������Ÿ���ת�ش�������ת�أ���ʹ������ת��
        {
            AccPed_Torque=Auto_Torque;
            Limit_Mode_zi[1]=1;
            Veh_Limit_Speed=Err_AccPed_Max_Speed;
        }       
        if (AccPed_Torque<Auto_Torque_Min_Get){ AccPed_Torque=Auto_Torque_Min_Get;}
    }

    if  ((    ((AccPed>100)&&(Limit_P==1))      
    || (Dang_P==1))&&(Dang_L4bit==10)&&(Motor_Speed/(sint16)Motor_Vehicle_Speed_Scale_Get>1))  //����ɲ���� 20
    {
        Limit_P=1;
        Vmh_Delta_Target=Intpol_s16s16(Speed_Vmh10ms_General_Addr,Motor_Speed_Average-P_Max_Speed*(sint16)Motor_Vehicle_Speed_Scale_Get); //Ŀ�공�� 20*Motor_Vehicle_Speed_Scale_Get
        Torque_Increment=Intpol_s16s16(Torque_Increment_General_Addr,Vmh_Delta_Target-Vmh_Delta_Vehco_Average_D);
       
        Auto_Torque=Demand_Torque+Torque_Increment;
        if ((AccPed_Torque>Auto_Torque)&&(AccPed_Torque_P_Enable==1)) //������Ÿ���ת�ش�������ת�أ���ʹ������ת��
        {
            AccPed_Torque=Auto_Torque;
            Limit_Mode_zi[0]=1;
            
            Veh_Limit_Speed=P_Max_Speed;
        }
        //������Сת��
        if (AccPed_Torque<Auto_Torque_Min_Get){ AccPed_Torque=Auto_Torque_Min_Get;}

    }else 
    {
       if ((Dang_P==0)||(Dang_L4bit!=10))   {
        Limit_P=0; 
       }
    }

    if  ((Dang_L4bit==5)&&((sint32)Motor_Speed/(sint16)Motor_Vehicle_Speed_Scale_Get<0))  //��������   1km/h����
    {
        R_Motor_Speed=-1*Motor_Speed_Average;
       
        R_Motor_Torque=-1*Demand_Torque;
        
        Vmh_Delta_Target=Intpol_s16s16(Speed_Vmh10ms_General_Addr,R_Motor_Speed-R_Max_Speed*(sint16)Motor_Vehicle_Speed_Scale_Get); //Ŀ�공�� 10*Motor_Vehicle_Speed_Scale_Get    
        Torque_Increment=Intpol_s16s16(Torque_Increment_General_Addr,Vmh_Delta_Target-(-1*Vmh_Delta_Vehco_Average_R));//R������ADֵҪȡ��ֵ

        R_Auto_Torque=-1*(R_Motor_Torque+Torque_Increment);
        
        if  ((AccPed_Torque<R_Auto_Torque)&&(AccPed_Torque_R_Enable==1)) //������Ÿ���ת�ش�������ת�أ���ʹ������ת��  R��Ϊ�� �����ã�
        {
            AccPed_Torque=R_Auto_Torque;
            Veh_Limit_Speed=R_Max_Speed;
        }
        //�������ת��
        if (AccPed_Torque>Auto_Torque_Max_Get){ AccPed_Torque=Auto_Torque_Max_Get;}


    }
    /*
    extern uint8 Lock_Vehicle_Active; //����״̬����
    extern uint8 Lock_Vehicle_Mode;   //����״̬      0 δ���� 1 1������      2 2������ 
    extern uint8 Heartbeat_Lock_Mode; //��������״̬  0 δ���� 1 ������������ 2 �������� 
    //Lock_Level_1_Max_Speed 1�����������

    */

    if  ((Dang_L4bit==10)&&(Lock_Vehicle_Enable==1)&&(Lock_Vehicle_Mode==1)&&(Lock_Vehicle_Active==1) 
        &&(Motor_Speed_Average/(sint16)Motor_Vehicle_Speed_Scale_Get>(sint16)(Lock_Level_1_Max_Speed-10)))  //1��Զ���������� 30
    
    
    {
        Limit_Mode_zi[3]=1;
        Veh_Limit_Speed=Lock_Level_1_Max_Speed;
               
        Vmh_Delta_Target=Intpol_s16s16(Speed_Vmh10ms_General_Addr,Motor_Speed_Average-Lock_Level_1_Max_Speed*(sint16)Motor_Vehicle_Speed_Scale_Get); //Ŀ�공�� 30*Motor_Vehicle_Speed_Scale_Get
        Torque_Increment=Intpol_s16s16(Torque_Increment_General_Addr,Vmh_Delta_Target-Vmh_Delta_Vehco_Average_D);
        Auto_Torque=Demand_Torque+Torque_Increment;
        if ((AccPed_Torque>Auto_Torque)) //������Ÿ���ת�ش�������ת�أ���ʹ������ת��
        {
            AccPed_Torque=Auto_Torque;
        }
        //������Сת��
        if (AccPed_Torque<Auto_Torque_Min_Get){ AccPed_Torque=Auto_Torque_Min_Get;}

    }



    if  ( (((Lock_Vehicle_Mode==2)&&(Lock_Vehicle_Enable==1))
        ||((Heartbeat_Lock_Mode==2)&&(Heartbeat_Lock_Vehicle_Enable==1))) 
        &&(Lock_Vehicle_Active==1) )  //2��Զ������ ���ն�����
    {
        Limit_Mode_zi[4]=1;
        Veh_Limit_Speed=0;
        AccPed_Torque=0;
    }


    
    /*
    //����������ר��
    extern uint8 Third_Lock_Active;              //��������
    extern uint8 Third_Lock_Mode_Active;         //��Ч����ģʽ
    extern uint8 Third_Lock_LimitSpeed_Active;   //��Ч�����ٶ� 
    
    */
    
    //����   Third_Lock_LimitSpeed_Active
    if  ((Dang_L4bit==10)&&(Third_Lock_Mode_Active==1)&&(Third_Lock_Active==1) 
        &&(Motor_Speed_Average/(sint16)Motor_Vehicle_Speed_Scale_Get>(sint16)(Third_Lock_LimitSpeed_Active-10)))  
    {
        Limit_Mode_zi[5]=1;
        Veh_Limit_Speed=Third_Lock_LimitSpeed_Active;
        LimtRpm=(sint32)Third_Lock_LimitSpeed_Active * Motor_Vehicle_Speed_Scale_Get;
        if (LimtRpm>15000){           LimtRpm=15000;        }
        Vmh_Delta_Target=Intpol_s16s16(Speed_Vmh10ms_General_Addr,Motor_Speed_Average- (sint16)LimtRpm); //Ŀ�공�� 30*Motor_Vehicle_Speed_Scale_Get
        Torque_Increment=Intpol_s16s16(Torque_Increment_General_Addr,Vmh_Delta_Target-Vmh_Delta_Vehco_Average_D);
        Auto_Torque=Demand_Torque+Torque_Increment;
        if ((AccPed_Torque>Auto_Torque)) //������Ÿ���ת�ش�������ת�أ���ʹ������ת��
        {
            AccPed_Torque=Auto_Torque;
        }
        //������Сת��
        if (AccPed_Torque<Auto_Torque_Min_Get){ AccPed_Torque=Auto_Torque_Min_Get;}
       
        
    }

    //����
    if  ( (Third_Lock_Mode_Active==2)&&(Third_Lock_Active==1) )  //2��Զ������ ���ն�����
    {
        Limit_Mode_zi[6]=1;
        Veh_Limit_Speed=0;
        AccPed_Torque=0;
    }
    
    
    
    if ((Limit_Mode_zi[0]==1)||(Limit_Mode_zi[1]==1)||(Limit_Mode_zi[2]==1)||(Limit_Mode_zi[3]==1)
      || (Limit_Mode_zi[4]==1) || (Limit_Mode_zi[5]==1)|| (Limit_Mode_zi[6]==1)) // ����ģʽ
    {
        Limit_Mode=1;
    }else
    {
        Limit_Mode=0;
    }
    //Oil_Pump_Install,Oil_Pump_Fault
    //���� �ͱù���ʱ����Ť������
    if ((Oil_Pump_Install==1)&&(Oil_Pump_Fault==1))
    {
        Limit_Mode=1;
        Veh_Limit_Speed=0;
        AccPed_Torque=0;        
        
    }
    
}

//��ع�������ת��
void Torque_Power_Limit(void) 
{
    /*
    sint16 Torque_High_Limit,Torque_Low_Limit; //���ת�������� ������ 1  0��5000  -5000��0 

    uint16 Max_Discharge_Power_Limit,Max_Charge_Power_Limit;//���10s��ŵ繦������  0~250 ��λ�� kW�� ƫ�ƣ� 0 �������ߣ� 1
    uint16 Max_Discharge_Current_Limit,Max_Charge_Current_Limit;//����ŵ��������  0��500 ��λ�� A�� ƫ�ƣ� 0 �������ߣ� 2

    uint16 B_Voltage,Output_Voltage;            //��ص�ѹ�������ѹ  0~800 ��λ�� V�� ƫ�ƣ� 0 ��   �����ߣ� 0.2 ��10���Ŵ���㣬0��8190 4095*2=8190��
    sint16 B_Current;                           //��ص���         -500~500 ��λ�� A�� ƫ�ƣ� -500 �������ߣ� 0.02 (10���Ŵ����-5000��5000����������ʾ�Ե�س�磬��������ʾ��طŵ�
    uint16 B_Charge_Current,B_Discharge_Current;//��س䡢�ŵ����   0��500 ��λ�� A�� ƫ�ƣ�0 ��    �����ߣ� 0.02���Ŵ�10������ 0��5000��
    sint16 B_Power;                             //��س�Ź���     -325~325 ��λ�� kW��ƫ�ƣ� -325�� �����ߣ� 0.01 (100���Ŵ���㣩�����ʱ�ʾ�Ե�س�磬 �����ʱ�ʾ��طŵ�    


    sint16 Driver_Torque_Limit=0;//������������
    sint16 Bms_Max_Torque_Driver; //BMS��������ת��
    sint16 Motor_Max_Torque_Driver;//�����������ת��
    */
    
    sint16 Motor_Max_Torque_Break_Table=0;
    sint32 Bms_MaxCharge_Power=0;
    //sint32 Bms_MaxCharge_Power_Curr=0;
    sint16 Torque_LowHigh_Limit=0;
   // sint32 Bms_Max_Torque_Break_Tmp=0;
   // sint32 Bms_Max_Torque_Driver_Tmp=0;
    
    sint16 Motor_Max_Torque_Driver_Table=0;
    sint32 Bms_MaxDisCharge_Power=0;
    //sint32 Bms_MaxDisCharge_Power_Curr=0;
    
    sint16 Torque_MotoDriver_Limit=0;
    
    //uint16 Motor_Efficieny=0;
    //uint16 Motor_Efficieny_Pow_Limit=0;//���������Ч��
    
    


       // Bms_MaxCharge_Power_Curr=((sint32)Max_Charge_Current_Limit*B_Voltage/100 );  //����BMS���������������г�繦�� 0.00kw   0.00A  0.0V
        
       // Bms_MaxDisCharge_Power_Curr=((sint32)Max_Discharge_Current_Limit*B_Voltage/100 );  //����BMS���ŵ�����������зŵ繦�� 0.00kw   0.00A  0.0V                
        
        //��ȡBMS���Ƴ�繦��  ȡ��С
       // Bms_MaxCharge_Power=(Bms_MaxCharge_Power_Curr<Max_Charge_Power_Limit) ? Bms_MaxCharge_Power_Curr:Max_Charge_Power_Limit;
        
      //  Bms_MaxDisCharge_Power=(Bms_MaxDisCharge_Power_Curr<Max_Discharge_Power_Limit) ? Bms_MaxDisCharge_Power_Curr:Max_Discharge_Power_Limit;
        //DEBUG_Max_Discharge_Power_Limit DEBUG_Max_Charge_Power_Limit
        
        if (DEBUG_Max_Charge_Power_Limit==0)    
        {
            Bms_MaxCharge_Power=Max_Charge_Power_Limit;
        }else 
        {
            Bms_MaxCharge_Power=DEBUG_Max_Charge_Power_Limit;
        }
        if (DEBUG_Max_Discharge_Power_Limit==0)  
        {
            Bms_MaxDisCharge_Power=Max_Discharge_Power_Limit;
        }else
        {
             Bms_MaxDisCharge_Power=DEBUG_Max_Discharge_Power_Limit;
        }
                
        //��ȡ���Ч��  
        //uint16 Motor_Efficieny_Fix=10000; //���Ч���Զ�����ֵ
        //Driver_Power                  //����繦��  100��
        //sint16 B_Power;               //��س�Ź���     -325~325 ��λ�� kW 
       
         Efficieny_Fix();
         
         Motor_Efficieny_Pow_Limit=(uint16)   ((((uint32)Motor_Efficieny_Fix)* Intpol_u16u16(Speed_Efficieny_Addr,abs(Motor_Speed_Average)))/10000);
        /***************************************************************************************************/
        //��ȡBMS���ƻ���ת��  Bms_MaxCharge_Power ��ع��ʵ�λ�� 0.01KW
        //if ((sint32)Bms_MaxCharge_Power/(1+abs(Motor_Speed))<13) //���Ƽ���ֵ������1241Nm
        //{
          
           // Bms_Max_Torque_Break=-1* 
        //Vehco_Type_Test=2
        switch (Vehco_Type_Test)
        {
            case 1: //����
            case 3: //D07 
        
        
            Bms_Max_Torque_Break_Tmp=-1*  
            
              
                (( 
                    ((   (sint32)Bms_MaxCharge_Power*955/(Motor_Efficieny_Pow_Limit/100)  )  *10) /(abs(Motor_Speed_Average)+1) ));
        
            break;
            default:
            case 2: //D11 D12 ����700N    M02
            Bms_Max_Torque_Break_Tmp=-1*  
             
                (
                (  
                  (  
                      ((sint32)Bms_MaxCharge_Power  *955)/10
                  )*(Motor_Efficieny_Pow_Limit/100)/ (abs(Motor_Speed_Average)+1)
                )
                );            
            break;
        }
        Bms_Max_Torque_Break=(sint16)( (Bms_Max_Torque_Break_Tmp>-3600) ? Bms_Max_Torque_Break_Tmp:-3600);            
        
        //} else 
        //{
        //    Bms_Max_Torque_Break=-1214;
        //}
        /****************************************************************************************************/
        //��ȡBMS��������ת��  Bms_MaxDisCharge_Power ��ع��ʵ�λ�� 0.01KW  
        Bms_Max_Torque_Driver_Tmp=
             
              (
                (  
                  (  
                      ((sint32)Bms_MaxDisCharge_Power  *955)/10
                  )*(Motor_Efficieny_Pow_Limit/100)/ (abs(Motor_Speed_Average)+1)
                )
              );      
                
       Bms_Max_Torque_Driver=(sint16)((Bms_Max_Torque_Driver_Tmp<3600) ? Bms_Max_Torque_Driver_Tmp:3600);
                
        /****************************************************************************************************/
  
  
        //����ȡ �궨���������ת��  
        Motor_Max_Torque_Break_Table=-1* Intpol_s16s16(Motor_Speed_Torque_Break_Addr,abs(Motor_Speed_Average));
       
        Motor_Max_Torque_Driver_Table=Intpol_s16s16(Motor_Speed_Torque_Addr,abs(Motor_Speed_Average));
        
        //��ȡ������ƻ���ת��   Torque_High_Limit   ��������ת�� Torque_MotoDriver_Limit
        if  (Dang_L4bit==5)
        {
            Torque_LowHigh_Limit=-1* Torque_High_Limit;  //ʼ���Ǹ���
            Torque_MotoDriver_Limit=-1* Torque_Low_Limit; //ʼ��������
            
        } else 
        {
            Torque_LowHigh_Limit=Torque_Low_Limit;
            Torque_MotoDriver_Limit=Torque_High_Limit;
        }
        
        
        
        
        
        Motor_Max_Torque_Break=(Motor_Max_Torque_Break_Table>Torque_LowHigh_Limit) ? Motor_Max_Torque_Break_Table:Torque_LowHigh_Limit;
        
        Motor_Max_Torque_Driver=(Motor_Max_Torque_Driver_Table<Torque_MotoDriver_Limit) ? Motor_Max_Torque_Driver_Table:Torque_MotoDriver_Limit; 
        
        
        
        //ȡ���
        Break_Torque_Limit=(Motor_Max_Torque_Break>Bms_Max_Torque_Break) ? Motor_Max_Torque_Break:Bms_Max_Torque_Break;
       
        Driver_Torque_Limit=(Motor_Max_Torque_Driver<Bms_Max_Torque_Driver) ? Motor_Max_Torque_Driver:Bms_Max_Torque_Driver;
        
   if (Torque_Power_Limit_Enable==1) 
    {     
        switch(Dang_L4bit)  //���ݵ�λ
        {
            case 5: //R��
                if (Demand_Torque>  -1* Break_Torque_Limit) 
                { 
                    Demand_Torque= -1* Break_Torque_Limit; 
                }
                
                if  (Demand_Torque< -1* Driver_Torque_Limit)
                {
                    Demand_Torque= -1* Driver_Torque_Limit;
                }
                
                break;
          case 10: //D��
                if (Demand_Torque<Break_Torque_Limit) 
                {
                    Demand_Torque=Break_Torque_Limit; 
                }
                
                if (Demand_Torque>Driver_Torque_Limit)
                {
                    Demand_Torque=Driver_Torque_Limit;                
                }
                
                break;
        }
        
     }
}
//���Ч���Զ�����ϵ������
void  Efficieny_Fix (void) 
{
    //uint16 Motor_Efficieny_Fix=10000; //���Ч���Զ�����ֵ  100.00%
    //sint16 Driver_Power           //����繦��  100��
    //sint16 B_Power;               //��س�Ź���     -325~325 ��λ�� kW 
    //Efficieny_Percent_Filter_And_All

    uint32 Motor_Efficieny_Fix_Tmp;   
    extern uint8 B_Version;//BMSͨѶЭ��汾 
    if ((Efficieny_Fix_Enable==1)&&(B_Version>17)&& (abs(Driver_Power)>1000)&& (abs(B_Power)>1000))           
    {
        Motor_Efficieny_Fix_Tmp=abs  ( (sint16)  ( (   ( (sint32)Driver_Power)   *10000)/B_Power ));
        
        if (Motor_Efficieny_Fix_Tmp>Motor_Efficieny_Fix )     
        {//Ч����
            Motor_Efficieny_Fix=(uint16)  ((((uint32)(Motor_Efficieny_Fix))*(Efficieny_Percent_Filter_And_All[0]-Efficieny_Percent_Filter_And_All[1]) 
            + Motor_Efficieny_Fix_Tmp*Efficieny_Percent_Filter_And_All[1])/Efficieny_Percent_Filter_And_All[0]);
        }else 
        {//Ч�ʽ�
            Motor_Efficieny_Fix=(uint16)  ((((uint32)(Motor_Efficieny_Fix))*(Efficieny_Percent_Filter_And_All[0]-Efficieny_Percent_Filter_And_All[2]) 
            + Motor_Efficieny_Fix_Tmp*Efficieny_Percent_Filter_And_All[2])/Efficieny_Percent_Filter_And_All[0]);
        }        
    }

}


void Driver_Power_Func(void) //�繦�ʼ���
{//Driver_Power                //����繦��  100��
    sint32 Driver_Power_Tmp=0;
    uint16 Motor_Efficieny=0;
    
    //��ȡ���Ч��
    Motor_Efficieny=Intpol_u16u16(Speed_Efficieny_Addr,abs(Motor_Speed_Average));
    
    Motor_Efficieny=(Motor_Efficieny>100)? Motor_Efficieny:100; 
    
    if (Motor_Speed>=0) 
    {
        Driver_Power_Tmp= ((sint32)Motor_Torque)*(Motor_Speed+1);
    
    
    } else 
    {
        Driver_Power_Tmp= ((sint32)Motor_Torque)*Motor_Speed;
    }
   
   
    //����ģʽ�繦��
    if (((Motor_Torque<0)&&(Dang_L4bit==10)) || ((Motor_Torque>0)&&(Dang_L4bit==5)))
    {
        switch (Vehco_Type_Test) 
        {
            default:
            case 1:
                Driver_Power=(sint16)((Driver_Power_Tmp*(Motor_Efficieny/100))/9550);
                break;
            case 2:
                Driver_Power=(sint16)((    (Driver_Power_Tmp/(Motor_Efficieny/100))*1000    )/955);
                break;
        }
    }else
    //����ģʽ�繦��
    {
        Driver_Power=(sint16)((    (Driver_Power_Tmp/(Motor_Efficieny/100))*1000    )/955);
    }
    
    
}


uint8 SetBit8(uint8 *Bitt) //�ϲ�8bitΪByter 
{
    uint8 Index=0;
    uint8 Bytee=0;
    for (Index=0;Index<8;Index++) 
    {
        Bytee&=~(1<<Index);
        Bytee|=((Bitt[Index]&&1)<<Index);
    }
    
    return (Bytee);
    
}

void Get8bit(uint8 *Bitt,uint8 Bytee)  //���byterΪ8��bit
{
    Bitt[0]=1&Bytee;
    Bitt[1]=(2&Bytee)>>1;
    Bitt[2]=(4&Bytee)>>2;
    Bitt[3]=(8&Bytee)>>3;
    Bitt[4]=(16&Bytee)>>4;
    Bitt[5]=(32&Bytee)>>5;
    Bitt[6]=(64&Bytee)>>6;
    Bitt[7]=(128&Bytee)>>7;

}






