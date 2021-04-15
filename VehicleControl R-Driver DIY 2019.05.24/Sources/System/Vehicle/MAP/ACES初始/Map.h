


#ifndef __MAP_H

    #define __MAP_H

    #include "TypeDef.h"


    typedef struct
    {
        uint16 xSize;
        uint16 xDist[4];
        uint16 values[4];
    }BattCD_uUW_CUR_TYPE;  //

    typedef struct
    {
        uint16 xSize;
        uint16 xDist[8];
        uint16 values[8];
    }Test_tTempUW_CUR_TYPE;  //

    typedef struct
    {
        uint16 xSize;
        uint16 ySize;
        uint16 xDist[8];
        uint16 yDist[4];
        uint16 values[8*4];
    }Test_tiTimeUW_MAP_TYPE;  //

    typedef struct
    {
        uint16 xSize;
        uint16 xDist[10];
        uint16 values[10];
    }addrTablePUW_TYPE;  //

    typedef struct
    {
        uint16 xSize;
        uint8 xDist[10];
        uint8 values[10];
    }AccPedCD_rUW_CUR_TYPE;  //

    typedef struct
    {
        uint16 numSizeUW;
    //底层驱动标示(勿改结构)
        uint8 DataSet_idxVerUB_CA[_KWP2000_LEN_K2REI_EMDSID];  //底层数据版本
        uint8 DataSet_idxUserVerUB_CA[16];  //用户数据版本
    //模拟量输入配置(勿改结构)
        uint8 ATDDrv_infoChanUB_CA[_ATD_NUM_CHAN];  //ATDDrv_infoChanUB_CA，16个通道的AD配置情况
        uint8 ATDDrv_idxChanSigConfUB_CA[_ATD_NUM_CHAN];  //ATDDrv_idxChanSigConfUB_CA，各个信号的通道配置
    //数字量输入配置(勿改结构)
        uint8 DIODrv_idxChanSigConfUB_CA[_DIO_NUM_CHAN];  //DIODrv_idxChanSigConfUB_CA，各个数字量输入的通道配置
        uint16 DIODrv_idxChanSwtTypeUW_CA[_DIO_NUM_CHAN];  //高低有效软件可配置数字量输入通道高低有效配置
        uint16 DIODrv_uLevelLowThresUW_C;  //高低有效软件可配置通道的低电平电压门限
        uint16 DIODrv_uLevelHighThresUW_C;  //高低有效软件可配通道的高电平电压门限
    //频率量输出配置(勿改结构)
        uint8 PWMDrv_idxScaleAUB_C;  //PWMDrv_idxScaleAUB_C, PWM模块中时钟源A的分频数
        uint8 PWMDrv_idxScaleBUB_C;  //PWMDrv_idxScaleBUB_C, PWM模块中时钟源B的分频数
        uint8 PWMDrv_idxScaleSAUB_C;  //PWMDrv_idxScaleSAUB_C, PWM模块中时钟源SA的分频数, fSA=fA/(2*N)
        uint8 PWMDrv_idxScaleSBUB_C;  //PWMDrv_idxScaleSBUB_C, PWM模块中时钟源SB的分频数, fSB=fB/(2*N)
        uint8 PWMDrv_idxChanTimerConfUB_CA[4];  //PWMDrv_idxChanTimerConfUB_CA, PWM各通道所用时钟源, X0(PWM1)与X2(PWM5)使用A或SA, X1(PWM3)和X3(PWM7)使用B或SB
        uint8 PWMDrv_idxFuncConfUB_CA[4];  //PWMDrv_idxFuncConfUB_CA, PWM模块中各功能所对应PWM管脚配置
    //PWM型低边驱动配置(勿改结构)
        uint8 LSPWMDrv_idxChanSigConfUB_CA[_LSPWM_NUM_CHAN];  //PWM型低边驱动各任务通道配置
    //开关型高低边驱动配置(勿改结构)
        uint8 L9825Drv_swtUseUB_CA[2];  //L9825Drv_swtUseUB_CA, L9825两块芯片的使能配置
        uint8 PSWTDrv_idxChanSigConfUB_CA[_PSWT_NUM_CHAN];  //PSWTDrv_idxChanSigConfUB_CA, 高低边驱动任务号配置，0~7为低边驱动，8~15为高边驱动
    //系统控制(勿改结构)
        uint32 SysCon_stIRevShuOffMskUI_C;  //SysCon_stIRevShuOffMaskUI_C
        uint32 SysCon_stRevShuOffMskUI_C;  //SysCon_stRevShuOffMaskUI_C
        uint16 SysCon_stExtWaitMskUW_C;  //SysCon_stExtWaitMskUW_C，PostDrive状态等待结束的事件Mask
        uint16 SysCon_stExtWait2MskUW_C;  //SysCon_stExtWait2MskUW_C，PostDrive状态等待结束的事件Mask
        uint32 SysCon_tiPostDriveMaxUI_C;  //SysCon_tiPostDriveMaxUI_C，PostDrive最长时间，s
        uint32 SysCon_tiShutdownMaxUI_C;  //SysCon_tiShutdownMaxUI_C，shutdown状态最长执行时间，s
        uint32 SysCon_tiPostDriveMinUI_C;  //SysCon_tiPostDriveMinUI_C，PostDrive最短时间，s
    //通讯设置(勿改结构)
        uint16 MSCANDrv_infoBaudrateUW_CA[3];  //MSCANDrv_infoBaudrateUW_CA
        uint16 KWPCAN_tiToBMaxUW_C;  //KWPCAN_tiToBMaxUW_C,ToB 最长时间50*100ms
        uint16 KWPCAN_tiToCMaxUW_C;  //KWPCAN_tiToCMaxUW_C,ToC 最长时间50*100ms
        uint16 KWPCAN_numFCWMaxUW_C;  //KWPCAN_numFCWMaxUW_C,最多的Flow Control Wait Frame数目16
        uint8 CCP_StationIdUB_CA[10];  //
    //底层其它功能(勿改结构)
        uint16 SigTst_bEnUW_C;  //SigTst_bEnUW_C，是否直接控制9825和9362
        uint8 SigTst_bChanL9825UB_CA[_L9825_NUM_CHAN];  //SigTst_bChanL9825UB_CA，各通道开关
        uint16 SigTst_fChanL9825UW_CA[_L9825_NUM_PWMCHAN];  //SigTst_fChanL9825UW_CA，L9825各通道的PWM频率
        uint16 SigTst_rChanL9825UW_CA[_L9825_NUM_PWMCHAN];  //SigTst_rChanL9825UW_CA，L9825各通道的占空比
        uint8 SigTst_bChanBTS724UB_CA[8];  //SigTst_bChanBTS724UB_CA，高边驱动开关
    //电源管理(勿改结构)
        uint16 BattCD_uLogicOnUW_C;  //逻辑电上电电压限值
        BattCD_uUW_CUR_TYPE BattCD_uUW_CUR;  //蓄电池电压特性
    }HardwareMap_infoMap1S_TYPE;

    typedef struct
    {
    //数据版本
        uint8 VehDataSet_idxSupervisorVerUB_CA[16];  //标准数据版本号
        uint8 VehDataSet_idxUserVerUB_CA[16];  //用户数据版本号
    //示例
        uint16 Test_datValueUW_C;  //单值标定变量示例
        Test_tTempUW_CUR_TYPE Test_tTempUW_CUR;  //曲线标定变量示例
        Test_tiTimeUW_MAP_TYPE Test_tiTimeUW_MAP;  //曲面标定变量示例
        uint8 Test_idxChannelUW_CA[20];  //Test_idxChannelUW_CA
        addrTablePUW_TYPE addrTablePUW;  //
        AccPedCD_rUW_CUR_TYPE AccPedCD_rUW_CUR;  //
    }VehicleMap_infoMap1S_TYPE;

    //底层驱动标示(勿改结构)
    #define DataSet_idxUserVerUB_CA HardwareMap_infoMap1SP->DataSet_idxUserVerUB_CA

    //模拟量输入配置(勿改结构)
    #define ATDDrv_infoChanUB_CA HardwareMap_infoMap1SP->ATDDrv_infoChanUB_CA
    #define ATDDrv_idxChanSigConfUB_CA HardwareMap_infoMap1SP->ATDDrv_idxChanSigConfUB_CA

    //数字量输入配置(勿改结构)
    #define DIODrv_idxChanSigConfUB_CA HardwareMap_infoMap1SP->DIODrv_idxChanSigConfUB_CA
    #define DIODrv_idxChanSwtTypeUW_CA HardwareMap_infoMap1SP->DIODrv_idxChanSwtTypeUW_CA
    #define DIODrv_uLevelLowThresUW_C HardwareMap_infoMap1SP->DIODrv_uLevelLowThresUW_C
    #define DIODrv_uLevelHighThresUW_C HardwareMap_infoMap1SP->DIODrv_uLevelHighThresUW_C

    //频率量输出配置(勿改结构)
    #define PWMDrv_idxScaleAUB_C HardwareMap_infoMap1SP->PWMDrv_idxScaleAUB_C
    #define PWMDrv_idxScaleBUB_C HardwareMap_infoMap1SP->PWMDrv_idxScaleBUB_C
    #define PWMDrv_idxScaleSAUB_C HardwareMap_infoMap1SP->PWMDrv_idxScaleSAUB_C
    #define PWMDrv_idxScaleSBUB_C HardwareMap_infoMap1SP->PWMDrv_idxScaleSBUB_C
    #define PWMDrv_idxChanTimerConfUB_CA HardwareMap_infoMap1SP->PWMDrv_idxChanTimerConfUB_CA
    #define PWMDrv_idxFuncConfUB_CA HardwareMap_infoMap1SP->PWMDrv_idxFuncConfUB_CA

    //PWM型低边驱动配置(勿改结构)
    #define LSPWMDrv_idxChanSigConfUB_CA HardwareMap_infoMap1SP->LSPWMDrv_idxChanSigConfUB_CA

    //开关型高低边驱动配置(勿改结构)
    #define L9825Drv_swtUseUB_CA HardwareMap_infoMap1SP->L9825Drv_swtUseUB_CA
    #define PSWTDrv_idxChanSigConfUB_CA HardwareMap_infoMap1SP->PSWTDrv_idxChanSigConfUB_CA

    //系统控制(勿改结构)
    #define SysCon_stIRevShuOffMskUI_C HardwareMap_infoMap1SP->SysCon_stIRevShuOffMskUI_C
    #define SysCon_stRevShuOffMskUI_C HardwareMap_infoMap1SP->SysCon_stRevShuOffMskUI_C
    #define SysCon_stExtWaitMskUW_C HardwareMap_infoMap1SP->SysCon_stExtWaitMskUW_C
    #define SysCon_stExtWait2MskUW_C HardwareMap_infoMap1SP->SysCon_stExtWait2MskUW_C
    #define SysCon_tiPostDriveMaxUI_C HardwareMap_infoMap1SP->SysCon_tiPostDriveMaxUI_C
    #define SysCon_tiShutdownMaxUI_C HardwareMap_infoMap1SP->SysCon_tiShutdownMaxUI_C
    #define SysCon_tiPostDriveMinUI_C HardwareMap_infoMap1SP->SysCon_tiPostDriveMinUI_C

    //通讯设置(勿改结构)
    #define MSCANDrv_infoBaudrateUW_CA HardwareMap_infoMap1SP->MSCANDrv_infoBaudrateUW_CA
    #define KWPCAN_tiToBMaxUW_C HardwareMap_infoMap1SP->KWPCAN_tiToBMaxUW_C
    #define KWPCAN_tiToCMaxUW_C HardwareMap_infoMap1SP->KWPCAN_tiToCMaxUW_C
    #define KWPCAN_numFCWMaxUW_C HardwareMap_infoMap1SP->KWPCAN_numFCWMaxUW_C
    #define CCP_StationIdUB_CA HardwareMap_infoMap1SP->CCP_StationIdUB_CA

    //底层其它功能(勿改结构)
    #define SigTst_bEnUW_C HardwareMap_infoMap1SP->SigTst_bEnUW_C
    #define SigTst_bChanL9825UB_CA HardwareMap_infoMap1SP->SigTst_bChanL9825UB_CA
    #define SigTst_fChanL9825UW_CA HardwareMap_infoMap1SP->SigTst_fChanL9825UW_CA
    #define SigTst_rChanL9825UW_CA HardwareMap_infoMap1SP->SigTst_rChanL9825UW_CA
    #define SigTst_bChanBTS724UB_CA HardwareMap_infoMap1SP->SigTst_bChanBTS724UB_CA

    //电源管理(勿改结构)
    #define BattCD_uLogicOnUW_C HardwareMap_infoMap1SP->BattCD_uLogicOnUW_C
    #define BattCD_uUW_CUR HardwareMap_infoMap1SP->BattCD_uUW_CUR

    //数据版本
    #define VehDataSet_idxSupervisorVerUB_CA VehicleMap_infoMap1SP->VehDataSet_idxSupervisorVerUB_CA
    #define VehDataSet_idxUserVerUB_CA VehicleMap_infoMap1SP->VehDataSet_idxUserVerUB_CA

    //示例
    #define Test_datValueUW_C VehicleMap_infoMap1SP->Test_datValueUW_C
    #define Test_tTempUW_CUR VehicleMap_infoMap1SP->Test_tTempUW_CUR
    #define Test_tiTimeUW_MAP VehicleMap_infoMap1SP->Test_tiTimeUW_MAP
    #define Test_idxChannelUW_CA VehicleMap_infoMap1SP->Test_idxChannelUW_CA
    #define addrTablePUW VehicleMap_infoMap1SP->addrTablePUW
    #define AccPedCD_rUW_CUR VehicleMap_infoMap1SP->AccPedCD_rUW_CUR

    /*******************************************************************************************
                             MAP全局变量设置 
    ********************************************************************************************/
#ifndef AECS_HW
    #pragma CONST_SEG __GPAGE_SEG PARAMETERS_ROM1
#else   
    #pragma CONST_SEG VCU_CAL
#endif   
    extern volatile const HardwareMap_infoMap1S_TYPE HardwareMap_infoMap1S; //底层标定数据
    extern volatile const VehicleMap_infoMap1S_TYPE VehicleMap_infoMap1S;  //整车标定数据
    #pragma CONST_SEG DEFAULT
    
    typedef struct
    {
        uint16 dataChkSumUW;
        uint16 idxProYearUW;
        uint8 idxProMonthUB;
        uint8 idxProDayUB; //程序刷写日期
        uint8 dataReserved1UB;
        uint8 idxHourUW;
        uint8 idxMinuteUB;
        uint8 idxSecondUB; //程序刷写时间
        uint8 idxToolIDUBA[60];
    }HardwareMap_infoDataCheck1S_TYPE;

#ifndef AECS_HW    
      #pragma CONST_SEG __GPAGE_SEG DATA_VERIFY1
#else
      #pragma CONST_SEG VDL_HWIOCONFIG_CAL
#endif      
       extern volatile const HardwareMap_infoDataCheck1S_TYPE HardwareMap_infoDataCheck1S;  
    #pragma CONST_SEG DEFAULT
            
              
    extern HardwareMap_infoMap1S_TYPE  * __far HardwareMap_infoMap1SP;   //指向GlabalMap的一个指针
    extern VehicleMap_infoMap1S_TYPE  * __far VehicleMap_infoMap1SP;     
  

#endif 
