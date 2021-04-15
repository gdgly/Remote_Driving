/*******************************************************************************
* �������ƣ�������ת��
* �ļ����� Vin_Interface_conversion.c
* �������� �� ��ͬӲ���ײ�ӿ�ת��
* ���ü�¼��
* ��ע�� 
* �޶���¼��
*		����		      �汾		  ����		����
* 2017.8.3                  ���    ���½ӿڶ���
*******************************************************************************/

#include "GlobalDef.h"
#include "Vin_Interface_conversion.h"

#ifndef AECS_HW
      #pragma DATA_SEG DEFAULT
#else  
      #pragma DATA_SEG S12X_RAM
#endif


uint8 bDiSatus[24]={0};     //DI�ڿ�����״̬
uint16 uADSatus[25]={0};    //ģ������ֵ
uint8 bDoSatus[20]={0};     //DO�ڿ�����״̬
uint16 uAD4Satus[4]={0};    //4·ADģ������ֵ
uint16 uAD_LOGICP_CHAN=0;    //��𿪹ص�ѹ

uint8 DoErr[20]={  0 }; 

extern uint32 fSet;  //Ƶ���趨   0.01~5000.00Hz
extern uint16 rSet;  //ռ�ձ��趨 0~10000


#ifndef AECS_HW
      #pragma CODE_SEG DEFAULT
#else  
      #pragma CODE_SEG VCU_CODE 
#endif


void VehCo_Interface(void) 
{ //�ײ�IO�ӿ�ת��
  uint8 Index=0;
  #ifndef AECS_HW //�׿�ƽ̨
  {
    VCU_C_1[5]=2;
      
    for(Index=0;Index<18;Index++)//���18·DIֵ�Ŀ�����
    {
        bDiSatus[Index]=DIODrv_GetChanState(Index);

         
    }
        
        bDiSatus[7]=!bDiSatus[7];//�յ���ѹ���� D07\���ٽ�� L��Ч
        bDiSatus[8]=!bDiSatus[8];//��չޱ��� L��Ч
        bDiSatus[9]=!bDiSatus[9];//�յ���ѹ���� D08 L��Ч
        bDiSatus[11]=!bDiSatus[11];//EPS����  L��Ч
        bDiSatus[12]=!bDiSatus[12];//DCDC����  L��Ч 
        bDiSatus[14]=!bDiSatus[14];//P  L��Ч 
        bDiSatus[15]=!bDiSatus[15];//R  L��Ч 
        bDiSatus[16]=!bDiSatus[16];//N  L��Ч 
        bDiSatus[17]=!bDiSatus[17];//D  L��Ч 
        
    for(Index=5;Index<23;Index++)//���18·DI�ڵ�ģ����    5-22
    {
        uADSatus[Index-5]=ATDDrv_GetChanResult(Index); 
    }
        
    uAD_LOGICP_CHAN=ATDDrv_GetChanResult(23);  // IGN��𿪹ص�ѹ
    
    for(Index=1;Index<5;Index++)//���4·ģ������ֵ  
    {
        uAD4Satus[Index-1]=ATDDrv_GetChanResult(Index); 
    }
    
    
    for (Index=1;Index<16;Index++) //���ø�·DO�����״̬   0·������
    {
        PSWTDrv_Interface(Index,bDoSatus[Index]); 
        DoErr[Index]=PSwtDrv_GetDiagStatus(Index,0) & (PSwtDrv_GetDiagStatus(Index,1)<<4) ; //��4 open  ��4 short 
        
    }

    //DoErr[3]=PSwtDrv_GetDiagStatus(3,0) & (PSwtDrv_GetDiagStatus(3,1)<<4) ; //��4 open  ��4 short 

  }
  #else  //���׿�˹ƽ̨ 
  {
    VCU_C_1[5]=1;
    //bDiSatus[9]= ATDDrv_GetChanResult(_ATD_INDEX_VLT10_CHAN)<SC_VDL_LevelLowThres  ?    1:0  ;// PTC�¶ȿ���2 L��Ч  X63
    bDiSatus[0]=DIODrv_GetChanState(_DIO_INDEX_DIN3_CHAN);//�������            X21  
    bDiSatus[1]=DIODrv_GetChanState(_DIO_INDEX_DIN6_CHAN);//ACC                 X24
    bDiSatus[2]=DIODrv_GetChanState(_DIO_INDEX_DIN5_CHAN);//START               X23
    bDiSatus[3]=DIODrv_GetChanState(_DIO_INDEX_DIN8_CHAN);//�յ�                X26
    bDiSatus[4]=DIODrv_GetChanState(_DIO_INDEX_DIN7_CHAN);//PTC                 X25

    bDiSatus[5]= ATDDrv_GetChanResult(_ATD_INDEX_VLT9_CHAN)<SC_VDL_LevelLowThres  ?    1:0   ;// PTC�¶ȿ���1 L��Ч  X56
    bDiSatus[6]= ATDDrv_GetChanResult(_ATD_INDEX_VLT5_CHAN)<SC_VDL_LevelLowThres   ?    1:0   ;// �յ��¶ȿ��� L��Ч  X70

    bDiSatus[7]=DIODrv_GetChanState(_DIO_INDEX_HVLTDI2_CHAN);//�յ���ѹ���� 750\���ٽ�� L��Ч     X8  
    bDiSatus[8]=DIODrv_GetChanState(_DIO_INDEX_DIN1_CHAN);//��չޱ��� L��Ч                X19
    bDiSatus[9]=DIODrv_GetChanState(_DIO_INDEX_DIN4_CHAN);//�յ���ѹ���� D08        X22
    bDiSatus[10]=DIODrv_GetChanState(_DIO_INDEX_HVLTDI4_CHAN);//�ƶ�����2           X10
    bDiSatus[11]=DIODrv_GetChanState(_DIO_INDEX_DIN2_CHAN);//EPS����  L��Ч         X20
    bDiSatus[12]=DIODrv_GetChanState(_DIO_INDEX_HVLTDI1_CHAN);//DCDC���� L��Ч      X7
    bDiSatus[13]=DIODrv_GetChanState(_DIO_INDEX_HVLTDI3_CHAN);//�ƶ�����1           X9
    bDiSatus[14]=DIODrv_GetChanState(_DIO_INDEX_HVLTDI5_CHAN);//P  L��Ч            X11
    bDiSatus[15]=DIODrv_GetChanState(_DIO_INDEX_HVLTDI6_CHAN);//R  L��Ч            X12
    bDiSatus[16]=DIODrv_GetChanState(_DIO_INDEX_HVLTDI7_CHAN);//N  L��Ч            X13
    bDiSatus[17]=DIODrv_GetChanState(_DIO_INDEX_HVLTDI8_CHAN);//D  L��Ч            X14
    bDiSatus[18]= ATDDrv_GetChanResult(_ATD_INDEX_HVLTWK1_CHAN)>SC_VDL_LevelHighThres  ?    1:0   ;// Ӳ������1�źţ���Ӧ�Ӳ��64   H��Ч  X64
    bDiSatus[19]= ATDDrv_GetChanResult(_ATD_INDEX_HVLTWK2_CHAN)>SC_VDL_LevelHighThres  ?    1:0   ;// Ӳ������2�źţ���Ӧ�Ӳ��65   H��Ч  X65
    bDiSatus[20]= ATDDrv_GetChanResult(_ATD_INDEX_HVLTWK3_CHAN)>SC_VDL_LevelHighThres  ?    1:0   ;// Ӳ������3�źţ���Ӧ�Ӳ��58   H��Ч  X58
    bDiSatus[21]=0;//����
    bDiSatus[22]=0;//����
    bDiSatus[23]=0;//����
    
    /***********************************************************************************************************/
    uADSatus[0] =ATDDrv_GetChanResult(_ATD_INDEX_VLT6_CHAN);    //�ƶ���Ƭ�������      X77   4095=5V
    uADSatus[1] =0;
    uADSatus[2] =0;
    uADSatus[3] =0;
    uADSatus[4] =0;
    uADSatus[5] =ATDDrv_GetChanResult(_ATD_INDEX_VLT9_CHAN);   // PTC�¶ȿ���1          X56   4095=5V
    uADSatus[6] =ATDDrv_GetChanResult(_ATD_INDEX_VLT5_CHAN);   // �յ��¶ȿ���          X70   4095=5V
    uADSatus[7] =ATDDrv_GetChanResult(_ATD_INDEX_HVLTDI2_CHAN);// �յ���ѹ����750\���ٽ�� X8 4095=25.6V
    uADSatus[8] =0;
    uADSatus[9] =0;
    uADSatus[10]=ATDDrv_GetChanResult(_ATD_INDEX_HVLTDI4_CHAN);// �ƶ�����2             X10   4095=25.6V
    uADSatus[11] =0;
    uADSatus[12]=ATDDrv_GetChanResult(_ATD_INDEX_HVLTDI1_CHAN);// DCDC����              X7    4095=25.6V
    uADSatus[13]=ATDDrv_GetChanResult(_ATD_INDEX_HVLTDI3_CHAN);// �ƶ�����1             X9    4095=25.6V
    uADSatus[14]=ATDDrv_GetChanResult(_ATD_INDEX_HVLTDI5_CHAN);// P                     X11   4095=25.6V
    uADSatus[15]=ATDDrv_GetChanResult(_ATD_INDEX_HVLTDI6_CHAN);// R                     X12   4095=25.6V
    uADSatus[16]=ATDDrv_GetChanResult(_ATD_INDEX_HVLTDI7_CHAN);// N                     X13   4095=25.6V
    uADSatus[17]=ATDDrv_GetChanResult(_ATD_INDEX_HVLTDI8_CHAN);// D                     X14   4095=25.6V
    uADSatus[18]=ATDDrv_GetChanResult(_ATD_INDEX_VLT7_CHAN);        // ģʽ�л�         X54   4095=5V
    uADSatus[19]=ATDDrv_GetChanResult(_ATD_INDEX_VLT8_CHAN);        // ����             X61   4095=5V
    uADSatus[20]=ATDDrv_GetChanResult(_ATD_INDEX_VLT10_CHAN);       // ����             X63   4095=5V
    uADSatus[21]=ATDDrv_GetChanResult(_ATD_INDEX_HVLTWK1_CHAN);       // Ӳ������1�ź�  X64   4095=25.6V
    uADSatus[22]=ATDDrv_GetChanResult(_ATD_INDEX_HVLTWK2_CHAN);       // Ӳ������2�ź�  X65   4095=25.6V
    uADSatus[23]=ATDDrv_GetChanResult(_ATD_INDEX_HVLTWK3_CHAN);       // Ӳ������3�ź�  X58   4095=25.6V
    uADSatus[24]=ATDDrv_GetChanResult(_ATD_INDEX_VBP_CHAN);           //VBP���̵�����ѹ X27   4095=25.6V
    uAD_LOGICP_CHAN=ATDDrv_GetChanResult(_ATD_INDEX_IGN_CHAN)  ;      //IGN��𿪹ص�ѹ X52   4095=25.6V
    /************************************************************************************************************/
    uAD4Satus[0]=ATDDrv_GetChanResult(_ATD_INDEX_VLT1_CHAN)  ;//����1   X68      4095=5V
    uAD4Satus[1]=ATDDrv_GetChanResult(_ATD_INDEX_VLT2_CHAN)  ;//����2   X75
    uAD4Satus[2]=ATDDrv_GetChanResult(_ATD_INDEX_VLT3_CHAN)  ;//�ƶ�1   X72
    uAD4Satus[3]=ATDDrv_GetChanResult(_ATD_INDEX_VLT4_CHAN)  ;//�ƶ�2   X79
    /***********************************************************************************************************/
    
    
    // PSWTDrv_Interface(Index,bDoSatus[Index]);
    
    /*
    PSWTDrv_Interface(_PSWT_INDEX_LSD1_CHAN,0      ); //LSD1    X41  PWM1 ����
    PSWTDrv_Interface(_PSWT_INDEX_LSD2_CHAN,0      ); //LSD2    X42  PWM2 ����
    PSWTDrv_Interface(_PSWT_INDEX_LSD3_CHAN,0      ); //LSD3    X43  PWM3 ����
    PSWTDrv_Interface(_PSWT_INDEX_LSD4_CHAN,0      ); //LSD4    X44  PWM4 ����
    */
    
    PSWTDrv_Interface(_PSWT_INDEX_LSD5_CHAN,bDoSatus[0]      ); //��ѹ���̵���        X45   
    PSWTDrv_Interface(_PSWT_INDEX_LSD6_CHAN,bDoSatus[6]      ); //�����ѹ���̵���    X46     
    PSWTDrv_Interface(_PSWT_INDEX_LSD7_CHAN,0      );           //����                X47
    PSWTDrv_Interface(_PSWT_INDEX_LSD8_CHAN,bDoSatus[1]      ); //��ص������12V��Դ    X29
    PSWTDrv_Interface(_PSWT_INDEX_LSD9_CHAN,bDoSatus[2]      ); //���ٷ���ʹ��           X30
    PSWTDrv_Interface(_PSWT_INDEX_LSD10_CHAN,bDoSatus[7]      ); //�յ�ʹ���ź�D07       X31
    PSWTDrv_Interface(_PSWT_INDEX_LSD11_CHAN,bDoSatus[5]      ); //PTC�̵��� D07         X32
    PSWTDrv_Interface(_PSWT_INDEX_LSD12_CHAN,bDoSatus[4]      ); //��������ź�   D07    X33    
    PSWTDrv_Interface(_PSWT_INDEX_LSD13_CHAN,bDoSatus[3]      ); //���ȵ���ʹ���ź� D07  X34   
    
    PSWTDrv_Interface(_PSWT_INDEX_HLSD1_CHAN,bDoSatus[15]      ); //DCDCʹ���ź�         X48
    PSWTDrv_Interface(_PSWT_INDEX_HLSD2_CHAN,bDoSatus[11]      );            //ECO״̬���          X49    
    PSWTDrv_Interface(_PSWT_INDEX_HLSD3_CHAN,bDoSatus[12]      );            //SPORT״̬���        X50  
    PSWTDrv_Interface(_PSWT_INDEX_HLSD4_CHAN,bDoSatus[10]      ); //TCS_OFF �����ź�     X51      
    
    PSWTDrv_Interface(_PSWT_INDEX_HSD1_CHAN,bDoSatus[13]      ); //��������ź�  8m      X38 
    PSWTDrv_Interface(_PSWT_INDEX_HSD2_CHAN,bDoSatus[14]      ); //PTC�̵���     8m      X39 
    
    
    /*************************************************************************************************************/
  }
  #endif
}


void MSCANDrv_SendMsgCAN_A(MSCANDrv_infoMSGS_TYPE infoMsgS) 
{   //����CAN
    #ifndef AECS_HW
        MSCANDrv_SendMsgCAN0( infoMsgS);    
    #else  
        MSCANDrv_SendMsgCAN1( infoMsgS);    
    #endif

    

}
void MSCANDrv_SendMsgCAN_B(MSCANDrv_infoMSGS_TYPE infoMsgS) 
{   //���-����CAN
    #ifndef AECS_HW
        MSCANDrv_SendMsgCAN1( infoMsgS);    
    #else  
        MSCANDrv_SendMsgCAN2( infoMsgS);    
    #endif

}
void MSCANDrv_SendMsgCAN_C(MSCANDrv_infoMSGS_TYPE infoMsgS)
{   //����CAN
    #ifndef AECS_HW
        MSCANDrv_SendMsgCAN2( infoMsgS);    
    #else  
        MSCANDrv_SendMsgCAN0( infoMsgS);    
    #endif

}

//CAN0������Ϣ������0
void CANNet_RecvMsgCAN0(MSCANDrv_infoMSGS_TYPE infoMsgS)
{
    //CAN��������  
    #ifndef AECS_HW
        //����
        CANNet_RecvMsgCAN_A( infoMsgS);    
    #else
        //����  
        CANNet_RecvMsgCAN_C( infoMsgS);    
    #endif
    
}
//CAN0������Ϣ������1
void CANNet_RecvMsgCAN1(MSCANDrv_infoMSGS_TYPE infoMsgS)
{
    //CAN��������  
    #ifndef AECS_HW
        //����
        CANNet_RecvMsgCAN_B( infoMsgS);    
    #else
        //����
        CANNet_RecvMsgCAN_A( infoMsgS);    
    #endif
    
}//CAN0������Ϣ������2
void CANNet_RecvMsgCAN2(MSCANDrv_infoMSGS_TYPE infoMsgS)
{
    //CAN��������  
    #ifndef AECS_HW
        //����
        CANNet_RecvMsgCAN_C( infoMsgS);    
    #else
        //����
        CANNet_RecvMsgCAN_B( infoMsgS);    
    #endif
    
}