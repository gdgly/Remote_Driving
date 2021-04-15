
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
  "ECD-105-156.000-02-00-E-011.000", //"ECD-000-001.000-01-00-I-010.001-010.001", //DataSet_idxVerUB_CA���ײ����ݵ�ǰ�汾
  "10.4", //DataSet_idxUserVerUB_CA[16],�û����ݰ汾
  {
    0b11110000,0b11110000,0b11110000,0b11110000,0b11110000,0b11110000,0b11110000,0b11110000,0b11110000,0b11110000,0b11110000,0b11110000,0b11110000,0b11110000,0b11110000,0b11110000,0b11110000,0b11110000,0b11110000,0b11110000,
    0b11110000,0b11110000,0b11110000,0b11110000
  }, //ATDDrv_infoChanUB_CA��16��ͨ����AD�������
  {
    0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,
    20,21,22,23
  }, //ATDDrv_idxChanSigConfUB_CA�������źŵ�ͨ������
  {
    0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17
  }, //DIODrv_idxChanSigConfUB_CA�����������������ͨ������
  {
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1
  }, //�ߵ���Ч�������������������ͨ���ߵ���Ч����
  3276, //�ߵ���Ч���������ͨ���ĵ͵�ƽ��ѹ����
  17199, //�ߵ���Ч�������ͨ���ĸߵ�ƽ��ѹ����
  0, //PWMDrv_idxScaleAUB_C, PWMģ����ʱ��ԴA�ķ�Ƶ��
  0, //PWMDrv_idxScaleBUB_C, PWMģ����ʱ��ԴB�ķ�Ƶ��
  19, //PWMDrv_idxScaleSAUB_C, PWMģ����ʱ��ԴSA�ķ�Ƶ��, fSA=fA/(2*N)
  19, //PWMDrv_idxScaleSBUB_C, PWMģ����ʱ��ԴSB�ķ�Ƶ��, fSB=fB/(2*N)
  {
    1,1,1,1
  }, //PWMDrv_idxChanTimerConfUB_CA, PWM��ͨ������ʱ��Դ, X0(PWM1)��X2(PWM5)ʹ��A��SA, X1(PWM3)��X3(PWM7)ʹ��B��SB
  {
    1,3,5,7
  }, //PWMDrv_idxFuncConfUB_CA, PWMģ���и���������ӦPWM�ܽ�����
  {
    0,1,2,3
  }, //PWM�͵ͱ�����������ͨ������
  {
    1,1
  }, //L9825Drv_swtUseUB_CA, L9825����оƬ��ʹ������
  {
    0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15
  }, //PSWTDrv_idxChanSigConfUB_CA, �ߵͱ�������������ã�0~7Ϊ�ͱ�������8~15Ϊ�߱�����
  65535, //SysCon_stIRevShuOffMaskUI_C
  65535, //SysCon_stRevShuOffMaskUI_C
  65535, //SysCon_stExtWaitMskUW_C��PostDrive״̬�ȴ��������¼�Mask
  65535, //SysCon_stExtWait2MskUW_C��PostDrive״̬�ȴ��������¼�Mask
  1000, //SysCon_tiPostDriveMaxUI_C��PostDrive�ʱ�䣬s
  1500, //SysCon_tiShutdownMaxUI_C��shutdown״̬�ִ��ʱ�䣬s
  250, //SysCon_tiPostDriveMinUI_C��PostDrive���ʱ�䣬s
  {
    1,1,1
  }, //MSCANDrv_infoBaudrateUW_CA
  2000, //KWPCAN_tiToBMaxUW_C,ToB �ʱ��50*100ms
  2000, //KWPCAN_tiToCMaxUW_C,ToC �ʱ��50*100ms
  16, //KWPCAN_numFCWMaxUW_C,����Flow Control Wait Frame��Ŀ16
  "ECTEK1", //CCP_StationIdUB_CA
  0, //SigTst_bEnUW_C���Ƿ�ֱ�ӿ���9825��9362
  {
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
  }, //SigTst_bChanL9825UB_CA����ͨ������
  {
    500,500,500,500
  }, //SigTst_fChanL9825UW_CA��L9825��ͨ����PWMƵ��
  {
    0,0,0,0
  }, //SigTst_rChanL9825UW_CA��L9825��ͨ����ռ�ձ�
  {
    0,0,0,0,0,0,0,0
  }, //SigTst_bChanBTS724UB_CA���߱���������
  12285, //�߼����ϵ��ѹ��ֵ
  {
    4,
    {
      0,5807,11613,20475
    },
    {
      0,1200,2400,4231
    }
  }, //���ص�ѹ����
};

volatile const VehicleMap_infoMap1S_TYPE VehicleMap_infoMap1S=
{
  "P156V100", //VehDataSet_idxSupervisorVerUB_CA����׼�汾
  "10.2", //VehDataSet_idxUserVerUB_CA���û��汾
  3000, //��ֵ�궨����ʾ��
  {
    8,
    {
      0,2048,6143,8190,12285,16380,18428,20475
    },
    {
      0,5120,10240,15360,20480,25600,30720,35840
    }
  }, //���߱궨����ʾ��
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
  }, //����궨����ʾ��
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
    HardwareMap_infoMap1S_TYPE  * __far HardwareMap_infoMap1SP;   //ָ��GlabalMap��һ��ָ��
    VehicleMap_infoMap1S_TYPE  * __far VehicleMap_infoMap1SP; 
#endif
