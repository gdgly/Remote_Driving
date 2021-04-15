/*******************************************************************************
* 工程名称：整车逻辑
* 文件名： Vehco_Logic.c
* 功能描述 ： 整车管理协调
* 引用记录：
* 备注： 
* 修订记录：
*		日期		      版本		  作者		描述
*   
*******************************************************************************/

#include "GlobalDef.h"
#include "VehCo.h"
#include "Custom_Func.h"
#include "Vehco_Logic.h"
#ifndef AECS_HW
      #pragma DATA_SEG __GPAGE_SEG RUNPAR_EEPROM
#else  
      #pragma DATA_SEG S12X_RAM
#endif

    extern uint8 Lock_Vehicle_Mode;   //锁车状态      0 未锁车 1 1级锁车      2 2级锁车 
    extern uint8 Heartbeat_Lock_Mode; //心跳锁车状态  0 未锁车 1 心跳锁车监听 2 心跳锁车 
    
#ifndef AECS_HW
      #pragma DATA_SEG DEFAULT
#else  
      #pragma DATA_SEG S12X_RAM
#endif

/*
0：关机 1：ON档 2：充电中 3：BMS上电完成 电机使能指令 4：电机READY
5：ON消失 自动减速中 6：下电警告 7：主动放电指令 8：主动放电中 9：放电完成 去使能指令
10: 关机准备完成，下控制电

12: 1级故障  13：2级故障 14：3级故障 ???
*/
uint8 Vehco_State;
//extern uint8 Vehco_State_Stop_Step;
uint8 Vehco_State_Debug;

uint8 MCU_Fault_Reset_Times;//MCU复位次数

uint16 Delay_Index=0,Delay_Index_Tmp=0;
//真空罐故障、EPS故障、空调故障、DC故障、辅助电池故障
uint8 Err_Vacuum_Tank,Err_Eps,Err_Air_Conditioning,Err_DCDC,Err_BatterLow;


//仪表LEDS1 8个bit  BEEP、SOC低、单体电压低、绝缘低、充电连接指示、充电状态指示
uint8 Beep,Batter_Err,Soc_or_Tmp_Low,Low_Cell_Voltage,Low_Isolation,Charge_Connect,Charge_State;
//仪表LEDS2 8个bit 限速模式、定速巡航模式、电机过热、回馈模式、系统故障、Ready灯
uint8 Limit_Mode,Constant_Mode,MCU_OverHeat,FeedBack_Mode,System_Fault,Ready,Ready_Led;

uint8 Motor_OverHeat=0;//电机 电控过热
uint8 Motor_OverHeat_High=0;// 电机 电控2级过热
uint8 Beep_Index,Ready_Index,Km100_Index;
uint16 Err_Dcdc_Delay=0; //ON档电后延迟判断12V电压不足12.5V 报DCDC故障 
uint16 Err_Dcdc_Di_Delay=0;
uint8 Breakk_Please_Start;//点火踩制动踏板提醒
uint8 Dang_NP_Please_Start;//点火置空档提醒
uint8 Brake_Shoe_Warning;//制动蹄片更换提醒
uint8 Tire_Pressure_Waring=0; //胎压过低提醒

uint16 Err_BatterLow_Delay=0; //辅助电池电压过低延迟
uint16 Err_Vacuum_Tank_Delay=0;


//sint16 Ins_Ele_10[10]={0};    //瞬时电耗10ms累计    0.0~1000.0KWh/100Km
//sint16 Ins_Ele_100[10]={0};   //瞬时电耗100ms累计   0.0~1000.0KWh/100Km
sint32 Ins_Ele_1000=0;        //瞬时电耗1000ms      0.0~1000.0KWh/100Km
 

uint16 Electric_Consume_Calculation_Times=0;//能耗计算时钟 0~1000 (999)

uint32 Vehicle_Speed_Add=1;     //车速积分累加值  0.1Km/h
sint32 Electric_Consume_Add=0;  //电功率积分累加值  0.01KW
uint32 Vehicle_Speed_Avg_Add=1;     // 平均 车速积分累加值  0.1Km/h
sint32 Electric_Consume_Avg_Add=0;  //平均 电功率积分累加值  0.01KW

sint32 AV_Ele_Calc=0; //平均电耗计算值
uint16 Electric_Consume_Calculation_Avg_Times=0; //平均能耗计算时钟 0~3600(2599)

uint16 Ins_Ele_NoCalc_Times=0;  //瞬时电耗未计算次数  *1s
uint16 AV_Ele_NoCalc_Times=0;   //平均电耗未计算次数  10min

sint32 Remaining_Mil_Calc=0;  //剩余里程  计算值 0~65535KM

extern uint8 Failure_Recovery_Delay[24];
extern uint16 Output_Voltage,B_Voltage,Min_B_Voltage_Limit;//总压  最低总压限值
extern sint16 B_Current; //电流
extern uint8 BattCD_bLogicOnUB; //逻辑电开关         //不可删除
extern uint16 BattCD_uUW;  //蓄电池电压          //不可删除
extern uint16 uAD4Satus[4];
extern uint8 Car_Door;//车门开

extern uint8 bDiSatus[24];
extern uint16 Breakk;
extern uint8 MCU_Fault_Level, FastDischarge_Fbd,Precharge_Allow,MCU_System_State,LostMcuCommunication;
extern uint8 LostBMSCommunication,B_Fault_Status,B_State,B_Mode;
//BMS主继电器控制请求 0、1;     BMS 3s断电延时0、1;    BMS故障保护许可 0、1
extern uint8 BMS_Contractor_Request,BMS_Power_Down_Request,BMS_Fault_Lockout_Request;
//MCU使能指令0、1;MCU复位指令0、1;MCU控制模式1、2、3
extern uint8 MCU_Enable,MCU_Fault_Reset,MCU_Control_Mode;
//电池电机辅助12V电源
extern uint8 BMS_MCU_12V_Enable,VCU_Diag_Code_1,VCU_Diag_Code_2,VCU_Diag_Code_3; 
extern uint16 DC_Voltage;

extern uint8 LostINSCommunication;
extern uint16 LostMcuCom_Index,LostBMSCom_Index,LostINSCom_Index;
extern uint8 ACPTC_No_LowSoc,ACPTC_No_LowSoc_Once;
extern uint8 SOC,Min_SOC_Limit,Isolation_Level,Dang_L4bit,Dang_P;
extern uint16 Min_Cell_Voltage,Min_Cell_Voltage_Limit,Min_Cell_Temp,Min_Cell_Temp_Limit;
extern sint16 Demand_Torque,Motor_Torque,Motor_Speed;  
extern sint16 Motor_Speed_Average;  
extern uint8  DTC_Code_1,DTC_Code_2; //MCU DTC CODE
extern uint8  VCU_Warning_Code_1,VCU_Warning_Code_2,VCU_Warning_Code_3;//整车报警代码123 
extern uint8 BMS_DTC_Code[8];//电池诊断报文
extern uint8 Err_AccPed; //油门踏板故障
extern uint8 Err_Breakk; //制动踏板故障
extern uint16 AccPed;
extern uint16 Max_Cell_Temp_Limit,Max_Cell_Temp;
extern uint8 DCDC_Enable,Fan_Enable,Dang_R_Enable;
extern uint8 Motor_Temperature,MCU_Temperature;
extern uint8 Torque_Mode;

extern uint8 BCD_Err_Code1,BCD_Err_Code2; //仪表故障码  A9样车专用
extern uint8 Vehco_Type_Test;
extern uint8 B_Version;//BMS通讯协议版本
extern uint8 Vehicle_Mode;//仪表车辆模式 0 1 2   7
extern uint8 Vehicle_Type_Identify_Err;  //电控型号不匹配故障
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
extern  uint16 Motor_Vehicle_Speed_Scale_Get;//转速-车速比例
extern  uint16 Demand_Limit_High_Get;//车速上限
extern  uint16 Demand_Limit_Low_Get;//车速下限
extern  uint16 Crawl_D_Speed_Max_Get;//从其他状态进入蠕动的速度  RPM
extern  uint8 Crawl_D_Torque_Delay_Max_Get;//蠕动更新间隔  单位10ms
extern  uint8 Speed_Buffer_Number_Get; //10ms车辆速度采样缓冲次数
extern  uint8 Speed_Average_Number_Get; //10ms平均车速缓冲次数
extern  uint8 Torque_Buffer_Number_AutoPark_Get;  //10ms车辆平均转矩采样缓冲次数 驻坡

//胎压检测

extern uint16 Tire_Pressure[8]; //轮胎压力值 0~1397.0Kpa 10倍
extern sint16 Tire_Temp[8];     //轮胎温度值 -273.0~1735.0℃ 10倍
extern uint8  Tire_Sensor[8];   //车轮传感器状态
extern uint8  Tire_Pressure_Test[8]; //压力阀检测
extern uint8  Tire_Pressure_Active; //胎压传感器活跃 


//远程锁车
extern uint8 Lock_Command,Heartbeat_Command,Lock_Life,Lock_Box_Mark; //终端远程锁车命令、心跳锁车命令、终端心跳值、终端心跳标识
extern uint8 Lock_Box_RandKey;//终端随机密匙
extern uint32 Lock_Box_SN;//锁车终端编号
extern uint16 Lock_Box_Com_Index;
extern uint8 LostLockBoxCommunication;
extern uint8 GetNewMsg_LockBox[3];
extern uint8 Lock_Vehicle_Active; //锁车状态激活
           

//第三方锁车专用
extern uint8 Third_Lock_Active;              //锁车激活
extern uint8 Third_Lock_Mode_Active;         //生效锁车模式
extern uint8 Third_Lock_LimitSpeed_Active;   //生效限速速度 

//电耗计算
extern uint8 Remaining_Mil;//剩余里程 0～255km      从V1.2版开始 2km/bit 0~510
extern uint8 AV_Ele;//平均电耗    0.5kwh/100KM  0~127.5
extern uint8 Ins_Ele;//瞬时电耗   0.5kwh/100KM  0~127.5
extern uint16 Rated_Energy; //额定能量 0~409.5kwh
extern uint16 AV_Ele_Calc_Min;// 最小百公里平均电耗 10.0KWh/100Km

//卡车
extern uint8  Oil_Pump_Install; //油泵控制器适配状态 0 未装 1 安装
extern uint8  Air_Pump_Install; //气泵控制器适配状态 
extern uint8  Fan_Pwm_Install;
extern uint8  Air_Pump_OverHeated;
extern uint16 Fan_Pwm_Rate;     //D15 风扇PWM计算值 0~100.00%
extern uint8  Air_Reservoirs_Pressure_1,Air_Reservoirs_Pressure_2;//储气桶压力 0.00~2.50Mpa
extern uint8  Air_Pump_Fault,Oil_Pump_Fault;//油泵故障状态 0 正常  1 故障
extern uint8  Oil_Pump_Still_OverTime; //油泵堵转 翻转超时
extern uint16 Air_Reservoirs_Pressure_Low_Index;
extern uint8  Gears_Install;
extern uint8 Shift_Gears; 

//充电互锁
extern uint8   No_Ins_Charge_CAN;//充电期间禁止INS特殊帧发送

    
#ifndef AECS_HW
  #pragma CODE_SEG DEFAULT
#else  
  #pragma CODE_SEG VCU_CODE 
#endif

//车辆上下电逻辑
void Vehco_Logic(void) 
{
   Vehco_State_Debug=Vehco_State_Stop_Step; 
    
    switch(Vehco_State) 
    {
      case 0:
          Vehco_State_Stop_Step=0; 
          break;
      case 11:  //MCU 1、2级故障  复位中
          MCU_Fault_Reset=1;
          MCU_Enable=0;
          Delay_Index++;
          if (MCU_Fault_Level==0) 
          {
              MCU_Fault_Reset=0;
              MCU_Enable=0;
              Vehco_State=1;  
              Vehicle_Mode=0;         
          }
          if (Delay_Index>100)//1秒超时
          {
              Vehco_State_Stop_Step=11;
              MCU_Fault_Reset=0;
              MCU_Enable=0;
              Vehco_State=1;              
          }
      
      
          break;    
      case 12: //电池报警，等待电机下电


          break;
      case 1: //已上ON档 等待STAR信号
          //START信号 & 电机系统正常 黄READY
          Ready=0;
          Ready_Led=0;
          //Vehicle_Mode=0; 
          if (((MCU_Fault_Level==1)||(MCU_Fault_Level==2))&&(MCU_Fault_Reset_Times<3)) //MCU三次复位  
          {
              Vehco_State=11;
              Delay_Index=0; 
              MCU_Fault_Reset_Times++;   
              Vehicle_Mode=7;
          }
          
          //uint8 Breakk_Please_Start;//点火踩制动踏板提醒
          //uint8 Dang_NP_Please_Start;//点火置空档提醒           
          if ((Dang_L4bit==0)||(Dang_L4bit==1)  )
          {
              Dang_NP_Please_Start=0;
              Failure_Recovery_Delay[5]=0;          
          }
          if ((Breakk>100)&&(AccPed<=100)) 
          {
              Breakk_Please_Start=0;
              Failure_Recovery_Delay[4]=0;          
          }
          
              //档位为N或P才可以点火//增加不能踩油门
          if ( (Vehicle_Type_Identify_Err==0)&&((Dang_L4bit==0)||(Dang_L4bit==1)) && (bDiSatus[2]==1)&& (Breakk>100)&&(AccPed<=100)&&(Err_AccPed==0)&&
              ((MCU_Fault_Level==0)||(MCU_Fault_Level==1))&&(No_Ins_Charge_CAN==0)&&(FastDischarge_Fbd==0)&&(Precharge_Allow==1)&&  ((MCU_System_State==1)||(MCU_System_State==3)||(MCU_System_State==7)   )    &&(LostMcuCommunication==0))
          
          {   //电池系统正常
              if ((LostBMSCommunication==0)&&   ((B_Fault_Status<=2)||  ((B_Fault_Status<=1)&&(B_Version<18))   )
              
              &&(((B_State==0)&&((B_Mode==0)||(B_Mode==1)))  ||  (B_State==1)&& ( (B_Mode==2)||(B_Mode==1) ) ||(B_State==4)&& ( (B_Mode==2))  ))
              {
                  BMS_Contractor_Request=1; 
                  Delay_Index=0;              
              
              }
          
          }else 
          { //点火失败提醒
              if (bDiSatus[2]==1)//拧至START
              {
                  //点火置空档提醒    
                  if ((Dang_L4bit==5)||(Dang_L4bit==10))
                  { 
                      Dang_NP_Please_Start=1;
                      Failure_Recovery_Delay[5]=200; 
                  }
                  //点火踩制动踏板提醒
                  if ((Breakk<100)||(AccPed>100)) 
                  { 
                      Breakk_Please_Start=1;
                      Failure_Recovery_Delay[4]=200; 
                  }
              }else 
              {   //松开START开始计时
                  //
                  if (Failure_Recovery_Delay[5]==0) 
                  {
                      Dang_NP_Please_Start=0;
                  }else 
                  {
                      --Failure_Recovery_Delay[5];
                  }
                  if (Failure_Recovery_Delay[4]==0) 
                  {
                      Breakk_Please_Start=0;
                  }else 
                  {
                      --Failure_Recovery_Delay[4];
                  }                          
                  //
              }
              
          }
          
          if  (BMS_Contractor_Request==1) 
          {
              
              if (Delay_Index>100)//1秒超时  
              {
                  BMS_Contractor_Request=0;
                  Vehco_State_Stop_Step=1;
                  Delay_Index=0;
              } 
              else
              {
                  Delay_Index++;
              }
              
              if ((B_State==2)&&  ((B_Mode==2)||(B_Mode==0))    ||(  (B_State==3)&&(B_Mode==3)   )      ) 
              {
                  Vehco_State=2;//进入预充状态
                  Delay_Index=0;
              }
          }
          break;
      case 2://预充阶段       18.12.05 改为9秒
          if  (Delay_Index>900)//5秒超时
          {
              BMS_Contractor_Request=0;
              Vehco_State_Stop_Step=2;
              Vehco_State=1;
              Delay_Index=0;          
          }
          else 
          {
              Delay_Index++;
          }
          //B_Voltage  Output_Voltage //0~819.0V
          //DC_Voltage   //0～1000.0V
          //  ((MCU_System_State==3)||   (MCU_System_State==1)  )  ) //A9 跳过MCU反馈
          if ((B_State==3)&&(B_Mode==3)
              &&(MCU_System_State==3)
              && (abs(B_Voltage-DC_Voltage)<300)
              &&(Delay_Index>50)      )
          {
              Vehco_State=3;    
              Delay_Index_Tmp=0;
              Delay_Index=0;
          }
          break;
      case 3://BMS上电完成 延迟后发送电机使能指令
          if (Delay_Index_Tmp>10) //电机使能延时100ms
          {
              MCU_Enable=1;
              MCU_Control_Mode=2;
              Delay_Index_Tmp=11;
              //Delay_Index=0;
              
          }else 
          {
              Delay_Index_Tmp++;
          }

          if (MCU_Enable==1) 
          {
              if (Delay_Index>200)  //发电机使能指令后等待电机响应  容许时间2s
              {
                  BMS_Contractor_Request=0;
                  Vehco_State_Stop_Step=3;
                  Vehco_State=1;
                  MCU_Enable=0;
                  Delay_Index=0;
                  Delay_Index_Tmp=0;
              } else
              {
                  Delay_Index++;
              }
              if  (MCU_System_State==5)//((MCU_System_State==5)||(MCU_System_State==3))     //兼容某些电机不发5
              {
                  MCU_Fault_Reset_Times=0;
                  Vehco_State=4;
                  Ready=1;
                  Ready_Led=1;
                  Vehicle_Mode=1;
                  Delay_Index=0; 
                  Delay_Index_Tmp=0;  
              }
          
          
          }
          break;
      case 4: //电机进入READY状态  仪表显示绿READY
          //此处是否运行故障判断  不同级别的故障跳入不同的状态
          //严重故障   B_Version
          //if ((MCU_Fault_Level>1)||(B_Fault_Status>1)||(LostMcuCommunication==1)||(LostBMSCommunication==1)) //电机故障2    电池故障4 电机通讯故障 电池通讯故障
          
          if ((MCU_Fault_Level>2)||    
             ((B_Fault_Status>1)&&(B_Version<18))  || (B_Fault_Status>2)    //BMS 1.8版以前2级故障下电，1.8版开始3级故障下电
             ||(LostMcuCommunication==1)||(LostBMSCommunication==1)) //电机故障3    电池故障2/3 电机通讯故障 电池通讯故障
          {
                  Ready=0;
                  Ready_Led=0; 
                  Vehicle_Mode=7;
                  MCU_Enable=0;          //先停电机 100ms后停电池
                  //BMS_Contractor_Request=0;
                  //Vehco_State_Stop_Step=4;
                  //Vehco_State=1;
          }
          if (Vehicle_Mode==7) 
          {
              if ((Delay_Index_Tmp>10)||(MCU_System_State!=5)) //100ms到或电机退出工作状态
              {
                  BMS_Contractor_Request=0;  //停电池
                  Vehco_State_Stop_Step=4;
                  Vehco_State=1;  
                  Delay_Index_Tmp=0;            
              
              }else 
              {
                  Delay_Index_Tmp++;
              }
          
          
          }
          
      
      
      
          break;
      case 5://减速程序 
          //丢失ON信号 速度过高，需要减速 在Vehco.c  VehCo_Afterrun中判断
          //On_LostInRuning
          //进入D滑行能量回馈 或R 滑行状态 在Custom_Func.c中处理
          
          //此时应该有仪表报警  Ready 黄绿交替闪烁 声音
          
          break;
      case 6: //BMS下电指令   仪表显示黄READY
          //丢失ON信号后 速度够低，可以下高压 在Vehco.c  VehCo_Afterrun中判断
          BMS_Power_Down_Request=0;
          BMS_Contractor_Request=0;
          Ready=0;
          Ready_Led=0;
          Vehicle_Mode=0;
          MCU_Enable=0;
          if (BMS_Contractor_Request==0) 
          {
              
              if (Delay_Index>200)      //2s后电池无法下电 不运行进入主动放电 状态10时是否要考虑DCV<36 ?
              {
                  Vehco_State=9;
                  Delay_Index=0;
                  Vehco_State_Stop_Step=6;
              } 
              else 
              {
                  Delay_Index++;
              }
              
          
              if  ((B_State==0)&&((B_Mode==0)||(B_Mode==1))||(   ((B_State==1)||(B_State==4))    &&  ((B_Mode==2)||(B_Mode==1))  ))  //BS=0&BM=0,1 OR BS=1&BM=2
              { //高压电池已经下电，可以进入电机主动放电

                  if (DC_Voltage>=500)
                  {
                     
                     
                     //if (Vehco_Type_Test==9)
                     if (FastDischarge_Enable==0)  //不允许主动放电
                     
                     {
                         Vehco_State=8 ;   //8米力信不支持主动放电;
                         Delay_Index=0; 
                     }else{
                         Vehco_State=7;
                         Delay_Index=0; 
                     }
                  } else 
                  {
                      Vehco_State=9;
                      Delay_Index=0;
                  
                  }
              }
          }
          break;
      case 7: //主动放电指令
          MCU_Enable=1;
          MCU_Control_Mode=3;
          
          if ((MCU_Control_Mode==3)&&(MCU_Enable==1)) 
          {
              if (Delay_Index>100) //1s后电机无法进入主动放电状态 退出主动放电  状态10时是否要考虑DCV<36 ?
              {
                  Vehco_State=9;
                  Delay_Index=0;
                  Vehco_State_Stop_Step=7;              
                  
              }
              else 
              {
                  Delay_Index++;
              }


              if ((FastDischarge_Fbd==1)&&(MCU_System_State==6)) 
              {
                  Vehco_State=8;
                  Delay_Index=0;
              }
          }
          break;
      case 8: //主动放电过程

          if (Delay_Index>300) //3s后电机无法放干净电  退出主动放电  状态10时是否要考虑DCV<36 ?
          {
              Vehco_State=9;
              Delay_Index=0;
              Vehco_State_Stop_Step=8;                  
                  
          }
          else 
          {
              Delay_Index++;
          }          
          if ((MCU_System_State==7)||(FastDischarge_Fbd==0)) //放电完成 
          {   
              Vehco_State=9;
              Delay_Index=0;                       
          }          

          break;
      case 9: //主动放电结束 电机去使能
          MCU_Enable=0;
          MCU_Control_Mode=2;          
      
          if ((MCU_System_State==1)||(MCU_System_State==8)) 
          {
              Vehco_State=10;
              Delay_Index=0;               
          }
           if (Delay_Index>100)//1S 电机无响应
           {  
              Vehco_State=10;
              Delay_Index=0;
              Vehco_State_Stop_Step=9;
           }
           else
           {
              Delay_Index++;
           }
          
          break;
      case 10: //下辅助电
          //MCU关机程序完成后,关闭MCU辅助电12V
          BMS_MCU_12V_Enable=0;
          VehCo_DOset(1);
          break;
      default:
          break;
    }




}


//车辆故障判断处理 
void Vehco_Error (void) 
{   //10ms
    uint8 VCU_Diag_Code_1_bit[8]={0},VCU_Diag_Code_2_bit[8]={0};
    uint8 VCU_Diag_Code_3_bit[8]={0};
    uint8 VCU_Warning_Code_1_bit[8]={0},VCU_Warning_Code_2_bit[8]={0},VCU_Warning_Code_3_bit[8]={0};
    

    //uint8 Err_Vacuum_Tank,Err_Eps,Err_Air_Conditioning,Err_DCDC,Err_BatterLow;
    //uint8 BMS_DTC_Code[8];//电池诊断报文
    //uint8 VCU_Warning_Code_1,VCU_Warning_Code_2,VCU_Warning_Code_3;//整车报警代码123 
    //uint8 Brake_Shoe_Warning 制动蹄片更换提醒
    extern uint8 Limit_P;
    extern uint8 Breakk_Please;//踩制动踏板提醒
    extern uint8 Safe_Belt;//安全带插入状态
    
    
    
    //车辆故障判断
    Vehco_Errpr_Judge();
    //蜂鸣器处理
    Beep_5050();
    
    
    //车辆故障代码整合
    VCU_Diag_Code_1_bit[7]=Err_AccPed;                                 //油门踏板传感器故障
    VCU_Diag_Code_1_bit[6]=Err_Breakk; //制动踏板故障     //制动踏板传感器故障   
    VCU_Diag_Code_1_bit[5]=LostMcuCommunication;         //电机总线脱离
    VCU_Diag_Code_1_bit[4]=LostBMSCommunication;         //BMS 总线脱离
    VCU_Diag_Code_1_bit[3]=((MCU_Fault_Level>1)||(Vehicle_Type_Identify_Err==1)  );    //电机故障  旧仪表先用此代码报电控型号不匹配
    VCU_Diag_Code_1_bit[2]=(B_Fault_Status>2);           //电池严重故障
    VCU_Diag_Code_1_bit[1]=MCU_OverHeat;                               //电机过热
    VCU_Diag_Code_1_bit[0]=(Max_Cell_Temp>Max_Cell_Temp_Limit);        // 电池过热
    VCU_Diag_Code_1=SetBit8(VCU_Diag_Code_1_bit);
    
    
    VCU_Diag_Code_2_bit[6]=Err_Vacuum_Tank;            //真空泵故障
    //VCU_Diag_Code_2_bit[5]=            //高压继电器故障 粘连 失败 
    VCU_Diag_Code_2_bit[4]=LostINSCommunication;       //仪表总线脱离
    //VCU_Diag_Code_2_bit[2]=           //高压互锁断路    
    VCU_Diag_Code_2_bit[0]=(Isolation_Level>0);       //绝缘电阻过低
    VCU_Diag_Code_2=SetBit8(VCU_Diag_Code_2_bit);

    VCU_Diag_Code_3_bit[5]=Vehicle_Type_Identify_Err;//电控型号不匹配故障
    //VCU_Diag_Code_3_bit[4]=    // 电池温度异常，请即刻停驶下车疏散！ 
    VCU_Diag_Code_3_bit[3]=Err_BatterLow;          //辅助电池异常
    VCU_Diag_Code_3_bit[2]=Err_DCDC;               //DC 故障
    VCU_Diag_Code_3_bit[1]=Err_Air_Conditioning;   //空调故障
    VCU_Diag_Code_3_bit[0]=Err_Eps;                //EPS 故障
    VCU_Diag_Code_3=SetBit8(VCU_Diag_Code_3_bit);
    
    //uint8 Breakk_Please_Start;//点火踩制动踏板提醒
    //uint8 Dang_NP_Please_Start;//点火置空档提醒    
    VCU_Warning_Code_1_bit[6]=Limit_P;
    VCU_Warning_Code_1_bit[3]=Tire_Pressure_Waring;//胎压过低
    VCU_Warning_Code_1_bit[2]=(Safe_Belt==0)&&(Motor_Speed/Motor_Vehicle_Speed_Scale_Get>10);
    VCU_Warning_Code_1_bit[1]=Dang_NP_Please_Start;
    VCU_Warning_Code_1_bit[0]=Breakk_Please||Breakk_Please_Start;

    VCU_Warning_Code_1=SetBit8(VCU_Warning_Code_1_bit);
    
    VCU_Warning_Code_2_bit[5]=Brake_Shoe_Warning;
    //VCU_Warning_Code_2_bit[4]=Car_Door && Ready_Led && (Safe_Belt==0); //车辆已上电，请勿离开驾驶室
    
    VCU_Warning_Code_2=SetBit8(VCU_Warning_Code_2_bit);
    
    VCU_Warning_Code_3=SetBit8(VCU_Warning_Code_3_bit);
    
}

void Vehco_Errpr_Judge(void)
{
  
    uint8 Err_DCDC_dE8=0;
    uint8 Err_DCDC_Bi=0;
    uint16 Fan_Pwm_Rate_Temp=0;
    
    Err_Eps=(bDiSatus[11] &&(Oil_Pump_Install==0)) || (((Oil_Pump_Fault==1)||(Oil_Pump_Still_OverTime==1)) &&(Oil_Pump_Install==1));
    //Err_Air_Conditioning=?;

    
    if (BattCD_uUW<Err_BatterLow_Limit)          //电池电压低于10.5V
    {
        Err_BatterLow_Delay++;
        if (Err_BatterLow_Delay>800)         //延迟8秒
        {
            Err_BatterLow=1;
            Err_BatterLow_Delay=900;
        }
    }else
    {
        Err_BatterLow=0;
        Err_BatterLow_Delay=0;    
    }
        
    //Err_Vacuum_Tank=bDiSatus[8];
    //Air_Pump_OverHeated=((bDiSatus[8]==0)&&(Air_Pump_Install==1)&&(Vehco_Type_Test!=16)&&(Vehco_Type_Test!=18)); //气泵过热报警  断线报警   H渣土车 D35C 无气泵温控线
    
    if (((Air_Reservoirs_Pressure_Low_Index==200)||(bDiSatus[8]==1))&&(Air_Pump_Install==0))                             //真空罐报警
    {
        Err_Vacuum_Tank_Delay++;
        if (Err_Vacuum_Tank_Delay>300)    //延迟3秒
        {
            Err_Vacuum_Tank=1;
            Err_Vacuum_Tank_Delay=400;
        }
    }else 
    {
        if ((Air_Pump_Install==1)
            &&(((Air_Reservoirs_Pressure_1<Air_Pump_Low_Pressure)||(Air_Reservoirs_Pressure_2<Air_Pump_Low_Pressure))||(Air_Pump_Fault==1))
            
            
            )
        {
            Err_Vacuum_Tank_Delay++;
            if (Err_Vacuum_Tank_Delay>200)    //延迟2秒
            {
                Err_Vacuum_Tank=1;
                Err_Vacuum_Tank_Delay=400;
            }     
        }else
        {
            Err_Vacuum_Tank_Delay=0;
            Err_Vacuum_Tank=0;
        }
    }
  
  
  
    //Fan_Enable  Motor_Temperature  MCU_Temperature -40～210℃  Motor_OverHeat_High
  
    //Brake_Shoe_Warning ;//制动蹄片更换提醒 常态接地，内部2.74K上拉5V  uAD4Satus[0] 0~4095 0~5V
    Brake_Shoe_Warning= Brake_Shoe_Check && (uAD4Satus[0]>2048);   
  
    /*
    extern uint16 Tire_Pressure[8]; //轮胎压力值 0~1397.0Kpa 10倍
    extern sint16 Tire_Temp[8];     //轮胎温度值 -273.0~1735.0℃ 10倍
    extern uint8  Tire_Sensor[8];   //车轮传感器状态
    extern uint8  Tire_Pressure_Test[8]; //压力阀检测    
    uint8 Tire_Pressure_Waring; //胎压过低提醒
    */
    
    
    if (Tire_Pressure_Active==1)
    {
        switch(Vehco_Type_Test) 
        {
              case 9:
                  if ((Tire_Pressure[0]<Tire_Pressure_Low_Level_M10) || (Tire_Pressure[1]<Tire_Pressure_Low_Level_M10)) 
                  {
                      Tire_Pressure_Waring=1;
                  }else 
                  {
                      Tire_Pressure_Waring=0;
                  }
                  break;
              default:
                  Tire_Pressure_Waring=0;
                  break;
        }
    }
    if ((Motor_Temperature>=Motor_Temperature_L1)||(MCU_Temperature>=MCU_Temperature_L1))  {Motor_OverHeat=1;}  // 电机80 控制器 50 启动风扇
    if ((Motor_Temperature>=Motor_Temperature_L2)||(MCU_Temperature>=MCU_Temperature_L2))  {Motor_OverHeat_High=1;}  //电机100 控制器 70  2级过热
   
    if ((Motor_Temperature<=Motor_Temperature_L1-10)&&(MCU_Temperature<=MCU_Temperature_L1-4))  {Motor_OverHeat=0;Motor_OverHeat_High=0;}   // 电机70 控制器 46 关闭风扇
   
   
    //Fan_Pwm_Rate
    //斯泰尔风机  500 Hz 6~25 (15)% 停机 35~55% 低速启动  55~95% 25~100转速
    
    if (Motor_Temperature>=Motor_Temperature_L1)  //140~120---->1~21
    {
        Fan_Pwm_Rate_Temp=Fan_Pwm_Rate_Temp+(1+Motor_Temperature-Motor_Temperature_L1); 
    }
    if (MCU_Temperature>=MCU_Temperature_L1)    //90~100------>2~22
    {
        Fan_Pwm_Rate_Temp=Fan_Pwm_Rate_Temp+2*(1+MCU_Temperature-MCU_Temperature_L1); 
    }
    //根据电机温度和电控温度计算风扇PWM
    Fan_Pwm_Rate=Intpol_u16u16((uint16 * __far)&Fan_Rate_Temp, Fan_Pwm_Rate_Temp); 

   
    if (DCDC_Enable==1) 
    {
        //1、优优 2、D11\D12-600Nm 3、D07-750   4、A9 大地和150Nm   5、D11\D12 大地和700Nm 6、A9力信180Nm 7、D10旭利 500Nm  8、D09 400Nm 大地和  9、D11L8米 1200Nm力信           
        if  (
            ((BattCD_uUW<Err_BatterLow_Limit+200)&& (  (Vehco_Type_Test==1)||(Vehco_Type_Test==2)||(Vehco_Type_Test==5)   )     )       //ON档有DCDC高压
            || 
            ((BattCD_uUW<Err_BatterLow_Limit+200)&& 
              //((Vehco_Type_Test==4)||(Vehco_Type_Test==3)||(Vehco_Type_Test==7)||(Vehco_Type_Test==8)||(Vehco_Type_Test==6)) 
              (  (Vehco_Type_Test!=1)&&(Vehco_Type_Test!=2)&&(Vehco_Type_Test!=5)   )      //ON档无DCDC高压
              &&(Vehco_State==4))
            ) 
        {
           
           
            Err_Dcdc_Delay++;
        }else 
        {
            Err_Dcdc_Delay=0;
        }
        
        if ((Err_Dcdc_Delay>2000))
        {
            Err_Dcdc_Delay=2100;
            Err_DCDC_dE8=1; //DCDC上电20秒后12V电压低于12.5V 报DCDC故障
        }else 
        {
            Err_DCDC_dE8=0;
        }
      
        
  
    }else{Err_Dcdc_Delay=0;}
    //Err_Dcdc_Di_Delay    Err_DCDC_Bi  收到DCDC故障硬线信号8秒后报DCDC故障
    if ((bDiSatus[12])&&(Vehco_State==4)) 
    {
       Err_Dcdc_Di_Delay++;
       if (Err_Dcdc_Di_Delay>800) 
       {
          Err_DCDC_Bi=1;
          Err_Dcdc_Di_Delay=900;
          
       } else 
       {
          Err_DCDC_Bi=0;
       }
    } 
    else 
    {
        Err_Dcdc_Di_Delay=0;
        Err_DCDC_Bi=0;
    }
    
    
    
    
    
    
    
    Err_DCDC=(Err_DCDC_Bi||Err_DCDC_dE8);
  
  
    if (BMS_MCU_12V_Enable==1) {LostMcuCom_Index++;LostBMSCom_Index++;LostINSCom_Index++;}
  
    if (LostMcuCom_Index>500)   //MCU 通讯超时5S
    {
        LostMcuCom_Index=501;
        LostMcuCommunication=1;
    } else 
    {
        LostMcuCommunication=0;
    }
    if (LostBMSCom_Index>500)   //BMS 通讯超时5S
    {
        LostBMSCom_Index=501;
        LostBMSCommunication=1;
    } else 
    {
        LostBMSCommunication=0;
    }
    if (LostINSCom_Index>1000)   //INS 通讯超时5S
    {  
        LostINSCom_Index=1001;
        LostINSCommunication=1;
    } else 
    {
        LostINSCommunication=0;
    }  
    //仪表LEDS1 8个bit  BEEP、SOC低、单体电压低、绝缘低、充电连接指示、充电状态指示
    //uint8 Beep,Batter_Err,Soc_or_Tmp_Low,Low_Cell_Voltage,Low_Isolation,Charge_Connect,Charge_State;
  


  
    Batter_Err=(B_Fault_Status>0);
    
    
    if ((ACPTC_No_LowSoc_Once==0)&&(Soc_or_Tmp_Low==0)&&(((SOC<=Min_SOC_Limit)&&(SOC!=0)) || ((Min_Cell_Temp<=Min_Cell_Temp_Limit)&&(Min_Cell_Temp!=0))==1))  
    {
        ACPTC_No_LowSoc=0;  //SOC首次过低时激活关空调互锁
        ACPTC_No_LowSoc_Once=1;
    }
    
    
    
    Soc_or_Tmp_Low=((SOC<=Min_SOC_Limit)&&(SOC!=0)) || ((Min_Cell_Temp<=Min_Cell_Temp_Limit)&&(Min_Cell_Temp!=0));

    Charge_State=  (3*Soc_or_Tmp_Low); //电量低 11：常亮
    
    Low_Cell_Voltage=( ((Min_Cell_Voltage<=Min_Cell_Voltage_Limit)&&(Min_Cell_Voltage!=0)) ||
                       ((B_Voltage<=Min_B_Voltage_Limit)&&(Min_B_Voltage_Limit!=0))         );  
                     //需增加总压过低   B_Voltage 0~819.0V    Min_B_Voltage_Limit
    
    
    Low_Isolation=(Isolation_Level>0);
  
    //仪表LEDS2 8个bit 限速模式、定速巡航模式、电机过热、回馈模式、系统故障、Ready灯
    //  uint8 Limit_Mode,Constant_Mode,MCU_OverHeat,FeedBack_Mode,System_Fault,Ready;    
    //MCU_OverHeat=(((DTC_Code_1&0x8)>>3)||((DTC_Code_1&0x10)>>4));
    MCU_OverHeat=(((DTC_Code_1&0x8)>>3)||((DTC_Code_1&0x10)>>4)||((DTC_Code_2&0x8)>>3)||((DTC_Code_2&0x10)>>4));  //2018.3.1 增加电机超温警报
    if (((Torque_Mode==2)||(Torque_Mode==3)||(Torque_Mode==9))&&(Demand_Torque<0))  //能量回馈模式灯
    {FeedBack_Mode=1;}
    else 
    {FeedBack_Mode=0;}
    
}

void Beep_5050 (void) 
{ //蜂鸣器处理
     uint8 beep1=0,beep2=0,beep3=0;
    //P档 500ms响  500ms停
    if ((Dang_L4bit==10)&&(Dang_P==1)&&(Motor_Speed/Motor_Vehicle_Speed_Scale_Get>10))
    {
        if  (Beep_Index<50) 
        {
            beep1=1;
        }else 
        {
            beep1=0; 
        }
        Beep_Index=(uint8)((Beep_Index+1)%100);
    }
    else
    {
       Beep_Index=0;
       beep1=0;
    }
    //行驶中丢ON档电  250ms响 250ms停 Ready黄绿闪烁   或锁车激活且已上高压
    //Ready_Index
    if ( (Vehco_State==5)
        ||  ((Third_Lock_Active==1)&&(Vehco_State==4))   
        ||(((Lock_Vehicle_Enable==1)||(Heartbeat_Lock_Vehicle_Enable==1)) &&(Lock_Vehicle_Active==1)&&(Vehco_State==4)) )
    {
        if(Ready_Index<25) 
        {
            beep2=1;
            Ready_Led=0;
            
        }else 
        {
            beep2=0;
            Ready_Led=1;
        }
        Ready_Index=(uint8)((Ready_Index+1)%50);
    }
    else
    {
        Ready_Index=0;
        beep2=0;   
        Ready_Led=Ready;     
    }
    // 100km/h限速    500ms响  500ms停
    if ( ((Gears_Install==0)||( Shift_Gears==0  ))&&(AccPed_Torque_100_Enable==1)&& (Dang_L4bit==10)&&(Motor_Speed/Motor_Vehicle_Speed_Scale_Get>(D100_Max_Speed-3))) //D档限速100 
    {
        if  (Km100_Index<50) 
        {
            beep3=1;
        }else 
        {
            beep3=0; 
        }
        Km100_Index=(uint8)((Km100_Index+1)%100);    
    }else 
    {
       Km100_Index=0;
       beep3=0;
    }
    
    
    if ((beep1==1)||(beep2==1)||(beep3==1)) 
    {
        Beep=1;  
    }else 
    {
        Beep=0;
    }

}


void Lock_Vehicle_Active_Init(void)
{
   //只有在刚上电且已经收到锁车指令才可以激活锁车状态
    if (Lock_Vehicle_Mode==255)
    {
        Lock_Vehicle_Mode=0;
    }
    if (Heartbeat_Lock_Mode==255)
    {
        Heartbeat_Lock_Mode=0;
    }
      
    if (
        ((Lock_Vehicle_Enable==1)&& ((Lock_Vehicle_Mode==1)||(Lock_Vehicle_Mode==2)))
        ||((Heartbeat_Lock_Vehicle_Enable==1)&& ((Heartbeat_Lock_Mode==2)))
        
        ) 
    {
        Lock_Vehicle_Active=1;//锁车状态激活
         
    }
    //锁车指令消失则锁车激活状态置零          必须松油门且档位置空档、P挡            
    if ((Heartbeat_Lock_Mode!=2)&&(Lock_Vehicle_Mode!=1)&&(Lock_Vehicle_Mode!=2)&&(AccPed<50)&&((Dang_L4bit==0)||(Dang_L4bit==1))) 
    {
        Lock_Vehicle_Active=0;
    }
}


void Lock_Box_Vehicle(void) //远程锁车程序  100ms运行一次
{   
    /*
    extern uint8 Lock_Command,Heartbeat_Command,Lock_Life,Lock_Box_Mark; //终端远程锁车命令、心跳锁车命令、终端心跳值、终端心跳标识
    extern uint8 Lock_Box_RandKey;//终端随机密匙
    extern uint32 Lock_Box_SN;//锁车终端编号
    extern uint16 Lock_Box_Com_Index;
    extern uint8 LostLockBoxCommunication;
    extern uint8 GetNewMsg_LockBox[3];
    extern uint8 Lock_Vehicle_Active; //锁车状态激活
    extern uint8 Lock_Vehicle_Mode;   //锁车状态      0 未锁车 1 1级锁车      2 2级锁车 
    extern uint8 Heartbeat_Lock_Mode; //心跳锁车状态  0 未锁车 1 心跳锁车监听 2 心跳锁车    
    */
    //Lock_Level_1_Max_Speed 1级锁车最大车速
    
    MSCANDrv_infoMSGS_TYPE infoMsgS;
    uint8 SendMsg_BCC_Code=0,Bcc_Index=0;

    if (GetNewMsg_LockBox[0]==1)       //终端远程锁车命令 0=无效 1=1级锁车指令 2=2级锁车指令 3=保留  4=解锁指令
    {  //远程锁车应答
        GetNewMsg_LockBox[0]=0;//收1帧发1帧
        infoMsgS.infoMsgIDUI=0x1865F0A0;
        infoMsgS.bExtIDUB=1;
        infoMsgS.datMsgUBA[0]=Lock_Command;
        infoMsgS.datMsgUBA[1]=0;
        infoMsgS.datMsgUBA[2]=(uint8)((Lock_Box_RandKey+Lock_Command*2)/256);
        infoMsgS.datMsgUBA[3]=0;
        infoMsgS.datMsgUBA[4]=0;
        infoMsgS.datMsgUBA[5]=0;
        infoMsgS.datMsgUBA[6]=0;
        
        //SendMsg_BCC_Code  异或校验
        SendMsg_BCC_Code=0;
        for (Bcc_Index=0;Bcc_Index<7;Bcc_Index++) 
        {
        	  SendMsg_BCC_Code^=infoMsgS.datMsgUBA[Bcc_Index]	;	
        } 
        
        infoMsgS.datMsgUBA[7]=SendMsg_BCC_Code;       
        infoMsgS.numLenUB=8;
        infoMsgS.idxMsgPriorityUB=0x02;
        MSCANDrv_SendMsgCAN_A(infoMsgS); 
       
    }
    if (GetNewMsg_LockBox[1]==1)       //心跳锁车命令 0=无效 1=心跳锁车指令 2=保留 3=保留  4=解锁指令
    {  //心跳锁车应答
        GetNewMsg_LockBox[1]=0;//收1帧发1帧
        infoMsgS.infoMsgIDUI=0x1867F0A0;
        infoMsgS.bExtIDUB=1;
        infoMsgS.datMsgUBA[0]=Heartbeat_Command;
        infoMsgS.datMsgUBA[1]=0;
        infoMsgS.datMsgUBA[2]=(uint8)((Lock_Box_RandKey+Heartbeat_Command*2)/256);
        infoMsgS.datMsgUBA[3]=0;
        infoMsgS.datMsgUBA[4]=0;
        infoMsgS.datMsgUBA[5]=0;
        infoMsgS.datMsgUBA[6]=0;
        
        //SendMsg_BCC_Code  异或校验
        SendMsg_BCC_Code=0;
        for (Bcc_Index=0;Bcc_Index<7;Bcc_Index++) 
        {
        	  SendMsg_BCC_Code^=infoMsgS.datMsgUBA[Bcc_Index]	;	
        } 
        
        infoMsgS.datMsgUBA[7]=SendMsg_BCC_Code;       
        infoMsgS.numLenUB=8;
        infoMsgS.idxMsgPriorityUB=0x02;
        MSCANDrv_SendMsgCAN_A(infoMsgS);
        
    }
    
    if (AccPed<50)
    {
        switch (Lock_Command) 
        {
            case 1:
                Lock_Vehicle_Mode=1;
                break;
            case 2:
                Lock_Vehicle_Mode=2;
                break;
            case 4:
                Lock_Vehicle_Mode=0;
                break;
            default:
                break;
        }
        
        
        switch (Heartbeat_Command) 
        {
            case 1:
                if (Heartbeat_Lock_Mode!=2){ Heartbeat_Lock_Mode=1;  }
                break;
            case 4:
                Heartbeat_Lock_Mode=0;
                break;
            default:
                break;
        }
    
    }
    

    
    //如果已激活心跳锁车监听
    if (Heartbeat_Lock_Mode==1)
    {   
        Lock_Box_Com_Index++;    //100ms +1 
        if (Lock_Box_Com_Index>Lock_Box_Com_Index_Max)  //10秒拆机超时
        {
            Heartbeat_Lock_Mode=2;
            LostLockBoxCommunication=1;
        }
    }

    if ((Heartbeat_Lock_Mode==2)&&(Heartbeat_Lock_Vehicle_Enable==1)&&(Motor_Speed/Motor_Vehicle_Speed_Scale_Get<20)) 
    {
        Lock_Vehicle_Active=1;     //拆终端车速低立刻锁车
    }

    //收到终端心跳报文且BCC校验通过
    if (GetNewMsg_LockBox[2]==1)
    {
        GetNewMsg_LockBox[2]=0;
        if ((Lock_Box_Com_Index<Lock_Box_Com_Index_Max)&&(Heartbeat_Lock_Mode==2)) 
        {
            Heartbeat_Lock_Mode=1;//恢复到心跳监听状态
            LostLockBoxCommunication=0;
        }
    }
    
    
    
}

//电能消耗计算    10ms周期            2018.11.06 改写
void Electric_Consume_Calculation(uint8 Pow_Down) 
{

    // sint16 Ins_Ele_1000;      //瞬时电耗1000ms      0.0~1000.0KWh/100Km 0.1千瓦时每百公里     (0~10000Wh/Km)瓦时每公里
    // Electric_Consume_Calculation_Times 0~999
    //uint16 B_Voltage，sint16 B_Current   0.1V 0.1A
    // Vehicle_Speed=( abs(Motor_Speed_Average) /Motor_Vehicle_Speed_Scale_Get)+50;
    
    //uint8 Times_10=0;
    //uint8 Times_100=0; 
    //uint8 i=0;
    //sint32  Ins_Ele_Tmp=0,Ins_Ele_Tmp_Mathc=0;
    uint32  Vehicle_Speed_Tmp=0;  //0.00 km/h  瞬时车速 
    
    //uint32 Vehicle_Speed_Add=1;     //车速积分累加值    0.1Km/h
    //sint32 Electric_Consume_Add=0;  //电功率积分累加值    0.01KW
    //uint32 Vehicle_Speed_Avg_Add=1;     // 平均 车速积分累加值  0.1Km/h
    //sint32 Electric_Consume_Avg_Add=0;  //平均 电功率积分累加值  0.01KW
    /*
    sint32 AV_Ele_Calc=0; //平均电耗计算值
    uint16 Electric_Consume_Calculation_Avg_Times=0; //平均能耗计算时钟 0~3600(2599)    
    uint16 Ins_Ele_NoCalc_Times=0;  //瞬时电耗未计算次数  *1s
    uint16 AV_Ele_NoCalc_Times=0;   //平均电耗未计算次数  10min
    
          AV_Ele_Calc_Max_Times  600 // 平均电耗计算时间 单位：秒
    uint8 Remaining_Mil;//剩余里程 0～255km      从V1.2版开始 2km/bit 0~510
    uint16 Rated_Energy; //额定能量 0~409.5kwh
    uint8 SOC; 0~255    0.5%
    */
    
    //防止最小值 除数为0

    
    if ( ((abs (Motor_Speed_Average))==0) || (Motor_Vehicle_Speed_Scale_Get==0) )
    {       
        Vehicle_Speed_Tmp=0; //最小0.0Km/h     瞬时车速 
        //Ins_Ele_Tmp=0;

    }else 
    {
        Vehicle_Speed_Tmp=((uint32)(abs(Motor_Speed_Average)) * 10   /Motor_Vehicle_Speed_Scale_Get)   ;
        //当前瞬时油耗
        //Ins_Ele_Tmp=  (((sint32)Output_Voltage) * B_Current    / Vehicle_Speed_Tmp)/10;       
    }
   
    Vehicle_Speed_Add   = Vehicle_Speed_Add + Vehicle_Speed_Tmp ; 
    
    //Electric_Consume_Add= Electric_Consume_Add + ((sint32)Output_Voltage) * B_Current;
    Electric_Consume_Add= Electric_Consume_Add + ((sint32)Output_Voltage) * B_Current/1000;  //500.0V*1.0A=500.00W =0.50KW
    //1秒计数
    Electric_Consume_Calculation_Times=(Electric_Consume_Calculation_Times+1)  %100;   //0~99
    if (Electric_Consume_Calculation_Times==0) //达到1秒，计算瞬时电耗
    {   
         //1秒内车辆有行驶、600秒未行驶、能耗达到计算上下限,则计算并传递、清零 ;   
        if ( (Vehicle_Speed_Add>1500)    //1.5km/h 每秒积分
          || (Ins_Ele_NoCalc_Times>=  AV_Ele_Calc_Max_Times-1) 
          || (Electric_Consume_Add>=  2000000000/AV_Ele_Calc_Max_Times ) 
          || (Electric_Consume_Add<= -2000000000/AV_Ele_Calc_Max_Times ) )       
        
        {
            
            
            Vehicle_Speed_Avg_Add     = Vehicle_Speed_Avg_Add + Vehicle_Speed_Add-1;
            Electric_Consume_Avg_Add  = Electric_Consume_Avg_Add  + Electric_Consume_Add;

            //有行驶才刷新仪表            
            if (Vehicle_Speed_Add>600)
            {   
                //秒平均电耗
                Ins_Ele_1000=  ( Electric_Consume_Add / (Vehicle_Speed_Add/100)  );
                
                //仪表显示格式化  
                if (Ins_Ele_1000>1275) //极值限制 127.5 kwh/100km
                {
                    Ins_Ele=255;
                }else                 
                {
                    if (Ins_Ele_1000<0)
                    {
                        Ins_Ele=0;
                    }else
                    {
                        Ins_Ele=(uint8) (Ins_Ele_1000/5);
                    }
                }
            }

            Vehicle_Speed_Add=1;
            Electric_Consume_Add=0;
            Ins_Ele_NoCalc_Times=0;

        }else
        {   //无行驶，则继续累加不清零 不计算
            Ins_Ele_NoCalc_Times++;
        }
        
        
        
        
        Electric_Consume_Calculation_Avg_Times=(Electric_Consume_Calculation_Avg_Times+1)  %AV_Ele_Calc_Max_Times; //0~599  10分钟 AV_Ele_Calc_Max_Times=600
        //达到10分钟 计算平均电耗
        if (Electric_Consume_Calculation_Avg_Times==0)
        {
            //车辆有行驶、1小时未行驶、能耗达到计算上下限,则计算并传递、清零
            if  ((Vehicle_Speed_Avg_Add>1500*AV_Ele_Calc_Max_Times)      //1.5km/h  10分钟积分   250米
              ||(AV_Ele_NoCalc_Times>= (3600/AV_Ele_Calc_Max_Times)-1)
              ||(Electric_Consume_Avg_Add >=2000000000) 
              ||(Electric_Consume_Avg_Add<= -2000000000) )
            
            {   

                //有行驶才刷新仪表  刷新平均能耗
                if (Vehicle_Speed_Avg_Add>1500*AV_Ele_Calc_Max_Times)
                {
                    //10%步进
                    //AV_Ele_Calc=9*(AV_Ele_Calc/10) +   ( Electric_Consume_Add  / (Vehicle_Speed_Add/100)  ) /10 ;
                    AV_Ele_Calc= Electric_Consume_Add  / (Vehicle_Speed_Add/100)   ;
                    //仪表显示格式化
                    if (AV_Ele_Calc>1275) //极值限制 127.5
                    {
                        AV_Ele=255;
                    }else                 
                    {
                        if ((AV_Ele_Calc<0)||( AV_Ele_Calc<((sint32)AV_Ele_Calc_Min))  )
                        {
                            //AV_Ele=(uint8)AV_Ele_Calc_Min/5;
                            AV_Ele=(uint8)((uint16)AV_Ele*9/10+   ( AV_Ele_Calc_Min/5)/10 );
                        }else
                        {
                            AV_Ele=(uint8)((uint16)AV_Ele*9/10+   ( AV_Ele_Calc/5)/10 );
                        }
                    }
                    Vehicle_Speed_Avg_Add=1;
                }
                //清除数据，重新累计
                Electric_Consume_Avg_Add=0;
                AV_Ele_NoCalc_Times=0;
            }else
            {
                AV_Ele_NoCalc_Times++;
            }
            
            //剩余里程计算
            /*
            uint8 Remaining_Mil;//剩余里程 0～255km      从V1.2版开始 2km/bit 0~510
            uint16 Rated_Energy; //额定能量 0~409.5kwh
            uint8 SOC; 0~255    0.5%
            AV_Ele_Calc  0.0~1000.0KWh/100Km 0.1千瓦时每百公里
            */            
            if ((SOC>0)&&(Rated_Energy>0)&&(AV_Ele>0)  )
            {

               Remaining_Mil_Calc=  100* ( ((sint32) Rated_Energy*SOC/200) )/ AV_Ele ;

               
              
               if ((Remaining_Mil_Calc > 510)||(Remaining_Mil_Calc<0)  )
               {
                  Remaining_Mil=255;
               }else
               {
                  Remaining_Mil= (uint8) (Remaining_Mil_Calc /2);               
               }
                
                           
            }else
            {
                Remaining_Mil=0;
            }
            
            
                    
        }
    }
   
    /*   
   
    Times_10=Electric_Consume_Calculation_Times%10; //0~9
    Times_100=(Electric_Consume_Calculation_Times/10) %10; //0~9

    Ins_Ele_10[Times_10]=(sint16)Ins_Ele_Tmp;

    if (Times_10==9) //100ms  
    {
      
      Ins_Ele_Tmp_Mathc=0;
      for (i=0;i<10;i++) 
      {
          Ins_Ele_Tmp_Mathc=Ins_Ele_Tmp_Mathc+ Ins_Ele_10[i];
      }
      //100ms平均
      Ins_Ele_100[Times_100]=(sint16) (Ins_Ele_Tmp_Mathc/10);
     
      if (Times_100==9) //1000ms
      {
          Ins_Ele_Tmp_Mathc=0;      
          for (i=0;i<10;i++) 
          {
              Ins_Ele_Tmp_Mathc=Ins_Ele_Tmp_Mathc+ Ins_Ele_100[i];
          }         
          //1s平均
          Ins_Ele_1000=(sint16)(Ins_Ele_Tmp_Mathc/10);
      }
      
    } 
    */
    //改到前面计数
    //Electric_Consume_Calculation_Times=(Electric_Consume_Calculation_Times+1)  %100;   //0~99 
    
    // 下电强制计算用于存储EEPROM
    if ((Pow_Down==1)&&(Vehicle_Speed_Avg_Add>1500*AV_Ele_Calc_Max_Times))
    {
        //有行驶才刷新仪表  刷新平均能耗
                
        AV_Ele_Calc=   Electric_Consume_Add  / (Vehicle_Speed_Add/100)     ;
        
        //仪表显示格式化
        if (AV_Ele_Calc>1275) //极值限制 127.5
        {
            AV_Ele=255;
        }else                 
        {
            if ((AV_Ele_Calc<0)||( AV_Ele_Calc<((sint32)AV_Ele_Calc_Min))  )
            {
                AV_Ele=(uint8)((uint16)AV_Ele*9/10+   ( AV_Ele_Calc_Min/5)/10 );
            }else
            {
                AV_Ele=(uint8)((uint16)AV_Ele*9/10+   ( AV_Ele_Calc/5)/10 );
            }
        }        
        //清除数据，重新累计
        Vehicle_Speed_Avg_Add=1;
        Electric_Consume_Avg_Add=0;
        AV_Ele_NoCalc_Times=0;
    }
}









