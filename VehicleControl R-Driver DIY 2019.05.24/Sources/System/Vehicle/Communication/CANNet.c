/*******************************************************************************
* 工程名称：整车管理
* 文件名： CANNet.c
* 功能描述 ： CAN总线管理
* 引用记录：
* 备注： 
* 修订记录：
*		日期		      版本		  作者		描述
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



sint16 Steering_Wheel_Angle_Code=0; //方向盘旋转编码  -32768,360°/5000 右转为正
uint8  Steering_Wheel_Angle_ZeroMark=0; //方向盘是否经过0点校正


sint16 Steering_Wheel_Angle_BaseCAN=0;
uint8  AccPed_AD255=0;
uint8  Breakk_AD255=0;
uint8  Clutch_AD255=0;
uint8  Steering_Wheel_Angle_Mot_Temp=0;
uint8  Steering_Wheel_Angle_Life=0; 

//********************************************************************************//
uint8  EPS_Contral_Mode=0;//EPS控制模式
sint16 EPS_Demand_Angle=0;//EPS角度命令 0.1度步进   -1080.0~1080.0°
uint8  EPS_Angle_MaxSpeed=0;//EPS 最大角速度

sint8 Vehco_Acceleration_Command=0;   //加速度指令 -12.8~12.7
uint8 EPB_Command=0;    //电子手刹指令 0 无动作 1 拉起 2 释放 3 行进中制动
uint8 Dang_Command=0;   //档位指令 0 N 1 D 2 R 3 保留
uint8 Emergency_Braking=0;  //紧急制动指令

uint8 DriverMode_Command=0; //自驾模式状态 0 手动 1 保留 2 保留 3 远程
uint8 Door_Command=0;       //车门控制命令 0 无动作 1 开门 2 关门 3 保留
uint8 Beep_Command=0;       //喇叭控制     0 无动作 1 开启 2 关闭 3 保留
uint8 R_DriverSystem_State=0;       //远程驾驶系统状态 0 连接中 1 连接完成 2 断开中 3 断开
uint8 R_DriverSystem_Level=2;       //远程驾驶系统可信级别 0 不可信，立刻接管 1 低质量 ，人工监视 2 最佳状态，完全控制
uint8 R_DriverSystem_Fault_Level=0; //远程驾驶系统故障级别 0 无故障 1 轻微 提示 2 一般故障 跛行 3 严重故障 退出远程驾驶状态
uint8 AC_Mode=1;          //车辆冷暖空调控制 0 关机 1 制冷 2 制热 3 自动
uint8 AC_Temperature=1;   //车辆冷暖空调设置温度 0.5 偏移16

uint8 R_Break_Mode=0;   //远程制动激活   0x00：未激活,本地控制 0x01：激活，制动控制器执行远程网关指令 0x02:融合激活，取最大
uint8 R_Break_Rang=0;     //远程制动力请求 0~100%

//***************以下为从车辆接收数据********************

uint8 EPS_Run_Mode;//EPS返回当前控制模式
sint16 EPS_Driver_Torque;//EPS返回驾驶员方向盘力矩 -12.8~12.7Nm
sint16 EPS_Torque; //EPS输出力矩 0.3Nm/bit -38.4~38.1Nm
sint16 EPS_Angle; //EPS当前角度 0.1度步进   -1080.0~1080.0°
uint8 EPS_Temp;//EPS控制器温度 2℃/bit 0~126
uint8 EPS_ERR_Level;    //EPS故障级别
uint8 EPS_Life=255;         //EPS心跳



uint8   EPB_State=0;                //EPB状态
uint8   Dang_State=0;               //车辆档位状态
sint16  Motor_Speed=0,Motor_Torque=0;   //电机转速、电机扭矩
sint8   Vehco_Acceleration_Back=0; //车辆当前上报加速度 -12.8~12.7

uint8   DriverMode_State=0;       //自驾模式状态
uint8   DoorState=0;              //车门状态
sint16  Vehco_Speed=0;            //车速   
uint8   SOC=0;                    //SOC 精度0.5% 0~100%
uint8   SmartPowerCommand=0;       //智能系统电源指令  00 启动  01 关机
uint8   Change_State=0;           //充电状态 00 无需充电 01充电完成 02 充电中 03 电量低需要充电
uint8   Change_Connect_State=0;   //充电连接状态 0 未连接 1 充电枪已插入
uint8   Vehco_Fault_Level=0;      //整车故障级别
uint8   VehcoState_Live=255;        //整车状态通讯心跳

uint16  B_Voltage=0;    //电池电压 0~800.0V
sint16  B_Current=0;    //电池电流 -500.0~500.0A

uint8   Vehco_Power_State=0;    //整车动力状态 00 INIT ON档 01 READY 02 跛行 03 故障无高压

uint8   Isolation_Level=0;      //绝缘故障等级 0 正常 1 一般 2 严重 3 保留 
uint8   MCU_Fault_Level=0;      //电控故障级别  0 正常 1轻微 2 一般 3 严重
uint8   BMS_Fault_Level=0;      //BMS故障级别   0 正常 1轻微 2 一般 3 严重

uint8   Vehco_Break_Mode=0;     //制动控制器模式 0x00：未激活,本地控制 0x01：激活，制动控制器执行远程网关指令 0x02:融合激活，取最大
uint8   Vehco_Break_Rang=0;     //制动力状态  0~100%
uint8   Vehco_Break_Life=255;   //制动报文心跳

extern uint8   RightTrunSignal_state;        //右转向灯状态
extern uint8   LeftTrunSignal_state;        //左转向灯状态
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

uint8 SetBit8(uint8 *Bitt) //合并8bit为Byter 
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

void Get8bit(uint8 *Bitt,uint8 Bytee)  //拆分byter为8个bit
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

//CAN0接收消息处理函数
void CANNet_RecvMsgCAN_A(MSCANDrv_infoMSGS_TYPE infoMsgS)
{
    uint8 RevMsg_BCC_Code=0,Bcc_Index=0;
    //CAN接收样例
    CANNet_infoIDCAN0UI=infoMsgS.infoMsgIDUI;
    CANNet_datMsgCAN0UB=infoMsgS.datMsgUBA[0];
    //CAN0接收分类处理
    
    switch(CANNet_infoIDCAN0UI) 
    {
        case 0x1802A0B0:    
        //EPS状态
            memcpy(CanRec0,infoMsgS.datMsgUBA,sizeof(CanRec0));      
            /*
            uint8 EPS_Run_Mode;//EPS返回当前控制模式
            sint16 EPS_Driver_Torque;//EPS返回驾驶员方向盘力矩 -12.8~12.7Nm
            sint16 EPS_Torque; //EPS输出力矩 0.3Nm/bit -38.4~38.1Nm
            sint16 EPS_Angle; //EPS当前角度 0.1度步进   -1080.0~1080.0°
            uint8 EPS_Temp;//EPS控制器温度 2℃/bit 0~126
            uint8 EPS_ERR_Level;    //EPS故障级别
            uint8 EPS_Life;         //EPS心跳
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
        //行车状态
            memcpy(CanRec0,infoMsgS.datMsgUBA,sizeof(CanRec0));      
            /*
            uint8   EPB_State=0;                //EPB状态
            uint8   Dang_State=0;               //车辆档位状态
            sint16  Motor_Speed=0,Motor_Torque=0;   //电机转速、电机扭矩
            sint8   Vehco_Acceleration_Back=0; //车辆当前上报加速度
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
        //车辆状态1
            memcpy(CanRec0,infoMsgS.datMsgUBA,sizeof(CanRec0));      
            /*
            uint8   DriverMode_State=0;       //自驾模式状态
            uint8   DoorState=0;              //车门状态
            sint16  Vehco_Speed=0;            //车速   
            uint8   SOC=0;                    //SOC 精度0.5% 0~100%
            uint8   SmartPowerComand=0;       //智能系统电源指令  00 启动  01 关机
            uint8   Change_State=0;           //充电状态 00 无需充电 01充电完成 02 充电中 03 电量低需要充电
            uint8   Change_Connect_State=0;   //充电连接状态 0 未连接 1 充电枪已插入
            uint8   Vehco_Fault_Level=0;      //整车故障级别
            uint8   VehcoState_Live=255;        //整车状态通讯心跳
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
        //车辆状态2
            memcpy(CanRec0,infoMsgS.datMsgUBA,sizeof(CanRec0));      
            /*
            uint16  B_Voltage=0;    //电池电压 0~800.0V
            sint16  B_Current=0;    //电池电流 -500.0~500.0A            
            */
            B_Voltage= (CanRec0[0]+((uint16)CanRec0[1])*256)*2;
            B_Current= (sint16)((CanRec0[2]+((sint32)CanRec0[3])*256)/5-5000);
            if ((B_Current<=-5000)||(B_Current>=8100)){B_Current=0;} //电流极值置零
             
            break; 
        case 0x1811A0B0:    
        //车辆状态3
            memcpy(CanRec0,infoMsgS.datMsgUBA,sizeof(CanRec0));      
            
            break; 
        case 0x1812A0B0:    
        //车辆状态4
            memcpy(CanRec0,infoMsgS.datMsgUBA,sizeof(CanRec0));      
            //Vehco_Power_State //整车动力状态 00 INIT ON档 01 READY 02 跛行 03 故障无高压
            Vehco_Power_State=(CanRec0[2]&0x30)>>4;
            
            break; 
        case 0x1813A0B0:    
        //车辆故障状态
            memcpy(CanRec0,infoMsgS.datMsgUBA,sizeof(CanRec0));      
            /*
            uint8   Isolation_Level=0;      //绝缘故障等级 0 正常 1 一般 2 严重 3 保留 
            uint8   MCU_Fault_Level=0;      //电控故障级别  0 正常 1轻微 2 一般 3 严重
            uint8   BMS_Fault_Level=0;      //BMS故障级别   0 正常 1轻微 2 一般 3 严重
            */            
            Isolation_Level=CanRec0[0]&0x03;
            BMS_Fault_Level=CanRec0[3]&0x03;
            MCU_Fault_Level=(CanRec0[3]&0x0c)>>2;

            break; 
        
        case 0x602:
            //线控制动控制器状态

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

//CAN1接收消息处理函数1
void CANNet_RecvMsgCAN_B(MSCANDrv_infoMSGS_TYPE infoMsgS)
{   
    sint32       temp=0;
    uint8 RevMsg_BCC_Code=0,Bcc_Index=0;

    //CAN接收样例    uint8 CanRec1[8]
    CANNet_infoIDCAN1UI=infoMsgS.infoMsgIDUI;
    CANNet_datMsgCAN1UB=infoMsgS.datMsgUBA[0];
    switch(CANNet_infoIDCAN1UI) 
    {
        case 0x000000AC:
            // 改装模拟方向盘 
            memcpy(CanRec1,infoMsgS.datMsgUBA,sizeof(CanRec1));      
                     
            sint16 Steering_Wheel_Angle_BaseCAN  0.1°
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
        //实验室EPS编码器状态
            memcpy(CanRec1,infoMsgS.datMsgUBA,sizeof(CanRec1));      
            temp     =   ((sint32)CanRec1[1])*256 + CanRec1[0]  -32768;
            if ((temp<= -20000)||(temp>= 20000))            {     temp=0;       }
            Steering_Wheel_Angle_Code=(sint16) temp -Wheel_Offset;
            Steering_Wheel_Angle_ZeroMark = CanRec1[2] ;
            break; 
        
        case 0x602:
            //线控制动控制器状态
            /*
            uint8   Vehco_Break_Mode=0;     //制动控制器模式 0x00：未激活,本地控制 0x01：激活，制动控制器执行远程网关指令 0x02:融合激活，取最大
            uint8   Vehco_Break_Rang=0;     //制动力状态  0~100%
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
            //摇杆转向报文解析
            memcpy(CanRec1,infoMsgS.datMsgUBA,sizeof(CanRec1));
            if (CanRec1[2] <= 0x7F)
            {
                EPS_Demand_Angle = CanRec1[2]/128*(-1080);
            }  
            else
            (
                 EPS_Demand_Angle = (0xFF-0x80)/128*1080;
            )
            // Vehco_Acceleration_Command摇杆油门报文解析


            break; 
            
        default:
            break;
    }      
}

//CAN2接收消息处理函数2
void CANNet_RecvMsgCAN_C(MSCANDrv_infoMSGS_TYPE infoMsgS)
{
    CANNet_infoIDCAN2UI=infoMsgS.infoMsgIDUI;
    CANNet_datMsgCAN2UB=infoMsgS.datMsgUBA[0];
}

//发送消息管理
void CANNet_Manage1ms(void)
{
     /*
        //SendMsg_BCC_Code  异或校验
        SendMsg_BCC_Code=0;
        for (Bcc_Index=0;Bcc_Index<7;Bcc_Index++) 
        {
        	  SendMsg_BCC_Code^=infoMsgS.datMsgUBA[Bcc_Index]	;	
        } 
        
        infoMsgS.datMsgUBA[7]=SendMsg_BCC_Code;      
     */
    uint8 SendMsg_BCC_Code=0,Bcc_Index=0; //异或校验
    uint8 Bit8[8]={0};
    //  VehCo_stNewUB 为Running 或 Afterrun状态时才允许发CAN数据
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
                //远程控制器 EPS命令  20ms
                /*
                uint8  EPS_Contral_Mode=0;//EPS控制模式
                sint16 EPS_Demand_Angle=0;//EPS角度命令 0.1度步进   -1080.0~1080.0°
                uint8  EPS_Angle_MaxSpeed=0;//EPS 最大角速度
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
                //远程控制器 车速命令  20ms
                /*
                sint8 Vehco_Acceleration_Command=0;   //加速度指令 -12.8~12.7
                uint8 EPB_Command=0;    //电子手刹指令 0 无动作 1 拉起 2 释放 3 行进中制动
                uint8 Dang_Command=0;   //档位指令 0 N 1 D 2 R 3 保留
                uint8 Emergency_Braking=0;  //紧急制动指令                
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
                //远程车辆控制命令  50ms
                /*
                uint8 DriverMode_Command=0; //自驾模式状态 0 手动 1 保留 2 保留 3 远程
                uint8 Door_Command=0;       //车门控制命令 0 无动作 1 开门 2 关门 3 保留
                uint8 Beep_Command=0;       //喇叭控制     0 无动作 1 开启 2 关闭 3 保留
                uint8 R_DriverSystem_State=0;       //远程驾驶系统状态 0 连接中 1 连接完成 2 断开中 3 断开
                uint8 R_DriverSystem_Level=0;       //远程驾驶系统可信级别 0 不可信，立刻接管 1 低质量 ，人工监视 2 最佳状态，完全控制
                uint8 R_DriverSystem_Fault_Level=0; //远程驾驶系统故障级别 0 无故障 1 轻微 提示 2 一般故障 跛行 3 严重故障 退出远程驾驶状态
                uint8 AC_Mode;          //车辆冷暖空调控制 0 关机 1 制冷 2 制热 3 自动
                uint8 AC_Temperature;   //车辆冷暖空调设置温度 0.5 偏移16
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
                //线控制动报文
                //uint8 R_Break_Mode=0;   //远程制动激活
                //uint8 R_Break_Rang=0;     //远程制动力请求 0~100%                
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
        
        CANNet_idxSchedUB=(uint8)((CANNet_idxSchedUB+1)%100); //100ms一个完整循环   0~99
        

    }
  
}


