/********************************************************************************
* 常州易控汽车电子有限公司
* (c) Copyright 2009-2015 ECTEK
* 保留所有权利
********************************************************************************
* 工程名称：整车控制器
* 文件名： HardwareLib.h
* 功能描述 ： 底层硬件驱总库
* 引用记录：该文件被GlobalDef.h包含
* 备注： 
* 修订记录：
*		日期		      版本		  作者		描述
*   2015.11.17           00.01.00     李中     针对EC3202V00硬件修改接口
*******************************************************************************/

#ifndef __HARDWARELIB_H
#define __HARDWARELIB_H


    #include "TypeDef.h"
    
    
    /*******************************************************************************************
                                 公用函数库
    ********************************************************************************************/
    
    //一维查表插值
    uint16 Intpol_u16u16(uint16 * __far addrTablePUW, uint16 dataXUW); //1维查表插值，X,Y都是16位无符号整数
    uint16 Intpol_s16u16(uint16 * __far addrTablePUW, sint16 dataXW); //1维查表插值，X是有符号整数，Y是无符号整数
    sint16 Intpol_u16s16(uint16 * __far addrTablePUW, uint16 dataXUW);//1维查表插值，X是无符号整数，Y是有符号整数
    sint16 Intpol_s16s16(uint16 * __far addrTablePUW, sint16 dataXW); //1维查表插值，X,Y都是16位有符号整数
    
    //二维查表插值
    uint16 Intpol_u16u16u16(uint16 * __far addrTablePUW, uint16 dataXUW,uint16 dataYUW); //2维查表插值，X,Y,Z都是16位无符号整数  
    sint16 Intpol_u16u16s16(uint16 * __far addrTablePUW, uint16 dataXUW,uint16 dataYUW);//2维查表插值，X,Y都是16位无符号，Z是有符号
    uint16 Intpol_u16s16u16(uint16 * __far addrTablePUW, uint16 dataXUW,sint16 dataYW); //2维查表插值，X,Z都是16位无符号，Y是有符号
    sint16 Intpol_s16u16s16(uint16 * __far addrTablePUW, sint16 dataXW,uint16 dataYUW); //2维查表插值，X,Z都是16位有符号，Y是无符号
    

    /*******************************************************************************************
                                 底层驱动接口
    ********************************************************************************************/      
    #define _MSCAN_NUM_CHAN 3
    #define _ECT_NUM_CHAN 8
    #define _ECT_NUM_TASK 8
    #define _L9825_NUM_CHAN 16
    #define _L9825_NUM_PWMCHAN 4    
    #define _ATD_NUM_CHAN 24
    #define _DIO_NUM_CHAN 18 
    #define _LSPWM_NUM_CHAN 4
    #define _PSWT_NUM_CHAN 16
    
    
    //模拟量输入
    #define _ATD_VOLT_REFERENCE 5
    
    #define _ATD_INDEX_UBATTERY_CHAN 0   //蓄电池电压，内部连接
    #define _ATD_INDEX_ACCPED1_CHAN  1   //加速踏板1，对应接插件X61
    #define _ATD_INDEX_ACCPED2_CHAN  2   //加速踏板2，对应接插件X62
    #define _ATD_INDEX_BRAKE1_CHAN   3   //制动1，对应接插件X57
    #define _ATD_INDEX_BRAKE2_CHAN   4   //制动2，对应接插件X58
    #define _ATD_INDEX_LOGICP_CHAN   23 //逻辑电信号，对应接插件X73
    
    uint16 ATDDrv_GetChanResult(uint8 idxChanUB); //得到上述通道的AD值 
    
    
    //数字量输入
    //前9路通道可通过软件配置高低电平有效
    #define _DIO_INDEX_BAK1_CHAN    0   //开关1信号，对应接插件X10
    #define _DIO_INDEX_BAK2_CHAN    1   //开关2信号，对应接插件X1
    #define _DIO_INDEX_BAK3_CHAN    2   //开关3信号，对应接插件X2
    #define _DIO_INDEX_BAK4_CHAN    3   //开关4信号，对应接插件X17
    #define _DIO_INDEX_BAK5_CHAN    4   //开关5信号，对应接插件X18
    #define _DIO_INDEX_BAK6_CHAN    5   //开关6信号，对应接插件X13
    #define _DIO_INDEX_BAK7_CHAN    6   //开关7信号，对应接插件X14
    #define _DIO_INDEX_BAK8_CHAN    7   //开关8信号，对应接插件X9
    #define _DIO_INDEX_BAK9_CHAN    8   //开关9信号，对应接插件X5
    #define _DIO_INDEX_BAK10_CHAN   9   //开关10信号，对应接插件X6
    #define _DIO_INDEX_BAK11_CHAN   10  //开关11信号，对应接插件X3
    #define _DIO_INDEX_BAK12_CHAN   11  //开关12信号，对应接插件X4
    #define _DIO_INDEX_BAK13_CHAN   12  //开关13信号，对应接插件X7
    #define _DIO_INDEX_BAK14_CHAN   13  //开关14信号，对应接插件X11
    #define _DIO_INDEX_BAK15_CHAN   14  //开关15信号，对应接插件X15
    #define _DIO_INDEX_BAK16_CHAN   15  //开关16信号，对应接插件X19
    #define _DIO_INDEX_BAK17_CHAN   16  //开关17信号，对应接插件X22
    #define _DIO_INDEX_BAK18_CHAN   17  //开关18信号，对应接插件X23
    
    uint8 DIODrv_GetChanState(uint8 idxChanUB); //得到上述通道的AD值
    
    
    //故障诊断返回值
    #define _DSM_INDEX_OK_STATUS    0x0
    #define _DSM_INDEX_ERR_STATUS   0x1
    #define _DSM_INDEX_NVA_STATUS   0xFF
    
    
    //PWM型信号输出（低边驱动）
    #define _LSPWM_INDEX_BAK1_CHAN  0  //输出1，对应接插件X45
    #define _LSPWM_INDEX_BAK2_CHAN  1  //输出2，对应接插件X38
    #define _LSPWM_INDEX_BAK3_CHAN  2  //输出3，对应接插件X46
    #define _LSPWM_INDEX_BAK4_CHAN  3  //输出4，对应接插件X40
    
    #define _PWM_DUTYCYCLE_FULL 10000  //10000表示占空比为100.00%
    #define _PWM_DUTYCYCLE_FACTOR   100
    
    //该函数须放在10ms任务中调用
    void LSPWMDrv_Interface(uint8 idxChanUB,uint32 fSetUI,uint16 rSetUW); //输出上述通道的低边驱动频率及占空比，频率精度0.01Hz，即放大100倍，占空比精度为0.01%，即放大100倍
    
    /*频率型驱动通道故障状态*/
    #define _PPWM_DIAG_INDEX_OPENLOAD_STATUS        0
    #define _PPWM_DIAG_INDEX_SHORTCIRCUIT_STATUS    1
    
    /*读取频率型驱动通道故障*/
    uint8 LSPWMDrv_GetDiagStatus(uint8 idxChanUB,uint8 idxDiagTypeUB); /*idxDiagTypeUB输入上述故障类型,返回值为0或1,1表示输入故障类型发生了故障*/
    
    
    //高低边驱动
    #define _PSWT_INDEX_BAK1_CHAN    0    //主继电器驱动，对应接插件X33
    #define _PSWT_INDEX_BAK2_CHAN    1    //驱动2，对应接插件X44   
    #define _PSWT_INDEX_BAK3_CHAN    2    //驱动3，对应接插件X41
    #define _PSWT_INDEX_BAK4_CHAN    3    //驱动4，对应接插件X43
    #define _PSWT_INDEX_BAK5_CHAN    4    //驱动5，对应接插件X37
    #define _PSWT_INDEX_BAK6_CHAN    5    //驱动6，对应接插件X42
    #define _PSWT_INDEX_BAK7_CHAN    6    //驱动7，对应接插件X47
    #define _PSWT_INDEX_BAK8_CHAN    7    //驱动8，对应接插件X48
    #define _PSWT_INDEX_BAK9_CHAN    8    //驱动9，对应接插件X20
    #define _PSWT_INDEX_BAK10_CHAN   9    //驱动10，对应接插件X16
    #define _PSWT_INDEX_BAK11_CHAN   10   //驱动11，对应接插件X12
    #define _PSWT_INDEX_BAK12_CHAN   11   //驱动12，对应接插件X8
    #define _PSWT_INDEX_BAK13_CHAN   12   //驱动13，对应接插件X36
    #define _PSWT_INDEX_BAK14_CHAN   13   //驱动14，对应接插件X32
    #define _PSWT_INDEX_BAK15_CHAN   14   //驱动15，对应接插件X28
    #define _PSWT_INDEX_BAK16_CHAN   15   //驱动16，对应接插件X24
    
    //该函数须放在10ms任务中调用
    void PSWTDrv_Interface(uint8 idxChanUB,uint8 bOpenUB); //输出上述通道的低边驱动状态, bOpen=1时为打开, 否则为关闭
    
    /*开关型功率驱动通道故障状态*/
    #define _PSWT_DIAG_INDEX_OPENLOAD_STATUS        0 /*开路*/
    #define _PSWT_DIAG_INDEX_SHORTCIRCUIT_STATUS    1 /*短路*/
    
    /*读取开关型功率驱动通道故障*/
    uint8 PSwtDrv_GetDiagStatus(uint8 idxChanUB,uint8 idxDiagTypeUB); /*idxDiagTypeUB输入上述故障类型,返回值为0或1,1表示输入故障类型发生了故障*/
    
    
    //CAN通讯接口
    typedef struct 
    {
        uint32 infoMsgIDUI;
        uint8  bExtIDUB;
        uint8  datMsgUBA[8];
        uint8  numLenUB;
        uint8  idxMsgPriorityUB;
        uint16 tiTimeStampUW;
    } MSCANDrv_infoMSGS_TYPE;
    
    //发送接口，须在1msCAN发送任务中调用
    void MSCANDrv_SendMsgCAN0(MSCANDrv_infoMSGS_TYPE infoMsgS);
    void MSCANDrv_SendMsgCAN1(MSCANDrv_infoMSGS_TYPE infoMsgS);
    void MSCANDrv_SendMsgCAN2(MSCANDrv_infoMSGS_TYPE infoMsgS);
    
    //读取CAN接口故障状态
    uint8 CANNet_GetErrStatus(uint8 idxCANUB); //idxCANUB为CAN通道号（0~2），返回值为0或1,1表示有故障，0表示无故障
    
    
    //EEPROM存储
    extern uint8 EEPROMDrv_bSaveInstantUB; //用于人为控制EEE存储
    
    
    /*******************************************************************************************
                                 EOL数据管理
    ********************************************************************************************/
    #define _EOLDATA_INDEX_ERR_VEHICLE 0
    #define _EOLDATA_INDEX_ERR_ENGINE 1
    #define _EOLDATA_INDEX_ERR_OTHER 2
    
    extern uint8 EOLData_stChkErrUB;
    
    
    /*******************************************************************************************
                                 底层已处理物理量
    ********************************************************************************************/
    extern uint8 BattCD_bLogicOnUB; //逻辑电开关
    extern uint16 BattCD_uUW;  //蓄电池电压
    
    
    /*******************************************************************************************
                                整车控制状态
    *******************************************************************************************/
    #define _VEHCO_INDEX_STATUS_INIT 0
    #define _VEHCO_INDEX_STATUS_RUNNING 1
    #define _VEHCO_INDEX_STATUS_AFTERRUN 2
    #define _VEHCO_INDEX_STATUS_FINISH 3
    
    extern uint8 VehCo_stNewUB; //整车控制状态
    

#endif
