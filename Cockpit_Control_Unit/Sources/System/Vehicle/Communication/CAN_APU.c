/*******************************************************************************
* 工程名称：増程器CAN通讯程序
* 文件名： CAN_APU.c
* 功能描述 ： 
* 引用记录：
* 备注： 
* 修订记录：
*		日期		      版本		  作者		描述
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


  //増程器工作指令,暖风开启请求,机械空调开启请求,
uint8  APU_CMD,APU_Warm_CMD,Mechanical_AC_CMD,APU_Order=0;
  //増程器目标发电功率命令
uint16 APU_Demand_Power; 
  //目标工作转速  
uint16 APU_Demand_Speed;


  //APU初始化状态,OBD指示灯,机械空调状态,増程器启动标志位,发动机机油压力报警,増程器放电完成标志位,増程器当前状态,増程器故障等级      
uint8  APU_Staus,APU_OBD_LED,Mechanical_AC,APU_Start_Staus,Engine_Oil_Pressure_Alarm,APU_Discharged_Flag,APU_Mode,APU_Fault_Status;
  //増程器转速,増程器输出转矩
uint16 APU_Speed,APU_Torque;
sint16 APU_Power;//増程器输出功率
  //増程器母线电压，増程器当前转速最大功率
uint16 APU_DC_Voltage,APU_Power_Limit;
  //増程器母线电流，増程器交流电流
sint16 APU_DC_Current,APU_AC_Current;

 //    发动机进气压力,        发动机油箱液位,  发动机瞬时燃油消耗率, 节气门位置开度实际值
uint16 Engine_Inlet_Pressure, Engine_Oil_Level,Fuel_Consumption_Rate,Engine_Throttle_Rate;
 //    发动机进气温度,          发动机冷却液温度,          増程器温度,     増程器控制器GCU温度 
sint16 Engine_Inlet_Temperature,Engine_Coolant_Temperature,APU_Temperature,GCU_Temperature;
 //   发动机故障字              増程器故障字
uint8 Engine_Fault_Word[8]={0}, APU_Fault_Word[8]={0};
 //   APU生命信号
uint8 APU_Life;
extern uint16 Max_B_Voltage_Limit;//増程器最高允许输出电压,最大、最小PACK电压极限 //0~819.0V   0~818  单位： V， 偏移： 0 ，比例尺： 0.2      （10倍放大计算，0～8190 4095*2=8190)
extern uint16 VCU_Life;
#ifndef AECS_HW
      #pragma CODE_SEG DEFAULT
#else  
      #pragma CODE_SEG VCU_CODE
      
#endif

//増程器接收消息处理函数
void CANNet_RecvMsgCAN_APU(MSCANDrv_infoMSGS_TYPE infoMsgS)
{    


    CANNet_infoID_APU=infoMsgS.infoMsgIDUI;


    
    switch(CANNet_infoID_APU) 
    {   //APU_Enable
        case 0x1860A0B0: //APU_ Status 0    50ms
            memcpy(CanRecAPU,infoMsgS.datMsgUBA,sizeof(CanRecAPU));    
            //APU初始化状态,OBD指示灯,机械空调状态,増程器启动标志位,发动机机油压力报警,増程器放电完成标志位,増程器当前状态,増程器故障等级      
            //APU_Staus,APU_OBD_LED,Mechanical_AC,APU_Start_Staus,Engine_Oil_Pressure_Alarm,APU_Discharged_Flag,APU_Mode,APU_Fault_Status;
            APU_Staus       = 0x01 & CanRecAPU[0];      //APU初始化状态 0-初始化未完成；1-初始化已完成
            APU_OBD_LED     =(0x02 & CanRecAPU[0])>>1;
            Mechanical_AC   =(0x04 & CanRecAPU[0])>>2;
            APU_Start_Staus =(0x18 & CanRecAPU[0])>>3; //APU启动成功标志位 0-未启动；1-启动成功；2-启动失败；3-Void；
            
            Engine_Oil_Pressure_Alarm = 0x01 & CanRecAPU[1]; 
            APU_Discharged_Flag       =(0x80 & CanRecAPU[1])>>7;
            
            APU_Mode  = 0x07 & CanRecAPU[2];     //Bit0-Bit2 増程器当前状态 0-停机状态；1-起动状态；2-拖动状态；3-怠速状态；4-发电状态；5-主动放电 ;
            APU_Fault_Status = (0xE0 & CanRecAPU[3])>>5; //Bit5-Bit7：増程器故障等级 0-正常；1-二级故障；2-三级故障；3-四级故障;
            break;
        case 0x1861A0B0: //APU_ Status 1    50ms
            memcpy(CanRecAPU,infoMsgS.datMsgUBA,sizeof(CanRecAPU));    
            //増程器转速,増程器输出功率,増程器输出转矩
            //APU_Speed,APU_Power,APU_Torque;            
            APU_Speed = (CanRecAPU[0]+CanRecAPU[1]*256)/4; //0~16383rpm
            //APU_Power = (CanRecAPU[2]+CanRecAPU[3]*256)/5-6553 ;//   -655.3~655.3 KW 10倍
            APU_Power = (sint16)  (((sint32)CanRecAPU[2]+(sint32)CanRecAPU[3]*256)/5-6553) ;//   -655.3~655.3 KW 10倍
            APU_Torque= (CanRecAPU[4]+CanRecAPU[5]*256)/5; //0~1310.0Nm              10倍
                                                                                 

            break;
        case 0x1862A0B0: //APU_ Status 2    50ms
            memcpy(CanRecAPU,infoMsgS.datMsgUBA,sizeof(CanRecAPU));    
            //増程器母线电压，増程器当前转速最大功率
            //APU_DC_Voltage,APU_Power_Limit;
            //増程器母线电流，増程器交流电流
            //APU_DC_Current,APU_AC_Current;            
            APU_DC_Voltage= (CanRecAPU[0]+CanRecAPU[1]*256) /5;     //0~1310.7V       10倍
            //APU_DC_Current= (CanRecAPU[2]+CanRecAPU[3]*256) /5 -6553;//-655.3~655.3A   10倍
            //APU_AC_Current= (CanRecAPU[4]+CanRecAPU[5]*256) /5 -6553;//-655.3~655.3A   10倍
            APU_DC_Current=(sint16)   (((sint32)CanRecAPU[2]+(sint32)CanRecAPU[3]*256) /5 -6553);//-655.3~655.3A   10倍
            APU_AC_Current=(sint16)   (((sint32)CanRecAPU[4]+(sint32)CanRecAPU[5]*256) /5 -6553);//-655.3~655.3A   10倍
            
            
            APU_Power_Limit=(CanRecAPU[6]+CanRecAPU[7]*256) /5;     //0~1310.7KW      10倍
            break;
        case 0x1863A0B0: //APU_ Status 3    50ms
            memcpy(CanRecAPU,infoMsgS.datMsgUBA,sizeof(CanRecAPU));    
            //    发动机进气压力,        发动机油箱液位,  发动机瞬时燃油消耗率, 节气门位置开度实际值
            //    Engine_Inlet_Pressure, Engine_Oil_Level,Fuel_Consumption_Rate,Engine_Throttle_Rate;
            //    发动机进气温度,          发动机冷却液温度,          増程器温度,     増程器控制器GCU温度 
            //    Engine_Inlet_Temperature,Engine_Coolant_Temperature,APU_Temperature,GCU_Temperature;            
            Engine_Inlet_Pressure     = CanRecAPU[0]*15 ;  //发动机进气压力  0~381.0 kPa   10倍
            Engine_Inlet_Temperature  = CanRecAPU[1]-48 ;  //发动机进气温度  -48~206℃
            Engine_Oil_Level          = CanRecAPU[2]*4  ;  //发动机油箱液位  0~100.0%      10倍
            Engine_Coolant_Temperature= CanRecAPU[3]-48 ;  //发动机冷却液温度-48~206℃
            APU_Temperature           = CanRecAPU[4]-48 ;  //増程器温度      -48~206℃
            GCU_Temperature           = CanRecAPU[5]-48 ;  //増程器GCU温度   -48~206℃
            Fuel_Consumption_Rate     = (uint16)((uint32)CanRecAPU[6]*1532/10);//发动机瞬时燃油消耗率  0~38.912 mL/s   1000倍
            Engine_Throttle_Rate      = (uint16)((uint32)CanRecAPU[7]*392/10); //节气门位置开度实际值  0~100.00%       100倍
            break;
        case 0x1C64A0B0: //APU_ Fault 1    1000ms
            //   发动机故障字   8byte           
            memcpy(Engine_Fault_Word,infoMsgS.datMsgUBA,8);
            break;
        case 0x1C65A0B0: //APU_ Fault 2    1000ms
            //   増程器故障字   3byte
            memcpy(APU_Fault_Word,infoMsgS.datMsgUBA,3);
            break;
        case 0x1866A0B0: //APU_ Life 0    100ms
            memcpy(CanRecAPU,infoMsgS.datMsgUBA,sizeof(CanRecAPU));    
            APU_Life=CanRecAPU[0];    //APU生命信号
            break;
        default:
            break;
    }
}


//増程器发送消息处理函数 1ms
void CANNet_SendMsgCAN_APU(void)
{
    MSCANDrv_infoMSGS_TYPE infoMsgS;
    uint8 APU_Order_Bit[8]={0};
    switch(CANNet_SendMsgCAN_APU_idxSchedUB)
    {
         case 1:
         case 51:
            //CAN0  VCU_Cmd 50ms  主控制帧   
            
            //増程器工作指令 0-停机指令；1-工作(启动)指令；2-发电指令；3-主动放电指令;
            //APU_CMD
            //暖风开启请求, 机械空调开启请求
            //APU_Warm_CMD,Mechanical_AC_CMD,
            
            //目标工作转速  30rmp步进            
            //APU_Demand_Speed;  0~7650rpm
            //増程器目标发电功率命令  100.0~0.1KW
            //APU_Demand_Power; 
            //増程器最高允许输出电压
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
            infoMsgS.datMsgUBA[0]=0;//保留
            infoMsgS.datMsgUBA[1]=APU_Order ;//&0x03)   |  ((APU_Warm_CMD&0x01)<<2)   |  ((Mechanical_AC_CMD&0x01)<<3);
            infoMsgS.datMsgUBA[2]=(uint8)((APU_Demand_Power*5)&0x00ff) ;//1310.7~0.0KW   10倍
            infoMsgS.datMsgUBA[3]=(uint8)((APU_Demand_Power*5)>>8);
            infoMsgS.datMsgUBA[4]=(uint8)(Max_B_Voltage_Limit&0x00ff); //0~819.0V 10倍
            infoMsgS.datMsgUBA[5]=(uint8)(Max_B_Voltage_Limit>>8);
            if (APU_Demand_Speed>7650)
            {
                infoMsgS.datMsgUBA[6]=0;
            }else
            {
                infoMsgS.datMsgUBA[6]=(uint8)(APU_Demand_Speed/30);
            }
            infoMsgS.datMsgUBA[7]=0;//保留
            infoMsgS.numLenUB=8;
            infoMsgS.idxMsgPriorityUB=0x01;
            MSCANDrv_SendMsgCAN_B(infoMsgS);
            break;
        case 10:
        case 60:
            //VCU心跳
            infoMsgS.infoMsgIDUI=0x08F000A0;
            infoMsgS.bExtIDUB=1;
            infoMsgS.datMsgUBA[0]=0;
            infoMsgS.datMsgUBA[1]=0;
            infoMsgS.datMsgUBA[2]=0;
            infoMsgS.datMsgUBA[3]=0;
            infoMsgS.datMsgUBA[4]=0;
            infoMsgS.datMsgUBA[5]=0;
            infoMsgS.datMsgUBA[6]=0;
            infoMsgS.datMsgUBA[7]=(uint8)(VCU_Life_APU);       //VCU心跳 0～255
            infoMsgS.numLenUB=8;
            infoMsgS.idxMsgPriorityUB=0x02;
            MSCANDrv_SendMsgCAN_B(infoMsgS);
            VCU_Life_APU=(uint8)((VCU_Life_APU+1)%256); //0～255  
            break;
        default:
            break;
    }
    CANNet_SendMsgCAN_APU_idxSchedUB=(uint8)((CANNet_SendMsgCAN_APU_idxSchedUB+1)%100); //100ms一个完整循环
}





