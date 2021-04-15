/*******************************************************************************
* 工程名称：第三方终端锁车、限速功能
* 文件名： Vin_Third_Lock.c
* 功能描述 ： 
* 引用记录：
* 备注： 
* 修订记录：
*		日期		      版本		  作者		描述
*   
*******************************************************************************/
#include <string.h>
#include "GlobalDef.h"
#include "Vin_Third_Lock.h"

    
#ifndef AECS_HW
       #pragma DATA_SEG DEFAULT
#else  
      #pragma DATA_SEG S12X_RAM
#endif

uint8 Third_Lock_PowOn_Delay=0; //第三方锁车上电3秒判断锁车状态计时器   100ms +1


uint8 Third_Lock_GetNewMsg_HongQuan=0;
uint8 Third_Lock_GetNewMsg_MaiJiaMai=0;
uint8 Third_Lock_RandKey[5] ={0} ;
uint8 Third_Lock_Com_Index =0;
uint8 Third_Lock_Life =0;

uint8 Third_Lock_Command=0;             //终端锁车CAN指令

uint8 Third_Lock_Active=0;              //锁车激活
uint8 Third_Lock_Mode_Active=0;         //生效锁车模式
uint8 Third_Lock_LimitSpeed_Active=0;   //生效限速速度 


extern uint8 Third_Lock_Mode;        //EEPROM     锁车模式
extern uint8 Third_Lock_LimitSpeed;  //EEPROM     限速速度
extern uint8 Third_Lock_HongQuan_Active;  //杭州鸿泉终端激活状态
extern uint8 Third_Lock_MaiJiaMai_Active;  //河南迈佳迈终端激活状态



extern uint16 AccPed,Breakk;
extern uint8 Dang_L4bit;
extern sint16 Motor_Speed;
extern uint16 Motor_Vehicle_Speed_Scale_Get;   //多档车改为UINT16



#ifndef AECS_HW
      #pragma CODE_SEG DEFAULT
#else  
      #pragma CODE_SEG VCU_CODE
      
#endif

//迈佳迈  上电3秒检测心跳、锁车指令，锁车动作：限速10； 随时检测解锁指令；满足安全条件下解除锁车指令  (车速为0，档位空档或P档，无油门，有刹车)

//鸿泉    上电3秒检测心跳、锁车指令，锁车动作：禁止行驶； 随时检测解锁指令；满足安全条件下解除锁车指令(车速为0，档位空档或P档，无油门，有刹车)
//        随时检测、执行限速指令； 随时检测解除限速指令，满足安全条件下解除限速指令 (车速为0，档位空档或P档，无油门，有刹车)




//三方锁车程序
void Third_Lock_Vehicle(void) 
{

    //上电3秒检测心跳及锁车指令
    if (Third_Lock_PowOn_Delay<=Third_Lock_PowOn_Delay_Max) 
    {
        Third_Lock_PowOn_Delay++;
        Third_Lock_Com_Index++;
        //锁车终端激活  执行锁车和心跳判断
        if ((Third_Lock_HongQuan_Active==1)||(Third_Lock_MaiJiaMai_Active==1)) 
        {
            if (Third_Lock_GetNewMsg_HongQuan==1) //收到杭州鸿泉指令
            {   Third_Lock_GetNewMsg_HongQuan=0;
                switch (Third_Lock_Command)
                {
                    default:
                        break;
                    case 1:   //限速
                        Third_Lock_Mode=1;
                        if (Third_Lock_LimitSpeed<15) 
                        {
                            Third_Lock_LimitSpeed=15;
                        }
                        if (Third_Lock_LimitSpeed>120)
                        {
                            Third_Lock_LimitSpeed=120;
                        }
                        break;
                    case 2:   //锁车
                        Third_Lock_Mode=2;
                        Third_Lock_LimitSpeed=0;
                        break;
                    case 4:  //解锁
                        Third_Lock_Mode=0;
                        Third_Lock_LimitSpeed=0;                      
                        break;    
                }
            }
            if (Third_Lock_GetNewMsg_MaiJiaMai==1) //收到迈佳迈指令
            {   Third_Lock_GetNewMsg_MaiJiaMai=0;
                switch (Third_Lock_Command)
                {
                    default:
                        break;
                    case 1:   //解锁
                        Third_Lock_Mode=0;
                        Third_Lock_LimitSpeed=0;
                        break;
                    case 2:   //锁车 限速10
                        Third_Lock_Mode=1;
                        Third_Lock_LimitSpeed=10;
                        break;
                }
            }
            //丢失终端通讯 2.8秒      激活锁车
            if (Third_Lock_Com_Index>=Third_Lock_PowOn_Delay_Max-2) 
            {
                if (Third_Lock_HongQuan_Active==1) 
                {
                    Third_Lock_Mode=2;
                    Third_Lock_LimitSpeed=0;
                    
                    Third_Lock_Active=1; 
                    Third_Lock_Mode_Active=Third_Lock_Mode;
                    Third_Lock_LimitSpeed_Active=0; 
                
                }
                if (Third_Lock_MaiJiaMai_Active==1) 
                {
                    Third_Lock_Mode=1;
                    Third_Lock_LimitSpeed=10;                

                    Third_Lock_Active=1; 
                    Third_Lock_Mode_Active=Third_Lock_Mode;
                    Third_Lock_LimitSpeed_Active=0; 

                }
            }
            //锁车判断   Breakk
            //迈佳迈  上电3秒检测心跳、锁车指令，锁车动作：限速10； 随时检测解锁指令；满足安全条件下解除锁车指令  (车速为0，档位空档或P档，无油门，有刹车)

            //鸿泉    上电3秒检测心跳、锁车指令，锁车动作：禁止行驶； 随时检测解锁指令；满足安全条件下解除锁车指令(车速为0，档位空档或P档，无油门，有刹车)
            //        随时检测、执行限速指令； 随时检测解除限速指令，满足安全条件下解除限速指令 (车速为0，档位空档或P档，无油门，有刹车)            
            
            //上电前3秒，满足安全条件下执行锁车、解锁指令
            if ((Breakk>100)&&(AccPed<100)&&((Dang_L4bit==0)||(Dang_L4bit==1)) ) 
            {
                switch (Third_Lock_Mode) 
                {
                    default:
                        break;
                    case 0: //解锁   停车解锁
                        if (abs(Motor_Speed)/Motor_Vehicle_Speed_Scale_Get<=1) 
                        {
                            Third_Lock_Mode_Active=Third_Lock_Mode;
                            Third_Lock_LimitSpeed_Active=Third_Lock_LimitSpeed;
                            Third_Lock_Active=0;                        
                        }
                        break;
                    case 1: //限速
                        Third_Lock_Active=1;
                        Third_Lock_Mode_Active=Third_Lock_Mode;
                        Third_Lock_LimitSpeed_Active=Third_Lock_LimitSpeed;
                        break;
                    case 2: //锁车
                        Third_Lock_Active=1; 
                        Third_Lock_Mode_Active=Third_Lock_Mode;
                        Third_Lock_LimitSpeed_Active=0; 
                        break;
                }
            
            }


        }        
    }else 
    {   //3秒以后
        //锁车终端激活  执行锁车和心跳判断
        if ((Third_Lock_HongQuan_Active==1)||(Third_Lock_MaiJiaMai_Active==1)) 
        {
            if (Third_Lock_GetNewMsg_HongQuan==1) //收到杭州鸿泉指令
            {   Third_Lock_GetNewMsg_HongQuan=0;
                switch (Third_Lock_Command)
                {
                    default:
                        break;
                    case 1:   //限速
                        Third_Lock_Mode=1;
                        if (Third_Lock_LimitSpeed<15) 
                        {
                            Third_Lock_LimitSpeed=15;
                        }
                        if (Third_Lock_LimitSpeed>120)
                        {
                            Third_Lock_LimitSpeed=120;
                        }
                        break;
                    case 2:   //锁车
                        Third_Lock_Mode=2;
                        Third_Lock_LimitSpeed=0;
                        break;
                    case 4:  //解锁
                        Third_Lock_Mode=0;
                        Third_Lock_LimitSpeed=0;                      
                        break;    
                }
            }
            if (Third_Lock_GetNewMsg_MaiJiaMai==1) //收到迈佳迈指令
            {   Third_Lock_GetNewMsg_MaiJiaMai=0;
                switch (Third_Lock_Command)
                {
                    default:
                        break;
                    
                    case 1:   //解锁
                        Third_Lock_Mode=0;
                        Third_Lock_LimitSpeed=0;
                        break;
                    /*    
                    case 2:   //限速10   3秒后不接收限速指令
                        Third_Lock_Mode=1;
                        Third_Lock_LimitSpeed=10;
                        break;
                    */
                }
            }

            //锁车判断 
            
            //上电3秒后，满足安全条件下执行锁车、解锁指令
            if ((Breakk>100)&&(AccPed<100)&&((Dang_L4bit==0)||(Dang_L4bit==1)||(Dang_L4bit==10)) ) 
            {
                switch (Third_Lock_Mode) 
                {
                    default:
                        break;
                    case 0: //解锁    停车解锁
                        if ((abs(Motor_Speed)/Motor_Vehicle_Speed_Scale_Get<=1)&&((Dang_L4bit==0)||(Dang_L4bit==1)) )
                        {
                            Third_Lock_Active=0; 
                            Third_Lock_Mode_Active=Third_Lock_Mode;
                            Third_Lock_LimitSpeed_Active=0;                                                   
                        }
                        break;
                    case 1: //限速      鸿泉执行限速指令，迈佳迈前面已经不接收限速指令
                        
                        Third_Lock_Active=1;
                        Third_Lock_Mode_Active=Third_Lock_Mode;
                        Third_Lock_LimitSpeed_Active=Third_Lock_LimitSpeed;                        
                        break;
                    case 2: //锁车
                        //Third_Lock_Active=1;  //3秒后不执行锁车指令
                        //Third_Lock_Mode_Active=Third_Lock_Mode;
                        //Third_Lock_LimitSpeed_Active=Third_Lock_LimitSpeed;
                        break;
                }
            
            }


        }
    }
    if (Third_Lock_Enable==0)
    {
        Third_Lock_Mode=0;
        Third_Lock_LimitSpeed=0;
        Third_Lock_HongQuan_Active=0;
        Third_Lock_MaiJiaMai_Active=0;
        
        Third_Lock_Active=0;
        Third_Lock_Mode_Active=0;
        Third_Lock_LimitSpeed_Active=0;
        
    }
    
    
    
}
















