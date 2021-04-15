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
#include "Custom_Func.h"
//#include <string.h>
//#include "stdlib.h"
//#include <stdio.h>
//#include <math.h>
#include "Vin_Rw.h"
#include "CAN_ESK.h"
#include "CAN_APU.h"

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

uint8 INS_Life; //仪表生命信号 每周期+1        BYTE1
uint32 INS_TD; //总行驶里程，2KM/bit 0~131072KM     BYTE3-4
uint8 Hand_Brake, Low_12V, Brake_Signa, Gas_Cap, ABRS_Signal, Central_Lock, Emer_Signal, Safe_Belt;
         //手刹、12V电压低、制动尾灯亮、油箱门开、安全气囊启动、中控锁开、   双闪开、    安全带插入BYTE2 bit 1~8
uint8 Brake_Error_INS,Car_Door;//制动器故障、车门开 
uint8 INS_Err,INS_Flash_Err,INS_Bat_Low;
      //仪表故障、FALSH故障、时钟电池电量低 BYTE8 bit1~3

uint8 B_State,B_Mode,B_Fault_Status,Balancing_Status,B_Fault_Lockout_Status;
      //电池状态、电池模式、故障状态、均衡允许状态、故障关闭状态
uint8 SOC,SOH;
      //物理有效值0~125 单位： %， 偏移： 0 ，比例尺： 0.5   内存中为0~255
uint16 Pack_Res;
      //电池内阻 物理有效值0~65.535 单位： Ohm， 偏移： 0 ，比例尺： 0.001 内存中为0~65535 
uint16 Pack_Capacity;
      //电池电量 物理有效值0~125（409.5 新版12位旧版8位） 单位： Ahr， 偏移： 0 ，比例尺： 0.5（0.1新版）       
uint8 B_Message_Counter;//电池心跳 ,每100ms+1, 0~14, 15=保留
uint8 B_Version;//BMS通讯协议版本 
      
uint16 B_Voltage,Output_Voltage;            //电池电压、输出电压  0~800 单位： V， 偏移： 0 ，   比例尺： 0.2 （10倍放大计算，0～8190 4095*2=8190）
sint16 B_Current;                           //电池电流         -500~500 单位： A， 偏移： -500 ，比例尺： 0.02 (10倍放大计算-5000～5000）正电流表示对电池充电，负电流表示电池放电
uint16 B_Charge_Current,B_Discharge_Current;//电池充、放电电流   0～500 单位： A， 偏移：0 ，    比例尺： 0.02（放大100倍计算 0～50000）
sint16 B_Power;                             //电池充放功率     -325~325 单位： kW，偏移： -325， 比例尺： 0.01 (100倍放大计算）正功率表示对电池充电， 负功率表示电池放电    
uint8 Ambient_Temperature;                  //电池包内环境温度  -50~200 单位： ℃，偏移： -50，  比例尺： 1  
//uint16 B_Current_test;

uint16 Cell_Voltage_Sum;                        //单体电压总和 0~650 单位： V， 偏移： 0 ，比例尺： 0.01   (100倍放大计算）
uint16 Max_Cell_Voltage,Min_Cell_Voltage;       //最大 最小单体电压12bit  0~6.1440 单位： V， 偏移： 0 ，比例尺： 0.0015  (1000倍放大计算 4095*15=61,425）
uint8 Max_Cell_Voltage_Num,Min_Cell_Voltage_num;//最大 最小单体电压单体编号  0~250

uint16 Ave_Cell_Temp,Max_Cell_Temp,Min_Cell_Temp;//平均 最大 最小单体温度 -50~154 单位： ℃， 偏移： -50 ，比例尺： 0.2（10倍放大计算 -50.0~154.0 内存值500为0.0℃）
uint8 Max_Cell_Temp_Num,Min_Cell_Temp_Num;       //最大  最小单体温度对应单体编号 0~250


uint16 Max_B_Voltage_Limit,Min_B_Voltage_Limit;       //最大、最小PACK电压极限   0~818  单位： V， 偏移： 0 ，比例尺： 0.2      （10倍放大计算，0～8190 4095*2=8190）
uint16 Max_Cell_Voltage_Limit,Min_Cell_Voltage_Limit;  //最大、最小单体电压极限   0~6   单位： V， 偏移： 0 ，比例尺： 0.0015    (1000倍放大计算 4095*15=61,425）
uint16 Max_Discharge_Current_Limit,Max_Charge_Current_Limit;//最大充放电电流极限  0～500 单位： A， 偏移： 0 ，比例尺： 2       （放大100倍计算 0～50000）

uint8 Max_SOC_Limit,Min_SOC_Limit;                     //最大、最小SOC限制      0~125 单位： %，  偏移： 0 ，比例尺： 0.5（2倍放大计算  内存值0～255）
uint16 Max_Discharge_Power_Limit,Max_Charge_Power_Limit;//最大10s充放电功率限制  0~250 单位： kW， 偏移： 0 ，比例尺： 1
uint16 Max_Cell_Temp_Limit,Min_Cell_Temp_Limit;        //最高、最低单体温度限制 -50~200 单位： ℃， 偏移： -50 ，比例尺： 1 （10倍放大计算  -50.0～200.0 内存值500为0.0℃）

sint16 Motor_Speed,Motor_Torque;//电机转速、电机扭矩，正转速为汽车前进方向，正扭矩为汽车前进方向，-15000～15000rpm, -5000～5000Nm, 偏移： 0 ，比例尺： 1
uint16 Motor_AC_Current;   //交流侧电流 0～1000 单位：A 偏移：0  比例尺：0.1 （10倍放大计算，0～1000.0A）
uint8 Precharge_Allow;    //允许预充标志位  0-不可预充   1-允许预充
uint8 FastDischarge_Fbd;  //快速放电状态位  0-无放电操作 1-放电中
uint8 IGBT_Enable_Fbd;    //IGBT工作状态位  0-禁用中 1-使能中
uint8 MCU_System_State;   //MCU状态反馈     0-初始化 1-上弱电正常 2-预留 3-允许电机运行 4-转速控制 5-转矩控制 6-下强电（快速放电）7-下弱电 8-故障
uint8 MCU_Life;           //MCU心跳  0~15   每周期+1

uint8 Motor_Temperature,MCU_Temperature; //电机温度、MCU温度  -40～210C  偏移 -40 比例尺 1
sint16 Torque_High_Limit,Torque_Low_Limit; //电机转矩上下限 比例尺 1  0～5000  -5000～0 
uint8 MCU_Fault_Level; //故障级别  0:无故障 1:警告报警(电机需要降功率处理)2:故障(零转矩输出)3:比较严重故障(需要关断电机)4:严重故障(需要停车处理)
uint8 MCU_Life2;   //MCU心跳  0~15   每周期+1


uint8  DTC_Code_1,DTC_Code_2; //DTC CODE
sint16 DC_Current;//直流电流（估计值） ， 电机驱动状态为负， 发电状态为正 误差较大 -1000～1000A 偏移 -1000 比例尺 1 
uint16 DC_Voltage;//直流电压  0～1000.0V  偏移0 比例尺0.1
uint8  Motor_Rotation_Count;  //电机转数循环计数 0~255
uint8  MCU_Life3;   //MCU心跳  0~15   每周期+1


uint8 Isolation_Life,Isolation_Status,Isolation_Level; 
//绝缘检测仪心跳 0～255，工作状态 0正常1不正常，故障等级 0 正常 大于500欧姆/V 1 一级故障 100～500欧姆/V 2 二级故障 小于100欧姆/V 
uint16 Isolation_Pos;//正极绝缘 单位K  偏移0  比例尺1
uint16 Isolation_Neg;//负极绝缘 单位K  偏移0  比例尺1
uint16 Isolation;//总绝缘 取正负中较小一项

//上位机命令帧HC_CMD
// 命令、用户级别、设备地址、操作设备数量、数据帧ID、帧数据、时间戳、收到新报文
uint8  HC_CMD=0,User_Level,Device_Id=0,Device_Num=0,HC_Data_Id=0,HC_Data[316]=0,Time_Mark=0,GetNewMessage_HC_CMD=0;
uint8  Need_HC_Data=0;//需要接收的数据帧数
uint8  Rw_Times_Reset=0;//可刷写次数复位
//以下为CAN发送使用变量

uint8 INS_Leds_1,INS_Leds_2;//仪表灯状态                     以下2016.5.4 未完成
//仪表LEDS1 8个bit  BEEP、SOC或温度低、单体电压低、绝缘低、充电连接指示、充电状态指示
extern uint8 Beep,Batter_Err,Soc_or_Tmp_Low,Low_Cell_Voltage,Low_Isolation,Charge_Connect,Charge_State;
//仪表LEDS2 8个bit 真空罐故障、限速模式、定速巡航模式、电机过热、回馈模式、系统故障、Ready灯
extern uint8 Err_Vacuum_Tank,Limit_Mode,Constant_Mode,MCU_OverHeat,FeedBack_Mode,System_Fault,Ready,Ready_Led;   



uint8 Veh_Limit_Speed;//巡航、限制车速  0.5km/h/bit 0~127.5    从V1.2版开始 1km/bit 0~180
uint8 Remaining_Mil;//剩余里程 0～255km      从V1.2版开始 2km/bit 0~510
uint8 AV_Ele;//平均电耗    0.5kwh/100KM  0~127.5
uint8 Ins_Ele;//瞬时电耗   0.5kwh/100KM  0~127.5

uint16 VCU_Life;//VCU心跳 

uint8 Vehicle_Mode,Key_Status,Charging_Driving_Mode,Vehicle_Speed,Pedal_State,Brake_State;
//整车功能模式、钥匙位置、驾驶&充电模式、车速、油门行程、制动行程

uint8 VCU_Diag_Code_1,VCU_Diag_Code_2,VCU_Diag_Code_3;//整车故障码123


uint8 VCU_BMS_CMD;//整车BMS控制命令字
//BMS主继电器控制请求 0、1;     BMS 3s断电延时0、1;    BMS故障保护许可 0、1
uint8 BMS_Contractor_Request,BMS_Power_Down_Request,BMS_Fault_Lockout_Request;

uint8 VCU_MCU_CMD;//整车对MCU控制命令字
//MCU使能指令0、1;MCU复位指令0、1;MCU控制模式1、2、3
uint8 MCU_Enable,MCU_Fault_Reset,MCU_Control_Mode;
uint8 VCU_MCU_CMD_L4bit;


uint8 MCU_Live_Counter; //循环计数器 0~15
uint16 Demand_Limit_High_Positive,Demand_Limit_Low_Negative;//转矩转速指令限制下限12bit 0~4095 Nm / 4*(0~4095) rpm 网络上一律发正
sint16 Demand_Torque;//力矩给定值  -5000～5000   偏移  -5000 比例1 单位Nm  正为前进方向 速度模式时无效
sint16 Demand_Speed; //速度给定值  -15000～15000 偏移 -15000 比例1 单位Rpm 正为前进方向 转矩模式时无效
//uint16 Torque_DisC_Power_Limit,Torque_Cha_Power_Limit;//10秒放、充电限制转矩

uint8 LostMcuCommunication,LostBMSCommunication,LostINSCommunication; 

uint16 LostMcuCom_Index,LostBMSCom_Index,LostINSCom_Index;




uint8  ABS_Active=0;//ABS激活信号
//A9样车专用
uint8 BCD_Err_Code1=0,BCD_Err_Code2=0; //仪表故障码  A9样车专用
extern uint16 AccPed,Breakk;
extern  uint8 bDiSatus[24];
extern  uint16 uADSatus[18];
extern  uint16 uAD4Satus[4];
extern  uint8 bLso;
extern  uint8 bHso;
extern  uint8 Error_PowOn_Delay;


extern  uint8 Can_Send_Enable;
extern  uint8 Vehco_Type_Test;
extern  uint8 Dang_L4bit;//档位状态   1 5 0 10 PRDN
extern  uint8 Torque_Mode;




    //根据车型选择曲线标定值
extern  uint16 * __far AccPedCD_rUW_CUR_Addr;//油门开度
extern  uint16 * __far AccPedCD_rUW_CUR2_Addr;//油门开度2
extern  uint16 * __far Motor_Speed_Torque_Addr;//转速_最大转矩
extern  uint16 * __far AccPed_Rip_Addr;//油门曲线
extern  uint16 * __far Speed_Vmh10ms_Crawl_D_Addr;//D档蠕动 转速-AD曲线
extern  uint16 * __far Torque_Increment_Crawl_D_Addr; //D档蠕动 AD修正转矩给定每周期增量
extern  uint16 * __far Speed_Max_Torque_Crawl_D_Addr; //D档蠕行模式最大力矩
extern  uint16 * __far Speed_Vmh10ms_General_Addr;//通用相对转速-AD曲线 
extern  uint16 * __far Torque_Increment_General_Addr;//通用AD修正转矩给定每周期增量
extern  uint16 * __far Speed_Vmh10ms_D_Park_Addr;//D档驻坡 转速-AD曲线
extern  uint16 * __far Torque_Increment_D_Park_Addr;//D档驻坡AD修正转矩给定每周期增量
extern  uint16 * __far Speed_Vmh10ms_Break_Addr;//制动回馈速度-AD曲线
extern  uint16 * __far Torque_Increment_Break_Addr;//制动回馈AD修正转矩给定每周期增量
extern  uint16 * __far Motor_Speed_Torque_Break_Addr;//电机最大制动力矩 40kw 114.4
extern  uint16 * __far FeedBack_Coast_D_Torque_Addr;//D档滑行回馈 速度-力矩  定点
extern  uint16 * __far FeedBack_Break_D_Torque_Addr;//D档制动回馈 速度-力矩 定点
extern  uint16 * __far Speed_Efficieny_Addr;//电机速度-转换效率曲线
     //根据车型选择单值标定值
extern  uint8 Motor_Vehicle_Speed_Scale_Get;//转速-车速比例
extern  uint16 Demand_Limit_High_Get;//车速上限
extern  uint16 Demand_Limit_Low_Get;//车速下限
extern  uint16 Crawl_D_Speed_Max_Get;//从其他状态进入蠕动的速度  RPM
extern  uint8 Crawl_D_Torque_Delay_Max_Get;//蠕动更新间隔  单位10ms
extern  uint8 Speed_Buffer_Number_Get; //10ms车辆速度采样缓冲次数
extern  uint8 Speed_Average_Number_Get; //10ms平均车速缓冲次数
extern  uint8 Torque_Buffer_Number_AutoPark_Get;  //10ms车辆平均转矩采样缓冲次数 驻坡


    #ifndef AECS_HW
      #pragma CODE_SEG DEFAULT
    #else  
      #pragma CODE_SEG SEG_VDL_CAN
    #endif


//CAN0接收消息处理函数
void CANNet_RecvMsgCAN_A(MSCANDrv_infoMSGS_TYPE infoMsgS)
{
   // uint8 Index;
    //CAN接收样例
    CANNet_infoIDCAN0UI=infoMsgS.infoMsgIDUI;
    CANNet_datMsgCAN0UB=infoMsgS.datMsgUBA[0];
    //CAN0接收分类处理
    
    switch(CANNet_infoIDCAN0UI) 
    {

            
            
      case 0xC20A0A6: //INS Status 50ms  仪表状态帧
            memcpy(CanRec0,infoMsgS.datMsgUBA,sizeof(CanRec0));
            
            
            //LostINSCom_Index
            if (((CanRec0[0]==0) && (INS_Life==255))|| (CanRec0[0]==INS_Life+1) ) //心跳正常
            {
                LostINSCom_Index=0;
            }
            
            
            
            INS_Life=CanRec0[0];
            INS_TD=(CanRec0[2] + ((uint32)CanRec0[3])*256 + ((uint32)CanRec0[4])*65536 )    *2;
            
          //uint8 Hand_Brake, Low_12V, Brake_Signa, Gas_Cap, ABRS_Signal, Central_Lock, Emer_Signal, Safe_Belt;
          //手刹、12V电压低、制动尾灯亮、油箱门开、安全气囊启动、中控锁开、   双闪开、    安全带插入BYTE2 bit 1~8
          //uint8 Brake_Error_INS,Car_Door;//制动器故障、车门开
          
            Hand_Brake=1&CanRec0[1];
            Low_12V= (2&CanRec0[1])>>1;
            //Brake_Signa=(4&CanRec0[1])>>2;   //新版不用
            //Gas_Cap=(8&CanRec0[1])>>3;       //新版不用

            Brake_Error_INS=(4&CanRec0[1])>>2; 
            Car_Door=(8&CanRec0[1])>>3;
            
            ABRS_Signal=(16&CanRec0[1])>>4;
            Central_Lock=(32&CanRec0[1])>>5;
            Emer_Signal=(64&CanRec0[1])>>6;
            Safe_Belt=(128&CanRec0[1])>>7;
          //INS_Err,INS_Flash_Err,INS_Bat_Low;
          //仪表故障、FALSH故障、时钟电池电量低 BYTE8 bit1~3
            INS_Err=1&CanRec0[7];
            INS_Flash_Err= (2&CanRec0[7])>>1;
            INS_Bat_Low=(4&CanRec0[1])>>2;
            break;
      case 0x18ffa1f3: //BCU Status 20ms   BMS状态帧
            memcpy(CanRec0,infoMsgS.datMsgUBA,sizeof(CanRec0));
          //uint8 B_State,B_Mode,B_Fault_Status,Balancing_Status,B_Fault_Lockout_Status;
          //电池状态、电池模式、故障状态、均衡允许状态、故障关闭状态
            B_State=0x0F&CanRec0[0];
            B_Mode=CanRec0[0]>>4;
            B_Fault_Status=0x0F&CanRec0[1];
            Balancing_Status=(0x30&CanRec0[1])>>4;
            B_Fault_Lockout_Status=(0xC0&CanRec0[1])>>6;
            SOC=CanRec0[2];
            SOH=CanRec0[3];
            if (B_Version<=10) //旧版BMS协议 
            {
                Pack_Capacity=CanRec0[4];  
                Pack_Res=CanRec0[5]+((uint16)CanRec0[6])*256;
                //Isolation=25000;
                

            }
            else  //新版BMS协议
            {
                Pack_Res=CanRec0[4]+((uint16)CanRec0[5])*256;
                Pack_Capacity=CanRec0[6]+((uint16)(0x0F&CanRec0[7]))*256;
            }
            
            

            LostBMSCom_Index=0;
     
            
            B_Message_Counter= CanRec0[7]>>4;
            
            
            
            
            break;
      case 0x18ffa2f3: //BCU Pack Status 20ms  PACK状态帧
            memcpy(CanRec0,infoMsgS.datMsgUBA,sizeof(CanRec0));
            B_Voltage= (CanRec0[0]+((uint16)(0x0F&CanRec0[1]))*256)*2;    //0~819.0V  
            Output_Voltage=((CanRec0[1]>>4)+((uint16)CanRec0[2])*16)*2;   //0~819.0V

                        
            //B_Current=(CanRec0[3]+CanRec0[4]*256)/5-5000; //放大10倍计算 -500.0～500.0 A             
            B_Current= (sint16)  (((sint32)CanRec0[3]+(sint32)CanRec0[4]*256)     /5-5000); //放大10倍计算 -500.0～500.0 A             
            if ((B_Current<=-5000)||(B_Current>=5000)){B_Current=0;} //电流极值置零
            
            if (B_Current<=0) //放电电流 0～500.0A 
            {
                B_Discharge_Current=-1*B_Current;
                B_Charge_Current=0;
            } 
            else //充电电流 0～500.0A 
            {
                B_Charge_Current=B_Current;
                B_Discharge_Current=0;
            }

            //B_Power=(sint16)((CanRec0[5]+((uint16)CanRec0[6])*256)-32768);
            //B_Power=CanRec0[5]+CanRec0[6]*256;
            //B_Power=(CanRec0[5]+CanRec0[6]*256)-32500; //放大100倍计算  -325.00～325.00KW
           
            B_Power= (sint16)  (((sint32)CanRec0[5]+(sint32)CanRec0[6]*256)-32500); //放大100倍计算  -325.00～325.00KW            
              
            if ((B_Power<=-32500)||(B_Power>=32500)){B_Power=0;} //放电功率极值置零
            Ambient_Temperature=CanRec0[7];
            break;
      case 0x18ffa4f3: //单体电压极限状态 Cell Voltage Status 100ms
            memcpy(CanRec0,infoMsgS.datMsgUBA,sizeof(CanRec0));
            Cell_Voltage_Sum=CanRec0[0]+((uint16)CanRec0[1])*256;             //0~655.35V
            Max_Cell_Voltage=(CanRec0[2]+((uint16)(0x0f&CanRec0[3]))*256)*15; //0~6.1425V
            Min_Cell_Voltage=((CanRec0[3]>>4)+((uint16)CanRec0[4])*16)*15;
            Max_Cell_Voltage_Num=CanRec0[5];
            Min_Cell_Voltage_num=CanRec0[6];
            break;
      case 0x18ffa5f3: //单体温度极限状态 Cell Temperature Status 100ms
            memcpy(CanRec0,infoMsgS.datMsgUBA,sizeof(CanRec0));
            Ave_Cell_Temp=(CanRec0[0]+((uint16)CanRec0[1])*256)*2;       //-50.0~154.0℃ 
            Max_Cell_Temp=(CanRec0[2]+((uint16)0x0f&CanRec0[3])*256)*2;  //-50.0~154.0℃ 
            Min_Cell_Temp=((CanRec0[3]>>4)+((uint16)CanRec0[4])*16)*2;   //-50.0~154.0℃ 
            Max_Cell_Temp_Num=CanRec0[5];
            Max_Cell_Temp_Num=CanRec0[6];
            break;
      case 0x18ffa6f3: //BMS 极限状态 1 BMS Limits Status 1  100ms
            memcpy(CanRec0,infoMsgS.datMsgUBA,sizeof(CanRec0));
            Max_B_Voltage_Limit=(CanRec0[0]+(uint16)(0x0f&CanRec0[1])*256)*2;     //0~819.0V
            Min_B_Voltage_Limit=((CanRec0[1]>>4)+((uint16)CanRec0[2])*16)*2;      //0~819.0V
            Max_Cell_Voltage_Limit=(CanRec0[3]+(uint16)(0x0f&CanRec0[4])*256)*15; //0~6.1425V
            Min_Cell_Voltage_Limit=((CanRec0[4]>>4)+((uint16)CanRec0[5])*16)*15;  //0~6.1425V
            Max_Discharge_Current_Limit=((uint16)CanRec0[6])*200;                 //0~500.00A  持续放电电流限制
            Max_Charge_Current_Limit=((uint16)CanRec0[7])*200;                    //0~500.00A  持续充电电流限制 
            break;
      case 0x18ffa7f3: //BMS 极限状态 2 BMS Limits Status 2  100ms
            memcpy(CanRec0,infoMsgS.datMsgUBA,sizeof(CanRec0));
            Max_SOC_Limit=CanRec0[0];                       
            Min_SOC_Limit=CanRec0[1];
            Max_Discharge_Power_Limit=CanRec0[2]*100;       //0~250.00KW  放电10秒限制功率
            Max_Charge_Power_Limit=CanRec0[3]*100;          //0~250.00KW  充电/回收10秒限制功率
            Max_Cell_Temp_Limit=((uint16)CanRec0[4])*10;//-50.0~154.0℃ 
            Min_Cell_Temp_Limit=((uint16)CanRec0[5])*10;//-50.0~154.0℃ 
            
            
            break;
      case 0x18FFA8F3: //电池组绝缘电阻 Battery Isolation Resistance   250ms  1.2新增
            memcpy(CanRec0,infoMsgS.datMsgUBA,sizeof(CanRec0));
            Isolation_Life=CanRec0[0];
            Isolation_Status=CanRec0[1];
            Isolation_Level=CanRec0[2];
            Isolation_Pos=CanRec0[4]+CanRec0[5]*256;
            Isolation_Neg=CanRec0[6]+CanRec0[7]*256;
            if (Isolation_Pos >  Isolation_Neg) 
            {
                  Isolation=Isolation_Neg;
            
            } 
            else 
            {
                  Isolation=Isolation_Pos;
            }
      
            if (Isolation>25000) {Isolation=25000;}
      
            break;
      case 0x18FFA9F3: //单体极柱温度极限 1 Cell Post Temperature Limits 1  1000ms  1.2新增
      
            break;
      case 0x18FFAAF3: //单体极柱温度极限 2 Cell Post Temperature Limits 2  1000ms  1.2新增
      
            break;
      case 0x18FFABF3: //单体极柱温度极限 3 Cell Post Temperature Limits 3  1000ms  1.2新增
      
            break;
      case 0x18FFACF3: //电池基本信息 Battery Information  1.2新增 
            memcpy(CanRec0,infoMsgS.datMsgUBA,sizeof(CanRec0));
            B_Version=CanRec0[7];
      
            break;
      case 0x18fecaf3: //BCU 诊断报文 BCU DM1  100ms
       
            break;
      case 0xCFF0008:  //ISG MCU STATUS 1   10ms
            memcpy(CanRec0,infoMsgS.datMsgUBA,sizeof(CanRec0));
            //Motor_Speed=(CanRec0[0]+CanRec0[1]*256)-15000;       //-15000~15000 rpm
            Motor_Speed=(sint16)  (((sint32)CanRec0[0]+(sint32)CanRec0[1]*256)  -15000);       //-15000~15000 rpm
                        
            if ((Motor_Speed<=-15000)||(Motor_Speed>=15000)) { Motor_Speed=0;} //电机转速极值置零
            //Motor_Torque=(CanRec0[2]+CanRec0[3]*256)-5000;      //-5000-5000 Nm
            Motor_Torque=(sint16) (((sint32)CanRec0[2]+(sint32)CanRec0[3]*256)-5000);      //-5000-5000 Nm
            
            if ((Motor_Torque<=-5000)||(Motor_Torque>=5000)) {Motor_Torque=0;}  //电机转矩极值置零
            Motor_AC_Current=CanRec0[4]+CanRec0[5]*256;   //0~1000.0A
            Precharge_Allow=(16&CanRec0[6])>>4;
            FastDischarge_Fbd=(32&CanRec0[6])>>5;
            IGBT_Enable_Fbd=(64&CanRec0[6])>>6;
            MCU_System_State=0x0f&CanRec0[7];
            
            if ((((0xf0&CanRec0[7])>>4==0) && (MCU_Life==15))|| ((0xf0&CanRec0[7])>>4==MCU_Life+1) ) //心跳正常
            {
                LostMcuCom_Index=0;
            }
            
            
            MCU_Life=(0xf0&CanRec0[7])>>4;
            
            
            
            break;
      case 0xCFF0108:  //ISG MCU STATUS 2   20ms
            memcpy(CanRec0,infoMsgS.datMsgUBA,sizeof(CanRec0));
            Motor_Temperature=CanRec0[0];                //-40～210℃ 
            MCU_Temperature=CanRec0[1];                  //-40～210℃ 
            Torque_High_Limit=CanRec0[2]+CanRec0[3]*256; // 0~5000Nm
            //Torque_Low_Limit=(CanRec0[4]+CanRec0[5]*256)-5000;  //-5000~0Nm
            Torque_Low_Limit=(sint16) (((sint32)CanRec0[4]+(sint32)CanRec0[5]*256)-5000);  //-5000~0Nm
            
            if (Torque_Low_Limit<=-5000){Torque_Low_Limit=0;}    //转矩负限制极值置零
            MCU_Fault_Level=0x0f&CanRec0[6];
            MCU_Life2=(0xf0&CanRec0[7])>>4;
            break;
      case 0xCFF0208:  //ISG MCU STATUS 3   10/20ms
            memcpy(CanRec0,infoMsgS.datMsgUBA,sizeof(CanRec0));      
            DTC_Code_1=CanRec0[0];
            DTC_Code_2=CanRec0[1];            
            //DC_Current=(CanRec0[2]+CanRec0[3]*256)-1000;   //-1000～1000A
            DC_Current=(sint16)  (((sint32)CanRec0[2]+(sint32)CanRec0[3]*256)-1000);   //-1000～1000A
            
            
            if ((DC_Current<=-1000)||(DC_Current>=1000)){DC_Current=0;} //MCU 母线电流极值置零
            DC_Voltage=CanRec0[4]+CanRec0[5]*256;   //0～1000.0V
            Motor_Rotation_Count=CanRec0[6];   //电机转数计数 
            MCU_Life3=(0xf0&CanRec0[7])>>4;
            break;
      case 0xCFF0308:  //ISG MCU STATUS 4   50ms
       
            break;
      case 0x18F0010B:
            //ABS_Active
            memcpy(CanRec0,infoMsgS.datMsgUBA,sizeof(CanRec0));      
            //ABS_Active=(0x20&CanRec0[0])>>5; //bit 5     10 0000
            ABS_Active=(0x30&CanRec0[0])>>4; //bit 5&6     11 0000
            break;
      case 0x0C64A001:  //HC_CMD 上位机命令帧 单次应答制
            
            memcpy(CanRec0,infoMsgS.datMsgUBA,sizeof(CanRec0)); 
            // 命令、用户级别、设备地址、操作设备数量、数据帧ID、帧数据、时间戳、收到新报文
            //uint8  HC_CMD,User_Level,Device_Id,Device_Num,HC_Data_Id,HC_Data[316],Time_Mark,GetNewMessage_HC_CMD;            
            GetNewMessage_HC_CMD=1;
            switch(CanRec0[0]) 
            {
                case 0xA0:  //注册帧
                    HC_CMD=     CanRec0[0] ;
                    User_Level= CanRec0[1] ;
                    Rw_Times_Reset=CanRec0[2];
                    break;
                case 0xA3:  //读取帧
                    HC_CMD=     CanRec0[0] ;
                    Device_Id=  CanRec0[1] ;
                    Device_Num= CanRec0[2] ;
                    Time_Mark=  CanRec0[3] ;
                    
                    break;
                case 0xA1:  //保持帧
                    HC_CMD=     CanRec0[0] ;
                    Time_Mark=  CanRec0[1] ;
                    break;
                case 0xA6:  //写入帧
                    HC_CMD=     CanRec0[0] ;
                    Device_Id=  CanRec0[1] ;
                    Device_Num= CanRec0[2] ;
                    Time_Mark=  CanRec0[3] ;               
                    
                    break;
                case 0xAF:  //注销帧
                    HC_CMD=     CanRec0[0] ;
                    break;
                default:    //数据帧  0x01~2D
                    if ((CanRec0[0]>0x00)&&(CanRec0[0]<0x2D) && (Need_HC_Data>0))
                    { //有效数据帧
                        HC_CMD=0xFF;
                        HC_Data_Id=CanRec0[0];
                        CANNet_Get_HC_Data (CanRec0);
                    }
                    
                    
                    break;
            }
            
            
            
            
            
            
            
            break;
      default:
            break;
    }
    //电子排档接收消息处理函数
    if (ESK_Signal_Enable==1) {  CANNet_RecvMsgCAN_ESK (infoMsgS);}
    
}

//CAN1接收消息处理函数1
void CANNet_RecvMsgCAN_B(MSCANDrv_infoMSGS_TYPE infoMsgS)
{
    //CAN接收样例
    CANNet_infoIDCAN1UI=infoMsgS.infoMsgIDUI;
    CANNet_datMsgCAN1UB=infoMsgS.datMsgUBA[0];
    
    //増程器接收消息处理函数
    if (APU_Enable==1) {CANNet_RecvMsgCAN_APU(infoMsgS) ;    }
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
    uint8 VCU_BMS_CMD_Bit[8]={0},VCU_MCU_CMD_Bit[8]={0},INS_Leds_2_Bit[8]={0},INS_Leds_1_Bit[8]={0};
    //  VehCo_stNewUB 为Running 或 Afterrun状态时才允许发CAN数据
    if ((Can_Send_Enable==1)&&((VehCo_stNewUB==_VEHCO_INDEX_STATUS_RUNNING)||(VehCo_stNewUB==_VEHCO_INDEX_STATUS_AFTERRUN)) )
    {
        
        
        MSCANDrv_infoMSGS_TYPE infoMsgS;
        switch(CANNet_idxSchedUB)
        {
             case 0:
             case 50:
                //CAN0  整车控制帧 VCU_C 50ms  主控制帧   ***完成
                infoMsgS.infoMsgIDUI=0x08F000A0;
                infoMsgS.bExtIDUB=1;
                infoMsgS.datMsgUBA[0]=VCU_C_1[0];
                infoMsgS.datMsgUBA[1]=VCU_C_1[1];
                infoMsgS.datMsgUBA[2]=VCU_C_1[2];
                infoMsgS.datMsgUBA[3]=VCU_C_1[3];
                infoMsgS.datMsgUBA[4]=VCU_C_1[4];

                //VCU硬件代码  01 苏州奥易克斯 VC04A   02 苏州易控  EC36003CU001
                #ifndef AECS_HW
                    infoMsgS.datMsgUBA[5]=2;
                #else  
                    infoMsgS.datMsgUBA[5]=1; 
                #endif


                infoMsgS.datMsgUBA[6]=VCU_C_1[6];
                infoMsgS.datMsgUBA[7]=(uint8)(VCU_Life);       //VCU心跳
                infoMsgS.numLenUB=8;
                infoMsgS.idxMsgPriorityUB=0x01;
                MSCANDrv_SendMsgCAN_A(infoMsgS);
                VCU_Life=(uint8)((VCU_Life+1)%256); //0～255 
                break;
                
             case 1:
             case 51:
                //CAN0  整车状态帧 VCU_S 50ms  车速， 车辆模式
                //Motor_Speed -15000~15000 sint16
                //Vehicle_Speed -50～205（150）km/h
                //Motor_Vehicle_Speed_Scale_Get  //转速-车速比例， 优优: 114.411  D11: 36.632  即百公里加速1秒时对应的10ms转速增量（加速度）

                
                
                Vehicle_Speed=( abs(Motor_Speed) /Motor_Vehicle_Speed_Scale_Get)+50;
                
                
                //AccPed,Breakk;
                Pedal_State=(uint8)(AccPed/100);
                Brake_State=(uint8)(Breakk/100);
                
                infoMsgS.infoMsgIDUI=0x08F200A0;
                infoMsgS.bExtIDUB=1;
                infoMsgS.datMsgUBA[0]=Vehicle_Mode;         //整车功能模式  0 1 7  **未做充电状态
                infoMsgS.datMsgUBA[1]=Key_Status;           //钥匙位置 档位
                infoMsgS.datMsgUBA[2]=Charging_Driving_Mode;//驾驶&充电模式   **未完成
                infoMsgS.datMsgUBA[3]=0;                  
                infoMsgS.datMsgUBA[4]=0;                   
                infoMsgS.datMsgUBA[5]=Vehicle_Speed;        //车速       1km/h /bit -50～205（180）km/h
                if (infoMsgS.datMsgUBA[5]==0) {infoMsgS.datMsgUBA[5]=50;} //车速-50极值修正
                infoMsgS.datMsgUBA[6]=Pedal_State;          //油门行程   1%/bit  0~100%
                infoMsgS.datMsgUBA[7]=Brake_State;          //制动行程   1%/bit  0~100%
                infoMsgS.numLenUB=8;
                infoMsgS.idxMsgPriorityUB=0x02;
                MSCANDrv_SendMsgCAN_A(infoMsgS);
                
                break;



             case 2:
             case 52:
                //CAN0  整车故障帧 VCU_F 50ms    整车故障信息
                
                
                

                
                infoMsgS.infoMsgIDUI=0x041000A0;
                infoMsgS.bExtIDUB=1;
                if (Error_PowOn_Delay>=40)
                {
                  infoMsgS.datMsgUBA[0]=VCU_Diag_Code_1;   //整车故障码1
                  infoMsgS.datMsgUBA[1]=VCU_Diag_Code_2;   //整车故障码2
                  infoMsgS.datMsgUBA[2]=VCU_Diag_Code_3;   //整车故障码3
                }else
                {
                  infoMsgS.datMsgUBA[0]=0;   //整车故障码1
                  infoMsgS.datMsgUBA[1]=0;   //整车故障码2
                  infoMsgS.datMsgUBA[2]=0;   //整车故障码3
                }
                
                infoMsgS.datMsgUBA[3]=0;
                infoMsgS.datMsgUBA[4]=0;
                infoMsgS.datMsgUBA[5]=0;//Warning Code1
                infoMsgS.datMsgUBA[6]=0;//Warning Code2
                infoMsgS.datMsgUBA[7]=0;//Warning Code3
                infoMsgS.numLenUB=8;
                infoMsgS.idxMsgPriorityUB=0x03;
                
               
               
                MSCANDrv_SendMsgCAN_A(infoMsgS);
             
                break;    
               
             case 3:
             case 53:
                //CAN0  整车仪表控制帧1 VCUtoINS_C1 50ms   仪表控制的信息， 与电池相关    
                //仪表LEDS1 8个bit  BEEP、SOC或温度低、单体电压低、绝缘低、充电连接指示、充电状态指示
                //uint8 Beep,Batter_Err,Soc_or_Tmp_Low,Low_Cell_Voltage,Low_Isolation,Charge_Connect,Charge_State;
                INS_Leds_1_Bit[7]=Beep;
                INS_Leds_1_Bit[6]=Batter_Err;
                INS_Leds_1_Bit[5]=Soc_or_Tmp_Low;
                INS_Leds_1_Bit[4]=Low_Cell_Voltage;
                INS_Leds_1_Bit[3]=Low_Isolation;
                INS_Leds_1_Bit[2]=Charge_Connect;
                INS_Leds_1_Bit[1]=(Charge_State&0x2)>>1;
                INS_Leds_1_Bit[0]=Charge_State&0x1;            
                
                
                
                INS_Leds_1=SetBit8 (INS_Leds_1_Bit);
                infoMsgS.infoMsgIDUI=0x1810A6A0;
                infoMsgS.bExtIDUB=1;
                if (VehCo_stNewUB==_VEHCO_INDEX_STATUS_RUNNING) 
                {
                    infoMsgS.datMsgUBA[0]=(uint8)(SOC/2);   //SOC 1%/bit 
                } else
                {
                    infoMsgS.datMsgUBA[0]=0;
                }
                
                infoMsgS.datMsgUBA[1]=(uint8)((B_Current+4000)&0x00ff);// 电池放电电流 0.1A/bit  偏移-400A 范围-400.0~1200.0A
                infoMsgS.datMsgUBA[2]=(uint8)((B_Current+4000)>>8);    //高8
                infoMsgS.datMsgUBA[3]=(uint8)(B_Voltage&0x00ff);  //电池总电压 0.1V/bit 
                infoMsgS.datMsgUBA[4]=(uint8)(B_Voltage>>8);      //高8
                if (B_Version<=10) {Isolation=rand()%500+11000;}      //旧版BMS协议随机修正绝缘电阻值11.0～11.5Mohm
                
                infoMsgS.datMsgUBA[5]=(uint8)(Isolation/100);          //整车绝缘电阻 0～25.5M ohm
                infoMsgS.datMsgUBA[6]=(uint8)(Max_Cell_Temp/10);       //单体最高温度 -50～200 ℃
                if (infoMsgS.datMsgUBA[6]==0) {infoMsgS.datMsgUBA[6]=50;}//单体最高温度无数据-50度极值修正
                infoMsgS.datMsgUBA[7]=(uint8)(INS_Leds_1);             //仪表灯状态1
                infoMsgS.numLenUB=8;
                infoMsgS.idxMsgPriorityUB=0x04;
                MSCANDrv_SendMsgCAN_A(infoMsgS);
                
                break;    
               
             case 4:
             case 54:
                //CAN0  整车对仪表控制帧2 VCUtoINS_C2 50ms   仪表控制的信息， 与电池不相关的
                //INS_Leds_2_Bit
                //仪表LEDS2 8个bit 真空罐故障、限速模式、定速巡航模式、电机过热、回馈模式、系统故障、Ready灯
                //uint8 Err_Vacuum_Tank,Limit_Mode,Constant_Mode,MCU_OverHeat,FeedBack_Mode,System_Fault,Ready;  
                INS_Leds_2_Bit[7]=(Torque_Mode==4)||(Torque_Mode==8);
                INS_Leds_2_Bit[6]=Err_Vacuum_Tank;
                INS_Leds_2_Bit[5]=Limit_Mode;
                INS_Leds_2_Bit[4]=Constant_Mode;
                INS_Leds_2_Bit[3]=MCU_OverHeat;
                INS_Leds_2_Bit[2]=FeedBack_Mode;
                INS_Leds_2_Bit[1]=System_Fault;
                INS_Leds_2_Bit[0]=Ready_Led;           
                
                
                
                INS_Leds_2=SetBit8 (INS_Leds_2_Bit);
                infoMsgS.infoMsgIDUI=0x1811A6A0;
                infoMsgS.bExtIDUB=1;
                infoMsgS.datMsgUBA[0]=0;                  
                infoMsgS.datMsgUBA[1]=INS_Leds_2;          //仪表灯状态2
                infoMsgS.datMsgUBA[2]=Veh_Limit_Speed;     //巡航、限定车速 0～127.5km/h  0.5km/h/bit    从V1.2版开始 1km/bit 0~180
                infoMsgS.datMsgUBA[3]=Remaining_Mil;       //剩余里程 0～255km  从V1.2版开始 2km/bit 0~510                      **未完成
                infoMsgS.datMsgUBA[4]=AV_Ele;              //平均电耗    0.5kwh/100KM  0~127.5         **未完成
                infoMsgS.datMsgUBA[5]=Ins_Ele;             //瞬时电耗   0.5kwh/100KM  0~127.5          **未完成
                infoMsgS.datMsgUBA[6]=0;                 
                infoMsgS.datMsgUBA[7]=0;                  
                infoMsgS.numLenUB=8;
                infoMsgS.idxMsgPriorityUB=0x05;
                MSCANDrv_SendMsgCAN_A(infoMsgS);
                
                break;    

             case 5:
             case 25:
             case 45:
             case 65:
             case 85:
                //CAN0  整车对BMS控制帧 VCUtoBMS_C 20ms
                //VCU_BMS_CMD_Bit[8]
                //BMS_Contractor_Request,BMS_Power_Down_Request,BMS_Fault_Lockout_Request;
                
                VCU_BMS_CMD_Bit[0]=BMS_Contractor_Request;
                VCU_BMS_CMD_Bit[2]=BMS_Power_Down_Request;
                VCU_BMS_CMD_Bit[4]=BMS_Fault_Lockout_Request;
                
                VCU_BMS_CMD=SetBit8 (VCU_BMS_CMD_Bit);
               
                infoMsgS.infoMsgIDUI=0x18FFA0F3;
                infoMsgS.bExtIDUB=1;
                infoMsgS.datMsgUBA[0]=VCU_BMS_CMD;                     //整车对BMS控制命令字
                infoMsgS.datMsgUBA[1]=(uint8)(DC_Voltage&0x00ff);      //电机控制器母线电压 0～1000.0V
                infoMsgS.datMsgUBA[2]=(uint8)(DC_Voltage>>8);          //高8
                infoMsgS.datMsgUBA[3]=(uint8)(Motor_AC_Current&0x00ff);//电机交流侧电流 0～1000.0A
                infoMsgS.datMsgUBA[4]=(uint8)(Motor_AC_Current>>8);    //高8
                infoMsgS.datMsgUBA[5]=0;
                infoMsgS.datMsgUBA[6]=0;
                infoMsgS.datMsgUBA[7]=0;
                infoMsgS.numLenUB=8;
                infoMsgS.idxMsgPriorityUB=0x06;
                MSCANDrv_SendMsgCAN_A(infoMsgS);
                
                break;    

             case 6:
             case 16:
             case 26:
             case 36:
             case 46:
             case 56:
             case 66:
             case 76:
             case 86:
             case 96:
                //CAN0  整车对MCU控制帧 VCUtoMCU_C 10ms
                //MCU使能指令0、1;MCU复位指令0、1;MCU控制模式1、2、3
                //uint8 MCU_Enable,MCU_Fault_Reset,MCU_Control_Mode;
                //VCU_MCU_CMD_Bit[8]
                VCU_MCU_CMD_Bit[0]=MCU_Enable;
                VCU_MCU_CMD_Bit[1]=MCU_Fault_Reset;
                VCU_MCU_CMD_Bit[2]=MCU_Control_Mode&0x1;
                VCU_MCU_CMD_Bit[3]=MCU_Control_Mode&0x2;            
                
                VCU_MCU_CMD_Bit[4]=MCU_Live_Counter&0x1;
                VCU_MCU_CMD_Bit[5]=MCU_Live_Counter&0x2;
                VCU_MCU_CMD_Bit[6]=MCU_Live_Counter&0x4;
                VCU_MCU_CMD_Bit[7]=MCU_Live_Counter&0x8;
                
                
                VCU_MCU_CMD=SetBit8 (VCU_MCU_CMD_Bit);
                VCU_MCU_CMD_L4bit=VCU_MCU_CMD&0x0F;
                
                //VCU_MCU_CMD=(VCU_MCU_CMD&0x0F)+((MCU_Live_Counter&0x0F)<<4);
                Demand_Limit_High_Positive=Demand_Limit_High_Get;
                Demand_Limit_Low_Negative=Demand_Limit_Low_Get;
                
                infoMsgS.infoMsgIDUI=0xCFF08EF;
                infoMsgS.bExtIDUB=1;
                infoMsgS.datMsgUBA[0]=VCU_MCU_CMD; //整车对MCU控制命令字
                infoMsgS.datMsgUBA[1]=(uint8)(Demand_Limit_High_Positive&0x00FF);       //速度上限0~7位
                infoMsgS.datMsgUBA[2]=(uint8)((Demand_Limit_High_Positive&0x0F00)>>8)+((Demand_Limit_Low_Negative&0x000F)<<4);   //速度上限8~11位+速度下限0~3位
                infoMsgS.datMsgUBA[3]=(uint8)((Demand_Limit_Low_Negative&0x0FF0)>>4);   //速度下限4～11位
                infoMsgS.datMsgUBA[4]=(uint8)((Demand_Torque+5000)&0x00FF);                    //力矩给定值  -5000～5000
                infoMsgS.datMsgUBA[5]=(uint8)((Demand_Torque+5000)>>8);                        //高8位
                infoMsgS.datMsgUBA[6]=(uint8)((Demand_Speed+15000)&0x00FF);                     //速度给定值  -15000～15000
                infoMsgS.datMsgUBA[7]=(uint8)((Demand_Speed+15000)>>8);                         //高8位
                infoMsgS.numLenUB=8;
                infoMsgS.idxMsgPriorityUB=0x07;
                MSCANDrv_SendMsgCAN_A(infoMsgS);
                
                MCU_Live_Counter=(uint8)((MCU_Live_Counter+1)%16); //0～15 
                break;    
            
        //以下为A9样车专用CAN协议 125Kbps CAN1  
        case 7:
        case 57:    //MCS1    电机控制器状态1   50ms     0X268     Motor_Speed    MCU_Temperature  Motor_Temperature
            INS_Leds_2_Bit[7]=Ready_Led;
            INS_Leds_2_Bit[6]=0;
            INS_Leds_2_Bit[5]=0;
            INS_Leds_2_Bit[4]=0;
            INS_Leds_2_Bit[3]=0;
            INS_Leds_2_Bit[2]=0;
            INS_Leds_2_Bit[1]=0;
            INS_Leds_2_Bit[0]=0;           
            INS_Leds_2=SetBit8 (INS_Leds_2_Bit);

            infoMsgS.infoMsgIDUI=0X268;
            infoMsgS.bExtIDUB=0;
            //infoMsgS.datMsgUBA[0]=(uint8)( 85*(abs(Motor_Speed/Motor_Vehicle_Speed_Scale_Get)) &0x00FF);  //电机转速
            
            
            infoMsgS.datMsgUBA[0]=(((sint32)abs(Motor_Speed))  *85/Motor_Vehicle_Speed_Scale_Get) &0x00FF; //电机转速
                        
            //infoMsgS.datMsgUBA[1]=(uint8)( 85*(abs(Motor_Speed/Motor_Vehicle_Speed_Scale_Get))     >>8);
          
            infoMsgS.datMsgUBA[1]=((((sint32)abs(Motor_Speed))  *85/Motor_Vehicle_Speed_Scale_Get) >>8) &0x00FF; //电机转速  高8位
                     
            infoMsgS.datMsgUBA[2]=(((sint16)MCU_Temperature)-40)*10 &0x00FF ;
            infoMsgS.datMsgUBA[3]=((((sint16)MCU_Temperature)-40)*10 >>8) &0x00FF;
            infoMsgS.datMsgUBA[4]=(((sint16)Motor_Temperature)-40)*10 &0x00FF ;
            infoMsgS.datMsgUBA[5]=((((sint16)Motor_Temperature)-40)*10 >>8) &0x00FF;
            infoMsgS.datMsgUBA[6]=INS_Leds_2;
            infoMsgS.datMsgUBA[7]=(Breakk/100)&&1;
            infoMsgS.numLenUB=8;
            infoMsgS.idxMsgPriorityUB=0x01;
            if ((Vehco_Type_Test==4)||(Vehco_Type_Test==6)) {              MSCANDrv_SendMsgCAN_B(infoMsgS);            }
      
            break;
        case 8:
        case 58:    //MCS2    电机控制器状态2   50ms    0X368    B_Voltage   B_Current    Dang_L4bit
            infoMsgS.infoMsgIDUI=0X368;
            infoMsgS.bExtIDUB=0;
            infoMsgS.datMsgUBA[0]=B_Voltage&0x00FF;
            infoMsgS.datMsgUBA[1]=B_Voltage>>8;
            infoMsgS.datMsgUBA[2]=SOC/2;
            infoMsgS.datMsgUBA[3]=AccPed&0x00FF;
            infoMsgS.datMsgUBA[4]=AccPed>>8;
            infoMsgS.datMsgUBA[5]=B_Current&0x00FF;
            infoMsgS.datMsgUBA[6]=(B_Current>>8)&0x00FF;
            switch (Dang_L4bit) 
            {
                case 0:
                default:
                    infoMsgS.datMsgUBA[7]=0;
                    break;
                case 5:
                    infoMsgS.datMsgUBA[7]=2;
                    break;
                case 10:
                    infoMsgS.datMsgUBA[7]=1;
                    break;
                case 1:
                    infoMsgS.datMsgUBA[7]=3;
                    break;
            }
            
            infoMsgS.numLenUB=8;
            infoMsgS.idxMsgPriorityUB=0x02;
            if ((Vehco_Type_Test==4)||(Vehco_Type_Test==6)) {              MSCANDrv_SendMsgCAN_B(infoMsgS);            }        
            break;
        case 9:
        case 59:    //MCF     电机控制器故障BCD    50ms    0X439
            infoMsgS.infoMsgIDUI=0X439;
            infoMsgS.bExtIDUB=0;
            infoMsgS.datMsgUBA[0]=BCD_Err_Code1;
            infoMsgS.datMsgUBA[1]=BCD_Err_Code2;
            infoMsgS.datMsgUBA[2]=0;
            infoMsgS.datMsgUBA[3]=0;
            infoMsgS.datMsgUBA[4]=0;
            infoMsgS.datMsgUBA[5]=0;
            infoMsgS.datMsgUBA[6]=0;
            infoMsgS.datMsgUBA[7]=0;
            infoMsgS.numLenUB=8;
            infoMsgS.idxMsgPriorityUB=0x03;
            if ((Vehco_Type_Test==4)||(Vehco_Type_Test==6)) {              MSCANDrv_SendMsgCAN_B(infoMsgS);            }        
            break;

            default:
                break;
        }
        
        CANNet_idxSchedUB=(uint8)((CANNet_idxSchedUB+1)%100); //100ms一个完整循环
        
        //増程器发送消息处理函数 1ms
        if (APU_Enable==1) {      CANNet_SendMsgCAN_APU();     }
    }
  
}


