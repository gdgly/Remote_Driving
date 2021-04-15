/*******************************************************************************
* 工程名称：机械及电子排档杆处理程序
* 文件名： Vin_ESK.c
* 功能描述 ： 
* 引用记录：
* 备注： 
* 修订记录：
*		日期		      版本		  作者		描述
*   
*******************************************************************************/
#include <string.h>
#include "GlobalDef.h"
#include "Vin_ESK.h"

    
#ifndef AECS_HW
       #pragma DATA_SEG DEFAULT
#else  
      #pragma DATA_SEG S12X_RAM
#endif




uint8 ESK_Signal_Type=0;//档位信号类型 0 电平信号 1 CAN总线信号 
uint8 ESK_Type;//电子换档位类型  0 拨片式、旋钮式（点动信号）  1 排档杆式、旋钮式（自锁信号）
uint8 ESK_Fault;//电子换档器故障
uint8 ESK_Live;//电子换档器心跳信号 0~15

uint8 ESK_SelfLock_Request; //电子换挡器信号请求  自锁式  手刹另外硬线接入
uint8 ESK_Inching_Request;  //电子换挡器信号请求  点动式  含手刹信号

uint8 Dang_S,Dang_E,Dang_EE,Dang_C;
uint8 Breakk_Please;//踩制动踏板提醒


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

extern sint8  Gears_GetPow; //变速箱取力开关状态  0 未输出  1 输出 3 异常
extern uint8  Gears_Install;

 
#ifndef AECS_HW
      #pragma CODE_SEG DEFAULT
#else  
      #pragma CODE_SEG VCU_CODE
      
#endif


//档位状态判断 10ms
void Vehco_Dang_Status(void) 
{   
    
    if (ESK_Signal==1)  //收到CAN通讯使用标定类型，否则使用电平式信号类型
    {
        ESK_Signal_Type=ESK_Signal_Enable;
    }else 
    {
        ESK_Signal_Type=0;
    }

    switch(ESK_Signal_Type) 
    { //根据信号类型决定档位请求信号 ,不包含 点动式

      case 1: //CAN信号
          if (ESK_Type==1)
          {   //1 排档杆式、旋钮式（自锁信号）
              Dang_P=bDiSatus[14];   //手刹信号仍由硬线电平控制
              switch(ESK_SelfLock_Request) 
              {
                  default:
                  case 0:
                      Dang_S=0; Dang_E=0; Dang_EE=0;  Dang_R=0; Dang_N=1; Dang_D=0,Dang_C=0;
                      break;
                  case 2:
                      Dang_S=1; Dang_E=0; Dang_EE=0;  Dang_R=0; Dang_N=0; Dang_D=1,Dang_C=0;//   S E E+ 是否保留D
                        break;
                  case 3:
                      Dang_S=0; Dang_E=1; Dang_EE=0;  Dang_R=0; Dang_N=0; Dang_D=1,Dang_C=0;//  E
                      break;
                  case 4:
                      Dang_S=0; Dang_E=0; Dang_EE=1;  Dang_R=0; Dang_N=0; Dang_D=1,Dang_C=0;//  E+
                      break;
                  case 5:
                      Dang_S=0; Dang_E=0; Dang_EE=0;  Dang_R=1; Dang_N=0; Dang_D=0,Dang_C=0;
                      break;
                  case 10:
                      Dang_S=0; Dang_E=0; Dang_EE=0;  Dang_R=0; Dang_N=0; Dang_D=1,Dang_C=0;
                      break;
                 case 11:
                      Dang_S=0; Dang_E=0; Dang_EE=0;  Dang_R=0; Dang_N=0; Dang_D=1,Dang_C=1;//  C
                      break;
              }
          }else 
          {   //ESK_Type!=1 拨片式、旋钮式（点动信号)    未做
              //uint8 ESK_Inching_Request;  //电子换挡器信号请求  点动式  含手刹信号     bit 0001,1111  含义 P,-2,-1,+1,+2
              Dang_S=0; Dang_E=0; Dang_EE=0;  Dang_R=0; Dang_N=0; Dang_D=0; Dang_P=0; Dang_C=0;
              
              
              
              
          }


          LostESKCom_Index++;

          if (LostESKCom_Index>500)   //ESK 通讯超时5S
          {
              LostESKCom_Index=501;
              LostESKCommunication=1;
          } else 
          {
              LostESKCommunication=0;
          }

          //故障处理  自锁式  点动未做   
          if ((ESK_Fault==1)||(LostESKCommunication ==1))
          {
              Dang_S=0; Dang_E=0; Dang_EE=0;  Dang_R=0; Dang_N=0; Dang_D=0; Dang_P=0;
          }
          break;
      default:
      case 0: //电平式信号
          Dang_P=bDiSatus[14];    //手刹信号由硬线电平控制
          Dang_R=bDiSatus[15];
          Dang_N=bDiSatus[16];
          Dang_D=bDiSatus[17];                
          Dang_S=0;
          Dang_E=0;
          Dang_EE=0;
          Dang_C=0;
          break;
    }
    
    // ESK_Signal_Type;//档位信号类型    0 电平信号    1 CAN总线信号 
    // ESK_Type;       //电子换档位类型  0 拨片式、旋钮式（点动信号）  1 排档杆式、旋钮式（自锁信号）   
   
    if ((ESK_Signal_Type==0)                           //类型0 电平信号
        || ((ESK_Signal_Type==1)&&(ESK_Type==1))  )    //类型1 CAN总线信号&&（自锁信号）   

    {   //排档式、自锁式换挡逻辑
        Vehco_Dang_SelfLock_Status();
    }else 
    {   //拨片式、旋钮式点动换挡逻辑    未做
        Vehco_Dang_ESK_Inching_Request_Status();
    }
}
 //拨片式、旋钮式点动换挡逻辑
void Vehco_Dang_ESK_Inching_Request_Status(void) 
{//uint8 ESK_Inching_Request;  //电子换挡器信号请求  点动式  含手刹信号     bit 0001,1111  含义 P,-2,-1,+1,+2




}

//排档式、自锁式换挡逻辑
void Vehco_Dang_SelfLock_Status(void)
{   
    //车未停稳前换挡，油门无效，车速为-100~100后才生效。 Motor_Speed
    
    switch(Dang_D+(Dang_N<<1)+(Dang_R<<2)+(Dang_P<<3))  // PRND 0000
    {
        case 0:
        case 2:
            Dang_L4bit=0;  //N档  0010 0000
            Breakk_Please=0;
            break;
        case 1:
        case 9:
            if (Dang_L4bit!=10) 
            {
                if (   ((Breakk>=10)||(Motor_Speed>=10*Motor_Vehicle_Speed_Scale_Get))
                    && (AccPed<=100)&&(Motor_Speed>=(sint16) -2* ((sint16)Motor_Vehicle_Speed_Scale_Get)))  //踩油门挂挡无效，松油门后档位生效  踩制动或车速大于10档位有效
                {
                    Dang_L4bit=10; //D档  0001 1001
                    Breakk_Please=0 ;

                } 
                else 
                {
                    Dang_L4bit=0; //该处做操作错误报警
                    if ((Breakk<100)||(AccPed>100))
                    {
                      Breakk_Please=1 ;
                    }
                }
            }
            break;
        case 4:
        case 12:
            if (Dang_L4bit!=5)
            {
                if ((Breakk>=10)&&(AccPed<=100)&&(Motor_Speed<=2*(sint16)Motor_Vehicle_Speed_Scale_Get)) //踩油门挂挡无效，松油门后档位生效   踩制动档位有效
                {
                    Dang_L4bit=5;  //R档  0100 1100
                    Breakk_Please=0;
                } 
                else
                {
                    Dang_L4bit=0;   //该处做操作错误报警
                    if ((Breakk<100)||(AccPed>100))
                    {
                      Breakk_Please=1 ;
                    }
                }
            }
            break;
        case 8:
        case 10:
            Dang_L4bit=1;  //P档  1000 1010 
            Breakk_Please=0; 
            break;
         default:
            //其他配合为错误 需置空档
            Dang_L4bit=0;
            break;   
    }
    //取力器工作时档位锁定到空挡
    if ((Gears_GetPow==1)&&(Gears_Install==1))
    {
        Dang_L4bit=Dang_P;
    }


}
















