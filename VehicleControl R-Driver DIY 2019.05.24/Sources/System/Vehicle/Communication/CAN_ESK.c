/*******************************************************************************
* 工程名称：电子排档杆CAN通讯程序
* 文件名： CAN_ESK.c
* 功能描述 ： 
* 引用记录：
* 备注： 
* 修订记录：
*		日期		      版本		  作者		描述
*   
*******************************************************************************/
#include <string.h>
#include "GlobalDef.h"
#include "CAN_ESK.h"

    
#ifndef AECS_HW
       #pragma DATA_SEG DEFAULT
#else  
      #pragma DATA_SEG S12X_RAM
#endif

uint32 CANNet_infoID_ESK;
uint8 CanRecESK[8]={0};
uint8 LostESKCommunication=0;
uint16 LostESKCom_Index=0;
uint8 ESK_Signal=0;
    
extern uint8 ESK_Signal_Type;//档位信号类型 0 电平信号 1 CAN总线信号 
extern uint8 ESK_Type;//电子换档位类型  0 拨片式、旋钮式（点动信号）  1 排档杆式、旋钮式（自锁信号）
extern uint8 ESK_Fault;//电子换档器故障
extern uint8 ESK_Live;//电子换档器心跳信号 0~15

extern uint8 ESK_SelfLock_Request; //电子换挡器信号请求  自锁式  手刹另外硬线接入
extern uint8 ESK_Inching_Request;  //电子换挡器信号请求  点动式  含手刹信号


 
#ifndef AECS_HW
      #pragma CODE_SEG DEFAULT
#else  
      #pragma CODE_SEG VCU_CODE
      
#endif

//电子排档接收消息处理函数
void CANNet_RecvMsgCAN_ESK(MSCANDrv_infoMSGS_TYPE infoMsgS)
{

    // uint8 Index;
    //CAN接收样例
    CANNet_infoID_ESK=infoMsgS.infoMsgIDUI;

    //CAN0接收分类处理
    
    switch(CANNet_infoID_ESK) 
    {
      case 0x0CEEA9A0:
            //ESK 电子排档杆
            /*
            uint8 ESK_Signal_Type=0;//档位信号类型 0 电平信号 1 CAN总线信号 
          
            */
            memcpy(CanRecESK,infoMsgS.datMsgUBA,sizeof(CanRecESK));                 
            ESK_Inching_Request=CanRecESK[0];         //电子换挡器信号请求  点动式  含手刹信号            
            ESK_SelfLock_Request=0x0F&CanRecESK[1];   //电子换挡器信号请求  自锁式  手刹另外硬线接入
            ESK_Type=0x01&CanRecESK[2];               //电子换档位类型  0 拨片式、旋钮式（点动信号）  1 排档杆式、旋钮式（自锁信号）
            ESK_Fault=(0x02&CanRecESK[2])>>1;         //电子换档器故障
            
           // ESK_Signal=1;//收到电子排档信号  
           // LostESKCom_Index=0;
            
            
            //先判断心跳信息再更新心跳计数
            if (   (((CanRecESK[2])>>4==0) && (ESK_Live==15)) 
                ||   ((CanRecESK[2])>>4==ESK_Live+1) ) //心跳正常
            {
                LostESKCom_Index=0;
                ESK_Signal=1;//收到电子排档信号            
            }
            ESK_Live=CanRecESK[2]>>4;                 //电子换档器心跳信号 0~15 
             
             
            break;
      default:
            break;
    }
}









