/*******************************************************************************
* 工程名称：驱动层转换
* 文件名： Vin_Interface_conversion.c
* 功能描述 ： 不同硬件底层接口转化
* 引用记录：
* 备注： 
* 修订记录：
*		日期		      版本		  作者		描述
* 2017.8.3                  宋罡    更新接口定义
*******************************************************************************/

#include "GlobalDef.h"
#include "Vin_Interface_conversion.h"

#ifndef AECS_HW
      #pragma DATA_SEG DEFAULT
#else  
      #pragma DATA_SEG S12X_RAM
#endif


uint8 bDiSatus[24]={0};     //DI口开关量状态
uint16 uADSatus[25]={0};    //模拟量数值
uint8 bDoSatus[20]={0};     //DO口开关量状态
uint16 uAD4Satus[4]={0};    //4路AD模拟量数值
uint16 uAD_LOGICP_CHAN=0;    //点火开关电压

uint8 DoErr[20]={  0 }; 

extern uint32 fSet;  //频率设定   0.01~5000.00Hz
extern uint16 rSet;  //占空比设定 0~10000


#ifndef AECS_HW
      #pragma CODE_SEG DEFAULT
#else  
      #pragma CODE_SEG VCU_CODE 
#endif


void VehCo_Interface(void) 
{ //底层IO接口转换
  uint8 Index=0;
  #ifndef AECS_HW //易控平台
  {
    VCU_C_1[5]=2;
      
    for(Index=0;Index<18;Index++)//获得18路DI值的开关量
    {
        bDiSatus[Index]=DIODrv_GetChanState(Index);

         
    }
        
        bDiSatus[7]=!bDiSatus[7];//空调中压开关 D07\限速解除 L有效
        bDiSatus[8]=!bDiSatus[8];//真空罐报警 L有效
        bDiSatus[9]=!bDiSatus[9];//空调中压开关 D08 L有效
        bDiSatus[11]=!bDiSatus[11];//EPS故障  L有效
        bDiSatus[12]=!bDiSatus[12];//DCDC故障  L有效 
        bDiSatus[14]=!bDiSatus[14];//P  L有效 
        bDiSatus[15]=!bDiSatus[15];//R  L有效 
        bDiSatus[16]=!bDiSatus[16];//N  L有效 
        bDiSatus[17]=!bDiSatus[17];//D  L有效 
        
    for(Index=5;Index<23;Index++)//获得18路DI口的模拟量    5-22
    {
        uADSatus[Index-5]=ATDDrv_GetChanResult(Index); 
    }
        
    uAD_LOGICP_CHAN=ATDDrv_GetChanResult(23);  // IGN点火开关电压
    
    for(Index=1;Index<5;Index++)//获得4路模拟量数值  
    {
        uAD4Satus[Index-1]=ATDDrv_GetChanResult(Index); 
    }
    
    
    for (Index=1;Index<16;Index++) //设置各路DO口输出状态   0路不操作
    {
        PSWTDrv_Interface(Index,bDoSatus[Index]); 
        DoErr[Index]=PSwtDrv_GetDiagStatus(Index,0) & (PSwtDrv_GetDiagStatus(Index,1)<<4) ; //低4 open  高4 short 
        
    }

    //DoErr[3]=PSwtDrv_GetDiagStatus(3,0) & (PSwtDrv_GetDiagStatus(3,1)<<4) ; //低4 open  高4 short 

  }
  #else  //奥易克斯平台 
  {
    VCU_C_1[5]=1;
    //bDiSatus[9]= ATDDrv_GetChanResult(_ATD_INDEX_VLT10_CHAN)<SC_VDL_LevelLowThres  ?    1:0  ;// PTC温度开关2 L有效  X63
    bDiSatus[0]=DIODrv_GetChanState(_DIO_INDEX_DIN3_CHAN);//充电连接            X21  
    bDiSatus[1]=DIODrv_GetChanState(_DIO_INDEX_DIN6_CHAN);//ACC                 X24
    bDiSatus[2]=DIODrv_GetChanState(_DIO_INDEX_DIN5_CHAN);//START               X23
    bDiSatus[3]=DIODrv_GetChanState(_DIO_INDEX_DIN8_CHAN);//空调                X26
    bDiSatus[4]=DIODrv_GetChanState(_DIO_INDEX_DIN7_CHAN);//PTC                 X25

    bDiSatus[5]= ATDDrv_GetChanResult(_ATD_INDEX_VLT9_CHAN)<SC_VDL_LevelLowThres  ?    1:0   ;// PTC温度开关1 L有效  X56
    bDiSatus[6]= ATDDrv_GetChanResult(_ATD_INDEX_VLT5_CHAN)<SC_VDL_LevelLowThres   ?    1:0   ;// 空调温度开关 L有效  X70

    bDiSatus[7]=DIODrv_GetChanState(_DIO_INDEX_HVLTDI2_CHAN);//空调中压开关 750\限速解除 L有效     X8  
    bDiSatus[8]=DIODrv_GetChanState(_DIO_INDEX_DIN1_CHAN);//真空罐报警 L有效                X19
    bDiSatus[9]=DIODrv_GetChanState(_DIO_INDEX_DIN4_CHAN);//空调中压开关 D08        X22
    bDiSatus[10]=DIODrv_GetChanState(_DIO_INDEX_HVLTDI4_CHAN);//制动开关2           X10
    bDiSatus[11]=DIODrv_GetChanState(_DIO_INDEX_DIN2_CHAN);//EPS故障  L有效         X20
    bDiSatus[12]=DIODrv_GetChanState(_DIO_INDEX_HVLTDI1_CHAN);//DCDC故障 L有效      X7
    bDiSatus[13]=DIODrv_GetChanState(_DIO_INDEX_HVLTDI3_CHAN);//制动开关1           X9
    bDiSatus[14]=DIODrv_GetChanState(_DIO_INDEX_HVLTDI5_CHAN);//P  L有效            X11
    bDiSatus[15]=DIODrv_GetChanState(_DIO_INDEX_HVLTDI6_CHAN);//R  L有效            X12
    bDiSatus[16]=DIODrv_GetChanState(_DIO_INDEX_HVLTDI7_CHAN);//N  L有效            X13
    bDiSatus[17]=DIODrv_GetChanState(_DIO_INDEX_HVLTDI8_CHAN);//D  L有效            X14
    bDiSatus[18]= ATDDrv_GetChanResult(_ATD_INDEX_HVLTWK1_CHAN)>SC_VDL_LevelHighThres  ?    1:0   ;// 硬件唤醒1信号，对应接插件64   H有效  X64
    bDiSatus[19]= ATDDrv_GetChanResult(_ATD_INDEX_HVLTWK2_CHAN)>SC_VDL_LevelHighThres  ?    1:0   ;// 硬件唤醒2信号，对应接插件65   H有效  X65
    bDiSatus[20]= ATDDrv_GetChanResult(_ATD_INDEX_HVLTWK3_CHAN)>SC_VDL_LevelHighThres  ?    1:0   ;// 硬件唤醒3信号，对应接插件58   H有效  X58
    bDiSatus[21]=0;//保留
    bDiSatus[22]=0;//保留
    bDiSatus[23]=0;//保留
    
    /***********************************************************************************************************/
    uADSatus[0] =ATDDrv_GetChanResult(_ATD_INDEX_VLT6_CHAN);    //制动蹄片更换检测      X77   4095=5V
    uADSatus[1] =0;
    uADSatus[2] =0;
    uADSatus[3] =0;
    uADSatus[4] =0;
    uADSatus[5] =ATDDrv_GetChanResult(_ATD_INDEX_VLT9_CHAN);   // PTC温度开关1          X56   4095=5V
    uADSatus[6] =ATDDrv_GetChanResult(_ATD_INDEX_VLT5_CHAN);   // 空调温度开关          X70   4095=5V
    uADSatus[7] =ATDDrv_GetChanResult(_ATD_INDEX_HVLTDI2_CHAN);// 空调中压开关750\限速解除 X8 4095=25.6V
    uADSatus[8] =0;
    uADSatus[9] =0;
    uADSatus[10]=ATDDrv_GetChanResult(_ATD_INDEX_HVLTDI4_CHAN);// 制动开关2             X10   4095=25.6V
    uADSatus[11] =0;
    uADSatus[12]=ATDDrv_GetChanResult(_ATD_INDEX_HVLTDI1_CHAN);// DCDC故障              X7    4095=25.6V
    uADSatus[13]=ATDDrv_GetChanResult(_ATD_INDEX_HVLTDI3_CHAN);// 制动开关1             X9    4095=25.6V
    uADSatus[14]=ATDDrv_GetChanResult(_ATD_INDEX_HVLTDI5_CHAN);// P                     X11   4095=25.6V
    uADSatus[15]=ATDDrv_GetChanResult(_ATD_INDEX_HVLTDI6_CHAN);// R                     X12   4095=25.6V
    uADSatus[16]=ATDDrv_GetChanResult(_ATD_INDEX_HVLTDI7_CHAN);// N                     X13   4095=25.6V
    uADSatus[17]=ATDDrv_GetChanResult(_ATD_INDEX_HVLTDI8_CHAN);// D                     X14   4095=25.6V
    uADSatus[18]=ATDDrv_GetChanResult(_ATD_INDEX_VLT7_CHAN);        // 模式切换         X54   4095=5V
    uADSatus[19]=ATDDrv_GetChanResult(_ATD_INDEX_VLT8_CHAN);        // 备用             X61   4095=5V
    uADSatus[20]=ATDDrv_GetChanResult(_ATD_INDEX_VLT10_CHAN);       // 备用             X63   4095=5V
    uADSatus[21]=ATDDrv_GetChanResult(_ATD_INDEX_HVLTWK1_CHAN);       // 硬件唤醒1信号  X64   4095=25.6V
    uADSatus[22]=ATDDrv_GetChanResult(_ATD_INDEX_HVLTWK2_CHAN);       // 硬件唤醒2信号  X65   4095=25.6V
    uADSatus[23]=ATDDrv_GetChanResult(_ATD_INDEX_HVLTWK3_CHAN);       // 硬件唤醒3信号  X58   4095=25.6V
    uADSatus[24]=ATDDrv_GetChanResult(_ATD_INDEX_VBP_CHAN);           //VBP主继电器电压 X27   4095=25.6V
    uAD_LOGICP_CHAN=ATDDrv_GetChanResult(_ATD_INDEX_IGN_CHAN)  ;      //IGN点火开关电压 X52   4095=25.6V
    /************************************************************************************************************/
    uAD4Satus[0]=ATDDrv_GetChanResult(_ATD_INDEX_VLT1_CHAN)  ;//油门1   X68      4095=5V
    uAD4Satus[1]=ATDDrv_GetChanResult(_ATD_INDEX_VLT2_CHAN)  ;//油门2   X75
    uAD4Satus[2]=ATDDrv_GetChanResult(_ATD_INDEX_VLT3_CHAN)  ;//制动1   X72
    uAD4Satus[3]=ATDDrv_GetChanResult(_ATD_INDEX_VLT4_CHAN)  ;//制动2   X79
    /***********************************************************************************************************/
    
    
    // PSWTDrv_Interface(Index,bDoSatus[Index]);
    
    /*
    PSWTDrv_Interface(_PSWT_INDEX_LSD1_CHAN,0      ); //LSD1    X41  PWM1 复用
    PSWTDrv_Interface(_PSWT_INDEX_LSD2_CHAN,0      ); //LSD2    X42  PWM2 复用
    PSWTDrv_Interface(_PSWT_INDEX_LSD3_CHAN,0      ); //LSD3    X43  PWM3 复用
    PSWTDrv_Interface(_PSWT_INDEX_LSD4_CHAN,0      ); //LSD4    X44  PWM4 复用
    */
    
    PSWTDrv_Interface(_PSWT_INDEX_LSD5_CHAN,bDoSatus[0]      ); //低压主继电器        X45   
    PSWTDrv_Interface(_PSWT_INDEX_LSD6_CHAN,bDoSatus[6]      ); //电机高压主继电器    X46     
    PSWTDrv_Interface(_PSWT_INDEX_LSD7_CHAN,0      );           //备用                X47
    PSWTDrv_Interface(_PSWT_INDEX_LSD8_CHAN,bDoSatus[1]      ); //电池电机辅助12V电源    X29
    PSWTDrv_Interface(_PSWT_INDEX_LSD9_CHAN,bDoSatus[2]      ); //高速风扇使能           X30
    PSWTDrv_Interface(_PSWT_INDEX_LSD10_CHAN,bDoSatus[7]      ); //空调使能信号D07       X31
    PSWTDrv_Interface(_PSWT_INDEX_LSD11_CHAN,bDoSatus[5]      ); //PTC继电器 D07         X32
    PSWTDrv_Interface(_PSWT_INDEX_LSD12_CHAN,bDoSatus[4]      ); //倒挡输出信号   D07    X33    
    PSWTDrv_Interface(_PSWT_INDEX_LSD13_CHAN,bDoSatus[3]      ); //风扇低速使能信号 D07  X34   
    
    PSWTDrv_Interface(_PSWT_INDEX_HLSD1_CHAN,bDoSatus[15]      ); //DCDC使能信号         X48
    PSWTDrv_Interface(_PSWT_INDEX_HLSD2_CHAN,bDoSatus[11]      );            //ECO状态输出          X49    
    PSWTDrv_Interface(_PSWT_INDEX_HLSD3_CHAN,bDoSatus[12]      );            //SPORT状态输出        X50  
    PSWTDrv_Interface(_PSWT_INDEX_HLSD4_CHAN,bDoSatus[10]      ); //TCS_OFF 激活信号     X51      
    
    PSWTDrv_Interface(_PSWT_INDEX_HSD1_CHAN,bDoSatus[13]      ); //倒挡输出信号  8m      X38 
    PSWTDrv_Interface(_PSWT_INDEX_HSD2_CHAN,bDoSatus[14]      ); //PTC继电器     8m      X39 
    
    
    /*************************************************************************************************************/
  }
  #endif
}


void MSCANDrv_SendMsgCAN_A(MSCANDrv_infoMSGS_TYPE infoMsgS) 
{   //整车CAN
    #ifndef AECS_HW
        MSCANDrv_SendMsgCAN0( infoMsgS);    
    #else  
        MSCANDrv_SendMsgCAN1( infoMsgS);    
    #endif

    

}
void MSCANDrv_SendMsgCAN_B(MSCANDrv_infoMSGS_TYPE infoMsgS) 
{   //充电-备用CAN
    #ifndef AECS_HW
        MSCANDrv_SendMsgCAN1( infoMsgS);    
    #else  
        MSCANDrv_SendMsgCAN2( infoMsgS);    
    #endif

}
void MSCANDrv_SendMsgCAN_C(MSCANDrv_infoMSGS_TYPE infoMsgS)
{   //调试CAN
    #ifndef AECS_HW
        MSCANDrv_SendMsgCAN2( infoMsgS);    
    #else  
        MSCANDrv_SendMsgCAN0( infoMsgS);    
    #endif

}

//CAN0接收消息处理函数0
void CANNet_RecvMsgCAN0(MSCANDrv_infoMSGS_TYPE infoMsgS)
{
    //CAN接收样例  
    #ifndef AECS_HW
        //整车
        CANNet_RecvMsgCAN_A( infoMsgS);    
    #else
        //调试  
        CANNet_RecvMsgCAN_C( infoMsgS);    
    #endif
    
}
//CAN0接收消息处理函数1
void CANNet_RecvMsgCAN1(MSCANDrv_infoMSGS_TYPE infoMsgS)
{
    //CAN接收样例  
    #ifndef AECS_HW
        //备用
        CANNet_RecvMsgCAN_B( infoMsgS);    
    #else
        //整车
        CANNet_RecvMsgCAN_A( infoMsgS);    
    #endif
    
}//CAN0接收消息处理函数2
void CANNet_RecvMsgCAN2(MSCANDrv_infoMSGS_TYPE infoMsgS)
{
    //CAN接收样例  
    #ifndef AECS_HW
        //调试
        CANNet_RecvMsgCAN_C( infoMsgS);    
    #else
        //备用
        CANNet_RecvMsgCAN_B( infoMsgS);    
    #endif
    
}