/*******************************************************************************
* �������ƣ�������CANͨѶ����
* �ļ����� CAN_APU.c
* �������� �� 
* ���ü�¼��
* ��ע�� 
* �޶���¼��
*		����		      �汾		  ����		����
*   
*******************************************************************************/
#include <string.h>
#include "GlobalDef.h"
#include "CAN_APU.h"
#include "Custom_Func.h"
    
#ifndef AECS_HW
       #pragma DATA_SEG DEFAULT
#else  
      #pragma DATA_SEG S12X_RAM
#endif

uint32 CANNet_infoID_APU;
uint8  CANNet_SendMsgCAN_APU_idxSchedUB=0;
uint8  CanRecAPU[8]={0};
uint8  VCU_Life_APU=0;


  //����������ָ��,ů�翪������,��е�յ���������,
uint8  APU_CMD,APU_Warm_CMD,Mechanical_AC_CMD,APU_Order=0;
  //������Ŀ�귢�繦������
uint16 APU_Demand_Power; 
  //Ŀ�깤��ת��  
uint16 APU_Demand_Speed;


  //APU��ʼ��״̬,OBDָʾ��,��е�յ�״̬,������������־λ,����������ѹ������,�������ŵ���ɱ�־λ,��������ǰ״̬,���������ϵȼ�      
uint8  APU_Staus,APU_OBD_LED,Mechanical_AC,APU_Start_Staus,Engine_Oil_Pressure_Alarm,APU_Discharged_Flag,APU_Mode,APU_Fault_Status;
  //������ת��,���������ת��
uint16 APU_Speed,APU_Torque;
sint16 APU_Power;//�������������
  //������ĸ�ߵ�ѹ����������ǰת�������
uint16 APU_DC_Voltage,APU_Power_Limit;
  //������ĸ�ߵ�������������������
sint16 APU_DC_Current,APU_AC_Current;

 //    ����������ѹ��,        ����������Һλ,  ������˲ʱȼ��������, ������λ�ÿ���ʵ��ֵ
uint16 Engine_Inlet_Pressure, Engine_Oil_Level,Fuel_Consumption_Rate,Engine_Throttle_Rate;
 //    �����������¶�,          ��������ȴҺ�¶�,          �������¶�,     ������������GCU�¶� 
sint16 Engine_Inlet_Temperature,Engine_Coolant_Temperature,APU_Temperature,GCU_Temperature;
 //   ������������              ������������
uint8 Engine_Fault_Word[8]={0}, APU_Fault_Word[8]={0};
 //   APU�����ź�
uint8 APU_Life;
extern uint16 Max_B_Voltage_Limit;//������������������ѹ,�����СPACK��ѹ���� //0~819.0V   0~818  ��λ�� V�� ƫ�ƣ� 0 �������ߣ� 0.2      ��10���Ŵ���㣬0��8190 4095*2=8190)
extern uint16 VCU_Life;
#ifndef AECS_HW
      #pragma CODE_SEG DEFAULT
#else  
      #pragma CODE_SEG VCU_CODE
      
#endif

//������������Ϣ������
void CANNet_RecvMsgCAN_APU(MSCANDrv_infoMSGS_TYPE infoMsgS)
{    


    CANNet_infoID_APU=infoMsgS.infoMsgIDUI;


    
    switch(CANNet_infoID_APU) 
    {   //APU_Enable
        case 0x1860A0B0: //APU_ Status 0    50ms
            memcpy(CanRecAPU,infoMsgS.datMsgUBA,sizeof(CanRecAPU));    
            //APU��ʼ��״̬,OBDָʾ��,��е�յ�״̬,������������־λ,����������ѹ������,�������ŵ���ɱ�־λ,��������ǰ״̬,���������ϵȼ�      
            //APU_Staus,APU_OBD_LED,Mechanical_AC,APU_Start_Staus,Engine_Oil_Pressure_Alarm,APU_Discharged_Flag,APU_Mode,APU_Fault_Status;
            APU_Staus       = 0x01 & CanRecAPU[0];      //APU��ʼ��״̬ 0-��ʼ��δ��ɣ�1-��ʼ�������
            APU_OBD_LED     =(0x02 & CanRecAPU[0])>>1;
            Mechanical_AC   =(0x04 & CanRecAPU[0])>>2;
            APU_Start_Staus =(0x18 & CanRecAPU[0])>>3; //APU�����ɹ���־λ 0-δ������1-�����ɹ���2-����ʧ�ܣ�3-Void��
            
            Engine_Oil_Pressure_Alarm = 0x01 & CanRecAPU[1]; 
            APU_Discharged_Flag       =(0x80 & CanRecAPU[1])>>7;
            
            APU_Mode  = 0x07 & CanRecAPU[2];     //Bit0-Bit2 ��������ǰ״̬ 0-ͣ��״̬��1-��״̬��2-�϶�״̬��3-����״̬��4-����״̬��5-�����ŵ� ;
            APU_Fault_Status = (0xE0 & CanRecAPU[3])>>5; //Bit5-Bit7�����������ϵȼ� 0-������1-�������ϣ�2-�������ϣ�3-�ļ�����;
            break;
        case 0x1861A0B0: //APU_ Status 1    50ms
            memcpy(CanRecAPU,infoMsgS.datMsgUBA,sizeof(CanRecAPU));    
            //������ת��,�������������,���������ת��
            //APU_Speed,APU_Power,APU_Torque;            
            APU_Speed = (CanRecAPU[0]+CanRecAPU[1]*256)/4; //0~16383rpm
            //APU_Power = (CanRecAPU[2]+CanRecAPU[3]*256)/5-6553 ;//   -655.3~655.3 KW 10��
            APU_Power = (sint16)  (((sint32)CanRecAPU[2]+(sint32)CanRecAPU[3]*256)/5-6553) ;//   -655.3~655.3 KW 10��
            APU_Torque= (CanRecAPU[4]+CanRecAPU[5]*256)/5; //0~1310.0Nm              10��
                                                                                 

            break;
        case 0x1862A0B0: //APU_ Status 2    50ms
            memcpy(CanRecAPU,infoMsgS.datMsgUBA,sizeof(CanRecAPU));    
            //������ĸ�ߵ�ѹ����������ǰת�������
            //APU_DC_Voltage,APU_Power_Limit;
            //������ĸ�ߵ�������������������
            //APU_DC_Current,APU_AC_Current;            
            APU_DC_Voltage= (CanRecAPU[0]+CanRecAPU[1]*256) /5;     //0~1310.7V       10��
            //APU_DC_Current= (CanRecAPU[2]+CanRecAPU[3]*256) /5 -6553;//-655.3~655.3A   10��
            //APU_AC_Current= (CanRecAPU[4]+CanRecAPU[5]*256) /5 -6553;//-655.3~655.3A   10��
            APU_DC_Current=(sint16)   (((sint32)CanRecAPU[2]+(sint32)CanRecAPU[3]*256) /5 -6553);//-655.3~655.3A   10��
            APU_AC_Current=(sint16)   (((sint32)CanRecAPU[4]+(sint32)CanRecAPU[5]*256) /5 -6553);//-655.3~655.3A   10��
            
            
            APU_Power_Limit=(CanRecAPU[6]+CanRecAPU[7]*256) /5;     //0~1310.7KW      10��
            break;
        case 0x1863A0B0: //APU_ Status 3    50ms
            memcpy(CanRecAPU,infoMsgS.datMsgUBA,sizeof(CanRecAPU));    
            //    ����������ѹ��,        ����������Һλ,  ������˲ʱȼ��������, ������λ�ÿ���ʵ��ֵ
            //    Engine_Inlet_Pressure, Engine_Oil_Level,Fuel_Consumption_Rate,Engine_Throttle_Rate;
            //    �����������¶�,          ��������ȴҺ�¶�,          �������¶�,     ������������GCU�¶� 
            //    Engine_Inlet_Temperature,Engine_Coolant_Temperature,APU_Temperature,GCU_Temperature;            
            Engine_Inlet_Pressure     = CanRecAPU[0]*15 ;  //����������ѹ��  0~381.0 kPa   10��
            Engine_Inlet_Temperature  = CanRecAPU[1]-48 ;  //�����������¶�  -48~206��
            Engine_Oil_Level          = CanRecAPU[2]*4  ;  //����������Һλ  0~100.0%      10��
            Engine_Coolant_Temperature= CanRecAPU[3]-48 ;  //��������ȴҺ�¶�-48~206��
            APU_Temperature           = CanRecAPU[4]-48 ;  //�������¶�      -48~206��
            GCU_Temperature           = CanRecAPU[5]-48 ;  //������GCU�¶�   -48~206��
            Fuel_Consumption_Rate     = (uint16)((uint32)CanRecAPU[6]*1532/10);//������˲ʱȼ��������  0~38.912 mL/s   1000��
            Engine_Throttle_Rate      = (uint16)((uint32)CanRecAPU[7]*392/10); //������λ�ÿ���ʵ��ֵ  0~100.00%       100��
            break;
        case 0x1C64A0B0: //APU_ Fault 1    1000ms
            //   ������������   8byte           
            memcpy(Engine_Fault_Word,infoMsgS.datMsgUBA,8);
            break;
        case 0x1C65A0B0: //APU_ Fault 2    1000ms
            //   ������������   3byte
            memcpy(APU_Fault_Word,infoMsgS.datMsgUBA,3);
            break;
        case 0x1866A0B0: //APU_ Life 0    100ms
            memcpy(CanRecAPU,infoMsgS.datMsgUBA,sizeof(CanRecAPU));    
            APU_Life=CanRecAPU[0];    //APU�����ź�
            break;
        default:
            break;
    }
}


//������������Ϣ������ 1ms
void CANNet_SendMsgCAN_APU(void)
{
    MSCANDrv_infoMSGS_TYPE infoMsgS;
    uint8 APU_Order_Bit[8]={0};
    switch(CANNet_SendMsgCAN_APU_idxSchedUB)
    {
         case 1:
         case 51:
            //CAN0  VCU_Cmd 50ms  ������֡   
            
            //����������ָ�� 0-ͣ��ָ�1-����(����)ָ�2-����ָ�3-�����ŵ�ָ��;
            //APU_CMD
            //ů�翪������, ��е�յ���������
            //APU_Warm_CMD,Mechanical_AC_CMD,
            
            //Ŀ�깤��ת��  30rmp����            
            //APU_Demand_Speed;  0~7650rpm
            //������Ŀ�귢�繦������  100.0~0.1KW
            //APU_Demand_Power; 
            //������������������ѹ
            //Max_B_Voltage_Limit //0~819.0V 
            
                APU_Order_Bit[0]=APU_CMD&0x1;
                APU_Order_Bit[1]=(APU_CMD&0x2)>>1;
                APU_Order_Bit[2]=APU_Warm_CMD&0x1;
                APU_Order_Bit[3]=(APU_Warm_CMD&0x2)>>1;            
                APU_Order_Bit[4]=Mechanical_AC_CMD&0x1;
                APU_Order_Bit[5]=0;
                APU_Order_Bit[6]=0;
                APU_Order_Bit[7]=0;
                
                
                APU_Order=SetBit8 (APU_Order_Bit);

 
            
            infoMsgS.infoMsgIDUI=0x1850B0A0;
            infoMsgS.bExtIDUB=1;
            infoMsgS.datMsgUBA[0]=0;//����
            infoMsgS.datMsgUBA[1]=APU_Order ;//&0x03)   |  ((APU_Warm_CMD&0x01)<<2)   |  ((Mechanical_AC_CMD&0x01)<<3);
            infoMsgS.datMsgUBA[2]=(uint8)((APU_Demand_Power*5)&0x00ff) ;//1310.7~0.0KW   10��
            infoMsgS.datMsgUBA[3]=(uint8)((APU_Demand_Power*5)>>8);
            infoMsgS.datMsgUBA[4]=(uint8)(Max_B_Voltage_Limit&0x00ff); //0~819.0V 10��
            infoMsgS.datMsgUBA[5]=(uint8)(Max_B_Voltage_Limit>>8);
            if (APU_Demand_Speed>7650)
            {
                infoMsgS.datMsgUBA[6]=0;
            }else
            {
                infoMsgS.datMsgUBA[6]=(uint8)(APU_Demand_Speed/30);
            }
            infoMsgS.datMsgUBA[7]=0;//����
            infoMsgS.numLenUB=8;
            infoMsgS.idxMsgPriorityUB=0x01;
            MSCANDrv_SendMsgCAN_B(infoMsgS);
            break;
        case 10:
        case 60:
            //VCU����
            infoMsgS.infoMsgIDUI=0x08F000A0;
            infoMsgS.bExtIDUB=1;
            infoMsgS.datMsgUBA[0]=0;
            infoMsgS.datMsgUBA[1]=0;
            infoMsgS.datMsgUBA[2]=0;
            infoMsgS.datMsgUBA[3]=0;
            infoMsgS.datMsgUBA[4]=0;
            infoMsgS.datMsgUBA[5]=0;
            infoMsgS.datMsgUBA[6]=0;
            infoMsgS.datMsgUBA[7]=(uint8)(VCU_Life_APU);       //VCU���� 0��255
            infoMsgS.numLenUB=8;
            infoMsgS.idxMsgPriorityUB=0x02;
            MSCANDrv_SendMsgCAN_B(infoMsgS);
            VCU_Life_APU=(uint8)((VCU_Life_APU+1)%256); //0��255  
            break;
        default:
            break;
    }
    CANNet_SendMsgCAN_APU_idxSchedUB=(uint8)((CANNet_SendMsgCAN_APU_idxSchedUB+1)%100); //100msһ������ѭ��
}





