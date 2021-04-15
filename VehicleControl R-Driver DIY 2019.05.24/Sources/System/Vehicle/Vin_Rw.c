/*******************************************************************************
* 工程名称：Vin码刷写及通讯功能
* 文件名： Vin_Rw.c
* 功能描述 ： 
* 引用记录：
* 备注： 
* 修订记录：
*		日期		      版本		  作者		描述
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
    //Vin可刷写次数、电机号可刷写次数、电池编码可刷写次数、电控可刷写次数
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



//    刷写故障码       发送刷写应答帧  当前发送广播帧数      延迟发送广播帧   需要发送广播帧数 
uint8  Vin_Rw_Errcode=0,VCU_RESPONSE=0,VCU_ROADCASTE=0,VCU_ROADCASTE_Delay=0,VCU_ROADCASTE_Number=0;
uint8  CANNet_Send_VinRwMsg_Index; //20ms间隔
uint16 LostHCCommunication_Index=0; //通讯丢失计数
uint8  LostHCCommunication=0;       //上位机通讯丢失
uint8  VCU_ROADCASTE_Repeat_Times=0; //广播帧已重发次数
uint8  Device_Id_H4=0,Device_Id_L4=0;
uint8  Data_OK[45]=0;//接收数据帧完整检测
uint8  Need_Flash=0; //接收完成，需要Flash编程

uint8 Vin_RW_ReadyForSend=0;

//上位机命令帧HC_CMD
// 命令、用户级别、设备地址、操作设备数量、数据帧ID、帧数据、时间戳、收到新报文
extern uint8  HC_CMD,User_Level,Device_Id,Device_Num,HC_Data_Id,HC_Data[316],Time_Mark,GetNewMessage_HC_CMD;
//需要接收的数据帧数
extern uint8  Need_HC_Data;


extern uint8 Rw_Times_Reset;//可刷写次数复位

#ifndef AECS_HW
      #pragma CODE_SEG DEFAULT
#else  
      #pragma CODE_SEG VCU_CODE
      
#endif
void Vin_Rw_Main(void) 
{ //Vin码读写主程序 10ms被调用一次
    uint16 i=0;

    if ((Rw_Vin_Times==0)||(Rw_Vin_Times==255)){Rw_Vin_Times=4;}
    if ((Rw_Mot_Times==0)||(Rw_Mot_Times==255)){Rw_Mot_Times=4;}
    if ((Rw_Bms_Times==0)||(Rw_Bms_Times==255)){Rw_Bms_Times=4;}
    if ((Rw_MCU_Times==0)||(Rw_MCU_Times==255)){Rw_MCU_Times=4;}
    
    if (GetNewMessage_HC_CMD==1)
    {   //收到新报文
        GetNewMessage_HC_CMD=0;
        
        switch(HC_CMD) 
        {
            case 0xA0:  //注册帧
                VCU_RESPONSE=1;  //应答一帧
                if   ((User_Level==1)||         //Reader
                      (User_Level==2)||         //Writer
                      (User_Level==0)||         //兼容旧模式
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
            case 0xA3:  //读取帧
                VCU_RESPONSE=1;  //应答一帧
                if (Time_Mark==0){ User_Level=0; }//上位机为旧模式
                if( (User_Level==0)||(User_Level==1)||(User_Level==2)||(User_Level==0xF9) )
                {   //合法用户可以读取数据    
                    //Device_Id、Device_Num、Time_Mark  Vin_Code  //HC_Data[316]   0-316(314) 15组21字节编码
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
                              
                              
                              
                          }else { Vin_Rw_Errcode=0x42; }//不能读取的设备地址
                          break;                    
                        case 0xB: //电机
                          if ((Device_Id_L4==1)&&(Device_Num==1))
                          { 
                              //(void) memcpy(HC_Data,Mot_Code,21);
                              for (i=0;i<21;i++){  HC_Data[i]=Mot_Code[i];         }
                          }else { Vin_Rw_Errcode=0x42; }//不能读取的设备地址             
                          break;                    
                        case 0xC: //电池
                          if ( (Device_Id_L4>0) && (Device_Id_L4<0xF) && (Device_Num+Device_Id_L4 < 0x10)) 
                          {
                              //(void) memcpy(HC_Data,Bms_Code,21);
                              //(void) memcpy(HC_Data,Bms_Code+(Device_Id_L4-1)*21 ,Device_Num*21);
                          
                          
                          }else { Vin_Rw_Errcode=0x42; }//不能读取的设备地址  
                          break;                    
                        case 0xD: //电控
                          if ((Device_Id_L4==1)&&(Device_Num==1))
                          { 
                             // (void) memcpy(HC_Data,MCU_Code,21);
                              for (i=0;i<21;i++){  HC_Data[i]= MCU_Code[i];         }
                          }else { Vin_Rw_Errcode=0x42; }//不能读取的设备地址              
                          break;                    
                        default:   
                            break;                    
                    }
                }else 
                {   //User_Level=3; //已注销或非法用户，不可读写
                    Vin_Rw_Errcode=0x32;
                }
                break;
            case 0xA1:  //保持帧
                VCU_RESPONSE=1;  //应答一帧
                LostHCCommunication_Index=0;
                break;
            case 0xA6:  //写入帧
               /*   Device_Id    Device_Num    Time_Mark   */ 
                VCU_RESPONSE=1;  //应答一帧
                HC_Data_Id=0;
                if (Time_Mark==0){ User_Level=0; }//上位机为旧模式
                if( (User_Level==0)||(User_Level==2)||(User_Level==0xF9) ) 
                {   //用户合法 可以写
                    Vin_Rw_Errcode=0;
                    LostHCCommunication_Index=0;
                    Device_Id_H4=Device_Id>>4;
                    Device_Id_L4=Device_Id&0x0F;
                    //是否正在等待数据帧
                    if (Need_HC_Data==0)
                    {
                      
                        switch(Device_Id_H4)
                        {
                            case 0xA: //VIN
                              
                              if ((User_Level!=0xF9)&&(Rw_Vin_Times<=1)){Vin_Rw_Errcode=0x41;}//写次数耗尽   
                              else
                              {
                                  if ((Device_Id_L4==1)&&(Device_Num==1))
                                  {   //合法参数，等待3帧数据帧
                                      Need_HC_Data=3;
                                      (void)memset(Data_OK,0,Need_HC_Data+1);
                                  }else { Vin_Rw_Errcode=0x31; }//参数值超过范围
                              }
                              break;                    
                            case 0xB: //电机 
                              if ((User_Level!=0xF9)&&(Rw_Mot_Times<=1)){Vin_Rw_Errcode=0x41;}//写次数耗尽   
                              else
                              {                              
                                  if ((Device_Id_L4==1)&&(Device_Num==1))
                                  {   //合法参数, 等待3帧数据帧
                                      Need_HC_Data=3;    
                                      (void)memset(Data_OK,0,Need_HC_Data+1);
                                  }else { Vin_Rw_Errcode=0x31; }//参数值超过范围                        
                              }
                              break;                    
                            case 0xC: //电池  
                              if ((User_Level!=0xF9)&&(Rw_Bms_Times<=1)){Vin_Rw_Errcode=0x41;}//写次数耗尽   
                              else
                              {                                
                                  if ((Device_Id_L4>0) && (Device_Id_L4<0xF) && (Device_Num+Device_Id_L4 < 0x10))
                                  {   //合法参数，等待Device_Num *3 帧数据帧
                                      Need_HC_Data=Device_Num*3;
                                      (void)memset(Data_OK,0,Need_HC_Data+1);
                                  }else { Vin_Rw_Errcode=0x31; }//参数值超过范围                        
                              }
                              break;                    
                            case 0xD: //电控     
                              if ((User_Level!=0xF9)&&(Rw_MCU_Times<=1)){Vin_Rw_Errcode=0x41;}//写次数耗尽   
                              else
                              {                                
                                  if ((Device_Id_L4==1)&&(Device_Num==1))
                                  {   //合法参数，等待3帧数据帧
                                      Need_HC_Data=3;
                                      (void)memset(Data_OK,0,Need_HC_Data+1);
                                  }else { Vin_Rw_Errcode=0x31; }//参数值超过范围                        
                              }
                              break;                    
                            default:   
                                break;                    
                        }
                    }else 
                    { //正在等待数据帧
                        Vin_Rw_Errcode=0x21;
                    
                    }
                                        
                }else
                {  //用户不合法
                   
                    Vin_Rw_Errcode=0x32;
                    Need_HC_Data=0;
                }
                
                break;
            case 0xAF:  //注销帧
                VCU_RESPONSE=1;
                Need_HC_Data=0;
                User_Level=3; //不可写入
                if (LostHCCommunication==0) {Vin_Rw_Errcode=0;}
                break;
            case 0xFF:  //数据帧
            case 0x00:
                
                break;
            default:   
                VCU_RESPONSE=1;
                Vin_Rw_Errcode=0x11;//错误的命令字 
                HC_CMD=0xAF;        //关闭连接
                User_Level=3;       //注销用户                
                break;
        }

    }else
    {   //连续10s未收到报文
        if ((User_Level!=3)&&(User_Level!=0))
        {
            LostHCCommunication_Index++;
            if (LostHCCommunication_Index>1000)  //通讯超时 10s
            {
                VCU_RESPONSE=1;
                LostHCCommunication=1;
                Need_HC_Data=0; //直接置0 可能有问题
                Vin_Rw_Errcode=0x51;
                HC_CMD=0xAF;        //超时关闭连接
                User_Level=3;       //注销用户
            }
        }
    }
    
    //20ms一个完整循环
    //CANNet_Send_VinRwMsg_Index=(uint8)((CANNet_Send_VinRwMsg_Index+1)%2);
    //刷写VIN通讯功能    20ms发送一次
    //if (CANNet_Send_VinRwMsg_Index==0) {  CANNet_Send_VinRwMsg(); }   
    Vin_RW_ReadyForSend=1;
    
    if ((Need_HC_Data<=HC_Data_Id)&&(Need_HC_Data>0))
    {
        HC_Data_Check();
    }
    
    //Need_Flash=1
    //接收完成，需要Flash编程
    if (Need_Flash==1)
    {
        Need_Flash=0;
        Flash_Times++;
        //EEPROMDrv_bSaveInstantUB=1;
        
        
        
    }
    
    
}

void CANNet_Send_VinRwMsg(void)
{  //刷写VIN通讯功能    20ms发送一次
    MSCANDrv_infoMSGS_TYPE infoMsgS;
    
    if (VCU_RESPONSE>0)
    {    //发送应答帧 0x0C6501A0 
         VCU_RESPONSE=VCU_RESPONSE-1; //应答计数-1
         infoMsgS.infoMsgIDUI=0x0C6501A0;
         infoMsgS.bExtIDUB=1;
         infoMsgS.numLenUB=8;
         infoMsgS.idxMsgPriorityUB=0x08;
         switch(HC_CMD) 
         {        
            case 0xA0:  //注册帧
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
                    Vin_Rw_Errcode=0; //清空故障
                }
                infoMsgS.datMsgUBA[6]=0;
                infoMsgS.datMsgUBA[7]=0;      
                VCU_ROADCASTE_Delay=0;
                VCU_ROADCASTE=0;
                VCU_ROADCASTE_Number=0;     
                break;
            case 0xA3:  //读取帧
                infoMsgS.datMsgUBA[0]=0xA3;
                infoMsgS.datMsgUBA[1]=Vin_Rw_Errcode;
                infoMsgS.datMsgUBA[2]=Time_Mark;
                infoMsgS.datMsgUBA[3]=0;
                infoMsgS.datMsgUBA[4]=0;
                infoMsgS.datMsgUBA[5]=0;
                infoMsgS.datMsgUBA[6]=0;
                infoMsgS.datMsgUBA[7]=0;
                if (Vin_Rw_Errcode==0) //读取正常
                {
                    VCU_ROADCASTE_Repeat_Times=0;//广播帧已重发次数归零
                    VCU_ROADCASTE_Delay=1;       //广播帧下一周期开始  延迟1周期
                    VCU_ROADCASTE=0;             //当前发送帧数
                    VCU_ROADCASTE_Number=Device_Num*3;  //需要广播帧数 3 6 9 ……
                    
                }else
                {
                    VCU_ROADCASTE_Delay=0;
                    VCU_ROADCASTE=0;
                    VCU_ROADCASTE_Number=0;
                }
                break;
            case 0xA1:  //保持帧
                infoMsgS.datMsgUBA[0]=0xA1;
                infoMsgS.datMsgUBA[1]=Vin_Rw_Errcode;
                infoMsgS.datMsgUBA[2]=Time_Mark;
                infoMsgS.datMsgUBA[3]=0;
                infoMsgS.datMsgUBA[4]=0;
                infoMsgS.datMsgUBA[5]=0;
                infoMsgS.datMsgUBA[6]=0;
                infoMsgS.datMsgUBA[7]=0;
                break;
            case 0xA6:  //写入帧
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
            case 0xAF:  //注销帧
                
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
            case 0xFF:  //数据帧
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
  
  
    //VCU_ROADCASTE_Times  广播帧重复次数
    //VCU_ROADCASTE_Repeat_Times=0; //广播帧已重发次数   
    
       /*
       VCU_ROADCASTE_Delay=1;       //广播帧下一周期开始
       VCU_ROADCASTE=0;             //当前发送帧数
       VCU_ROADCASTE_Number=Device_Num*3;  //需要广播帧数
       HC_Data[316];
       */
    
    
    
    //发送广播帧
    if (VCU_ROADCASTE<VCU_ROADCASTE_Number)//VCU_ROADCASTE_Number需要广播的帧数  //VCU_ROADCASTE   开始发送时为0  每发送完一帧+1 
    {
        if (VCU_ROADCASTE_Delay>0)
        {
            VCU_ROADCASTE_Delay=VCU_ROADCASTE_Delay-1;//广播延迟计数减1
        }else 
        {   //广播延迟计数结束，开始发送广播帧 0x0CFF01A0;
            
            infoMsgS.infoMsgIDUI=0x0CFF01A0;
            infoMsgS.bExtIDUB=1;
            infoMsgS.numLenUB=8;
            infoMsgS.idxMsgPriorityUB=0x08;
                
            infoMsgS.datMsgUBA[0]=VCU_ROADCASTE+1;  //数据帧编号 1 2 3 （4 5 6…… BMS）
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
{   //处理接收到的数据帧
    //Device_Id_H4 Device_Id_L4 Device_Id  Device_Num  
    //Vin_Code[21];  Mot_Code[21]; MCU_Code[21];                       
    //HC_Data[316]
    //User_Level   Vin_Rw_Errcode
    
    //Need_HC_Data  目标数据帧数量
    //HC_Data_Id    数据帧ID 1 2 3 
    
    
    VCU_RESPONSE=1; //处理一帧数据帧，发送应答一帧
    Vin_Rw_Errcode=0x01;
    
    if ((HC_Data_Id>0)&&(HC_Data_Id<0x2D) && (Need_HC_Data>0))
    {  //数据帧ID合法且需要接收数据
        if (Need_HC_Data+1>HC_Data_Id)
        {  //接收数据未完成
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
        {   //接收数据完成,做数据帧完整检查
            
            HC_Data_Id=0;
            for (Index=1;Index>=Need_HC_Data;Index++)
            {
                Data_OK_Total=Data_OK_Total && Data_OK[Index];
                //Data_OK[Index]=0;
            }
            
            
            if (Data_OK_Total==1)
            {   //数据帧完整，可以写入
                //(void)memset(Data_OK,0,Need_HC_Data+1);
               
                switch(Device_Id_H4)
                {
                    case 0xA://VIN
                        //(void)memcpy(Vin_Code,HC_Data,sizeof(Vin_Code));
                        for (i=0;i<21;i++){   Vin_Code[i]=HC_Data[i];        }
                        Need_HC_Data=0;//不再接收数据
                        Vin_Rw_Errcode=0;
                        Need_Flash=1;//接收完成，需要Flash编程
                        break;
                    case 0xB://电机
                        //(void)memcpy(Mot_Code,HC_Data,sizeof(Mot_Code));
                        for (i=0;i<21;i++){   Mot_Code[i]=HC_Data[i];        }
                        Need_HC_Data=0;//不再接收数据
                        Vin_Rw_Errcode=0;
                        Need_Flash=1;//接收完成，需要Flash编程
                        break;
                    case 0xC://电池
                         //(void)memcpy(Bms_Code,HC_Data,sizeof(Mot_Code));
                         //(void) memcpy(HC_Data,Bms_Code+(Device_Id_L4-1)*21 ,Device_Num*21);
                        //(void)memcpy(Bms_Code+(Device_Id_L4-1)*21,HC_Data,Device_Num*21);                   
                        Need_HC_Data=0;//不再接收数据
                        Vin_Rw_Errcode=0;
                        Need_Flash=1;//接收完成，需要Flash编程                    
                    
                    
                        break;
                    case 0xD://电控
                        //(void)memcpy(MCU_Code,HC_Data,sizeof(MCU_Code));
                        for (i=0;i<21;i++){   MCU_Code[i]=HC_Data[i]; }
                        Need_HC_Data=0;//不再接收数据
                        Vin_Rw_Errcode=0;
                        Need_Flash=1;//接收完成，需要Flash编程  
                        break;                      
                    default:
                        //设备ID错误
                        Vin_Rw_Errcode=0x12;
                        Need_HC_Data=0;//不再接收数据
                        Need_Flash=0;
                        break;
                }
                if (EEPROMDrv_bSaveInstantUB==1)
                { //EEPROM区未准备好，不可烧写
                    Vin_Rw_Errcode=0xFD;
                    //Need_HC_Data=0;//不再接收数据
                    Need_Flash=0;               
                }else 
                { //EEPROM区准备好，可烧写
                    if (((User_Level==2)||(User_Level==0))&&(Need_Flash==1)) 
                    {   //剩余烧写次数减1
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
            { //数据帧不完整
                    Vin_Rw_Errcode=0x13;
                    Need_HC_Data=0;//不再接收数据
                    Need_Flash=0;              
            }
            
            
        }



}






















