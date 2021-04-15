/*******************************************************************************
* 工程名称：卡车油泵、气泵控制程序
* 文件名： Vin_N_In_One.c
* 功能描述 ： 
* 引用记录：
* 备注： 
* 修订记录：
*		日期		      版本		  作者		描述
*   
*******************************************************************************/
#include <string.h>
#include "GlobalDef.h"
#include "Vin_N_In_One.h"

    
#ifndef AECS_HW
       #pragma DATA_SEG DEFAULT
#else  
      #pragma DATA_SEG S12X_RAM
#endif



/*
uint8 ESK_Signal_Type=0;//档位信号类型 0 电平信号 1 CAN总线信号 
uint8 ESK_Type;//电子换档位类型  0 拨片式、旋钮式（点动信号）  1 排档杆式、旋钮式（自锁信号）
uint8 ESK_Fault;//电子换档器故障
uint8 ESK_Live;//电子换档器心跳信号 0~15

uint8 ESK_SelfLock_Request; //电子换挡器信号请求  自锁式  手刹另外硬线接入
uint8 ESK_Inching_Request;  //电子换挡器信号请求  点动式  含手刹信号

uint8 Dang_S,Dang_E,Dang_EE;
uint8 Breakk_Please;//踩制动踏板提醒

*/

/*
0：关机 1：ON档 2：充电中 3：BMS上电完成 电机使能指令 4：电机READY
5：ON消失 自动减速中 6：下电警告 7：主动放电指令 8：主动放电中 9：放电完成 去使能指令
10: 关机准备完成，下控制电

12: 1级故障  13：2级故障 14：3级故障 ???
*/
uint8 Air_Pump_OverHeated=0;  //气泵过热
sint16 Air_Pump_Fan_Delay_Index=0; //气泵风扇关闭延迟  0.1秒步进 
uint16 Air_Reservoirs_Pressure_Low_Index=0; //气泵工作气压仍过低计数 0.1秒
sint16 Air_Pump_WorkTime_Index=0;//     0.1S
uint8 Air_Pump_Cool=0;

sint16 Air_Pump_Torque_Last;//气泵电机前周期转矩 0.01N.m  -100~100
sint16 Air_Pump_Speed_Last;
sint16 Air_Pressure_Delay=0;


uint8 Air_Pump_Still=0,Oil_Pump_Still=0; // 堵转标志

uint8 Oil_Pump_Still_OverTime=0; //油泵堵转超时

uint8 Air_Reservoirs_Pressure_Last_1=0,Air_Reservoirs_Pressure_Last_2=0;

extern uint8 Air_Reservoirs_Over_Pressure_1,Air_Reservoirs_Over_Pressure_2;
extern uint8  Air_Pressure_Ok;   //达到排气点
extern uint8  Air_Pressure_Ok_OverTime;

extern uint8 Vehco_State,Ready,Vehicle_Speed;
                                          
extern uint8 Dang_P,Dang_R,Dang_N,Dang_D; //档位输入开关状态
extern uint8 Dang_L4bit;//档位状态   1 5 0 10 PRDN
extern uint8 bDiSatus[24];     //DI口开关量状态                        
extern uint16 Breakk;//制动踏板状态
extern uint16 AccPed;//经油门曲线处理后的油门值
extern sint16 Motor_Speed;
extern uint16 Motor_Vehicle_Speed_Scale_Get;//转速-车速比例
extern uint8 LostESKCommunication;
extern uint16 LostESKCom_Index;
extern uint8 ESK_Signal; //收到电子档位信号
 
 

extern uint16 Oil_Pump_DTC; //油泵控制器故障代码
extern uint16 Oil_Pump_DC_Voltage;//油泵母线电压     0.1V
extern uint16 Oil_Pump_DC_Current;//油泵母线电流     0.1A
extern sint16 Oil_Pump_Temp;//油泵控制器温度   -40~150
extern uint8 Oil_Pump_Fault;//油泵故障状态 0 正常  1 故障

extern sint16 Oil_Pump_Speed;//油泵电机转速   -6000~6000rpm
extern sint16 Oil_Pump_Torque;//油泵电机转矩 0.01N.m  -100~100

extern uint8  Oil_Pump_Cmd_Mode;//油泵运行模式指令  0 停止 1 转矩模式 2 转速模式
extern uint8  Oil_Pump_Fault_Reset; //油泵故障复位命令
extern sint16 Oil_Pump_Cmd_Torque;//油泵转矩指令   0.01N.m -100~100
extern sint16 Oil_Pump_Cmd_Speed; //油泵转速指令   -6000~6000 rpm


extern uint16 Air_Pump_DTC; //气泵控制器故障代码
extern uint16 Air_Pump_DC_Voltage;//气泵母线电压     0.1V
extern uint16 Air_Pump_DC_Current;//气泵母线电流     0.1A
extern sint16 Air_Pump_Temp;//气泵控制器温度   -40~150
extern uint8 Air_Pump_Fault;//气泵故障状态 0 正常  1 故障

extern sint16 Air_Pump_Speed;//气泵电机转速   -6000~6000rpm
extern sint16 Air_Pump_Torque;//气泵电机转矩 0.01N.m  -100~100

extern uint8  Air_Pump_Cmd_Mode;//气泵运行模式指令  0 停止 1 转矩模式 2 转速模式
extern uint8  Air_Pump_Fault_Reset; //气泵故障复位命令
extern sint16 Air_Pump_Cmd_Torque;//气泵转矩指令   0.01N.m -100~100
extern sint16 Air_Pump_Cmd_Speed; //气泵转速指令   -6000~6000 rpm

extern uint8  Oil_Pump_Install; //油泵控制器适配状态 0 未装 1 安装
extern uint8  Air_Pump_Install; //气泵控制器适配状态 0 未装 1 安装
extern uint8  Gears_Install; //变速箱安装 状态
extern uint8  Fan_Pwm_Install;
extern uint8  Air_Pump_Fan;//气泵散热风扇  关闭时延迟180秒

extern uint8 Air_Reservoirs_Pressure_1,Air_Reservoirs_Pressure_2;//储气桶压力 0.00~2.50Mpa
extern uint8 LostBMSCommunication,LostINSCommunication,Air_Pump_Communication,Oil_Pump_Communication;

extern uint16  Min_B_Voltage_Limit,B_Voltage;//0~819.0V  
extern uint8   B_Fault_Status, B_Version;//BMS通讯协议版本 

extern sint16 Air_Pump_Speed_DrapDown_Get;
extern sint16 Air_Pump_Torque_DrapDown_Get;
extern uint8 Air_Start_Press_Rate_Get;
extern uint8 Air_Over_Reset_Get;
extern uint16 Air_Pressure_Delay_Max_Get;
extern uint16 Air_Pump_WorkTime_Get;
extern uint16 Air_Pump_Fan_Delay_Get;
extern uint16 Air_Pump_Taget_Pressure_Get;
extern sint16 Air_Pump_Speed_Limit_Get;
extern uint16 Air_Pump_Torque_Limit_Get;

extern sint16 Oil_Pump_Speed_Limit_Get;
extern uint16 Oil_Pump_Torque_Limit_Get;

extern uint16 * __far   Oil_Pump_Speed_VSpeed_Addr;

 
#ifndef AECS_HW
      #pragma CODE_SEG DEFAULT
#else  
      #pragma CODE_SEG VCU_CODE
      
#endif

//电池判断
uint8 Power_OK(void) //BMS通讯正常且无2级以上故障
{
    
    uint8 Power=0;
    if (
        (LostBMSCommunication==0)
        //&&((B_Fault_Status<=2) ||  ((B_Fault_Status<=1)&&(B_Version<18))   )       //电池有故障油泵也要保持工作
        &&(B_Voltage>=Min_B_Voltage_Limit)
        &&(B_Voltage!=0)
        &&((VehCo_stNewUB==_VEHCO_INDEX_STATUS_RUNNING)||(VehCo_stNewUB==_VEHCO_INDEX_STATUS_AFTERRUN))
        ) 
    { Power=1; }else{ Power=0;}
  
    return (Power);
    
}



//油泵控制程序  //100ms间隔运行
void Oil_Pump_Control(void) 
{
    uint8 Power=0,Oil_Pump_Cmd_Mode_Tmp=0,Oil_Pump_Fault_Reset_Tmp=0;
    sint16 Oil_Pump_Cmd_Torque_Tmp=0;
    sint16 Oil_Pump_Cmd_Speed_Tmp=0;
    sint16 Oil_Pump_Speed_Get=0;
    
    Power=Power_OK();

    //电源、通讯正常且已经上高压或有车速
    if (
       (Power==1)&&(Oil_Pump_DC_Voltage/9>(B_Voltage/10) )&& (Oil_Pump_Communication==0)
       //&&((Vehco_State==4)||(Vehco_State==5)||(Vehicle_Speed>50))
       &&(Oil_Pump_Fault==0) 
       &&(Oil_Pump_Debug==0)
       &&(Oil_Pump_Still_OverTime==0)
        ) 
        
    {
        Oil_Pump_Cmd_Mode_Tmp=2;                      //2 转速模式
        Oil_Pump_Cmd_Torque_Tmp=Oil_Pump_Torque_Limit_Get;//限制转矩8.68
        Oil_Pump_Fault_Reset_Tmp=0;
        
        //获取当前车速对应油泵转速
        //AccPed2=Intpol_u16u16(AccPedCD_rUW_CUR2_Addr,uAD2); //0.375～2.05V
 
        Oil_Pump_Speed_Get= Intpol_s16s16(Oil_Pump_Speed_VSpeed_Addr, Vehicle_Speed-50) ;



       //升速控制
       if (Oil_Pump_Cmd_Speed<Oil_Pump_Speed_Get)
       {
          Oil_Pump_Cmd_Speed_Tmp=Oil_Pump_Cmd_Speed+Oil_Pump_Speed_Step[0];
          
          Oil_Pump_Cmd_Speed_Tmp=(Oil_Pump_Cmd_Speed_Tmp>Oil_Pump_Speed_Get) 
                                  ? Oil_Pump_Speed_Get:Oil_Pump_Cmd_Speed_Tmp;
       }else 
       {
          Oil_Pump_Cmd_Speed_Tmp=Oil_Pump_Speed_Get;
       }
       //降速控制
       if (Oil_Pump_Cmd_Speed>Oil_Pump_Speed_Get) 
       {
          Oil_Pump_Cmd_Speed_Tmp=Oil_Pump_Cmd_Speed-Oil_Pump_Speed_Step[1];
          
          Oil_Pump_Cmd_Speed_Tmp=(Oil_Pump_Cmd_Speed_Tmp<Oil_Pump_Speed_Get) 
                                  ? Oil_Pump_Speed_Get:Oil_Pump_Cmd_Speed_Tmp;
       }else
       {
          Oil_Pump_Cmd_Speed=Oil_Pump_Speed_Get;
       }
       //Oil_Pump_Still 堵转、反转判断
       
       
       
    }
    else
    {
        Oil_Pump_Cmd_Mode_Tmp   =0;
        Oil_Pump_Cmd_Torque_Tmp =0;
        Oil_Pump_Cmd_Speed_Tmp  =0;            
        //油泵复位程序  电源正常 油泵通讯正常
        if ((Oil_Pump_Fault==1)&&(Power==1)&&(Oil_Pump_Communication==0))
        {
            Oil_Pump_Fault_Reset_Tmp=1;
           
        }else 
        {
            Oil_Pump_Fault_Reset_Tmp=0;
        }
    }

    
    
    if (Oil_Pump_Debug==1)
    {
        Oil_Pump_Cmd_Mode   =Oil_Pump_Cmd_Mode_Debug;
        Oil_Pump_Fault_Reset=Oil_Pump_Fault_Reset_Debug;
        Oil_Pump_Cmd_Torque =Oil_Pump_Cmd_Torque_Debug;
        Oil_Pump_Cmd_Speed  =Oil_Pump_Cmd_Speed_Debug;
        
    }else 
    {
        Oil_Pump_Cmd_Mode   =Oil_Pump_Cmd_Mode_Tmp;
        Oil_Pump_Fault_Reset=Oil_Pump_Fault_Reset_Tmp;
        Oil_Pump_Cmd_Torque =Oil_Pump_Cmd_Torque_Tmp;
        Oil_Pump_Cmd_Speed  =Oil_Pump_Cmd_Speed_Tmp;    
    }
    //限制油泵转速
    if  (abs(Oil_Pump_Cmd_Speed)>Oil_Pump_Speed_Limit_Get)
    
    { 
        Oil_Pump_Cmd_Speed=(Oil_Pump_Cmd_Speed<0)?
                           -1 * Oil_Pump_Speed_Limit_Get:Oil_Pump_Speed_Limit_Get;  
    }

}


//气泵控制程序  //100ms间隔运行
void Air_Pump_Control(void) 
{
    //Air_Reservoirs_Pressure_1 Air_Pump_Taget_Pressure_Get   Air_Pump_Speed_Limit_Get
    
    uint8 Power=0,Air_Pump_Cmd_Mode_Tmp=0,Air_Pump_Fault_Reset_Tmp=0;
    sint16 Air_Pump_Cmd_Torque_Tmp=0;
    sint16 Air_Pump_Cmd_Speed_Tmp=0;
    
    
    Power=Power_OK();
    
    //电源、通讯正常且已经上高压或有车速   Air_Pump_Fan  Air_Reservoirs_Pressure_Low_Index
    if (
     (Power==1)&&(Air_Pump_DC_Voltage/9>(B_Voltage/10) )&& (Air_Pump_Communication==0)
     //&&((Ready==1)||(Vehicle_Speed>50))
     &&(Air_Pump_Fault==0)&&(Air_Pump_OverHeated==0)
     //&&(Air_Pump_WorkTime_Index<Air_Pump_WorkTime_Get*2)
     &&(Air_Pump_Cool==0) 
     &&(Air_Pump_Debug==0)
     &&(Air_Pressure_Ok_OverTime==0)
     &&(BattCD_bLogicOnUB==1)
                                             
      )
    {
 
        Air_Pump_Cmd_Mode_Tmp=2;//速度模式
        Air_Pump_Cmd_Torque_Tmp=Air_Pump_Torque_Limit_Get;//限制转矩
        Air_Pump_Fault_Reset_Tmp=0;

        //小于  0.85倍目标压力启动
        if ((Air_Reservoirs_Pressure_1< (uint8)((uint16)Air_Pump_Taget_Pressure_Get)*Air_Start_Press_Rate_Get/100) 
            || (Air_Reservoirs_Pressure_2< (uint8)((uint16)Air_Pump_Taget_Pressure_Get)*Air_Start_Press_Rate_Get/100)  )
        {
            //升速控制             
            if (Air_Pump_Cmd_Speed<Air_Pump_Speed_Limit_Get)
            {
                Air_Pump_Cmd_Speed_Tmp=Air_Pump_Cmd_Speed+Air_Pump_Speed_Step[0]; 
                Air_Pump_Cmd_Speed_Tmp=(Air_Pump_Cmd_Speed_Tmp>Air_Pump_Speed_Limit_Get) 
                                       ? Air_Pump_Speed_Limit_Get:Air_Pump_Cmd_Speed_Tmp;                 
            }else
            {
                Air_Pump_Cmd_Speed_Tmp=Air_Pump_Speed_Limit_Get;
            }
                    
        }
        else
        {
      
            if (Air_Pump_Cmd_Mode==2)
            {
                Air_Pump_Cmd_Speed_Tmp=Air_Pump_Speed_Limit_Get;
            }
            
        }
        
        //达到最大转矩，且转矩陡降4N.m       
        if  ( (Air_Reservoirs_Pressure_1> (uint8)Air_Pump_Taget_Pressure_Get/3) 
              && (Air_Reservoirs_Pressure_2> (uint8)Air_Pump_Taget_Pressure_Get/3)
              && (Air_Pressure_Ok==0)&&(Air_Pump_Speed >=Air_Pump_Speed_Limit_Get )&&  ((Air_Pump_Torque<=(Air_Pump_Torque_Limit_Get/2)  ) ||((Air_Pump_Torque_Last-Air_Pump_Torque)>Air_Pump_Torque_DrapDown_Get))   )
            
          
        {
            Air_Pressure_Ok=1;
            Air_Pressure_Delay=0;
            Air_Reservoirs_Over_Pressure_1   =Air_Reservoirs_Pressure_1;
            Air_Reservoirs_Over_Pressure_2   =Air_Reservoirs_Pressure_2;

        }
        //借用排气的逻辑做堵转     堵转Air_Pump_Speed_DrapDown_Get      Air_Pump_Torque_Limit_Get
        
        if (
            (Air_Pump_Cmd_Speed >=Air_Pump_Speed_Limit_Get )&&
            ((Air_Pump_Speed_Last-Air_Pump_Speed)>Air_Pump_Speed_DrapDown_Get)
              &&((Air_Reservoirs_Pressure_1 <Air_Pump_Taget_Pressure_Get) ||(Air_Reservoirs_Pressure_2 <Air_Pump_Taget_Pressure_Get))) 
        {
            Air_Pump_Still=1;
            //Air_Pressure_Delay=0;
            Air_Reservoirs_Over_Pressure_1   =Air_Reservoirs_Pressure_1;
            Air_Reservoirs_Over_Pressure_2   =Air_Reservoirs_Pressure_2;       
        }
        
        if ( ((Air_Pressure_Ok==1)||(Air_Pump_Still==1))  &&(Air_Pressure_Delay<Air_Pressure_Delay_Max_Get))
        {
            Air_Pressure_Delay++;
                    
        }
        if (Air_Pressure_Delay>=Air_Pressure_Delay_Max_Get)
        //Air_Pressure_Ok_OverTime达到排气时间 停机
        {
            Air_Pressure_Ok_OverTime=1;     
            Air_Reservoirs_Over_Pressure_1   =Air_Reservoirs_Pressure_1;
            Air_Reservoirs_Over_Pressure_2   =Air_Reservoirs_Pressure_2;

        }
        
        
        
        if (Air_Pump_WorkTime_Index%5 ==0) //每隔0.5秒采样一次
        {
            Air_Pump_Torque_Last= Air_Pump_Torque;
            Air_Pump_Speed_Last=Air_Pump_Speed;
        }
        
        
        
        
        //达到目标压力减速 
        if ((Air_Reservoirs_Pressure_1 >Air_Pump_Taget_Pressure_Get) &&(Air_Reservoirs_Pressure_2 >Air_Pump_Taget_Pressure_Get) )
        {
            //降速控制
            if (Air_Pump_Cmd_Speed>0)
            {
                Air_Pump_Cmd_Speed_Tmp=Air_Pump_Cmd_Speed-Air_Pump_Speed_Step[1]; 
                Air_Pump_Cmd_Speed_Tmp=(Air_Pump_Cmd_Speed_Tmp>0)
                                       ?Air_Pump_Cmd_Speed_Tmp:0;                  
            }         
        }

        
        //速度为0 关闭气泵
        if (Air_Pump_Cmd_Speed_Tmp==0) 
        {
            Air_Pump_Cmd_Mode_Tmp   =0;
            Air_Pump_Cmd_Torque_Tmp =0;
        }
              
         
        //uint16 Air_Reservoirs_Pressure_Low_Index=0; 
        //气泵工作气压仍<0.1MPa ,计数 0.1秒步进    
        //气泵工作 气压不变                    20秒关机保护   2018.10.23 改为2/3低速长时间持续工作
        if ((Air_Pump_Cmd_Mode_Tmp!=0)&&
            ((Air_Reservoirs_Pressure_1<10)&&(Air_Reservoirs_Pressure_2<10))
            ||((Air_Reservoirs_Pressure_Last_2*3 <Air_Pump_Taget_Pressure_Get*2) &&(Air_Reservoirs_Pressure_Last_1*3 <Air_Pump_Taget_Pressure_Get*2)  &&(Air_Reservoirs_Pressure_Last_1==Air_Reservoirs_Pressure_1)&&(Air_Reservoirs_Pressure_Last_2==Air_Reservoirs_Pressure_2) )
            
            ) 
        
        
        {
            Air_Reservoirs_Pressure_Low_Index++;
            if (Air_Reservoirs_Pressure_Low_Index>=200) 
            {
               // Air_Pump_Cmd_Mode_Tmp   =0;
               // Air_Pump_Cmd_Torque_Tmp =0;
               // Air_Pump_Cmd_Speed_Tmp  =Air_Pump_Speed_Limit_Get*3/2;
               // Air_Pump_WorkTime_Index =200; 
                Air_Reservoirs_Pressure_Low_Index=200;
                
            }
        }else
        {
            Air_Reservoirs_Pressure_Low_Index=0;
        }
        if  (Air_Pump_Cmd_Mode_Tmp!=0)
        {
            Air_Reservoirs_Pressure_Last_1=Air_Reservoirs_Pressure_1;
            Air_Reservoirs_Pressure_Last_2=Air_Reservoirs_Pressure_2;
        }
        
        //气泵工作计时
        if (Air_Pump_Cmd_Mode_Tmp!=0) 
        {
            Air_Pump_WorkTime_Index++;
            if (Air_Pump_WorkTime_Index>=Air_Pump_WorkTime_Get) 
            { Air_Pump_Cool=1;            }
        }
        //气泵停机计时
        if ((Air_Pump_Cmd_Mode_Tmp==0)&&(Air_Pump_WorkTime_Index>0))
        {
            //快减       
            Air_Pump_WorkTime_Index--;
            Air_Pump_WorkTime_Index--;
            Air_Pump_WorkTime_Index--;
            Air_Pump_WorkTime_Index--;
            Air_Pump_WorkTime_Index--;
            Air_Pump_WorkTime_Index--;
            Air_Pump_WorkTime_Index--;
            Air_Pump_WorkTime_Index--;            
        }      
        if  (Air_Pump_WorkTime_Index<=0)
        {    
            Air_Pump_WorkTime_Index=0;  
            Air_Pump_Cool=0;        
        }                 
               
        
     } 
    else 
    {
        Air_Pump_Cmd_Mode_Tmp   =0;
        Air_Pump_Cmd_Torque_Tmp =0;
        Air_Pump_Cmd_Speed_Tmp  =0;            
        //气泵复位程序  电源正常 气泵通讯正常
        if ((Air_Pump_Fault==1)&&(Power==1)&&(Air_Pump_Communication==0))
        {
            Air_Pump_Fault_Reset_Tmp=1;

            
        }else 
        {
            Air_Pump_Fault_Reset_Tmp=0;
        }
        /*
         Air_Pressure_Ok_OverTime=1;     
         Air_Reservoirs_Over_Pressure_1   =Air_Reservoirs_Pressure_1;
         Air_Reservoirs_Over_Pressure_2   =Air_Reservoirs_Pressure_2;
        */
        //排气后重新启动气泵
        if ((Air_Reservoirs_Over_Pressure_2!=0)&&(Air_Reservoirs_Over_Pressure_1!=0)
            &&(Air_Reservoirs_Pressure_1!=0)&&(Air_Reservoirs_Pressure_2!=0)
            &&((Air_Reservoirs_Pressure_1<(uint8) (((uint16) Air_Reservoirs_Over_Pressure_1)*Air_Over_Reset_Get/100 )  )
            ||(Air_Reservoirs_Pressure_2<(uint8) (((uint16) Air_Reservoirs_Over_Pressure_2)*Air_Over_Reset_Get/100 )  ))
            )                                                                                    
            {
                Air_Pressure_Ok_OverTime=0;
                Air_Reservoirs_Over_Pressure_1=0;//(uint8) Air_Pump_Taget_Pressure_Get;
                Air_Reservoirs_Over_Pressure_2=0;//(uint8) Air_Pump_Taget_Pressure_Get;
                Air_Pressure_Delay=0;  
                Air_Pressure_Ok=0;
                Air_Pump_Still=0; 
                    
            }
        
        //气泵停机计时
        if ((Air_Pump_Cmd_Mode_Tmp==0)&&(Air_Pump_WorkTime_Index>0))
        {   
            Air_Pump_WorkTime_Index--;
            Air_Pump_WorkTime_Index--;
            Air_Pump_WorkTime_Index--;
            Air_Pump_WorkTime_Index--;
            Air_Pump_WorkTime_Index--;
            Air_Pump_WorkTime_Index--;
            Air_Pump_WorkTime_Index--;
            Air_Pump_WorkTime_Index--;            
            
        }
        if  (Air_Pump_WorkTime_Index<=0)
        {    
            Air_Pump_WorkTime_Index=0;  
            Air_Pump_Cool=0;        
        }                
    }
    
    if (Air_Pump_Debug==1)
    {
        Air_Pump_Cmd_Mode   =Air_Pump_Cmd_Mode_Debug;
        Air_Pump_Fault_Reset=Air_Pump_Fault_Reset_Debug;
        Air_Pump_Cmd_Torque =Air_Pump_Cmd_Torque_Debug;
        Air_Pump_Cmd_Speed  =Air_Pump_Cmd_Speed_Debug;
        
    }else 
    {   //  仪表通讯故障或长时间低气压，气泵进入持续低速运行模式，保障气压（开环控制）   
        if ((LostINSCommunication==1)||(Air_Reservoirs_Pressure_Low_Index>=200))
        {
            Air_Pump_Cmd_Mode   =2;
            Air_Pump_Fault_Reset=0;
            Air_Pump_Cmd_Torque =Air_Pump_Torque_Limit_Get;
            Air_Pump_Cmd_Speed  =Air_Pump_Speed_Limit_Get*2/3;  
                        
        }else
        {
            Air_Pump_Cmd_Mode   =Air_Pump_Cmd_Mode_Tmp;
            Air_Pump_Fault_Reset=Air_Pump_Fault_Reset_Tmp;
            Air_Pump_Cmd_Torque =Air_Pump_Cmd_Torque_Tmp;
            Air_Pump_Cmd_Speed  =Air_Pump_Cmd_Speed_Tmp;         
        }
 
        
          
    }
    
    if (Air_Pump_Cmd_Mode!=0){ Air_Pump_Fan_Delay_Index=Air_Pump_Fan_Delay_Get; }
    
   //风扇控制
    if ((Air_Pump_Cmd_Mode!=0)||(Air_Pump_OverHeated==1)||(Air_Pump_Fan_Delay_Index>0))
    {
        Air_Pump_Fan=1;
        if (Air_Pump_Cmd_Mode==0)
        {   
            Air_Pump_Fan_Delay_Index--;           
        }
    }else 
    {
        Air_Pump_Fan=0;
    }
    
}












