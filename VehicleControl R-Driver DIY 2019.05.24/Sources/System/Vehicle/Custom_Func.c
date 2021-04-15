/*******************************************************************************
* 工程名称：自定义函数集合
* 文件名： Custom_Func.c
* 功能描述 ： 车辆各状态转矩计算
* 引用记录：
* 备注： 
* 修订记录：
*		日期		      版本		  作者		描述
*   
*******************************************************************************/
#include "GlobalDef.h"
#include "Custom_Func.h"

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

sint16 Motor_Speed_Buffer[80]={0}, Motor_Speed_Delta,Vmh_Delta_Target,Vmh_Delta_Vehco;//转速缓冲、转速差、目标AD值、车辆AD值

sint16 Vmh_Delta_Target_Crawl_D,Torque_Increment_Crawl_D_Get;
sint16 Vmh_Delta_Target_FeedBack_Break,Torque_Increment_FeedBack_Break;
sint16 Vmh_Delta_Target_Parking_D,Torque_Increment_Parking_D;
sint16 Vmh_Delta_Target_Parking_R,Torque_Increment_Parking_R;



sint16 Motor_Torque_Buffer[80]={0};
sint16 Motor_Speed_Average_Buffer[80]={0};
sint16 Motor_Speed_Average;//平均转速
sint16 Motor_Torque_Average;//平均转矩
sint16 Torque_Increment;//转矩增量
sint16 Vmh_Delta_Vehco_Average_R=0,Vmh_Delta_Vehco_Average_D=0;//平均车辆AD值
sint32 Vmh_Delta_Vehco_Average_D_500=0,Vmh_Delta_Vehco_Average_R_500=0;  ////平均车辆AD值500倍中间值
uint16  Crawl_D_Max_Torque;//蠕行扭矩峰值
sint16 Crawl_D_Torque_New;//,Crawl_D_Torque_Buffer; //蠕动转矩
sint16 AccPed_Torque;//根据油门对应转矩
sint16 Auto_Parking_D_Torque,Auto_Parking_R_Torque;//驻坡转矩  
uint16 AccPed_Torque_Buffer;//油门转矩缓冲
uint16 AccPed_Slope_Delay_2key=0;
uint8 AccPed_Torque_ModeDelay=0; //油门模式进入0Nm缓冲
uint8 AccPed_Slope_Delay_Num=0; //进入油门给定斜坡周期 0~250

//D档滑行回馈
sint16 FeedBack_Torque_Coast;  //滑行回馈转矩
sint16 FeedBack_Torque_Coast_Buffer;  //滑行回馈转矩  10倍
sint16 FeedBack_Torque_Coast_Buffer_Quit; 
uint8 FeedBack_Coast_ModeDelay;
//uint8 FeedBack_Diff;//有效百分比
uint8 FeedBack_Slope_Coast_Enable;
uint8 FeedBack_Slope_Enable;//回馈起始斜坡开关
uint16 FeedBack_Coast_QuitDelay;



//D档制动回馈
sint16 FeedBack_Torque_Break; //制动回馈转矩
sint16 FeedBack_Torque_Break_Buffer;// 制动回馈转矩 10倍
sint16 FeedBack_Torque_Break_Buffer_Quit;
uint8 FeedBack_Break_ModeDelay=0;

//sint16 FeedBack_Break_Power;  //制动回馈电功率 100倍
uint16 FeedBack_Break_QuitDelay; //制动回馈退出斜坡延迟

//D蠕动
uint16 Crawl_D_ModeDelay;



//功率限制
sint16 Driver_Power;//电机电功率  100倍
sint16 Break_Torque_Limit=0;//回馈限制力矩
sint16 Bms_Max_Torque_Break; //BMS限制回馈转矩
sint16 Motor_Max_Torque_Break;//电机限制回馈转矩

sint16 Driver_Torque_Limit=0;//驱动限制力矩
sint16 Bms_Max_Torque_Driver; //BMS限制驱动转矩
sint16 Motor_Max_Torque_Driver;//电机限制驱动转矩
sint32 Bms_Max_Torque_Break_Tmp=0;  //BMS限制制动转矩计算值
sint32 Bms_Max_Torque_Driver_Tmp=0; //BMS限制驱动转矩计算值
uint16 Motor_Efficieny_Fix; //电机效率自动修正值
uint16 Motor_Efficieny_Pow_Limit=0;//电机修正后效率
//驻坡
sint16 Auto_Park_Speed_Accumulation;//退行转速累加
uint16 Auto_Park_Speed_Times;//退行增速时间累加
//限速
uint8 Limit_P=0;
extern uint8 Pow_Down_Enable;//故障限功率标志
extern uint8 MCU_Fault_Level;

extern uint8 bDiSatus[24];
extern sint16 Motor_Speed,Motor_Torque,Demand_Torque;
extern uint16 AccPed, Breakk;//油门、制动踏板状态
extern uint8 Dang_L4bit,Dang_P;//档位状态   1 5 0 10 PRDN
extern uint8 Err_AccPed;//油门故障
extern uint8 Vehco_State;
extern uint8 Veh_Limit_Speed;//巡航、限制车速  0.5km/h/bit 0~127.5    从V1.2版开始 1km/bit 0~180
//仪表LEDS2 8个bit 真空罐故障、限速模式、定速巡航模式、电机过热、回馈模式、系统故障、Ready灯
extern uint8 Err_Vacuum_Tank,Limit_Mode,Constant_Mode,MCU_OverHeat,FeedBack_Mode,System_Fault,Ready;
extern uint8 SOC;
extern uint16 B_Voltage,Output_Voltage;            //电池电压、输出电压  0~800 单位： V， 偏移： 0 ，   比例尺： 0.2 （10倍放大计算，0～8190 4095*2=8190）
//extern sint16 B_Current;                           //电池电流         -500~500 单位： A， 偏移： -500 ，比例尺： 0.02 (10倍放大计算-5000～5000）正电流表示对电池充电，负电流表示电池放电
//extern uint16 B_Charge_Current,B_Discharge_Current;//电池充、放电电流   0～500 单位： A， 偏移：0 ，    比例尺： 0.02（放大100倍计算 0～50000）
extern sint16 B_Power;                             //电池充放功率     -325~325 单位： kW，偏移： -325， 比例尺： 0.01 (100倍放大计算）正功率表示对电池充电， 负功率表示电池放电    

//********************************************************************
    //运行模式标识  0 自由滑行  
    //1 D档蠕动  2 D档滑行回馈 3 D档制动回馈  4 D档坡道驻车 
    //5 R档蠕动  6 R档滑行回馈 7 R档制动回馈  8 R档坡道驻车
    //9 前行(油门给定力矩）10 倒车 (油门给定力矩）
extern uint8 Torque_Mode;

extern uint16 Max_Charge_Current_Limit,Max_Charge_Power_Limit,B_Voltage;
extern uint16 Max_Discharge_Current_Limit,Max_Discharge_Power_Limit;
extern sint16 Torque_Low_Limit,Torque_High_Limit;

extern uint8  ABS_Active;//ABS激活信号
extern uint8 Err_Eps,Oil_Pump_Install,Oil_Pump_Fault;
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
extern  uint16 * __far Speed_Max_Torque_Crawl_D_Break_Addr;  //D档蠕行制动状态下最大驱动力矩
extern  uint16 * __far AutoPark_Motor_Speed_Torque_Addr;   //驻坡最大力矩

     //根据车型选择单值标定值
extern  uint16 Motor_Vehicle_Speed_Scale_Get;//转速-车速比例
extern  uint16 Demand_Limit_High_Get;//车速上限
extern  uint16 Demand_Limit_Low_Get;//车速下限
extern  uint16 Crawl_D_Speed_Max_Get;//从其他状态进入蠕动的速度  RPM
extern  uint8 Crawl_D_Torque_Delay_Max_Get;//蠕动更新间隔  单位10ms
extern  uint8 Speed_Buffer_Number_Get; //10ms车辆速度采样缓冲次数
extern  uint8 Speed_Average_Number_Get; //10ms平均车速缓冲次数
extern  uint8 Torque_Buffer_Number_AutoPark_Get;  //10ms车辆平均转矩采样缓冲次数 驻坡
extern  uint8 D_Park_Start_Speed_Get;//驻坡触发转速
extern  sint16 Auto_Park_Speed_Acc_Max_Get; //激活D驻坡最大退行转速累加
extern  uint16 Auto_Park_Speed_Times_Max_Get;//激活D驻坡最大退行增速时间 单位10ms
extern  uint8 Vehco_Type_Test;
extern  sint16 Auto_Torque_Min_Get,Auto_Torque_Max_Get; //自动转矩最小正值 自动转矩最大负值
extern  uint16 R_Vhms_Average_Ratio_Get,D_Vhms_Average_Ratio_Get;//Vhms平均值缓冲比例

//远程锁车
extern uint8 Lock_Vehicle_Active; //锁车状态激活
 

//第三方锁车专用
extern uint8 Third_Lock_Active;              //锁车激活
extern uint8 Third_Lock_Mode_Active;         //生效锁车模式
extern uint8 Third_Lock_LimitSpeed_Active;   //生效限速速度

//充电互锁
extern uint8   No_Ins_Charge_CAN;//充电期间禁止INS特殊帧发送


//换挡
extern uint8 Shift_Gears,Gears_Install,Shift_Gears_Enable,Gears_State;
extern uint16 Motor_Vehicle_Speed_Scale_Gears_Out_Get ;
extern sint16 Gears_Speed;
extern uint8 Gears_Shoft_Torque_State,Gears_Shoft_Torque_Delay;  //换挡过渡状态，换挡过渡延时
extern sint32 Gears_Torque_Buffer; //换挡过渡扭矩  0.1

    #ifndef AECS_HW
      #pragma CODE_SEG DEFAULT
    #else  
      #pragma CODE_SEG VCU_CODE 
    #endif

//车辆模式判断
void Torque_Mode_Judge(void) 
{
  
    
    
    switch(Dang_L4bit)  //根据档位
    {
        case 10://D
          
            //滑行回馈
            if ((Motor_Speed>(sint16)Motor_Vehicle_Speed_Scale_Get*7)&&(AccPed<50)&&(Breakk==0)&&(Limit_P==0))
            {   
                FeedBack_Coast_QuitDelay=10;
                switch (Torque_Mode)
                {
                    case 0:
                    case 9: 
                    case 1:   //从蠕行、油门和滑行进入滑行回馈
                        FeedBack_Coast_ModeDelay=0;         //0 Nm缓冲
                        FeedBack_Slope_Coast_Enable=1; //斜坡进入
                        FeedBack_Torque_Coast=0;
                        FeedBack_Torque_Coast_Buffer=0;
                        Torque_Mode=2; //D滑行回馈
                       
                        break;
                    case 3:
                        //从制动进入滑行回馈
                        if    (((Gears_Install==1)&&(FeedBack_Torque_Break>=-1*(Intpol_s16s16(FeedBack_Coast_D_Torque_Addr,Gears_Speed))))
                              ||((Gears_Install==0)&&(FeedBack_Torque_Break>=-1*(Intpol_s16s16(FeedBack_Coast_D_Torque_Addr,Motor_Speed_Average)))) )  //从制动回馈进入滑行回馈
                        {
                            //FeedBack_Coast_ModeDelay=FeedBack_Break_ModeDelay;//跳过0 Nm
                            
                            FeedBack_Coast_ModeDelay=FeedBack_Coast_ModeDelay_Max;  //跳过0 Nm
                            
                            FeedBack_Slope_Coast_Enable=1; //no斜坡进入
                            //FeedBack_Torque_Coast=Motor_Torque;
                            //FeedBack_Torque_Coast_Buffer=Motor_Torque*10;
                            FeedBack_Torque_Coast=FeedBack_Torque_Break;
                            FeedBack_Torque_Coast_Buffer=FeedBack_Torque_Break*10;
                            FeedBack_Break_QuitDelay=0;
                            Torque_Mode=2; //D滑行回馈
                        }
                        break;
                    default:
                       Torque_Mode=2; //D滑行回馈
                      
                       break;
                 }
            }
            
            //制动回馈
            if ((Motor_Speed>(sint16)Motor_Vehicle_Speed_Scale_Get*7)&&(AccPed<FeedBack_Break_Max_AccPed)&&(Breakk!=0))   
            {
                FeedBack_Break_QuitDelay=10;
                switch (Torque_Mode)
                {
                    case 9:
                    case 0: 
                    case 1://从油门、蠕行和滑行进入制动回馈
                        FeedBack_Break_ModeDelay=0;  //0 Nm缓冲
                        FeedBack_Slope_Enable=1; //斜坡进入
                        FeedBack_Torque_Break=0;
                        FeedBack_Torque_Break_Buffer=0;
                        Torque_Mode=3; //D制动回馈
        
                        
                       
                        break;
                    default:
                        Torque_Mode=3; //D制动回馈
                       
                        break;
                    case 2: //从滑行回馈进入制动回馈
                        FeedBack_Break_ModeDelay=FeedBack_Coast_ModeDelay;//跳过0 Nm
                        FeedBack_Slope_Enable=1; //斜坡进入
                        FeedBack_Torque_Break=FeedBack_Torque_Coast;
                        FeedBack_Torque_Break_Buffer=FeedBack_Torque_Coast*10;
                        Torque_Mode=3; //D制动回馈
     
                        break;
                }    
            }

            
            //D坡道驻车
            if (  (Auto_Park_Speed_Accumulation<Auto_Park_Speed_Acc_Max_Get)   
                &&(Auto_Park_Speed_Times>=Auto_Park_Speed_Times_Max_Get)
                &&(Breakk<=100)   
                &&(Auto_Parking_D_Torque_Enable==1)
                &&(Motor_Speed<-1*D_Park_Start_Speed_Get)
                &&(Vmh_Delta_Vehco<-2) 
                &&(Dang_P==0))
            {
                Torque_Mode=4; //D坡道驻车
                FeedBack_Break_QuitDelay=0;
                FeedBack_Coast_QuitDelay=0;   
            }


            //D蠕动 
            if (   (   ((Gears_Install==0)&&(Motor_Speed<Crawl_D_Speed_Max_Get))
                ||    ((Gears_Install==1)&&(Motor_Speed<Crawl_D_Speed_Max_Get)
                        //&&(Motor_Speed/Motor_Vehicle_Speed_Scale_Get <5)
                         )  )
                
                &&(AccPed<100)&&(Dang_P==0)) 
            {
                switch(Torque_Mode) 
                {
                    case 4: //从驻坡切换到D蠕动
                        if (((Crawl_D_Torque_New>Auto_Parking_D_Torque)&&(Vmh_Delta_Vehco>=0))||(Auto_Parking_D_Torque_Enable==0)   )
                        {
                            Torque_Mode=1;
                           
                        }
                        break;
                    case 3: //从制动回馈切换到D蠕动 
                        if (((Breakk==0)&&(Motor_Torque==0))||(FeedBack_Break_QuitDelay==0)) 
                        {
                            Crawl_D_ModeDelay=0;
                            Torque_Mode=1;
                                                 
                        }                       
                        break;
                    case 2: //从滑行回馈切换到D蠕动 
                        Crawl_D_ModeDelay=0;
                        Torque_Mode=1;
                       
                        break;
                    case 9:   //从油门D切换到D蠕动  不需要进入延时
                        //Crawl_D_ModeDelay=0;
                        Torque_Mode=1;
                       
                        break;
                    case 0: //从自由滑行切换到D蠕动
                        if (Motor_Torque==0)
                        {
                            Crawl_D_ModeDelay=0;
                            Torque_Mode=1;
                           
                        }
                        break;
                    default:
                        Torque_Mode=1;  //D蠕动
                          
                        break; 
                }
            } 


            
            if (AccPed>=50)  //油门给定模式 
            {
                /*
                AccPed_Torque_ModeDelay  //油门模式进入0Nm缓冲
                AccPed_Slope_Delay_Num   //油门进入斜坡缓冲
                */
                switch (Torque_Mode) 
                {
                  default:
                      Torque_Mode=9;
                      break;
                  case 9: 
                      if (AccPed==0) //油门模式中松过油门
                      {                       //无0有斜坡
                        //  AccPed_Torque_ModeDelay=AccPed_ModeDelay;
                          AccPed_Slope_Delay_Num=0;
                          AccPed_Torque_Buffer=0;
                          AccPed_Torque=0;                        
                      }
                      break;
                  case 4:  //从驻坡切换到前行  无0无斜坡
                      if (((AccPed_Torque>Auto_Parking_D_Torque)&&(Vmh_Delta_Vehco>=0))||(Auto_Parking_D_Torque_Enable==0)||(Dang_P==1)  )
                      {
                          Torque_Mode=9;
                       //   AccPed_Torque_ModeDelay=AccPed_ModeDelay;
                       //   AccPed_Slope_Delay_Num=AccPed_Slope_Delay;
                          AccPed_Torque_Buffer=AccPed_Torque*10;
                      }
                      break;
                  case 1: //从蠕动切换到前行  无0无斜坡
                      if (AccPed_Torque>=Crawl_D_Torque_New) 
                      {
                          Torque_Mode=9;
                        //  AccPed_Torque_ModeDelay=AccPed_ModeDelay;
                        //  AccPed_Slope_Delay_Num=AccPed_Slope_Delay;
                          AccPed_Torque_Buffer=AccPed_Torque*10;
                      }
                      break;
                  case 0: //从滑行切换到前行  无0有斜坡
                      Torque_Mode=9;
                    //  AccPed_Torque_ModeDelay=AccPed_ModeDelay;
                      AccPed_Slope_Delay_Num=0;
                      AccPed_Torque_Buffer=0;                  
                      AccPed_Torque=0; 
                      break;
                  case 2:   //从滑行回馈切换到前行  有0有斜坡
                      if ((Demand_Torque==0)||(FeedBack_Coast_QuitDelay==0)) 
                      {
                          Torque_Mode=9;
                          AccPed_Torque_ModeDelay=0;
                          AccPed_Slope_Delay_Num=0;  
                          AccPed_Torque_Buffer=0;
                          AccPed_Torque=0;                     
                      }
                      break;
                  case 3:   //从制动回馈切换到前行  有0有斜坡
                       if ((Demand_Torque==0)||(FeedBack_Break_QuitDelay==0)) 
                       {
                          Torque_Mode=9;
                          AccPed_Torque_ModeDelay=0;
                          AccPed_Slope_Delay_Num=0;                      
                          AccPed_Torque_Buffer=0;
                          AccPed_Torque=0;     
                       }
                  
                      break;
                }
            } else 
            {
                if (Torque_Mode==9) //油门模式中松油门  *拉着手刹无法进入滑行回馈
                {
                
                
                }
            
            }
            
            if (Vehco_State==5)//行驶中丢失ON信号
            {
                Torque_Mode=0; //滑行
               
                FeedBack_Break_QuitDelay=0;
                FeedBack_Coast_QuitDelay=0;
            }
            if ((MCU_Fault_Level==2)&&(Vehco_State==4)) // 电机2级故障，0转矩滑行
            {
                Torque_Mode=0; //滑行
               
                FeedBack_Break_QuitDelay=0;
                FeedBack_Coast_QuitDelay=0;

            }
            
            break;
        case 5://R
            

            if (AccPed>100)   
            {
                if (Torque_Mode==8) 
                {
                    if (((AccPed_Torque<Auto_Parking_R_Torque)&&(Vmh_Delta_Vehco<=0))||(Auto_Parking_R_Torque_Enable==0)  )
                    {
                         Torque_Mode=10;//从驻坡切换到倒车
                    }
                }
                else
                {
                    Torque_Mode=10;//倒车
                }
            }else 
            {
                Torque_Mode=10;
            }
            
            if ((Motor_Speed>50)&&(Vmh_Delta_Vehco>10)&&(Auto_Parking_R_Torque_Enable==1) )
            {
                Torque_Mode=8; //R坡道驻车    
            }
            
            if (Vehco_State==5)//行驶中丢失ON信号
            {
                Torque_Mode=0; //滑行
            }            
          
            if ((MCU_Fault_Level==2)&&(Vehco_State==4)) // 电机2级故障，0转矩滑行  
            
            {          
                Torque_Mode = 0; //滑行
               
                FeedBack_Break_QuitDelay=0;
                FeedBack_Coast_QuitDelay=0;

            }   
            break;
            
         
        case 0:  //N
        case 1:  //P
        default:
            Torque_Mode=0; //滑行
            break;      
    }

    
    //蠕行、倒挡、限速与驻坡期间禁止换挡
    switch(Torque_Mode) //根据车辆模式选择转矩处理程序
    {
        case 1: //D蠕动   
            Demand_Torque=Crawl_D_Torque_New;
            /*if ((Shift_Gears==0)&& (Gears_State!=0)&&(Gears_State!=15))
            {
                Shift_Gears_Enable=0;
            }else
            {
               Shift_Gears_Enable=1;
            } */
            Shift_Gears_Enable=1;
            break;      
        case 2: //D滑行回馈
            Demand_Torque=FeedBack_Torque_Coast;
            Shift_Gears_Enable=1;
            break;      
        case 3: //D制动回馈   
            Demand_Torque=FeedBack_Torque_Break;
            Shift_Gears_Enable=1;
            break;      
        case 4: //D坡道驻车 
            Demand_Torque=Auto_Parking_D_Torque;
            if ((Shift_Gears==0)&& (Gears_State!=0) &&(Gears_State!=15))
            {
                Shift_Gears_Enable=0;
            }else
            {
               Shift_Gears_Enable=1;
            }
            break;      
        case 10: //倒车
            Demand_Torque=AccPed_Torque;
            if ((Shift_Gears==0)&&( ( (Gears_State!=0)&&(Vehco_Type_Test!=16)   )  ||(  (Gears_State==15)&&(Vehco_Type_Test==16) )    ) ) 
            {
                Shift_Gears_Enable=0;
            }else 
            {
                Shift_Gears_Enable=1;
            }
            break; 
        case 9:  //前行    
            Demand_Torque=AccPed_Torque;
            Shift_Gears_Enable=!Limit_Mode;
            
            //Shift_Gears_Enable=1;
            break; 
        case 5: //R蠕动
            Demand_Torque=0;
            /*
            if ((Shift_Gears==0)&&( ( (Gears_State!=0)&&(Vehco_Type_Test!=16)   )  ||(  (Gears_State==15)&&(Vehco_Type_Test==16) )    ) ) 
            {
                Shift_Gears_Enable=0;
            }else 
            {
                Shift_Gears_Enable=1;
            }
            */
            Shift_Gears_Enable=1;
            break;      
        case 8: //R坡道驻车
            Demand_Torque=Auto_Parking_R_Torque;
            if ((Shift_Gears==0)&&( ( (Gears_State!=0)&&(Vehco_Type_Test!=16)   )  ||(  (Gears_State==15)&&(Vehco_Type_Test==16) )    ) ) 
            {
                Shift_Gears_Enable=0;
            }else 
            {
                Shift_Gears_Enable=1;
            }
            break;                           
     

        case 0: //滑行      非0时加斜坡降0
        default:
            Demand_Torque=0;
            Shift_Gears_Enable=1;
            break;
    }
    
    if   (Shift_Gears_Enable_Debug!=2 )  {  Shift_Gears_Enable= Shift_Gears_Enable_Debug;    }


    //换挡后扭矩平滑过渡处理
    Gears_Shoft_Torque();

    //多档箱空挡禁止扭矩输出
    if ((Gears_State==0)&&(Gears_Install==1)) 
    {
        Demand_Torque=0;
    }
    
    //电池功率限制转矩
    Torque_Power_Limit();
    //充电期间禁止行驶
    if (No_Ins_Charge_CAN==1)
    {
        Demand_Torque=0;
        if (Vehco_State==4)
        {                                      
            Vehco_State=1;
        }
    }
}

//换挡后扭矩平滑过渡处理
void Gears_Shoft_Torque (void) 
{   /*
    uint8 Gears_Shoft_Torque_State=0,Gears_Shoft_Torque_Delay=0;  //换挡过渡状态，换挡过渡延时
    sint32 Gears_Torque_Buffer=0; //换挡过渡扭矩  0.1
    */
    if ((Gears_Shoft_Torque_State==1)&&(Gears_Shoft_Torque_Delay< AccPed_Slope_Delay))
    {
        if((Demand_Torque>0) && (Gears_Torque_Buffer>0)  &&  ((sint32)Demand_Torque*10 >Gears_Torque_Buffer  ) ) 
        {
            Gears_Torque_Buffer=Gears_Torque_Buffer+ AccPed_Torque_Diff_Add;
            Gears_Shoft_Torque_Delay++;
            Demand_Torque= (sint16)(Gears_Torque_Buffer/10);            
            
        }else if ((Demand_Torque<0) && (Gears_Torque_Buffer<0) &&((sint32)Demand_Torque*10<Gears_Torque_Buffer  )   ) 
        {
            Gears_Torque_Buffer=Gears_Torque_Buffer - AccPed_Torque_Diff_Add;
            Gears_Shoft_Torque_Delay++;
            Demand_Torque= (sint16)(Gears_Torque_Buffer/10);            
            
        }else 
        {   //扭矩到了
            Gears_Torque_Buffer= (sint32)Demand_Torque*10;
            Gears_Shoft_Torque_State=0;
        }

    }else
    {   //Gears_Shoft_Torque_Delay斜坡时间跑到了 扭矩还不到  5% 递进  此时Gears_Shoft_Torque_State=1
        if (Gears_Shoft_Torque_State==1) 
        {
            if((Demand_Torque>0) && (Gears_Torque_Buffer>0)  &&  ((sint32)Demand_Torque*10 >Gears_Torque_Buffer  ) ) 
            {
                Gears_Torque_Buffer=Gears_Torque_Buffer+Demand_Torque/2;
                
            }else if ((Demand_Torque<0) && (Gears_Torque_Buffer<0) &&((sint32)Demand_Torque*10<Gears_Torque_Buffer  )   ) 
            {
                Gears_Torque_Buffer=Gears_Torque_Buffer-Demand_Torque/2;
            }else 
            {   //扭矩到了
                Gears_Torque_Buffer= (sint32)Demand_Torque*10;
                Gears_Shoft_Torque_State=0;
            }
            Demand_Torque=   (sint16)(Gears_Torque_Buffer/10); 
               
        }else
        {
            Gears_Shoft_Torque_Delay=AccPed_Slope_Delay+1;
            Gears_Shoft_Torque_State=0;
            //Gears_Torque_Buffer=0;          
        }
    
    }
    
}


 //电机速度缓冲处理
void Motor_Speed_Buffer_FIFO (void) 
{
    sint16 Vmh_Delta_Vehco_Tmp=0;
    sint32 Total_Speed=0;
    //Motor_Speed_Buffer[0]                   最老的速度  
    //Motor_Speed_Buffer[Speed_Buffer_Number_Get] 最新的速度 
    //Speed_Buffer_Number_AutoPark    Motor_Speed_Delta_AutoPark
    //sint16 Motor_Torque_Buffer[50]={0};
    //sint16 Motor_Torque_Average;//平均转矩
    //Torque_Buffer_Number_AutoPark_Get
    //Speed_Average_Number_Get
    //Motor_Speed_Average
    //Motor_Torque_Average
    //R_Vhms_Average_Ratio_M03 R档Vhms平均值缓冲比例
    //D_Vhms_Average_Ratio_M03 D档Vhms平均值缓冲比例
    //Vmh_Delta_Vehco_Average_R,Vmh_Delta_Vehco_Average_D;//平均车辆AD值
    
    uint8 Index=0;
    sint32 Motor_Speed_Average_Tmp=0,Motor_Torque_Average_Tmp=0;
  
    for(Index=79;Index>0;Index--)  //电机转速缓冲 FIFO 处理    80个速度 0~79  0 最新  79最旧 
    {
         Motor_Speed_Buffer[Index]=Motor_Speed_Buffer[Index-1];
    }
    Motor_Speed_Buffer[0]=Motor_Speed;
    Motor_Speed_Delta=Motor_Speed_Buffer[0]-Motor_Speed_Buffer[Speed_Buffer_Number_Get];  //缓冲期间转速差
    /*
    for (Index=1;Index<(Speed_Buffer_Number_Get+1);Index++)   
    {
        Total_Speed=Total_Speed+Motor_Speed_Buffer[Index];
    }
    */
    //Motor_Speed_Delta=Motor_Speed_Buffer[0]-(sint16)(Total_Speed/Speed_Buffer_Number_Get);  //缓冲期间转速差
    
    
    //Motor_Speed_Average=0;   //获取平均电机转速
    for(Index=0;Index<Speed_Average_Number_Get;Index++) 
    {
        Motor_Speed_Average_Tmp=Motor_Speed_Average_Tmp+Motor_Speed_Buffer[Index];
    }
    Motor_Speed_Average=(sint16)(Motor_Speed_Average_Tmp/Speed_Average_Number_Get);

    
    for(Index=79;Index>0;Index--)  //电机平均转速缓冲 FIFO 处理    80个速度 0~79  0 最新  79最旧 
    {
         Motor_Speed_Average_Buffer[Index]=Motor_Speed_Average_Buffer[Index-1];
    }
    Motor_Speed_Average_Buffer[0]=Motor_Speed_Average;
    
    if (Motor_Speed_Buffer_Type==1)//新式缓冲期间转速差  使用平均转速做转差
    {
        Motor_Speed_Delta=Motor_Speed_Average_Buffer[0]-Motor_Speed_Average_Buffer[Speed_Buffer_Number_Get];  
    }
   
    
    /*  平均转矩用不到 注销
    for(Index=Torque_Buffer_Number_AutoPark_Get;Index>0;Index--)  //电机转矩缓冲 FIFO 处理 1~Torque_Buffer_Number_AutoPark_Get  0最新  
    {
         Motor_Torque_Buffer[Index]=Motor_Torque_Buffer[Index-1];
         Motor_Torque_Average_Tmp=Motor_Torque_Average_Tmp+Motor_Torque_Buffer[Index];
    }
    Motor_Torque_Buffer[0]=Motor_Torque;
    //电机转矩平均值处理
    Motor_Torque_Average=(sint16)((Motor_Torque_Average_Tmp+Motor_Torque_Buffer[0])/(Torque_Buffer_Number_AutoPark_Get+1));
    */
    
       //未换挡或未装变速箱
    if ((Shift_Gears==0)||(Gears_Install==0))
    {
        Vmh_Delta_Vehco_Tmp=(sint16)((((sint32)Motor_Speed_Delta)*(1000/Speed_Buffer_Number_Get))/(sint16)Motor_Vehicle_Speed_Scale_Get); //车辆AD值  mh/10ms 
      
        if (Vmh_Delta_Vehco_Filter_Type==0)   //Vmh_Delta_Vehco过滤类型 0 老式82 、37过滤 1 无过滤
        {
            if (Vmh_Delta_Vehco_Tmp >Vmh_Delta_Vehco) 
            {
                  Vmh_Delta_Vehco=(Vmh_Delta_Vehco*8+Vmh_Delta_Vehco_Tmp*2)/10;
            }else
            
            {
                Vmh_Delta_Vehco=(Vmh_Delta_Vehco*3+Vmh_Delta_Vehco_Tmp*7)/10; 
            }
        }else 
        {
            Vmh_Delta_Vehco=Vmh_Delta_Vehco_Tmp;
        }


        //Vmh_Delta_Vehco_Average_R Vmh_Delta_Vehco_Average_D  R_Vhms_Average_Ratio_M03 D_Vhms_Average_Ratio_M03
        //R_Vhms_Average_Ratio_Get  B=B* (A-1)/A + C*1/A     
        //D档平均车辆AD值
        /*
        Vmh_Delta_Vehco_Average_D= 
              (Vmh_Delta_Vehco_Average_D * (D_Vhms_Average_Ratio_Get-1))/D_Vhms_Average_Ratio_Get
              + Vmh_Delta_Vehco/D_Vhms_Average_Ratio_Get;          
        */
        //Vmh_Delta_Vehco_Average_D_500

        Vmh_Delta_Vehco_Average_D_500=
              (Vmh_Delta_Vehco_Average_D_500 * (D_Vhms_Average_Ratio_Get-1)+ (sint32)Vmh_Delta_Vehco*500 )
              / D_Vhms_Average_Ratio_Get;
              
        Vmh_Delta_Vehco_Average_D= (sint16)(Vmh_Delta_Vehco_Average_D_500/500);

        //Vmh_Delta_Vehco_Average_D=(sint16) (((sint32)Vmh_Delta_Vehco_Average_D * (D_Vhms_Average_Ratio_Get-1) + Vmh_Delta_Vehco)/D_Vhms_Average_Ratio_Get);  





        //R档平均车辆AD值
        /*
        Vmh_Delta_Vehco_Average_R= 
              (Vmh_Delta_Vehco_Average_R * (R_Vhms_Average_Ratio_Get-1))/R_Vhms_Average_Ratio_Get
              + Vmh_Delta_Vehco/R_Vhms_Average_Ratio_Get;
        */
        // Vmh_Delta_Vehco_Average_R=(sint16)  (((sint32)Vmh_Delta_Vehco_Average_R * (R_Vhms_Average_Ratio_Get-1) + Vmh_Delta_Vehco)/R_Vhms_Average_Ratio_Get);

        Vmh_Delta_Vehco_Average_R_500=
              (Vmh_Delta_Vehco_Average_R_500 * (R_Vhms_Average_Ratio_Get-1)+ (sint32)Vmh_Delta_Vehco*500 )
              / R_Vhms_Average_Ratio_Get;
              
        Vmh_Delta_Vehco_Average_R= (sint16)(Vmh_Delta_Vehco_Average_R_500/500);
      
      
      
      }else 
    { //换档期间 不计算加速度
      

    } 

    
 
    
       



    
}


                                       
//D档滑行回馈转矩计算
void FeedBack_Coast (void) 
{
    sint32 FeedBack_Torque_Coast_Tmp=0;
    
    if ((AccPed>=50) || (SOC>199)||(Ready==0)||(Motor_Speed<=0)||(Vehco_State!=4)  ) 
    {
        FeedBack_Torque_Coast_Tmp=0;
    }else 
    {
        if (Gears_Install==0)   //直驱
        {
            FeedBack_Torque_Coast_Tmp=-1*(Intpol_s16s16(FeedBack_Coast_D_Torque_Addr,Motor_Speed_Average));
        }else                 //多档
        {   
            /*
            //变速器输出速比
            Motor_Vehicle_Speed_Scale_Gears_Out_Get 
            Motor_Vehicle_Speed_Scale_Get       
            Gears_Speed
            标定值为输出轴扭矩
            */
            FeedBack_Torque_Coast_Tmp=-1*(Intpol_s16s16(FeedBack_Coast_D_Torque_Addr,   Gears_Speed  ));
            FeedBack_Torque_Coast_Tmp=  FeedBack_Torque_Coast_Tmp * (sint16)Motor_Vehicle_Speed_Scale_Gears_Out_Get / (sint16)Motor_Vehicle_Speed_Scale_Get;
            
        }

    }
    

    
    if (FeedBack_Torque_Coast_Enable==1) 
    {
         // if (FeedBack_Coast_ModeDelay<(Speed_Buffer_Number_Get)/5 +4)      //滑行回馈 0 Nm 延时缓冲
         if (FeedBack_Coast_ModeDelay<FeedBack_Coast_ModeDelay_Max)      //滑行回馈 0 Nm 延时缓冲
         
         {
             // FeedBack_Diff=0;
              FeedBack_Torque_Coast=0;
              FeedBack_Coast_ModeDelay++;
         }else 
         {
             //进入时回馈力矩斜坡处理    
              if (FeedBack_Slope_Coast_Enable==1)
              {
                
              
                  if (FeedBack_Torque_Coast>FeedBack_Torque_Coast_Tmp) 
                  {
                      if (Gears_Install==0)   //直驱
                      {
                          FeedBack_Torque_Coast_Buffer=FeedBack_Torque_Coast_Buffer-FeedBack_Coast_Diff_Add * FeedBack_Diff_Rate; 
                      }else     //多档
                      {
                          FeedBack_Torque_Coast_Buffer=FeedBack_Torque_Coast_Buffer-FeedBack_Coast_Diff_Add * FeedBack_Diff_Rate * (sint16)Motor_Vehicle_Speed_Scale_Gears_Out_Get / (sint16)Motor_Vehicle_Speed_Scale_Get; 
                      }
                      FeedBack_Torque_Coast=FeedBack_Torque_Coast_Buffer/10;
                    
                 } 
                  else 
                 {
                      FeedBack_Slope_Coast_Enable=0;              
                 }
              
              }
              else 
              {
                    
                 FeedBack_Torque_Coast=(sint16)FeedBack_Torque_Coast_Tmp;
                 FeedBack_Torque_Coast_Buffer=0;
              }
         }
       
          //FeedBack_Coast_QuitDelay
         if (((Torque_Mode==2)&&(AccPed>=50))||(ABS_Active>0))  //踩油门踏板  力矩斜坡降0      慢退出   FeedBack_Coast_QuitDelay从10开始--
         {
             //退出滑行回馈斜坡延迟
             if ((Motor_Torque<-2)&&(FeedBack_Coast_QuitDelay>0)&&(Motor_Speed>(sint16)Motor_Vehicle_Speed_Scale_Get)) 
             {
                 //FeedBack_Torque_Coast_Quir_Add
                 //FeedBack_Torque_Coast=(Motor_Torque*7)/10;
                 if (Gears_Install==0)   //直驱
                 {
                    FeedBack_Torque_Coast_Buffer_Quit=FeedBack_Torque_Coast_Buffer_Quit + FeedBack_Torque_Coast_Quir_Add * FeedBack_Diff_Rate;
                 }else                    //多档
                 {
                    FeedBack_Torque_Coast_Buffer_Quit=FeedBack_Torque_Coast_Buffer_Quit + FeedBack_Torque_Coast_Quir_Add * FeedBack_Diff_Rate* (sint16)Motor_Vehicle_Speed_Scale_Gears_Out_Get / (sint16)Motor_Vehicle_Speed_Scale_Get; 
                 }
                 
                 
                 FeedBack_Torque_Coast=FeedBack_Torque_Coast_Buffer_Quit/10;
                 FeedBack_Coast_QuitDelay--;   // 每次减1   制动退出最多10周期
                 Torque_Mode=2;
            
             }else 
             {
                  FeedBack_Torque_Coast=0;
                  FeedBack_Coast_QuitDelay=0;
             }
         }
         else 
         {
              FeedBack_Torque_Coast_Buffer_Quit= FeedBack_Torque_Coast*10;
         }
         //制动力矩不为正
         if (FeedBack_Torque_Coast>0){  FeedBack_Torque_Coast=0;  }
      
    }

    else 
    {
         FeedBack_Torque_Coast=0;
    }
}

//D档制动回馈转矩计算
void FeedBack_Break (void) 
{ //要考虑 BMS限制功率  最大制动力矩曲线等等
  // Vmh_Delta_Target_FeedBack_Break  
  // Torque_Increment_FeedBack_Break
  // Speed_Vmh10ms_Break_Addr
  // Torque_Increment_Break_Addr
  // FeedBack_Torque_Break
  // FeedBack_Break_ModeDelay
  // Motor_Max_Torque_Break
  
 
  
  sint32 FeedBack_Torque_Break_Tmp=0;
  sint16 Motor_Max_Torque_Break_Table=0;
  sint16 Bms_MaxCharge_Power=0;
  sint16 Bms_MaxCharge_Power_Curr=0;
  //uint16 Motor_Efficieny=0;
  //sint32 FeedBack_Break_Power_Tmp=0;
  
  if ((SOC>199)||(Torque_Mode!=3)||(Breakk==0)||(Ready==0)||(Motor_Speed<=0)||(Vehco_State!=4)) 
  {
      FeedBack_Torque_Break=0;    
      //FeedBack_Break_Power=0;      
  } 
  else 
  {
     /*  //AD法
      Vmh_Delta_Target_FeedBack_Break=Intpol_s16s16(Speed_Vmh10ms_Break_Addr,Motor_Speed);
      Torque_Increment_FeedBack_Break=Intpol_s16s16(Torque_Increment_Break_Addr,Vmh_Delta_Target_FeedBack_Break-Vmh_Delta_Vehco);
      FeedBack_Torque_Break_Tmp=Motor_Torque+Torque_Increment_FeedBack_Break;
      FeedBack_Torque_Break=FeedBack_Torque_Break_Tmp;
     */  //定值查表法
      //FeedBack_Torque_Coast_Tmp=-1*(Intpol_s16s16(FeedBack_Coast_D_Torque_Addr,Motor_Speed));
      
      
      //FeedBack_Torque_Break_Tmp=-1*(Intpol_s16s16(FeedBack_Break_D_Torque_Addr,Motor_Speed_Average));
      if (Gears_Install==0)   //直驱
      {
          FeedBack_Torque_Break_Tmp=-1*(Intpol_s16s16(FeedBack_Break_D_Torque_Addr,Motor_Speed_Average));
      }else                 //多档
      {   
          /*
          //变速器输出速比
          Motor_Vehicle_Speed_Scale_Gears_Out_Get 
          Motor_Vehicle_Speed_Scale_Get       
          Gears_Speed
          标定值为输出轴扭矩
          */
          FeedBack_Torque_Break_Tmp=-1*(Intpol_s16s16(FeedBack_Break_D_Torque_Addr,   Gears_Speed  ));
          FeedBack_Torque_Break_Tmp=  FeedBack_Torque_Break_Tmp * (sint32)Motor_Vehicle_Speed_Scale_Gears_Out_Get / (sint32)Motor_Vehicle_Speed_Scale_Get;
          
      }     
     
     
     
     // FeedBack_Torque_Break=FeedBack_Torque_Break_Tmp;  
      //回馈电功率计算    最大210.00KW
      //FeedBack_Break_Power_Tmp= ((sint32)Motor_Torque)*(abs(Motor_Speed)+1);
      //FeedBack_Break_Power=(sint16)((FeedBack_Break_Power_Tmp*(Motor_Efficieny/100))/9550);      
  }
  
  
  
    
  if (FeedBack_Torque_Break_Enable==1) 
  {
      //if (FeedBack_Break_ModeDelay<(Speed_Buffer_Number_Get)/5 +4)      //进入制动回馈 0 Nm 延时缓冲
      if (FeedBack_Break_ModeDelay<FeedBack_Break_ModeDelay_Max)      //进入制动回馈 0 Nm 延时缓冲
      
      
      
      {
         // FeedBack_Diff=0;
          FeedBack_Torque_Break=0;
          FeedBack_Break_ModeDelay++;
      }else 
      {
          

          
          //进入时回馈力矩斜坡处理    
          if ((Breakk>100)&&((sint32)FeedBack_Torque_Break>FeedBack_Torque_Break_Tmp)&&(FeedBack_Slope_Enable==1)&& (Torque_Mode==3)) 
          {
              if (Gears_Install==0)   //直驱
              {
                  FeedBack_Torque_Break_Buffer=FeedBack_Torque_Break_Buffer-FeedBack_Break_Diff_Add * FeedBack_Diff_Rate;    
              }else                  //多档
              {
                  FeedBack_Torque_Break_Buffer=FeedBack_Torque_Break_Buffer-FeedBack_Break_Diff_Add * FeedBack_Diff_Rate* (sint16)Motor_Vehicle_Speed_Scale_Gears_Out_Get / (sint16)Motor_Vehicle_Speed_Scale_Get;    
              }
              
              FeedBack_Torque_Break=FeedBack_Torque_Break_Buffer/10;
              
          } 
          else 
          {   //斜坡完毕 正常制动力矩
              FeedBack_Torque_Break=(sint16)FeedBack_Torque_Break_Tmp;
              FeedBack_Torque_Break_Buffer=0;
              FeedBack_Slope_Enable=0;              
          }
      }
     



      //FeedBack_Break_QuitDelay
      if (   ((Torque_Mode==3)&&(Breakk<=100))||(ABS_Active>0))  //松制动踏板  力矩斜坡降0      慢退出   FeedBack_Break_QuitDelay从10开始--
      {
         if ((Motor_Torque<-2)&&(FeedBack_Break_QuitDelay>0)&&(Motor_Speed>(sint16)Motor_Vehicle_Speed_Scale_Get)) 
         {
            //FeedBack_Torque_Break=(Motor_Torque*9)/10;
            if (Gears_Install==0)   //直驱
            {
                FeedBack_Torque_Break_Buffer_Quit=FeedBack_Torque_Break_Buffer_Quit + FeedBack_Torque_Break_Quir_Add * FeedBack_Diff_Rate;
            }else                   //多档
            {
                FeedBack_Torque_Break_Buffer_Quit=FeedBack_Torque_Break_Buffer_Quit + FeedBack_Torque_Break_Quir_Add * FeedBack_Diff_Rate* (sint16)Motor_Vehicle_Speed_Scale_Gears_Out_Get /(sint16) Motor_Vehicle_Speed_Scale_Get;
            }
            
            FeedBack_Torque_Break=FeedBack_Torque_Break_Buffer_Quit/10;            
            
            FeedBack_Break_QuitDelay--;   // 每次减1   制动退出最多10周期
            Torque_Mode=3;
        
         } else 
         {
            //FeedBack_Torque_Break=0;
            FeedBack_Break_QuitDelay=0;                                                  
            if (ABS_Active>0){  FeedBack_Torque_Break=0; }
           
         }
      }else
      {
          FeedBack_Torque_Break_Buffer_Quit= FeedBack_Torque_Break*10;
      
      }
      
      
      
      //制动力矩不为正
      if (FeedBack_Torque_Break>0){  FeedBack_Torque_Break=0;  }
  } 
  else
  {
      FeedBack_Torque_Break=0;
  }
 
}



//D档蠕动转矩计算
void Crawl_D_Torque(void) 
{
     uint16  Crawl_D_Max_Torque_Table=0;
   
    //Vmh_Delta_Target  目标AD值  mh/10ms
    //Vmh_Delta_Vehco   车辆AD值  mh/10ms 
    //Torque_Increment  转矩增量  0.1Nm
    //Motor_Vehicle_Speed_Scale_Gears_Out_Get
    //Gears_Install

    if ((Limit_Mode==1)||(Torque_Mode!=1)&&(Torque_Mode!=4)  || (Crawl_D_Torque_Enable==0)||(Motor_Speed/(sint16)Motor_Vehicle_Speed_Scale_Get>15)|| ((sint32)( Motor_Speed/Motor_Vehicle_Speed_Scale_Get)<-10)||(Dang_P==1)||(Ready==0)||(Vehco_State!=4)) //速度超过-10~15km/h P档拉起 蠕动转矩归0 
    {
        //Crawl_D_Torque_New=0;
        Crawl_D_Torque_New=2; //修正其他模式下蠕行转矩为1
    } 
    else 
    {
        if (Gears_Install==0 )
        {       //单档
            Vmh_Delta_Target_Crawl_D=Intpol_s16s16(Speed_Vmh10ms_Crawl_D_Addr,Motor_Speed);
        }else   //多档  用电机转速反推传动轴转速获取 调整目标
        {
            Vmh_Delta_Target_Crawl_D=Intpol_s16s16(Speed_Vmh10ms_Crawl_D_Addr,  
            (sint16)(((sint32)Motor_Speed)*Motor_Vehicle_Speed_Scale_Gears_Out_Get/Motor_Vehicle_Speed_Scale_Get)   );
        }
        
        Torque_Increment_Crawl_D_Get=Intpol_s16s16(Torque_Increment_Crawl_D_Addr,Vmh_Delta_Target_Crawl_D-Vmh_Delta_Vehco);
        
        if (Breakk>0)
        {

            //制动情况下蠕行扭矩峰值
             Crawl_D_Max_Torque_Table=Intpol_s16u16(Speed_Max_Torque_Crawl_D_Break_Addr,Motor_Speed_Average);
            
            
            
        } 
        else 
        {
            //非制动情况下蠕行扭矩峰值
            Crawl_D_Max_Torque_Table=Intpol_s16u16(Speed_Max_Torque_Crawl_D_Addr,Motor_Speed_Average);
            
    
        }
        // 最大力矩20%增量
        Crawl_D_Max_Torque=(Crawl_D_Max_Torque*30+Crawl_D_Max_Torque_Table*20)/50;
        
        //Crawl_D_Torque_Buffer==Demand_Torque*10; 10倍计算
        Crawl_D_Torque_New=Demand_Torque+Torque_Increment_Crawl_D_Get;
        
        
        if (Crawl_D_Torque_New<1) {
            Crawl_D_Torque_New=1;
        }
    
        if (Crawl_D_ModeDelay < (Speed_Buffer_Number_Get)/Crawl_D_Torque_Delay_Max_Get +2)  //开始进入蠕动延时缓冲
        {
           Crawl_D_Torque_New=1;//缓冲期间转矩恒定给定值
           Crawl_D_ModeDelay++;
        }    
        
        if  ( (((Lock_Vehicle_Mode==2)&&(Lock_Vehicle_Enable==1))
            ||((Heartbeat_Lock_Mode==2)&&(Heartbeat_Lock_Vehicle_Enable==1))) 
            &&(Lock_Vehicle_Active==1) )  //2级远程锁车 拆终端锁车
        {
            
            Veh_Limit_Speed=0;
            Crawl_D_Torque_New=0;
        }
        
        
 
        if (Crawl_D_Torque_New>=Crawl_D_Max_Torque) //限制最大扭矩
        {
            Crawl_D_Torque_New=Crawl_D_Max_Torque;
        }
    }
}

//D自动驻坡转矩计算
void Auto_Parking(void) 
{    
    sint16 AccPed100_Torque=0,Vmh_Delta_Target_Tmp=0;
    

    //Auto_Park_Speed_Accumulation   单次退行转速累加 
    //Auto_Park_Speed_Times          退行增速时间累加 Motor_Speed_Buffer[0]  Auto_Park_Speed_Times_Max
    if ( (Dang_L4bit==10)  && (Motor_Speed_Buffer[0]<0) 
    //&& (Breakk==0) 
    ) 
    {
        if (Motor_Speed_Buffer[0]<Motor_Speed_Buffer[1])
        {
            Auto_Park_Speed_Times++;
            Auto_Park_Speed_Accumulation=Auto_Park_Speed_Accumulation+Motor_Speed;
            if (Auto_Park_Speed_Accumulation<-32000)
            {
                Auto_Park_Speed_Accumulation=-32000;
            }
            if (Auto_Park_Speed_Times>1000) 
            {
                Auto_Park_Speed_Times=1000;
            }
        }else 
        {
            Auto_Park_Speed_Times=0;
            Auto_Park_Speed_Accumulation=0;  
        }
    }
    else 
    {
        Auto_Park_Speed_Times=0;
        Auto_Park_Speed_Accumulation=0; 
    }
    
    
    
    
    if ((Motor_Speed>((sint16)Motor_Vehicle_Speed_Scale_Get*5))
    ||((sint32)Motor_Speed/(sint32)Motor_Vehicle_Speed_Scale_Get<-10)
    ||(Dang_P==1)||(Ready==0)||(Vehco_State!=4)) //速度超过5km/h退出计算
    {
        //Auto_Parking_D_Torque=0;
        Auto_Parking_D_Torque=1; //拉手刹期间最小蠕行转矩为1//加上斜坡降0
    } 
    else 
    {
        
        Vmh_Delta_Target_Tmp=Intpol_s16s16(Speed_Vmh10ms_D_Park_Addr,Motor_Speed_Average);
        if (Vmh_Delta_Target_Tmp>Vmh_Delta_Target_Parking_D)   //驻坡目标AD值变化斜坡处理
        {
            Vmh_Delta_Target_Parking_D=(Vmh_Delta_Target_Parking_D*2+ Vmh_Delta_Target_Tmp*8)/10;
        } else 
        {
            Vmh_Delta_Target_Parking_D=(Vmh_Delta_Target_Parking_D*7+ Vmh_Delta_Target_Tmp*3)/10;
        }
        
        Torque_Increment_Parking_D=Intpol_s16s16(Torque_Increment_D_Park_Addr,Vmh_Delta_Target_Parking_D-Vmh_Delta_Vehco_Average_D);
        
        //Auto_Parking_D_Torque=Motor_Torque+Torque_Increment_Parking_D; //用最近电机转矩作为基础值
        Auto_Parking_D_Torque=Demand_Torque+Torque_Increment_Parking_D; //用最近给定转矩作为基础值
        //Auto_Parking_D_Torque=Motor_Torque_Average+Torque_Increment_Parking_D; //用平均转矩作为基础值
        
        
        //AutoPark_Motor_Speed_Torque_Addr
        //AccPed100_Torque=Intpol_s16u16(Motor_Speed_Torque_Addr,Motor_Speed); //速度曲线限制最大转矩
        
        //AccPed100_Torque=Intpol_s16u16(AutoPark_Motor_Speed_Torque_Addr,Motor_Speed); //限制最大转矩
        AccPed100_Torque=Intpol_s16u16(AutoPark_Motor_Speed_Torque_Addr,Motor_Speed_Average); //限制最大转矩
        
        if (Auto_Parking_D_Torque>AccPed100_Torque) 
        { 
            Auto_Parking_D_Torque=AccPed100_Torque;
        }
        if (Auto_Parking_D_Torque<=0) {Auto_Parking_D_Torque=0;}
        if (Auto_Parking_D_Torque_Enable==0)
        {Auto_Parking_D_Torque=0;
        }
    }
    
    
    
}
//R自动驻坡转矩计算
void Auto_Parking_R(void) 
{   
    sint16 AccPed100_Torque=0,Vmh_Delta_Target_Tmp=0;
    if ((Motor_Speed/(sint16)Motor_Vehicle_Speed_Scale_Get<-5)||(Motor_Speed/(sint16)Motor_Vehicle_Speed_Scale_Get>10)||(Dang_P==1)||(Ready==0)||(Vehco_State!=4)) //速度超过-5km/h退出计算
    {
        Auto_Parking_R_Torque=0;
    } 
    else 
    {
        Vmh_Delta_Target_Tmp=Intpol_s16s16(Speed_Vmh10ms_D_Park_Addr,-1*Motor_Speed);
       
       if (Vmh_Delta_Target_Tmp<Vmh_Delta_Target_Parking_R)   //驻坡目标AD值变化斜坡处理
        {
            Vmh_Delta_Target_Parking_R=(Vmh_Delta_Target_Parking_R*2+ Vmh_Delta_Target_Tmp*8)/10;
        } else 
        {
            Vmh_Delta_Target_Parking_R=(Vmh_Delta_Target_Parking_R*7+ Vmh_Delta_Target_Tmp*3)/10;
        }
       
       
       
       
        Torque_Increment_Parking_R=Intpol_s16s16(Torque_Increment_D_Park_Addr,Vmh_Delta_Target_Parking_R-(-1*Vmh_Delta_Vehco));
       
        Auto_Parking_R_Torque=Demand_Torque-Torque_Increment_Parking_R; //用最近给定转矩作为基础值       
        //Auto_Parking_R_Torque=Motor_Torque_Average-Torque_Increment_Parking_R; 
        AccPed100_Torque=-1*(Intpol_s16u16(Motor_Speed_Torque_Addr,Motor_Speed));
        if (Auto_Parking_R_Torque<AccPed100_Torque) 
        { 
            Auto_Parking_R_Torque=AccPed100_Torque;
        }
        if (Auto_Parking_R_Torque>-1) { Auto_Parking_R_Torque=-1;}
        
        if (Auto_Parking_R_Torque_Enable==0) {
        Auto_Parking_R_Torque=0;
        }
    }
}




//油门转矩给定值计算 
void AccPed_Torque_Calculation(void)
{   //Pow_Down_Enable 故障限制功率标志 
    uint16 AccPed100_Torque=0,AccPed_Torque_Check_Table=0 ;
    sint16 R_Motor_Speed=0,R_Motor_Torque=0;//R档电机转速转矩
    sint16 R_Auto_Torque=0;// R档限速计算出最大力矩
    sint16 Auto_Torque=0;
    uint8 Limit_Mode_zi[7]={0};
    sint32 LimtRpm=0;
    
    //Motor_Speed_Torque_Addr 转速_最大转矩_75KW  比例114.4
    //使用平均速度查询 
    AccPed100_Torque=Intpol_s16u16(Motor_Speed_Torque_Addr,Motor_Speed_Average);//100%油门开度转矩
    AccPed_Torque_Check_Table=(uint16)((AccPed*((uint32)AccPed100_Torque))/10000);  //(sint16)((1000*((uint32)AccPed))/10000) 按油门开度比例计算转矩
    
    //放大10倍做缓冲积累
    /*
    AccPed_Torque_ModeDelay=0; //油门模式进入0-1Nm缓冲
    AccPed_Torque_ModeDelay=AccPed_ModeDelay;
    AccPed_Slope_Delay
    */
    
    
    if (AccPed_Torque_ModeDelay<AccPed_ModeDelay)      //油门给定 1 Nm 延时缓冲
    {
            
        AccPed_Torque_Buffer=10;
        AccPed_Torque_ModeDelay++;
    }else 
    {
        //进入时油门给定力矩斜坡处理    
        if (( AccPed_Slope_Delay_Num<AccPed_Slope_Delay ) && (AccPed_Torque_Buffer<AccPed_Torque_Check_Table*10)   )
        {
            AccPed_Torque_Buffer=AccPed_Torque_Buffer+AccPed_Torque_Diff_Add; 
            AccPed_Slope_Delay_Num++;
            AccPed_Slope_Delay_2key=1; 
        } 
        else 
        {
            if( (AccPed_Torque_Buffer<AccPed_Torque_Check_Table*10) && (AccPed_Slope_Delay_2key==1)     )
            {   //退出固定斜坡后如力矩小于油门给定值 则5%递增
                AccPed_Torque_Buffer=AccPed_Torque_Buffer + AccPed_Torque_Check_Table/2;
            
            } 
            else 
           {
               AccPed_Torque_Buffer=AccPed_Torque_Check_Table*10;
               AccPed_Slope_Delay_Num=AccPed_Slope_Delay;      
               AccPed_Slope_Delay_2key=0;    
          }
        }
    }
    
   
    
    
    
    switch(Dang_L4bit)  //根据档位决定转矩方向
    {
        case 10:
            AccPed_Torque=(sint16)AccPed_Torque_Buffer/10;// D 
            break;
        case 5:
            AccPed_Torque=-1*(sint16)AccPed_Torque_Buffer/10;//R
            break;
        case 0:
        case 1:
            AccPed_Torque=0;// N P
        default:
            break;
    }
    //在这里插入降功率处理 ?                                         
    
    
    if ((Ready==0)||(Vehco_State!=4)){AccPed_Torque=0;} //车未READY  油门无效
    
    
    //以下3个限速用曲线做
    
    Veh_Limit_Speed=0;
    if (((Gears_Install==0)||( Shift_Gears==0  ))&&(bDiSatus[7]==0 ) &&       //限速100解除
       (AccPed_Torque_100_Enable==1)&& (Dang_L4bit==10)&&(Motor_Speed/(sint16)Motor_Vehicle_Speed_Scale_Get>(D100_Max_Speed-10))) //D档限速100 
    {
        Vmh_Delta_Target=Intpol_s16s16(Speed_Vmh10ms_General_Addr,Motor_Speed_Average-D100_Max_Speed*(sint16)Motor_Vehicle_Speed_Scale_Get); //目标车速 100*Motor_Vehicle_Speed_Scale_Get
        Torque_Increment=Intpol_s16s16(Torque_Increment_General_Addr,Vmh_Delta_Target-Vmh_Delta_Vehco_Average_D);
        
        Auto_Torque=Demand_Torque+Torque_Increment;
        
        if (AccPed_Torque>Auto_Torque) //如果油门给定转矩大于限速转矩，则使用限速转矩
        {
            AccPed_Torque=Auto_Torque;
            Limit_Mode_zi[2]=1;
            Veh_Limit_Speed=D100_Max_Speed;
        }
        if (AccPed_Torque<Auto_Torque_Min_Get){ AccPed_Torque=Auto_Torque_Min_Get;}
    }
    if ( (Err_AccPed ==1)&&(Dang_L4bit==10)&&(Motor_Speed/(sint16)Motor_Vehicle_Speed_Scale_Get>0)) //油门故障时限速30  
    {
        Vmh_Delta_Target=Intpol_s16s16(Speed_Vmh10ms_General_Addr,Motor_Speed_Average-Err_AccPed_Max_Speed*(sint16)Motor_Vehicle_Speed_Scale_Get); //目标车速 30*Motor_Vehicle_Speed_Scale_Get
        Torque_Increment=Intpol_s16s16(Torque_Increment_General_Addr,Vmh_Delta_Target-Vmh_Delta_Vehco_Average_D);
       
        Auto_Torque=Demand_Torque+Torque_Increment;

        if (AccPed_Torque>Auto_Torque) //如果油门给定转矩大于限速转矩，则使用限速转矩
        {
            AccPed_Torque=Auto_Torque;
            Limit_Mode_zi[1]=1;
            Veh_Limit_Speed=Err_AccPed_Max_Speed;
        }       
        if (AccPed_Torque<Auto_Torque_Min_Get){ AccPed_Torque=Auto_Torque_Min_Get;}
    }

    if  ((    ((AccPed>100)&&(Limit_P==1))      
    || (Dang_P==1))&&(Dang_L4bit==10)&&(Motor_Speed/(sint16)Motor_Vehicle_Speed_Scale_Get>1))  //带手刹限速 20
    {
        Limit_P=1;
        Vmh_Delta_Target=Intpol_s16s16(Speed_Vmh10ms_General_Addr,Motor_Speed_Average-P_Max_Speed*(sint16)Motor_Vehicle_Speed_Scale_Get); //目标车速 20*Motor_Vehicle_Speed_Scale_Get
        Torque_Increment=Intpol_s16s16(Torque_Increment_General_Addr,Vmh_Delta_Target-Vmh_Delta_Vehco_Average_D);
       
        Auto_Torque=Demand_Torque+Torque_Increment;
        if ((AccPed_Torque>Auto_Torque)&&(AccPed_Torque_P_Enable==1)) //如果油门给定转矩大于限速转矩，则使用限速转矩
        {
            AccPed_Torque=Auto_Torque;
            Limit_Mode_zi[0]=1;
            
            Veh_Limit_Speed=P_Max_Speed;
        }
        //限制最小转矩
        if (AccPed_Torque<Auto_Torque_Min_Get){ AccPed_Torque=Auto_Torque_Min_Get;}

    }else 
    {
       if ((Dang_P==0)||(Dang_L4bit!=10))   {
        Limit_P=0; 
       }
    }

    if  ((Dang_L4bit==5)&&((sint32)Motor_Speed/(sint16)Motor_Vehicle_Speed_Scale_Get<0))  //倒挡限速   1km/h启用
    {
        R_Motor_Speed=-1*Motor_Speed_Average;
       
        R_Motor_Torque=-1*Demand_Torque;
        
        Vmh_Delta_Target=Intpol_s16s16(Speed_Vmh10ms_General_Addr,R_Motor_Speed-R_Max_Speed*(sint16)Motor_Vehicle_Speed_Scale_Get); //目标车速 10*Motor_Vehicle_Speed_Scale_Get    
        Torque_Increment=Intpol_s16s16(Torque_Increment_General_Addr,Vmh_Delta_Target-(-1*Vmh_Delta_Vehco_Average_R));//R档车辆AD值要取负值

        R_Auto_Torque=-1*(R_Motor_Torque+Torque_Increment);
        
        if  ((AccPed_Torque<R_Auto_Torque)&&(AccPed_Torque_R_Enable==1)) //如果油门给定转矩大于限速转矩，则使用限速转矩  R档为负 所以用＜
        {
            AccPed_Torque=R_Auto_Torque;
            Veh_Limit_Speed=R_Max_Speed;
        }
        //限制最大转矩
        if (AccPed_Torque>Auto_Torque_Max_Get){ AccPed_Torque=Auto_Torque_Max_Get;}


    }
    /*
    extern uint8 Lock_Vehicle_Active; //锁车状态激活
    extern uint8 Lock_Vehicle_Mode;   //锁车状态      0 未锁车 1 1级锁车      2 2级锁车 
    extern uint8 Heartbeat_Lock_Mode; //心跳锁车状态  0 未锁车 1 心跳锁车监听 2 心跳锁车 
    //Lock_Level_1_Max_Speed 1级锁车最大车速

    */

    if  ((Dang_L4bit==10)&&(Lock_Vehicle_Enable==1)&&(Lock_Vehicle_Mode==1)&&(Lock_Vehicle_Active==1) 
        &&(Motor_Speed_Average/(sint16)Motor_Vehicle_Speed_Scale_Get>(sint16)(Lock_Level_1_Max_Speed-10)))  //1级远程锁车限速 30
    
    
    {
        Limit_Mode_zi[3]=1;
        Veh_Limit_Speed=Lock_Level_1_Max_Speed;
               
        Vmh_Delta_Target=Intpol_s16s16(Speed_Vmh10ms_General_Addr,Motor_Speed_Average-Lock_Level_1_Max_Speed*(sint16)Motor_Vehicle_Speed_Scale_Get); //目标车速 30*Motor_Vehicle_Speed_Scale_Get
        Torque_Increment=Intpol_s16s16(Torque_Increment_General_Addr,Vmh_Delta_Target-Vmh_Delta_Vehco_Average_D);
        Auto_Torque=Demand_Torque+Torque_Increment;
        if ((AccPed_Torque>Auto_Torque)) //如果油门给定转矩大于限速转矩，则使用限速转矩
        {
            AccPed_Torque=Auto_Torque;
        }
        //限制最小转矩
        if (AccPed_Torque<Auto_Torque_Min_Get){ AccPed_Torque=Auto_Torque_Min_Get;}

    }



    if  ( (((Lock_Vehicle_Mode==2)&&(Lock_Vehicle_Enable==1))
        ||((Heartbeat_Lock_Mode==2)&&(Heartbeat_Lock_Vehicle_Enable==1))) 
        &&(Lock_Vehicle_Active==1) )  //2级远程锁车 拆终端锁车
    {
        Limit_Mode_zi[4]=1;
        Veh_Limit_Speed=0;
        AccPed_Torque=0;
    }


    
    /*
    //第三方锁车专用
    extern uint8 Third_Lock_Active;              //锁车激活
    extern uint8 Third_Lock_Mode_Active;         //生效锁车模式
    extern uint8 Third_Lock_LimitSpeed_Active;   //生效限速速度 
    
    */
    
    //限速   Third_Lock_LimitSpeed_Active
    if  ((Dang_L4bit==10)&&(Third_Lock_Mode_Active==1)&&(Third_Lock_Active==1) 
        &&(Motor_Speed_Average/(sint16)Motor_Vehicle_Speed_Scale_Get>(sint16)(Third_Lock_LimitSpeed_Active-10)))  
    {
        Limit_Mode_zi[5]=1;
        Veh_Limit_Speed=Third_Lock_LimitSpeed_Active;
        LimtRpm=(sint32)Third_Lock_LimitSpeed_Active * Motor_Vehicle_Speed_Scale_Get;
        if (LimtRpm>15000){           LimtRpm=15000;        }
        Vmh_Delta_Target=Intpol_s16s16(Speed_Vmh10ms_General_Addr,Motor_Speed_Average- (sint16)LimtRpm); //目标车速 30*Motor_Vehicle_Speed_Scale_Get
        Torque_Increment=Intpol_s16s16(Torque_Increment_General_Addr,Vmh_Delta_Target-Vmh_Delta_Vehco_Average_D);
        Auto_Torque=Demand_Torque+Torque_Increment;
        if ((AccPed_Torque>Auto_Torque)) //如果油门给定转矩大于限速转矩，则使用限速转矩
        {
            AccPed_Torque=Auto_Torque;
        }
        //限制最小转矩
        if (AccPed_Torque<Auto_Torque_Min_Get){ AccPed_Torque=Auto_Torque_Min_Get;}
       
        
    }

    //锁车
    if  ( (Third_Lock_Mode_Active==2)&&(Third_Lock_Active==1) )  //2级远程锁车 拆终端锁车
    {
        Limit_Mode_zi[6]=1;
        Veh_Limit_Speed=0;
        AccPed_Torque=0;
    }
    
    
    
    if ((Limit_Mode_zi[0]==1)||(Limit_Mode_zi[1]==1)||(Limit_Mode_zi[2]==1)||(Limit_Mode_zi[3]==1)
      || (Limit_Mode_zi[4]==1) || (Limit_Mode_zi[5]==1)|| (Limit_Mode_zi[6]==1)) // 限速模式
    {
        Limit_Mode=1;
    }else
    {
        Limit_Mode=0;
    }
    //Oil_Pump_Install,Oil_Pump_Fault
    //卡车 油泵故障时油门扭矩限制
    if ((Oil_Pump_Install==1)&&(Oil_Pump_Fault==1))
    {
        Limit_Mode=1;
        Veh_Limit_Speed=0;
        AccPed_Torque=0;        
        
    }
    
}

//电池功率限制转矩
void Torque_Power_Limit(void) 
{
    /*
    sint16 Torque_High_Limit,Torque_Low_Limit; //电机转矩上下限 比例尺 1  0～5000  -5000～0 

    uint16 Max_Discharge_Power_Limit,Max_Charge_Power_Limit;//最大10s充放电功率限制  0~250 单位： kW， 偏移： 0 ，比例尺： 1
    uint16 Max_Discharge_Current_Limit,Max_Charge_Current_Limit;//最大充放电电流极限  0～500 单位： A， 偏移： 0 ，比例尺： 2

    uint16 B_Voltage,Output_Voltage;            //电池电压、输出电压  0~800 单位： V， 偏移： 0 ，   比例尺： 0.2 （10倍放大计算，0～8190 4095*2=8190）
    sint16 B_Current;                           //电池电流         -500~500 单位： A， 偏移： -500 ，比例尺： 0.02 (10倍放大计算-5000～5000）正电流表示对电池充电，负电流表示电池放电
    uint16 B_Charge_Current,B_Discharge_Current;//电池充、放电电流   0～500 单位： A， 偏移：0 ，    比例尺： 0.02（放大10倍计算 0～5000）
    sint16 B_Power;                             //电池充放功率     -325~325 单位： kW，偏移： -325， 比例尺： 0.01 (100倍放大计算）正功率表示对电池充电， 负功率表示电池放电    


    sint16 Driver_Torque_Limit=0;//驱动限制力矩
    sint16 Bms_Max_Torque_Driver; //BMS限制驱动转矩
    sint16 Motor_Max_Torque_Driver;//电机限制驱动转矩
    */
    
    sint16 Motor_Max_Torque_Break_Table=0;
    sint32 Bms_MaxCharge_Power=0;
    //sint32 Bms_MaxCharge_Power_Curr=0;
    sint16 Torque_LowHigh_Limit=0;
   // sint32 Bms_Max_Torque_Break_Tmp=0;
   // sint32 Bms_Max_Torque_Driver_Tmp=0;
    
    sint16 Motor_Max_Torque_Driver_Table=0;
    sint32 Bms_MaxDisCharge_Power=0;
    //sint32 Bms_MaxDisCharge_Power_Curr=0;
    
    sint16 Torque_MotoDriver_Limit=0;
    
    //uint16 Motor_Efficieny=0;
    //uint16 Motor_Efficieny_Pow_Limit=0;//电机修正后效率
    
    


       // Bms_MaxCharge_Power_Curr=((sint32)Max_Charge_Current_Limit*B_Voltage/100 );  //根据BMS最大充电电流计算运行充电功率 0.00kw   0.00A  0.0V
        
       // Bms_MaxDisCharge_Power_Curr=((sint32)Max_Discharge_Current_Limit*B_Voltage/100 );  //根据BMS最大放电电流计算运行放电功率 0.00kw   0.00A  0.0V                
        
        //获取BMS限制充电功率  取最小
       // Bms_MaxCharge_Power=(Bms_MaxCharge_Power_Curr<Max_Charge_Power_Limit) ? Bms_MaxCharge_Power_Curr:Max_Charge_Power_Limit;
        
      //  Bms_MaxDisCharge_Power=(Bms_MaxDisCharge_Power_Curr<Max_Discharge_Power_Limit) ? Bms_MaxDisCharge_Power_Curr:Max_Discharge_Power_Limit;
        //DEBUG_Max_Discharge_Power_Limit DEBUG_Max_Charge_Power_Limit
        
        if (DEBUG_Max_Charge_Power_Limit==0)    
        {
            Bms_MaxCharge_Power=Max_Charge_Power_Limit;
        }else 
        {
            Bms_MaxCharge_Power=DEBUG_Max_Charge_Power_Limit;
        }
        if (DEBUG_Max_Discharge_Power_Limit==0)  
        {
            Bms_MaxDisCharge_Power=Max_Discharge_Power_Limit;
        }else
        {
             Bms_MaxDisCharge_Power=DEBUG_Max_Discharge_Power_Limit;
        }
                
        //获取电机效率  
        //uint16 Motor_Efficieny_Fix=10000; //电机效率自动修正值
        //Driver_Power                  //电机电功率  100倍
        //sint16 B_Power;               //电池充放功率     -325~325 单位： kW 
       
         Efficieny_Fix();
         
         Motor_Efficieny_Pow_Limit=(uint16)   ((((uint32)Motor_Efficieny_Fix)* Intpol_u16u16(Speed_Efficieny_Addr,abs(Motor_Speed_Average)))/10000);
        /***************************************************************************************************/
        //获取BMS限制回馈转矩  Bms_MaxCharge_Power 电池功率单位是 0.01KW
        //if ((sint32)Bms_MaxCharge_Power/(1+abs(Motor_Speed))<13) //限制计算值不超过1241Nm
        //{
          
           // Bms_Max_Torque_Break=-1* 
        //Vehco_Type_Test=2
        switch (Vehco_Type_Test)
        {
            case 1: //优优
            case 3: //D07 
        
        
            Bms_Max_Torque_Break_Tmp=-1*  
            
              
                (( 
                    ((   (sint32)Bms_MaxCharge_Power*955/(Motor_Efficieny_Pow_Limit/100)  )  *10) /(abs(Motor_Speed_Average)+1) ));
        
            break;
            default:
            case 2: //D11 D12 力信700N    M02
            Bms_Max_Torque_Break_Tmp=-1*  
             
                (
                (  
                  (  
                      ((sint32)Bms_MaxCharge_Power  *955)/10
                  )*(Motor_Efficieny_Pow_Limit/100)/ (abs(Motor_Speed_Average)+1)
                )
                );            
            break;
        }
        Bms_Max_Torque_Break=(sint16)( (Bms_Max_Torque_Break_Tmp>-3600) ? Bms_Max_Torque_Break_Tmp:-3600);            
        
        //} else 
        //{
        //    Bms_Max_Torque_Break=-1214;
        //}
        /****************************************************************************************************/
        //获取BMS限制驱动转矩  Bms_MaxDisCharge_Power 电池功率单位是 0.01KW  
        Bms_Max_Torque_Driver_Tmp=
             
              (
                (  
                  (  
                      ((sint32)Bms_MaxDisCharge_Power  *955)/10
                  )*(Motor_Efficieny_Pow_Limit/100)/ (abs(Motor_Speed_Average)+1)
                )
              );      
                
       Bms_Max_Torque_Driver=(sint16)((Bms_Max_Torque_Driver_Tmp<3600) ? Bms_Max_Torque_Driver_Tmp:3600);
                
        /****************************************************************************************************/
  
  
        //查表获取 标定电机最大回馈转矩  
        Motor_Max_Torque_Break_Table=-1* Intpol_s16s16(Motor_Speed_Torque_Break_Addr,abs(Motor_Speed_Average));
       
        Motor_Max_Torque_Driver_Table=Intpol_s16s16(Motor_Speed_Torque_Addr,abs(Motor_Speed_Average));
        
        //获取电机限制回馈转矩   Torque_High_Limit   限制驱动转矩 Torque_MotoDriver_Limit
        if  (Dang_L4bit==5)
        {
            Torque_LowHigh_Limit=-1* Torque_High_Limit;  //始终是负数
            Torque_MotoDriver_Limit=-1* Torque_Low_Limit; //始终是正数
            
        } else 
        {
            Torque_LowHigh_Limit=Torque_Low_Limit;
            Torque_MotoDriver_Limit=Torque_High_Limit;
        }
        
        
        
        
        
        Motor_Max_Torque_Break=(Motor_Max_Torque_Break_Table>Torque_LowHigh_Limit) ? Motor_Max_Torque_Break_Table:Torque_LowHigh_Limit;
        
        Motor_Max_Torque_Driver=(Motor_Max_Torque_Driver_Table<Torque_MotoDriver_Limit) ? Motor_Max_Torque_Driver_Table:Torque_MotoDriver_Limit; 
        
        
        
        //取最低
        Break_Torque_Limit=(Motor_Max_Torque_Break>Bms_Max_Torque_Break) ? Motor_Max_Torque_Break:Bms_Max_Torque_Break;
       
        Driver_Torque_Limit=(Motor_Max_Torque_Driver<Bms_Max_Torque_Driver) ? Motor_Max_Torque_Driver:Bms_Max_Torque_Driver;
        
   if (Torque_Power_Limit_Enable==1) 
    {     
        switch(Dang_L4bit)  //根据档位
        {
            case 5: //R档
                if (Demand_Torque>  -1* Break_Torque_Limit) 
                { 
                    Demand_Torque= -1* Break_Torque_Limit; 
                }
                
                if  (Demand_Torque< -1* Driver_Torque_Limit)
                {
                    Demand_Torque= -1* Driver_Torque_Limit;
                }
                
                break;
          case 10: //D档
                if (Demand_Torque<Break_Torque_Limit) 
                {
                    Demand_Torque=Break_Torque_Limit; 
                }
                
                if (Demand_Torque>Driver_Torque_Limit)
                {
                    Demand_Torque=Driver_Torque_Limit;                
                }
                
                break;
        }
        
     }
}
//电机效率自动修正系数计算
void  Efficieny_Fix (void) 
{
    //uint16 Motor_Efficieny_Fix=10000; //电机效率自动修正值  100.00%
    //sint16 Driver_Power           //电机电功率  100倍
    //sint16 B_Power;               //电池充放功率     -325~325 单位： kW 
    //Efficieny_Percent_Filter_And_All

    uint32 Motor_Efficieny_Fix_Tmp;   
    extern uint8 B_Version;//BMS通讯协议版本 
    if ((Efficieny_Fix_Enable==1)&&(B_Version>17)&& (abs(Driver_Power)>1000)&& (abs(B_Power)>1000))           
    {
        Motor_Efficieny_Fix_Tmp=abs  ( (sint16)  ( (   ( (sint32)Driver_Power)   *10000)/B_Power ));
        
        if (Motor_Efficieny_Fix_Tmp>Motor_Efficieny_Fix )     
        {//效率升
            Motor_Efficieny_Fix=(uint16)  ((((uint32)(Motor_Efficieny_Fix))*(Efficieny_Percent_Filter_And_All[0]-Efficieny_Percent_Filter_And_All[1]) 
            + Motor_Efficieny_Fix_Tmp*Efficieny_Percent_Filter_And_All[1])/Efficieny_Percent_Filter_And_All[0]);
        }else 
        {//效率降
            Motor_Efficieny_Fix=(uint16)  ((((uint32)(Motor_Efficieny_Fix))*(Efficieny_Percent_Filter_And_All[0]-Efficieny_Percent_Filter_And_All[2]) 
            + Motor_Efficieny_Fix_Tmp*Efficieny_Percent_Filter_And_All[2])/Efficieny_Percent_Filter_And_All[0]);
        }        
    }

}


void Driver_Power_Func(void) //电功率计算
{//Driver_Power                //电机电功率  100倍
    sint32 Driver_Power_Tmp=0;
    uint16 Motor_Efficieny=0;
    
    //获取电机效率
    Motor_Efficieny=Intpol_u16u16(Speed_Efficieny_Addr,abs(Motor_Speed_Average));
    
    Motor_Efficieny=(Motor_Efficieny>100)? Motor_Efficieny:100; 
    
    if (Motor_Speed>=0) 
    {
        Driver_Power_Tmp= ((sint32)Motor_Torque)*(Motor_Speed+1);
    
    
    } else 
    {
        Driver_Power_Tmp= ((sint32)Motor_Torque)*Motor_Speed;
    }
   
   
    //回馈模式电功率
    if (((Motor_Torque<0)&&(Dang_L4bit==10)) || ((Motor_Torque>0)&&(Dang_L4bit==5)))
    {
        switch (Vehco_Type_Test) 
        {
            default:
            case 1:
                Driver_Power=(sint16)((Driver_Power_Tmp*(Motor_Efficieny/100))/9550);
                break;
            case 2:
                Driver_Power=(sint16)((    (Driver_Power_Tmp/(Motor_Efficieny/100))*1000    )/955);
                break;
        }
    }else
    //驱动模式电功率
    {
        Driver_Power=(sint16)((    (Driver_Power_Tmp/(Motor_Efficieny/100))*1000    )/955);
    }
    
    
}


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






