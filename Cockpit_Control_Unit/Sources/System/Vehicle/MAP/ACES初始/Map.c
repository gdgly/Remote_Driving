
#include "GlobalDef.h"

#include "Map.h"


#ifndef AECS_HW
   #pragma CONST_SEG __GPAGE_SEG PARAMETERS_ROM1
#else  
   #pragma CONST_SEG VCU_CAL
#endif

volatile const HardwareMap_infoMap1S_TYPE HardwareMap_infoMap1S=
{
  540,
  "ECD-105-156.000-02-00-E-011.000", //"ECD-000-001.000-01-00-I-010.001-010.001", //DataSet_idxVerUB_CA，底层数据当前版本
  "10.4", //DataSet_idxUserVerUB_CA[16],用户数据版本
  {
    0b11110000,0b11110000,0b11110000,0b11110000,0b11110000,0b11110000,0b11110000,0b11110000,0b11110000,0b11110000,0b11110000,0b11110000,0b11110000,0b11110000,0b11110000,0b11110000,0b11110000,0b11110000,0b11110000,0b11110000,
    0b11110000,0b11110000,0b11110000,0b11110000
  }, //ATDDrv_infoChanUB_CA，16个通道的AD配置情况
  {
    0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,
    20,21,22,23
  }, //ATDDrv_idxChanSigConfUB_CA，各个信号的通道配置
  {
    0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17
  }, //DIODrv_idxChanSigConfUB_CA，各个数字量输入的通道配置
  {
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1
  }, //高低有效软件可配置数字量输入通道高低有效配置
  3276, //高低有效软件可配置通道的低电平电压门限
  17199, //高低有效软件可配通道的高电平电压门限
  0, //PWMDrv_idxScaleAUB_C, PWM模块中时钟源A的分频数
  0, //PWMDrv_idxScaleBUB_C, PWM模块中时钟源B的分频数
  19, //PWMDrv_idxScaleSAUB_C, PWM模块中时钟源SA的分频数, fSA=fA/(2*N)
  19, //PWMDrv_idxScaleSBUB_C, PWM模块中时钟源SB的分频数, fSB=fB/(2*N)
  {
    1,1,1,1
  }, //PWMDrv_idxChanTimerConfUB_CA, PWM各通道所用时钟源, X0(PWM1)与X2(PWM5)使用A或SA, X1(PWM3)和X3(PWM7)使用B或SB
  {
    1,3,5,7
  }, //PWMDrv_idxFuncConfUB_CA, PWM模块中各功能所对应PWM管脚配置
  {
    0,1,2,3
  }, //PWM型低边驱动各任务通道配置
  {
    1,1
  }, //L9825Drv_swtUseUB_CA, L9825两块芯片的使能配置
  {
    0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15
  }, //PSWTDrv_idxChanSigConfUB_CA, 高低边驱动任务号配置，0~7为低边驱动，8~15为高边驱动
  65535, //SysCon_stIRevShuOffMaskUI_C
  65535, //SysCon_stRevShuOffMaskUI_C
  65535, //SysCon_stExtWaitMskUW_C，PostDrive状态等待结束的事件Mask
  65535, //SysCon_stExtWait2MskUW_C，PostDrive状态等待结束的事件Mask
  1000, //SysCon_tiPostDriveMaxUI_C，PostDrive最长时间，s
  1500, //SysCon_tiShutdownMaxUI_C，shutdown状态最长执行时间，s
  250, //SysCon_tiPostDriveMinUI_C，PostDrive最短时间，s
  {
    1,1,1
  }, //MSCANDrv_infoBaudrateUW_CA
  2000, //KWPCAN_tiToBMaxUW_C,ToB 最长时间50*100ms
  2000, //KWPCAN_tiToCMaxUW_C,ToC 最长时间50*100ms
  16, //KWPCAN_numFCWMaxUW_C,最多的Flow Control Wait Frame数目16
  "ECTEK1", //CCP_StationIdUB_CA
  0, //SigTst_bEnUW_C，是否直接控制9825和9362
  {
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
  }, //SigTst_bChanL9825UB_CA，各通道开关
  {
    500,500,500,500
  }, //SigTst_fChanL9825UW_CA，L9825各通道的PWM频率
  {
    0,0,0,0
  }, //SigTst_rChanL9825UW_CA，L9825各通道的占空比
  {
    0,0,0,0,0,0,0,0
  }, //SigTst_bChanBTS724UB_CA，高边驱动开关
  12285, //逻辑电上电电压限值
  {
    4,
    {
      0,5807,11613,20475
    },
    {
      0,1200,2400,4231
    }
  }, //蓄电池电压特性
};

volatile const VehicleMap_infoMap1S_TYPE VehicleMap_infoMap1S=
{
  "P156V100", //VehDataSet_idxSupervisorVerUB_CA，标准版本
  "10.2", //VehDataSet_idxUserVerUB_CA，用户版本
  3000, //单值标定变量示例
  {
    8,
    {
      0,2048,6143,8190,12285,16380,18428,20475
    },
    {
      0,5120,10240,15360,20480,25600,30720,35840
    }
  }, //曲线标定变量示例
  {
    8,
    4,
    {
      0,500,1000,1200,1400,1600,1800,2000
    },
    {
      0,1920,3200,4800
    },
    {
      0,0,0,0,
      0,0,0,0,
      0,0,0,0,
      0,0,0,0,
      0,0,0,0,
      0,0,0,0,
      0,0,0,0,
      0,0,0,0
    }
  }, //曲面标定变量示例
  {
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
  }, //Test_idxChannelUW_CA
  {
    10,
    {
      0,0,0,0,0,0,0,0,0,0
    },
    {
      0,0,0,0,0,0,0,0,0,0
    }
  }, //addrTablePUW
  {
    10,
    {
      0,0,0,0,0,0,0,0,0,0
    },
    {
      0,0,0,0,0,0,0,0,0,0
    }
  }, //AccPedCD_rUW_CUR
};

#pragma CONST_SEG DEFAULT

#ifndef AECS_HW
    #pragma CONST_SEG __GPAGE_SEG DATA_VERIFY1
#else
    #pragma CONST_SEG VDL_HWIOCONFIG_CAL
#endif

    volatile const HardwareMap_infoDataCheck1S_TYPE HardwareMap_infoDataCheck1S=
    {
        0xE22D,
        2016,
        6,
        21,
        0,
        9,
        49,
        38,
        "ECS-000-000.000-90-61-I-002.003"
    };
#pragma CONST_SEG DEFAULT

#ifdef AECS_HW
    HardwareMap_infoMap1S_TYPE  * __far HardwareMap_infoMap1SP;   //指向GlabalMap的一个指针
    VehicleMap_infoMap1S_TYPE  * __far VehicleMap_infoMap1SP; 
#endif
