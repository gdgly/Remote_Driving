#include <string.h>
//#include "HWIO_API.h"
#include "CANNet.h"
#include "GlobalDef.h"

uint32 CANNet_infoIDCAN0UI;
uint8 CANNet_datMsgCAN0UB;

uint32 CANNet_infoIDCAN1UI;
uint8 CANNet_datMsgCAN1UB;

uint32 CANNet_infoIDCAN2UI;
uint8 CANNet_datMsgCAN2UB;

uint8 canip_slew_0x0F1_Rmsgbuff[9];
uint8 canip_fmcu_0x116_Rmsgbuff[9];
uint8 canip_fmcu_0x316_Rmsgbuff[9];
uint8 canip_fmcu_0x411_Rmsgbuff[9];
uint8 canop_bms_0x18B_Tmsgbuff[9];
uint8 canop_fmcu_0x181_Tmsgbuff[9];

uint16 CANNet_idxSchedUW;



//#pragma CODE_SEG SEG_VDL_CAN    
    #ifndef AECS_HW
      #pragma CONST_SEG DEFAULT
    #else  
      #pragma CODE_SEG SEG_VDL_CAN
    #endif


//CAN0接收消息处理函数
void CANNet_RecvMsgCAN0(MSCANDrv_infoMSGS_TYPE infoMsgS)
{
    //CAN接收样例
    CANNet_infoIDCAN0UI=infoMsgS.infoMsgIDUI;
    CANNet_datMsgCAN0UB=infoMsgS.datMsgUBA[2];
}

//CAN1接收消息处理函数
void CANNet_RecvMsgCAN1(MSCANDrv_infoMSGS_TYPE infoMsgS)
{
    uint32 id;
    uint8 i;
    id=infoMsgS.infoMsgIDUI;

      if(id==0x0F1) {
        for(i=1;i<9;i++)
          {
            canip_slew_0x0F1_Rmsgbuff[i]=infoMsgS.datMsgUBA[i-1];
          }
          canip_slew_0x0F1_Rmsgbuff[0]++;
      }

}

//CAN2接收消息处理函数
void CANNet_RecvMsgCAN2(MSCANDrv_infoMSGS_TYPE infoMsgS)
{
    uint32 id;
    uint8 i;
    id=infoMsgS.infoMsgIDUI;
   
      if(id==0x116) {
          for(i=1;i<9;i++)
          {
            canip_fmcu_0x116_Rmsgbuff[i]=infoMsgS.datMsgUBA[i-1];
          }
          canip_fmcu_0x116_Rmsgbuff[0]++;
       }
      if(id==0x316) { 
          for(i=1;i<9;i++)
          {
            canip_fmcu_0x316_Rmsgbuff[i]=infoMsgS.datMsgUBA[i-1];
          }
          canip_fmcu_0x316_Rmsgbuff[0]++;
       }
      if(id==0x411) {
          for(i=1;i<9;i++)
          {
            canip_fmcu_0x411_Rmsgbuff[i]=infoMsgS.datMsgUBA[i-1];
          }
          canip_fmcu_0x411_Rmsgbuff[0]++;
       }
}

//发送消息管理 

void CANNet_Manage1ms(void)
{
    
    MSCANDrv_infoMSGS_TYPE infoMsgS;
    if(CANNet_idxSchedUW%10==0)     //10ms发送loop
    {
       if(canop_bms_0x18B_Tmsgbuff[0]==1) 
       {      
        infoMsgS.infoMsgIDUI=0x18B;
        infoMsgS.idxMsgPriorityUB=0;
        infoMsgS.bExtIDUB=0;
        (void)memcpy(infoMsgS.datMsgUBA,canop_bms_0x18B_Tmsgbuff+1,8);
        MSCANDrv_SendMsgCAN1(infoMsgS);
       }
       if(canop_fmcu_0x181_Tmsgbuff[0]==1)
       
       {      
        infoMsgS.infoMsgIDUI=0x181;
        infoMsgS.idxMsgPriorityUB=1;
        infoMsgS.bExtIDUB=0;
        (void)memcpy(infoMsgS.datMsgUBA,canop_fmcu_0x181_Tmsgbuff+1,8);
        MSCANDrv_SendMsgCAN2(infoMsgS);
       }
       
    }
    if(CANNet_idxSchedUW%20==0) 
    {
    }
    if(CANNet_idxSchedUW%100==0) 
    {
       
    }
    if(CANNet_idxSchedUW%500==0) 
    {
    }
    if(CANNet_idxSchedUW%1000==0) 
    {
    }
    
    CANNet_idxSchedUW=(uint16)((CANNet_idxSchedUW+1)%1000); //1000ms重新循环计数
}