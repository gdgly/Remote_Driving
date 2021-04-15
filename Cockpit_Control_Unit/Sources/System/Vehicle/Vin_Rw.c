/*******************************************************************************
* �������ƣ�Vin��ˢд��ͨѶ����
* �ļ����� Vin_Rw.c
* �������� �� 
* ���ü�¼��
* ��ע�� 
* �޶���¼��
*		����		      �汾		  ����		����
*   
*******************************************************************************/
#include <string.h>
#include "GlobalDef.h"
#include "Vin_Rw.h"
#ifndef AECS_HW
      #pragma DATA_SEG __GPAGE_SEG RUNPAR_EEPROM
#else  
      #pragma DATA_SEG S12X_RAM
#endif
    //Vin��ˢд����������ſ�ˢд��������ر����ˢд��������ؿ�ˢд����
    extern uint8 Rw_Vin_Times,Rw_Mot_Times,Rw_Bms_Times,Rw_MCU_Times;  

    extern uint8 Vin_Code[21];                          
    extern uint8 Mot_Code[21];
    extern uint8 MCU_Code[21];
    extern uint8 Bms_Code[315];
    extern uint16 PowOn_Times;
    extern uint8 Flash_Times; 

    
#ifndef AECS_HW
       #pragma DATA_SEG DEFAULT
#else  
      #pragma DATA_SEG S12X_RAM
#endif



//    ˢд������       ����ˢдӦ��֡  ��ǰ���͹㲥֡��      �ӳٷ��͹㲥֡   ��Ҫ���͹㲥֡�� 
uint8  Vin_Rw_Errcode=0,VCU_RESPONSE=0,VCU_ROADCASTE=0,VCU_ROADCASTE_Delay=0,VCU_ROADCASTE_Number=0;
uint8  CANNet_Send_VinRwMsg_Index; //20ms���
uint16 LostHCCommunication_Index=0; //ͨѶ��ʧ����
uint8  LostHCCommunication=0;       //��λ��ͨѶ��ʧ
uint8  VCU_ROADCASTE_Repeat_Times=0; //�㲥֡���ط�����
uint8  Device_Id_H4=0,Device_Id_L4=0;
uint8  Data_OK[45]=0;//��������֡�������
uint8  Need_Flash=0; //������ɣ���ҪFlash���

uint8 Vin_RW_ReadyForSend=0;

//��λ������֡HC_CMD
// ����û������豸��ַ�������豸����������֡ID��֡���ݡ�ʱ������յ��±���
extern uint8  HC_CMD,User_Level,Device_Id,Device_Num,HC_Data_Id,HC_Data[316],Time_Mark,GetNewMessage_HC_CMD;
//��Ҫ���յ�����֡��
extern uint8  Need_HC_Data;


extern uint8 Rw_Times_Reset;//��ˢд������λ

#ifndef AECS_HW
      #pragma CODE_SEG DEFAULT
#else  
      #pragma CODE_SEG VCU_CODE
      
#endif
void Vin_Rw_Main(void) 
{ //Vin���д������ 10ms������һ��
    uint16 i=0;

    if ((Rw_Vin_Times==0)||(Rw_Vin_Times==255)){Rw_Vin_Times=4;}
    if ((Rw_Mot_Times==0)||(Rw_Mot_Times==255)){Rw_Mot_Times=4;}
    if ((Rw_Bms_Times==0)||(Rw_Bms_Times==255)){Rw_Bms_Times=4;}
    if ((Rw_MCU_Times==0)||(Rw_MCU_Times==255)){Rw_MCU_Times=4;}
    
    if (GetNewMessage_HC_CMD==1)
    {   //�յ��±���
        GetNewMessage_HC_CMD=0;
        
        switch(HC_CMD) 
        {
            case 0xA0:  //ע��֡
                VCU_RESPONSE=1;  //Ӧ��һ֡
                if   ((User_Level==1)||         //Reader
                      (User_Level==2)||         //Writer
                      (User_Level==0)||         //���ݾ�ģʽ
                      (User_Level==0xF9))       //SuperUser
                {
                    if ((Rw_Times_Reset==1)&&(User_Level==0xF9)) 
                    {
                        Rw_Vin_Times=0;
                        Rw_Mot_Times=0;
                        Rw_Bms_Times=0;
                        Rw_MCU_Times=0;
                    }
                    Vin_Rw_Errcode=0;
                    LostHCCommunication_Index=0;
                    LostHCCommunication=0;
                }else 
                {
                    User_Level=3; 
                    Vin_Rw_Errcode=0x32;                    
                }
                break;
            case 0xA3:  //��ȡ֡
                VCU_RESPONSE=1;  //Ӧ��һ֡
                if (Time_Mark==0){ User_Level=0; }//��λ��Ϊ��ģʽ
                if( (User_Level==0)||(User_Level==1)||(User_Level==2)||(User_Level==0xF9) )
                {   //�Ϸ��û����Զ�ȡ����    
                    //Device_Id��Device_Num��Time_Mark  Vin_Code  //HC_Data[316]   0-316(314) 15��21�ֽڱ���
                    Vin_Rw_Errcode=0;
                    LostHCCommunication_Index=0;
                    Device_Id_H4=Device_Id>>4;
                    Device_Id_L4=Device_Id&0x0F;
                    switch(Device_Id_H4)
                    {
                        case 0xA: //VIN
                          if ((Device_Id_L4==1)&&(Device_Num==1))
                          { 
                              //(void) memcpy(HC_Data,Vin_Code,21);
                              for (i=0;i<21;i++){   HC_Data[i]=Vin_Code[i];        }
                              
                              
                              
                          }else { Vin_Rw_Errcode=0x42; }//���ܶ�ȡ���豸��ַ
                          break;                    
                        case 0xB: //���
                          if ((Device_Id_L4==1)&&(Device_Num==1))
                          { 
                              //(void) memcpy(HC_Data,Mot_Code,21);
                              for (i=0;i<21;i++){  HC_Data[i]=Mot_Code[i];         }
                          }else { Vin_Rw_Errcode=0x42; }//���ܶ�ȡ���豸��ַ             
                          break;                    
                        case 0xC: //���
                          if ( (Device_Id_L4>0) && (Device_Id_L4<0xF) && (Device_Num+Device_Id_L4 < 0x10)) 
                          {
                              //(void) memcpy(HC_Data,Bms_Code,21);
                              //(void) memcpy(HC_Data,Bms_Code+(Device_Id_L4-1)*21 ,Device_Num*21);
                          
                          
                          }else { Vin_Rw_Errcode=0x42; }//���ܶ�ȡ���豸��ַ  
                          break;                    
                        case 0xD: //���
                          if ((Device_Id_L4==1)&&(Device_Num==1))
                          { 
                             // (void) memcpy(HC_Data,MCU_Code,21);
                              for (i=0;i<21;i++){  HC_Data[i]= MCU_Code[i];         }
                          }else { Vin_Rw_Errcode=0x42; }//���ܶ�ȡ���豸��ַ              
                          break;                    
                        default:   
                            break;                    
                    }
                }else 
                {   //User_Level=3; //��ע����Ƿ��û������ɶ�д
                    Vin_Rw_Errcode=0x32;
                }
                break;
            case 0xA1:  //����֡
                VCU_RESPONSE=1;  //Ӧ��һ֡
                LostHCCommunication_Index=0;
                break;
            case 0xA6:  //д��֡
               /*   Device_Id    Device_Num    Time_Mark   */ 
                VCU_RESPONSE=1;  //Ӧ��һ֡
                HC_Data_Id=0;
                if (Time_Mark==0){ User_Level=0; }//��λ��Ϊ��ģʽ
                if( (User_Level==0)||(User_Level==2)||(User_Level==0xF9) ) 
                {   //�û��Ϸ� ����д
                    Vin_Rw_Errcode=0;
                    LostHCCommunication_Index=0;
                    Device_Id_H4=Device_Id>>4;
                    Device_Id_L4=Device_Id&0x0F;
                    //�Ƿ����ڵȴ�����֡
                    if (Need_HC_Data==0)
                    {
                      
                        switch(Device_Id_H4)
                        {
                            case 0xA: //VIN
                              
                              if ((User_Level!=0xF9)&&(Rw_Vin_Times<=1)){Vin_Rw_Errcode=0x41;}//д�����ľ�   
                              else
                              {
                                  if ((Device_Id_L4==1)&&(Device_Num==1))
                                  {   //�Ϸ��������ȴ�3֡����֡
                                      Need_HC_Data=3;
                                      (void)memset(Data_OK,0,Need_HC_Data+1);
                                  }else { Vin_Rw_Errcode=0x31; }//����ֵ������Χ
                              }
                              break;                    
                            case 0xB: //��� 
                              if ((User_Level!=0xF9)&&(Rw_Mot_Times<=1)){Vin_Rw_Errcode=0x41;}//д�����ľ�   
                              else
                              {                              
                                  if ((Device_Id_L4==1)&&(Device_Num==1))
                                  {   //�Ϸ�����, �ȴ�3֡����֡
                                      Need_HC_Data=3;    
                                      (void)memset(Data_OK,0,Need_HC_Data+1);
                                  }else { Vin_Rw_Errcode=0x31; }//����ֵ������Χ                        
                              }
                              break;                    
                            case 0xC: //���  
                              if ((User_Level!=0xF9)&&(Rw_Bms_Times<=1)){Vin_Rw_Errcode=0x41;}//д�����ľ�   
                              else
                              {                                
                                  if ((Device_Id_L4>0) && (Device_Id_L4<0xF) && (Device_Num+Device_Id_L4 < 0x10))
                                  {   //�Ϸ��������ȴ�Device_Num *3 ֡����֡
                                      Need_HC_Data=Device_Num*3;
                                      (void)memset(Data_OK,0,Need_HC_Data+1);
                                  }else { Vin_Rw_Errcode=0x31; }//����ֵ������Χ                        
                              }
                              break;                    
                            case 0xD: //���     
                              if ((User_Level!=0xF9)&&(Rw_MCU_Times<=1)){Vin_Rw_Errcode=0x41;}//д�����ľ�   
                              else
                              {                                
                                  if ((Device_Id_L4==1)&&(Device_Num==1))
                                  {   //�Ϸ��������ȴ�3֡����֡
                                      Need_HC_Data=3;
                                      (void)memset(Data_OK,0,Need_HC_Data+1);
                                  }else { Vin_Rw_Errcode=0x31; }//����ֵ������Χ                        
                              }
                              break;                    
                            default:   
                                break;                    
                        }
                    }else 
                    { //���ڵȴ�����֡
                        Vin_Rw_Errcode=0x21;
                    
                    }
                                        
                }else
                {  //�û����Ϸ�
                   
                    Vin_Rw_Errcode=0x32;
                    Need_HC_Data=0;
                }
                
                break;
            case 0xAF:  //ע��֡
                VCU_RESPONSE=1;
                Need_HC_Data=0;
                User_Level=3; //����д��
                if (LostHCCommunication==0) {Vin_Rw_Errcode=0;}
                break;
            case 0xFF:  //����֡
            case 0x00:
                
                break;
            default:   
                VCU_RESPONSE=1;
                Vin_Rw_Errcode=0x11;//����������� 
                HC_CMD=0xAF;        //�ر�����
                User_Level=3;       //ע���û�                
                break;
        }

    }else
    {   //����10sδ�յ�����
        if ((User_Level!=3)&&(User_Level!=0))
        {
            LostHCCommunication_Index++;
            if (LostHCCommunication_Index>1000)  //ͨѶ��ʱ 10s
            {
                VCU_RESPONSE=1;
                LostHCCommunication=1;
                Need_HC_Data=0; //ֱ����0 ����������
                Vin_Rw_Errcode=0x51;
                HC_CMD=0xAF;        //��ʱ�ر�����
                User_Level=3;       //ע���û�
            }
        }
    }
    
    //20msһ������ѭ��
    //CANNet_Send_VinRwMsg_Index=(uint8)((CANNet_Send_VinRwMsg_Index+1)%2);
    //ˢдVINͨѶ����    20ms����һ��
    //if (CANNet_Send_VinRwMsg_Index==0) {  CANNet_Send_VinRwMsg(); }   
    Vin_RW_ReadyForSend=1;
    
    if ((Need_HC_Data<=HC_Data_Id)&&(Need_HC_Data>0))
    {
        HC_Data_Check();
    }
    
    //Need_Flash=1
    //������ɣ���ҪFlash���
    if (Need_Flash==1)
    {
        Need_Flash=0;
        Flash_Times++;
        //EEPROMDrv_bSaveInstantUB=1;
        
        
        
    }
    
    
}

void CANNet_Send_VinRwMsg(void)
{  //ˢдVINͨѶ����    20ms����һ��
    MSCANDrv_infoMSGS_TYPE infoMsgS;
    
    if (VCU_RESPONSE>0)
    {    //����Ӧ��֡ 0x0C6501A0 
         VCU_RESPONSE=VCU_RESPONSE-1; //Ӧ�����-1
         infoMsgS.infoMsgIDUI=0x0C6501A0;
         infoMsgS.bExtIDUB=1;
         infoMsgS.numLenUB=8;
         infoMsgS.idxMsgPriorityUB=0x08;
         switch(HC_CMD) 
         {        
            case 0xA0:  //ע��֡
                infoMsgS.datMsgUBA[0]=0xA0;
                if (Vin_Rw_Errcode==0)
                {
                    infoMsgS.datMsgUBA[1]=Vin_Rw_Errcode;
                    infoMsgS.datMsgUBA[2]=Rw_Vin_Times-1;
                    infoMsgS.datMsgUBA[3]=Rw_Mot_Times-1;
                    infoMsgS.datMsgUBA[4]=Rw_Bms_Times-1;
                    infoMsgS.datMsgUBA[5]=Rw_MCU_Times-1;
                }else
                {
                    infoMsgS.datMsgUBA[1]=Vin_Rw_Errcode;
                    infoMsgS.datMsgUBA[2]=0;
                    infoMsgS.datMsgUBA[3]=0;
                    infoMsgS.datMsgUBA[4]=0;
                    infoMsgS.datMsgUBA[5]=0;
                    Vin_Rw_Errcode=0; //��չ���
                }
                infoMsgS.datMsgUBA[6]=0;
                infoMsgS.datMsgUBA[7]=0;      
                VCU_ROADCASTE_Delay=0;
                VCU_ROADCASTE=0;
                VCU_ROADCASTE_Number=0;     
                break;
            case 0xA3:  //��ȡ֡
                infoMsgS.datMsgUBA[0]=0xA3;
                infoMsgS.datMsgUBA[1]=Vin_Rw_Errcode;
                infoMsgS.datMsgUBA[2]=Time_Mark;
                infoMsgS.datMsgUBA[3]=0;
                infoMsgS.datMsgUBA[4]=0;
                infoMsgS.datMsgUBA[5]=0;
                infoMsgS.datMsgUBA[6]=0;
                infoMsgS.datMsgUBA[7]=0;
                if (Vin_Rw_Errcode==0) //��ȡ����
                {
                    VCU_ROADCASTE_Repeat_Times=0;//�㲥֡���ط���������
                    VCU_ROADCASTE_Delay=1;       //�㲥֡��һ���ڿ�ʼ  �ӳ�1����
                    VCU_ROADCASTE=0;             //��ǰ����֡��
                    VCU_ROADCASTE_Number=Device_Num*3;  //��Ҫ�㲥֡�� 3 6 9 ����
                    
                }else
                {
                    VCU_ROADCASTE_Delay=0;
                    VCU_ROADCASTE=0;
                    VCU_ROADCASTE_Number=0;
                }
                break;
            case 0xA1:  //����֡
                infoMsgS.datMsgUBA[0]=0xA1;
                infoMsgS.datMsgUBA[1]=Vin_Rw_Errcode;
                infoMsgS.datMsgUBA[2]=Time_Mark;
                infoMsgS.datMsgUBA[3]=0;
                infoMsgS.datMsgUBA[4]=0;
                infoMsgS.datMsgUBA[5]=0;
                infoMsgS.datMsgUBA[6]=0;
                infoMsgS.datMsgUBA[7]=0;
                break;
            case 0xA6:  //д��֡
                infoMsgS.datMsgUBA[0]=0xA6;
                infoMsgS.datMsgUBA[1]=Vin_Rw_Errcode;
                infoMsgS.datMsgUBA[2]=Time_Mark;
                infoMsgS.datMsgUBA[3]=0;
                infoMsgS.datMsgUBA[4]=0;
                infoMsgS.datMsgUBA[5]=0;
                infoMsgS.datMsgUBA[6]=0;
                infoMsgS.datMsgUBA[7]=0;            
                VCU_ROADCASTE_Delay=0;
                VCU_ROADCASTE=0;
                VCU_ROADCASTE_Number=0;              
                break;
            case 0xAF:  //ע��֡
                
                infoMsgS.datMsgUBA[0]=0xAF;
                infoMsgS.datMsgUBA[1]=Vin_Rw_Errcode;
                infoMsgS.datMsgUBA[2]=0;
                infoMsgS.datMsgUBA[3]=0;
                infoMsgS.datMsgUBA[4]=0;
                infoMsgS.datMsgUBA[5]=0;
                infoMsgS.datMsgUBA[6]=0;
                infoMsgS.datMsgUBA[7]=0;
                VCU_ROADCASTE_Delay=0;
                VCU_ROADCASTE=0;
                VCU_ROADCASTE_Number=0;
                Vin_Rw_Errcode=0;                
                break;
            case 0xFF:  //����֡
                infoMsgS.datMsgUBA[0]=HC_Data_Id;
                infoMsgS.datMsgUBA[1]=Vin_Rw_Errcode;
                infoMsgS.datMsgUBA[2]=0;
                infoMsgS.datMsgUBA[3]=0;
                infoMsgS.datMsgUBA[4]=0;
                infoMsgS.datMsgUBA[5]=0;
                infoMsgS.datMsgUBA[6]=0;
                infoMsgS.datMsgUBA[7]=0;                
                VCU_ROADCASTE_Delay=0;
                VCU_ROADCASTE=0;
                VCU_ROADCASTE_Number=0;              
                break;
            default:   
                break;
         }
         MSCANDrv_SendMsgCAN_A(infoMsgS);
         if (HC_CMD!=0xff)
         {
            HC_CMD=0;
         }
    }
  
  
    //VCU_ROADCASTE_Times  �㲥֡�ظ�����
    //VCU_ROADCASTE_Repeat_Times=0; //�㲥֡���ط�����   
    
       /*
       VCU_ROADCASTE_Delay=1;       //�㲥֡��һ���ڿ�ʼ
       VCU_ROADCASTE=0;             //��ǰ����֡��
       VCU_ROADCASTE_Number=Device_Num*3;  //��Ҫ�㲥֡��
       HC_Data[316];
       */
    
    
    
    //���͹㲥֡
    if (VCU_ROADCASTE<VCU_ROADCASTE_Number)//VCU_ROADCASTE_Number��Ҫ�㲥��֡��  //VCU_ROADCASTE   ��ʼ����ʱΪ0  ÿ������һ֡+1 
    {
        if (VCU_ROADCASTE_Delay>0)
        {
            VCU_ROADCASTE_Delay=VCU_ROADCASTE_Delay-1;//�㲥�ӳټ�����1
        }else 
        {   //�㲥�ӳټ�����������ʼ���͹㲥֡ 0x0CFF01A0;
            
            infoMsgS.infoMsgIDUI=0x0CFF01A0;
            infoMsgS.bExtIDUB=1;
            infoMsgS.numLenUB=8;
            infoMsgS.idxMsgPriorityUB=0x08;
                
            infoMsgS.datMsgUBA[0]=VCU_ROADCASTE+1;  //����֡��� 1 2 3 ��4 5 6���� BMS��
            infoMsgS.datMsgUBA[1]=HC_Data[VCU_ROADCASTE*7  ];  //0  7   14  
            infoMsgS.datMsgUBA[2]=HC_Data[VCU_ROADCASTE*7+1];  //1  8   15
            infoMsgS.datMsgUBA[3]=HC_Data[VCU_ROADCASTE*7+2];  //2  9   16
            infoMsgS.datMsgUBA[4]=HC_Data[VCU_ROADCASTE*7+3];  //3  10  17
            infoMsgS.datMsgUBA[5]=HC_Data[VCU_ROADCASTE*7+4];  //4  11  18
            infoMsgS.datMsgUBA[6]=HC_Data[VCU_ROADCASTE*7+5];  //5  12  19
            infoMsgS.datMsgUBA[7]=HC_Data[VCU_ROADCASTE*7+6];  //6  13  20
    
            MSCANDrv_SendMsgCAN_A(infoMsgS);
            VCU_ROADCASTE=VCU_ROADCASTE+1;
        }
    }else 
    {
        if (VCU_ROADCASTE_Repeat_Times<VCU_ROADCASTE_Times) 
        {
            VCU_ROADCASTE=0;
            VCU_ROADCASTE_Repeat_Times++;
        }
    }

}

void CANNet_Get_HC_Data (uint8 Get_HC_Data[8])
{   //������յ�������֡
    //Device_Id_H4 Device_Id_L4 Device_Id  Device_Num  
    //Vin_Code[21];  Mot_Code[21]; MCU_Code[21];                       
    //HC_Data[316]
    //User_Level   Vin_Rw_Errcode
    
    //Need_HC_Data  Ŀ������֡����
    //HC_Data_Id    ����֡ID 1 2 3 
    
    
    VCU_RESPONSE=1; //����һ֡����֡������Ӧ��һ֡
    Vin_Rw_Errcode=0x01;
    
    if ((HC_Data_Id>0)&&(HC_Data_Id<0x2D) && (Need_HC_Data>0))
    {  //����֡ID�Ϸ�����Ҫ��������
        if (Need_HC_Data+1>HC_Data_Id)
        {  //��������δ���
            Data_OK[ HC_Data_Id ]=1;
            HC_Data[  (HC_Data_Id-1)*7  ]=Get_HC_Data[1];
            HC_Data[  (HC_Data_Id-1)*7+1]=Get_HC_Data[2];
            HC_Data[  (HC_Data_Id-1)*7+2]=Get_HC_Data[3];
            HC_Data[  (HC_Data_Id-1)*7+3]=Get_HC_Data[4];
            HC_Data[  (HC_Data_Id-1)*7+4]=Get_HC_Data[5];
            HC_Data[  (HC_Data_Id-1)*7+5]=Get_HC_Data[6];
            HC_Data[  (HC_Data_Id-1)*7+6]=Get_HC_Data[7];
            Vin_Rw_Errcode=0;
            
            
        }

    }


}

void HC_Data_Check(void) 
{     uint8 Index,Data_OK_Total=1;
      uint16 i=0;

    if (Need_HC_Data<=HC_Data_Id)
        {   //�����������,������֡�������
            
            HC_Data_Id=0;
            for (Index=1;Index>=Need_HC_Data;Index++)
            {
                Data_OK_Total=Data_OK_Total && Data_OK[Index];
                //Data_OK[Index]=0;
            }
            
            
            if (Data_OK_Total==1)
            {   //����֡����������д��
                //(void)memset(Data_OK,0,Need_HC_Data+1);
               
                switch(Device_Id_H4)
                {
                    case 0xA://VIN
                        //(void)memcpy(Vin_Code,HC_Data,sizeof(Vin_Code));
                        for (i=0;i<21;i++){   Vin_Code[i]=HC_Data[i];        }
                        Need_HC_Data=0;//���ٽ�������
                        Vin_Rw_Errcode=0;
                        Need_Flash=1;//������ɣ���ҪFlash���
                        break;
                    case 0xB://���
                        //(void)memcpy(Mot_Code,HC_Data,sizeof(Mot_Code));
                        for (i=0;i<21;i++){   Mot_Code[i]=HC_Data[i];        }
                        Need_HC_Data=0;//���ٽ�������
                        Vin_Rw_Errcode=0;
                        Need_Flash=1;//������ɣ���ҪFlash���
                        break;
                    case 0xC://���
                         //(void)memcpy(Bms_Code,HC_Data,sizeof(Mot_Code));
                         //(void) memcpy(HC_Data,Bms_Code+(Device_Id_L4-1)*21 ,Device_Num*21);
                        //(void)memcpy(Bms_Code+(Device_Id_L4-1)*21,HC_Data,Device_Num*21);                   
                        Need_HC_Data=0;//���ٽ�������
                        Vin_Rw_Errcode=0;
                        Need_Flash=1;//������ɣ���ҪFlash���                    
                    
                    
                        break;
                    case 0xD://���
                        //(void)memcpy(MCU_Code,HC_Data,sizeof(MCU_Code));
                        for (i=0;i<21;i++){   MCU_Code[i]=HC_Data[i]; }
                        Need_HC_Data=0;//���ٽ�������
                        Vin_Rw_Errcode=0;
                        Need_Flash=1;//������ɣ���ҪFlash���  
                        break;                      
                    default:
                        //�豸ID����
                        Vin_Rw_Errcode=0x12;
                        Need_HC_Data=0;//���ٽ�������
                        Need_Flash=0;
                        break;
                }
                if (EEPROMDrv_bSaveInstantUB==1)
                { //EEPROM��δ׼���ã�������д
                    Vin_Rw_Errcode=0xFD;
                    //Need_HC_Data=0;//���ٽ�������
                    Need_Flash=0;               
                }else 
                { //EEPROM��׼���ã�����д
                    if (((User_Level==2)||(User_Level==0))&&(Need_Flash==1)) 
                    {   //ʣ����д������1
                        switch(Device_Id_H4)
                        {
                            case 0xA:
                                Rw_Vin_Times--;
                                break;
                            case 0xB:
                                Rw_Mot_Times--;
                                break;
                            case 0xC:
                                Rw_Bms_Times--;
                                break;
                            case 0xD:
                                Rw_MCU_Times--;
                                break;
                            default:
                                break;
                        }
                    }                
                
                
                }
            }else 
            { //����֡������
                    Vin_Rw_Errcode=0x13;
                    Need_HC_Data=0;//���ٽ�������
                    Need_Flash=0;              
            }
            
            
        }



}






















