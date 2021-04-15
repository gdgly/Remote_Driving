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


uint8 R_Gate_Life[4] ={0};

uint8   LostEPSCommunication=0;
uint16  LostEPSCom_Index=0;
uint8   LostVehcoCommunication=0;
uint16  LostVehcoCom_Index=0;
uint8   LostVehcoStateCommunication=0;
uint16  LostVehcoStateCom_Index=0;
uint8   LostVehcoBreakCommunication=0;
uint16  LostVehcoBreakCom_Index=0;



uint8   LostVehcoSteering_Wheel_AngleCommunication=0;
uint16  LostVehcoSteering_Wheel_AngleCom_Index=0;


//*********************************************************************************//



sint16 Steering_Wheel_Angle_Code=0; //��������ת����  -32768,360��/5000 ��תΪ��
uint8  Steering_Wheel_Angle_ZeroMark=0; //�������Ƿ񾭹�0��У��


sint16 Steering_Wheel_Angle_BaseCAN=0;
uint8  AccPed_AD255=0;
uint8  Breakk_AD255=0;
uint8  Clutch_AD255=0;
uint8  Steering_Wheel_Angle_Mot_Temp=0;
uint8  Steering_Wheel_Angle_Life=0; 

//********************************************************************************//
uint8  EPS_Contral_Mode=0;//EPS����ģʽ
sint16 EPS_Demand_Angle=0;//EPS�Ƕ����� 0.1�Ȳ���   -1080.0~1080.0��
uint8  EPS_Angle_MaxSpeed=0;//EPS �����ٶ�

sint8 Vehco_Acceleration_Command=0;   //���ٶ�ָ�� -12.8~12.7
uint8 EPB_Command=0;    //������ɲָ�� 0 �޶��� 1 ���� 2 �ͷ� 3 �н����ƶ�
uint8 Dang_Command=0;   //��λָ�� 0 N 1 D 2 R 3 ����
uint8 Emergency_Braking=0;  //�����ƶ�ָ��

uint8 DriverMode_Command=0; //�Լ�ģʽ״̬ 0 �ֶ� 1 ���� 2 ���� 3 Զ��
uint8 Door_Command=0;       //���ſ������� 0 �޶��� 1 ���� 2 ���� 3 ����
uint8 Beep_Command=0;       //���ȿ���     0 �޶��� 1 ���� 2 �ر� 3 ����
uint8 R_DriverSystem_State=0;       //Զ�̼�ʻϵͳ״̬ 0 ������ 1 ������� 2 �Ͽ��� 3 �Ͽ�
uint8 R_DriverSystem_Level=2;       //Զ�̼�ʻϵͳ���ż��� 0 �����ţ����̽ӹ� 1 ������ ���˹����� 2 ���״̬����ȫ����
uint8 R_DriverSystem_Fault_Level=0; //Զ�̼�ʻϵͳ���ϼ��� 0 �޹��� 1 ��΢ ��ʾ 2 һ����� ���� 3 ���ع��� �˳�Զ�̼�ʻ״̬
uint8 AC_Mode=1;          //������ů�յ����� 0 �ػ� 1 ���� 2 ���� 3 �Զ�
uint8 AC_Temperature=1;   //������ů�յ������¶� 0.5 ƫ��16

uint8 R_Break_Mode=0;   //Զ���ƶ�����   0x00��δ����,���ؿ��� 0x01������ƶ�������ִ��Զ������ָ�� 0x02:�ںϼ��ȡ���
uint8 R_Break_Rang=0;     //Զ���ƶ������� 0~100%

//***************����Ϊ�ӳ�����������********************

uint8 EPS_Run_Mode;//EPS���ص�ǰ����ģʽ
sint16 EPS_Driver_Torque;//EPS���ؼ�ʻԱ���������� -12.8~12.7Nm
sint16 EPS_Torque; //EPS������� 0.3Nm/bit -38.4~38.1Nm
sint16 EPS_Angle; //EPS��ǰ�Ƕ� 0.1�Ȳ���   -1080.0~1080.0��
uint8 EPS_Temp;//EPS�������¶� 2��/bit 0~126
uint8 EPS_ERR_Level;    //EPS���ϼ���
uint8 EPS_Life=255;         //EPS����



uint8   EPB_State=0;                //EPB״̬
uint8   Dang_State=0;               //������λ״̬
sint16  Motor_Speed=0,Motor_Torque=0;   //���ת�١����Ť��
sint8   Vehco_Acceleration_Back=0; //������ǰ�ϱ����ٶ� -12.8~12.7

uint8   DriverMode_State=0;       //�Լ�ģʽ״̬
uint8   DoorState=0;              //����״̬
sint16  Vehco_Speed=0;            //����   
uint8   SOC=0;                    //SOC ����0.5% 0~100%
uint8   SmartPowerCommand=0;       //����ϵͳ��Դָ��  00 ����  01 �ػ�
uint8   Change_State=0;           //���״̬ 00 ������ 01������ 02 ����� 03 ��������Ҫ���
uint8   Change_Connect_State=0;   //�������״̬ 0 δ���� 1 ���ǹ�Ѳ���
uint8   Vehco_Fault_Level=0;      //�������ϼ���
uint8   VehcoState_Live=255;        //����״̬ͨѶ����

uint16  B_Voltage=0;    //��ص�ѹ 0~800.0V
sint16  B_Current=0;    //��ص��� -500.0~500.0A

uint8   Vehco_Power_State=0;    //��������״̬ 00 INIT ON�� 01 READY 02 ���� 03 �����޸�ѹ

uint8   Isolation_Level=0;      //��Ե���ϵȼ� 0 ���� 1 һ�� 2 ���� 3 ���� 
uint8   MCU_Fault_Level=0;      //��ع��ϼ���  0 ���� 1��΢ 2 һ�� 3 ����
uint8   BMS_Fault_Level=0;      //BMS���ϼ���   0 ���� 1��΢ 2 һ�� 3 ����

uint8   Vehco_Break_Mode=0;     //�ƶ�������ģʽ 0x00��δ����,���ؿ��� 0x01������ƶ�������ִ��Զ������ָ�� 0x02:�ںϼ��ȡ���
uint8   Vehco_Break_Rang=0;     //�ƶ���״̬  0~100%
uint8   Vehco_Break_Life=255;   //�ƶ���������

extern uint8   RightTrunSignal_state;        //��ת���״̬
extern uint8   LeftTrunSignal_state;        //��ת���״̬
extern  uint16 AccPed1;                                     
extern  uint16 AccPed,Breakk;
extern  uint8 bDiSatus[24];
extern  uint16 uADSatus[25];
extern  uint16 uAD4Satus[4];
extern  uint8 bLso;
extern  uint8 bHso;


    #ifndef AECS_HW
      #pragma CODE_SEG DEFAULT
    #else  
      #pragma CODE_SEG SEG_VDL_CAN
    #endif

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

//CAN0������Ϣ������
void CANNet_RecvMsgCAN_A(MSCANDrv_infoMSGS_TYPE infoMsgS)
{
    uint8 RevMsg_BCC_Code=0,Bcc_Index=0;
    //CAN��������
    CANNet_infoIDCAN0UI=infoMsgS.infoMsgIDUI;
    CANNet_datMsgCAN0UB=infoMsgS.datMsgUBA[0];
    //CAN0���շ��ദ��
    
    switch(CANNet_infoIDCAN0UI) 
    {
        case 0x1802A0B0:    
        //EPS״̬
            memcpy(CanRec0,infoMsgS.datMsgUBA,sizeof(CanRec0));      
            /*
            uint8 EPS_Run_Mode;//EPS���ص�ǰ����ģʽ
            sint16 EPS_Driver_Torque;//EPS���ؼ�ʻԱ���������� -12.8~12.7Nm
            sint16 EPS_Torque; //EPS������� 0.3Nm/bit -38.4~38.1Nm
            sint16 EPS_Angle; //EPS��ǰ�Ƕ� 0.1�Ȳ���   -1080.0~1080.0��
            uint8 EPS_Temp;//EPS�������¶� 2��/bit 0~126
            uint8 EPS_ERR_Level;    //EPS���ϼ���
            uint8 EPS_Life;         //EPS����
            */                        
            EPS_Run_Mode=CanRec0[0];
      			if (CanRec0[1]!=0) {EPS_Driver_Torque=(sint16)CanRec0[1]-128;}else{EPS_Driver_Torque=0;}
      			
      			if (CanRec0[2]!=0) {EPS_Torque=(((sint16)CanRec0[2])*3)-384;}else{EPS_Torque=0;}
      			
      			if ((CanRec0[3]!=0)||(CanRec0[4]!=0)) {EPS_Angle=((sint16)CanRec0[3]+((sint16)CanRec0[4])*256)-10800;}else{EPS_Angle=0;}

            EPS_Temp=(CanRec0[5]&0x3f)*2;
            EPS_ERR_Level= CanRec0[6] &0x03 ; 
            
            if  ((EPS_Life==CanRec0[7]-1)||  ((EPS_Life==255)&&(CanRec0[7]==0))) 
            {
                LostEPSCommunication=0;
                LostEPSCom_Index=0;
            }
            EPS_Life=CanRec0[7];
            
            
            
            
            
            break;
        case 0x1804A0B0:    
        //�г�״̬
            memcpy(CanRec0,infoMsgS.datMsgUBA,sizeof(CanRec0));      
            /*
            uint8   EPB_State=0;                //EPB״̬
            uint8   Dang_State=0;               //������λ״̬
            sint16  Motor_Speed=0,Motor_Torque=0;   //���ת�١����Ť��
            sint8   Vehco_Acceleration_Back=0; //������ǰ�ϱ����ٶ�
            */
            EPB_State   =CanRec0[0]&0x03;
            Dang_State  =(CanRec0[0]&0x0c)>>2;             
            Motor_Speed=((sint16)CanRec0[2]*256) +   CanRec0[1]-15000;
            if   ((Motor_Speed>=15000)||(Motor_Speed<=-15000))            {   Motor_Speed=0;         }
            Motor_Torque=((sint16)CanRec0[4]*256) +   CanRec0[3]-5000;
            if   ((Motor_Torque>5000)||(Motor_Torque<-5000))              {   Motor_Torque=0;        }
            Vehco_Acceleration_Back=(sint8) ((sint16)CanRec0[6]-128);

            RevMsg_BCC_Code=0;
            for (Bcc_Index=0;Bcc_Index<7;Bcc_Index++) 
            {
            	  RevMsg_BCC_Code^=CanRec0[Bcc_Index]	;	
            } 
            if (RevMsg_BCC_Code==CanRec0[7])
            {
                LostVehcoCommunication=0;
                LostVehcoCom_Index=0;            
            }
            
            break; 
        case 0x1806A0B0:    
        //����״̬1
            memcpy(CanRec0,infoMsgS.datMsgUBA,sizeof(CanRec0));      
            /*
            uint8   DriverMode_State=0;       //�Լ�ģʽ״̬
            uint8   DoorState=0;              //����״̬
            sint16  Vehco_Speed=0;            //����   
            uint8   SOC=0;                    //SOC ����0.5% 0~100%
            uint8   SmartPowerComand=0;       //����ϵͳ��Դָ��  00 ����  01 �ػ�
            uint8   Change_State=0;           //���״̬ 00 ������ 01������ 02 ����� 03 ��������Ҫ���
            uint8   Change_Connect_State=0;   //�������״̬ 0 δ���� 1 ���ǹ�Ѳ���
            uint8   Vehco_Fault_Level=0;      //�������ϼ���
            uint8   VehcoState_Live=255;        //����״̬ͨѶ����
            */            
            DriverMode_State=CanRec0[0]&0x03;
            DoorState       =(CanRec0[0]&0x04)>>2;
            Vehco_Speed     =(sint16)CanRec0[2]-50;
            if (Vehco_Speed==-50){              Vehco_Speed=0;            }
            SOC             =CanRec0[3];
            SmartPowerCommand=CanRec0[4]&0x03;
            Change_State    =(CanRec0[4]&0x0c)>>2;
            Change_Connect_State=  (CanRec0[4]&0x30)>>4;
            Vehco_Fault_Level=(CanRec0[4]&0xc0)>>6; 

            if  ((VehcoState_Live==CanRec0[7]-1)||  ((VehcoState_Live==255)&&(CanRec0[7]==0))) 
            {
                LostVehcoStateCommunication=0;
                LostVehcoStateCom_Index=0;             
            }
            VehcoState_Live=CanRec0[7];
            
            break; 
        case 0x1810A0B0:    
        //����״̬2
            memcpy(CanRec0,infoMsgS.datMsgUBA,sizeof(CanRec0));      
            /*
            uint16  B_Voltage=0;    //��ص�ѹ 0~800.0V
            sint16  B_Current=0;    //��ص��� -500.0~500.0A            
            */
            B_Voltage= (CanRec0[0]+((uint16)CanRec0[1])*256)*2;
            B_Current= (sint16)((CanRec0[2]+((sint32)CanRec0[3])*256)/5-5000);
            if ((B_Current<=-5000)||(B_Current>=8100)){B_Current=0;} //������ֵ����
             
            break; 
        case 0x1811A0B0:    
        //����״̬3
            memcpy(CanRec0,infoMsgS.datMsgUBA,sizeof(CanRec0));      
            
            break; 
        case 0x1812A0B0:    
        //����״̬4
            memcpy(CanRec0,infoMsgS.datMsgUBA,sizeof(CanRec0));      
            //Vehco_Power_State //��������״̬ 00 INIT ON�� 01 READY 02 ���� 03 �����޸�ѹ
            Vehco_Power_State=(CanRec0[2]&0x30)>>4;
            
            break; 
        case 0x1813A0B0:    
        //��������״̬
            memcpy(CanRec0,infoMsgS.datMsgUBA,sizeof(CanRec0));      
            /*
            uint8   Isolation_Level=0;      //��Ե���ϵȼ� 0 ���� 1 һ�� 2 ���� 3 ���� 
            uint8   MCU_Fault_Level=0;      //��ع��ϼ���  0 ���� 1��΢ 2 һ�� 3 ����
            uint8   BMS_Fault_Level=0;      //BMS���ϼ���   0 ���� 1��΢ 2 һ�� 3 ����
            */            
            Isolation_Level=CanRec0[0]&0x03;
            BMS_Fault_Level=CanRec0[3]&0x03;
            MCU_Fault_Level=(CanRec0[3]&0x0c)>>2;

            break; 
        
        case 0x602:
            //�߿��ƶ�������״̬

            memcpy(CanRec0,infoMsgS.datMsgUBA,sizeof(CanRec0));      
            Vehco_Break_Mode=CanRec0[0];
            Vehco_Break_Rang=CanRec0[1];
            if  ((Vehco_Break_Life==CanRec0[2]-1)||  ((Vehco_Break_Life==255)&&(CanRec0[2]==0)))
            {
            
            }
            Vehco_Break_Life=CanRec0[2];
            RevMsg_BCC_Code=0;
            for (Bcc_Index=0;Bcc_Index<7;Bcc_Index++) 
            {
            	  RevMsg_BCC_Code^=CanRec0[Bcc_Index]	;	
            } 
            if (RevMsg_BCC_Code==CanRec0[7])
            {
                LostVehcoBreakCommunication=0;
                LostVehcoBreakCom_Index=0;            
            }            
        
            break;
          
        default:
            break;
    }                                                                                                                  
    
}

//CAN1������Ϣ������1
void CANNet_RecvMsgCAN_B(MSCANDrv_infoMSGS_TYPE infoMsgS)
{   
    sint32       temp=0;
    uint8 RevMsg_BCC_Code=0,Bcc_Index=0;

    //CAN��������    uint8 CanRec1[8]
    CANNet_infoIDCAN1UI=infoMsgS.infoMsgIDUI;
    CANNet_datMsgCAN1UB=infoMsgS.datMsgUBA[0];
    switch(CANNet_infoIDCAN1UI) 
    {
        case 0x000000AC:
            // ��װģ�ⷽ���� 
            memcpy(CanRec1,infoMsgS.datMsgUBA,sizeof(CanRec1));      
                     
            sint16 Steering_Wheel_Angle_BaseCAN  0.1��
            uint8  AccPed_AD255=0;
            uint8  Breakk_AD255=0;
            uint8  Clutch_AD255=0;
            uint8  Steering_Wheel_Angle_Mot_Temp=0;
            uint8  Steering_Wheel_Angle_Life=0; 


           temp     =   ((sint32)CanRec1[1])*256 + CanRec1[0]  -20000;
            if ((temp<= -20000)||(temp>= 20000))            {     temp=0;       }
           Steering_Wheel_Angle_BaseCAN=(sint16) temp -Wheel_Offset;

           EPS_Demand_Angle
/*
           if (CanRec1[2]>127)
           {
               temp=(uint16)(383-CanRec1[2]);
           }
           
              temp =(uint16)CanRec1[2];
            Steering_Wheel_Angle_BaseCAN=
            AccPed_AD255=CanRec1[2];
            Breakk_AD255=CanRec1[3];
            Clutch_AD255=CanRec1[4];
            Steering_Wheel_Angle_Mot_Temp =CanRec1[5];
            
            if  ((Steering_Wheel_Angle_Life==CanRec1[6]-1)||  ((Steering_Wheel_Angle_Life==255)&&(CanRec1[6]==0)))
            {
                LostVehcoSteering_Wheel_AngleCommunication=0;
                LostVehcoSteering_Wheel_AngleCom_Index=0;            
            }
            Steering_Wheel_Angle_Life=CanRec1[6];            
            
 */           
                        
            break;
        case 0x1234:    
        //ʵ����EPS������״̬
            memcpy(CanRec1,infoMsgS.datMsgUBA,sizeof(CanRec1));      
            temp     =   ((sint32)CanRec1[1])*256 + CanRec1[0]  -32768;
            if ((temp<= -20000)||(temp>= 20000))            {     temp=0;       }
            Steering_Wheel_Angle_Code=(sint16) temp -Wheel_Offset;
            Steering_Wheel_Angle_ZeroMark = CanRec1[2] ;
            break; 
        
        case 0x602:
            //�߿��ƶ�������״̬
            /*
            uint8   Vehco_Break_Mode=0;     //�ƶ�������ģʽ 0x00��δ����,���ؿ��� 0x01������ƶ�������ִ��Զ������ָ�� 0x02:�ںϼ��ȡ���
            uint8   Vehco_Break_Rang=0;     //�ƶ���״̬  0~100%
            uint8   Vehco_Break_Life=0;
            */
            memcpy(CanRec1,infoMsgS.datMsgUBA,sizeof(CanRec1));      
            Vehco_Break_Mode=CanRec1[0];
            Vehco_Break_Rang=CanRec1[1];
            if  ((Vehco_Break_Life==CanRec1[2]-1)||  ((Vehco_Break_Life==255)&&(CanRec1[2]==0)))
            {
            
            }
            Vehco_Break_Life=CanRec1[2];
            RevMsg_BCC_Code=0;
            for (Bcc_Index=0;Bcc_Index<7;Bcc_Index++) 
            {
            	  RevMsg_BCC_Code^=CanRec1[Bcc_Index]	;	
            } 
            //if (RevMsg_BCC_Code==CanRec1[7])
            //{
                LostVehcoBreakCommunication=0;
                LostVehcoBreakCom_Index=0;            
            //}            
        
            break;       

            case 0x000000AC:    
            //ҡ��ת���Ľ���
            memcpy(CanRec1,infoMsgS.datMsgUBA,sizeof(CanRec1));
            if (CanRec1[2] <= 0x7F)
            {
                EPS_Demand_Angle = CanRec1[2]/128*(-1080);
            }  
            else
            (
                 EPS_Demand_Angle = (0xFF-0x80)/128*1080;
            )
            // Vehco_Acceleration_Commandҡ�����ű��Ľ���


            break; 
            
        default:
            break;
    }      
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
     /*
        //SendMsg_BCC_Code  ���У��
        SendMsg_BCC_Code=0;
        for (Bcc_Index=0;Bcc_Index<7;Bcc_Index++) 
        {
        	  SendMsg_BCC_Code^=infoMsgS.datMsgUBA[Bcc_Index]	;	
        } 
        
        infoMsgS.datMsgUBA[7]=SendMsg_BCC_Code;      
     */
    uint8 SendMsg_BCC_Code=0,Bcc_Index=0; //���У��
    uint8 Bit8[8]={0};
    //  VehCo_stNewUB ΪRunning �� Afterrun״̬ʱ������CAN����
    if (((VehCo_stNewUB==_VEHCO_INDEX_STATUS_RUNNING)||(VehCo_stNewUB==_VEHCO_INDEX_STATUS_AFTERRUN)) )
    {
        
        
        MSCANDrv_infoMSGS_TYPE infoMsgS;
        switch(CANNet_idxSchedUB)
        {


            case 0:
            case 20:
            case 40:
            case 60:
            case 80:
                //Զ�̿����� EPS����  20ms
                /*
                uint8  EPS_Contral_Mode=0;//EPS����ģʽ
                sint16 EPS_Demand_Angle=0;//EPS�Ƕ����� 0.1�Ȳ���   -1080.0~1080.0��
                uint8  EPS_Angle_MaxSpeed=0;//EPS �����ٶ�
                */  
                infoMsgS.infoMsgIDUI=0x1801B0C0;
                infoMsgS.bExtIDUB=1;                
                infoMsgS.numLenUB=8;
                infoMsgS.idxMsgPriorityUB=0x01;

                infoMsgS.datMsgUBA[0]=EPS_Contral_Mode;
                infoMsgS.datMsgUBA[1]=R_Gate_Life[0];
                if (Eps_AngleMaxSpeed !=0)
                {
                EPS_Angle_MaxSpeed=Eps_AngleMaxSpeed;
                }
                infoMsgS.datMsgUBA[2]=EPS_Angle_MaxSpeed;
                infoMsgS.datMsgUBA[3]=(uint8)((EPS_Demand_Angle+10800)&0x00ff);
                infoMsgS.datMsgUBA[4]=(uint8)((EPS_Demand_Angle+10800)>>8);
                infoMsgS.datMsgUBA[5]=EPS_Demand_Angle;
                infoMsgS.datMsgUBA[6]=0;
                                
                SendMsg_BCC_Code=0;
                for (Bcc_Index=0;Bcc_Index<7;Bcc_Index++) 
                {
                	  SendMsg_BCC_Code^=infoMsgS.datMsgUBA[Bcc_Index]	;	
                } 
                
                infoMsgS.datMsgUBA[7]=SendMsg_BCC_Code;      
                MSCANDrv_SendMsgCAN_A(infoMsgS);
                R_Gate_Life[0]=(uint8)((R_Gate_Life[0]++)%256);
                break; 
            case 2:
            case 22:
            case 42:
            case 62:
            case 82:
                //Զ�̿����� ��������  20ms
                /*
                sint8 Vehco_Acceleration_Command=0;   //���ٶ�ָ�� -12.8~12.7
                uint8 EPB_Command=0;    //������ɲָ�� 0 �޶��� 1 ���� 2 �ͷ� 3 �н����ƶ�
                uint8 Dang_Command=0;   //��λָ�� 0 N 1 D 2 R 3 ����
                uint8 Emergency_Braking=0;  //�����ƶ�ָ��                
                */
                infoMsgS.infoMsgIDUI=0x1803B0C0;
                infoMsgS.bExtIDUB=1;                
                infoMsgS.numLenUB=8;
                infoMsgS.idxMsgPriorityUB=0x02;  
 
                infoMsgS.datMsgUBA[0]=(uint8)( ((sint16) Vehco_Acceleration_Command)+128);
                //infoMsgS.datMsgUBA[0]=(uint8)( ((sint16) Vehco_Acceleration_Command*10)+128);

                Bit8[0]=0;
                Bit8[1]=0;
                Bit8[2]=EPB_Command&0x01;
                Bit8[3]=(EPB_Command&0x02)>>1;
                Bit8[4]=Dang_Command&0x01;
                Bit8[5]=(Dang_Command&0x02)>>1;
                Bit8[6]=(Dang_Command&0x04)>>2;
                Bit8[7]=(Dang_Command&0x08)>>3;
                infoMsgS.datMsgUBA[1]=SetBit8 (Bit8);
                infoMsgS.datMsgUBA[2]=R_Gate_Life[1];
                infoMsgS.datMsgUBA[3]=Emergency_Braking;
                infoMsgS.datMsgUBA[4]=0;
                infoMsgS.datMsgUBA[5]=0;
                infoMsgS.datMsgUBA[6]=0;     
                          
                SendMsg_BCC_Code=0;
                for (Bcc_Index=0;Bcc_Index<7;Bcc_Index++) 
                {
                	  SendMsg_BCC_Code^=infoMsgS.datMsgUBA[Bcc_Index]	;	
                } 
               
                infoMsgS.datMsgUBA[7]=SendMsg_BCC_Code;      
                MSCANDrv_SendMsgCAN_A(infoMsgS); 
                R_Gate_Life[1]=(uint8)((R_Gate_Life[1]++)%256);                             
                break;
            case 5:
            case 55:
                //Զ�̳�����������  50ms
                /*
                uint8 DriverMode_Command=0; //�Լ�ģʽ״̬ 0 �ֶ� 1 ���� 2 ���� 3 Զ��
                uint8 Door_Command=0;       //���ſ������� 0 �޶��� 1 ���� 2 ���� 3 ����
                uint8 Beep_Command=0;       //���ȿ���     0 �޶��� 1 ���� 2 �ر� 3 ����
                uint8 R_DriverSystem_State=0;       //Զ�̼�ʻϵͳ״̬ 0 ������ 1 ������� 2 �Ͽ��� 3 �Ͽ�
                uint8 R_DriverSystem_Level=0;       //Զ�̼�ʻϵͳ���ż��� 0 �����ţ����̽ӹ� 1 ������ ���˹����� 2 ���״̬����ȫ����
                uint8 R_DriverSystem_Fault_Level=0; //Զ�̼�ʻϵͳ���ϼ��� 0 �޹��� 1 ��΢ ��ʾ 2 һ����� ���� 3 ���ع��� �˳�Զ�̼�ʻ״̬
                uint8 AC_Mode;          //������ů�յ����� 0 �ػ� 1 ���� 2 ���� 3 �Զ�
                uint8 AC_Temperature;   //������ů�յ������¶� 0.5 ƫ��16
                */
                infoMsgS.infoMsgIDUI=0x1805B0C0;
                infoMsgS.bExtIDUB=1;                
                infoMsgS.numLenUB=8;
                infoMsgS.idxMsgPriorityUB=0x03;  

                Bit8[0]=DriverMode_Command&0x01;
                Bit8[1]=(DriverMode_Command&0x02)>>1;
                Bit8[2]=Door_Command&0x01;
                Bit8[3]=(Door_Command&0x02)>>1;
                Bit8[4]=Beep_Command&0x01;
                Bit8[5]=(Beep_Command&0x02)>>1;
                Bit8[6]=0;
                Bit8[7]=0; 
                infoMsgS.datMsgUBA[0]=SetBit8 (Bit8);
                //infoMsgS.datMsgUBA[1]=0;
                Bit8[0]= LeftTrunSignal_state&0x01;
                Bit8[1]=( LeftTrunSignal_state&0x02)>>1;
                Bit8[2]=RightTrunSignal_state&0x01;
                Bit8[3]=( RightTrunSignal_state&0x02)>>1;
                Bit8[4]=0;
                Bit8[5]=0;
                Bit8[6]=0;
                Bit8[7]=0; 
                infoMsgS.datMsgUBA[1]=SetBit8 (Bit8);       
                infoMsgS.datMsgUBA[2]=0;
                infoMsgS.datMsgUBA[3]=0;

                Bit8[0]=R_DriverSystem_State&0x01;
                Bit8[1]=(R_DriverSystem_State&0x02)>>1;
                Bit8[2]=R_DriverSystem_Level&0x01;
                Bit8[3]=(R_DriverSystem_Level&0x02)>>1;
                Bit8[4]=R_DriverSystem_Fault_Level&0x01;
                Bit8[5]=(R_DriverSystem_Fault_Level&0x02)>>1;
                Bit8[6]=0;
                Bit8[7]=0;                 
                infoMsgS.datMsgUBA[4]=SetBit8 (Bit8);
                
 //               Bit8[0]=AC_Mode&0x01;
 //               Bit8[1]=(AC_Mode&0x02)>>1;
 //               Bit8[2]=(AC_Mode&0x04)>>2;
                Bit8[0]=0x00;
                Bit8[1]=0x00;
                Bit8[2]=0x00;
                Bit8[3]=AC_Temperature&0x01;
                Bit8[4]=(AC_Temperature&0x02)>>1;
                Bit8[5]=(AC_Temperature&0x04)>>2;
                Bit8[6]=(AC_Temperature&0x08)>>3;
                Bit8[7]=(AC_Temperature&0x10)>>4;                
                infoMsgS.datMsgUBA[5]=SetBit8 (Bit8);
                infoMsgS.datMsgUBA[6]=0;
                infoMsgS.datMsgUBA[7]=R_Gate_Life[2];
                               
                MSCANDrv_SendMsgCAN_A(infoMsgS); 
                R_Gate_Life[2]=(uint8)((R_Gate_Life[2]++)%256);                                 
                break;  
            case 6:
            case 26:
            case 46:
            case 66:
            case 86:  
                //�߿��ƶ�����
                //uint8 R_Break_Mode=0;   //Զ���ƶ�����
                //uint8 R_Break_Rang=0;     //Զ���ƶ������� 0~100%                
                infoMsgS.infoMsgIDUI=0x601;
                infoMsgS.bExtIDUB=0;                
                infoMsgS.numLenUB=8;
                infoMsgS.idxMsgPriorityUB=0x04;                 

                infoMsgS.datMsgUBA[0]=R_Break_Mode;
                infoMsgS.datMsgUBA[1]=R_Break_Rang;
                infoMsgS.datMsgUBA[2]=R_Gate_Life[3];
                infoMsgS.datMsgUBA[3]=0;
                infoMsgS.datMsgUBA[4]=0;
                infoMsgS.datMsgUBA[5]=0;
                infoMsgS.datMsgUBA[6]=0;
                
                SendMsg_BCC_Code=0;
                for (Bcc_Index=0;Bcc_Index<7;Bcc_Index++) 
                {
                	  SendMsg_BCC_Code^=infoMsgS.datMsgUBA[Bcc_Index]	;	
                } 
               
                infoMsgS.datMsgUBA[7]=SendMsg_BCC_Code;
                
              // if (DriverMode_State==3) {
                MSCANDrv_SendMsgCAN_B(infoMsgS); 
              // }
                R_Gate_Life[3]=(uint8)((R_Gate_Life[0]++)%256);
                break;    
                              
            default:
                break;
        }
        
        CANNet_idxSchedUB=(uint8)((CANNet_idxSchedUB+1)%100); //100msһ������ѭ��   0~99
        

    }
  
}


