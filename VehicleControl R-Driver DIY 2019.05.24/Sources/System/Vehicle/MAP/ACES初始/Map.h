


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
    //�ײ�������ʾ(��Ľṹ)
        uint8 DataSet_idxVerUB_CA[_KWP2000_LEN_K2REI_EMDSID];  //�ײ����ݰ汾
        uint8 DataSet_idxUserVerUB_CA[16];  //�û����ݰ汾
    //ģ������������(��Ľṹ)
        uint8 ATDDrv_infoChanUB_CA[_ATD_NUM_CHAN];  //ATDDrv_infoChanUB_CA��16��ͨ����AD�������
        uint8 ATDDrv_idxChanSigConfUB_CA[_ATD_NUM_CHAN];  //ATDDrv_idxChanSigConfUB_CA�������źŵ�ͨ������
    //��������������(��Ľṹ)
        uint8 DIODrv_idxChanSigConfUB_CA[_DIO_NUM_CHAN];  //DIODrv_idxChanSigConfUB_CA�����������������ͨ������
        uint16 DIODrv_idxChanSwtTypeUW_CA[_DIO_NUM_CHAN];  //�ߵ���Ч�������������������ͨ���ߵ���Ч����
        uint16 DIODrv_uLevelLowThresUW_C;  //�ߵ���Ч���������ͨ���ĵ͵�ƽ��ѹ����
        uint16 DIODrv_uLevelHighThresUW_C;  //�ߵ���Ч�������ͨ���ĸߵ�ƽ��ѹ����
    //Ƶ�����������(��Ľṹ)
        uint8 PWMDrv_idxScaleAUB_C;  //PWMDrv_idxScaleAUB_C, PWMģ����ʱ��ԴA�ķ�Ƶ��
        uint8 PWMDrv_idxScaleBUB_C;  //PWMDrv_idxScaleBUB_C, PWMģ����ʱ��ԴB�ķ�Ƶ��
        uint8 PWMDrv_idxScaleSAUB_C;  //PWMDrv_idxScaleSAUB_C, PWMģ����ʱ��ԴSA�ķ�Ƶ��, fSA=fA/(2*N)
        uint8 PWMDrv_idxScaleSBUB_C;  //PWMDrv_idxScaleSBUB_C, PWMģ����ʱ��ԴSB�ķ�Ƶ��, fSB=fB/(2*N)
        uint8 PWMDrv_idxChanTimerConfUB_CA[4];  //PWMDrv_idxChanTimerConfUB_CA, PWM��ͨ������ʱ��Դ, X0(PWM1)��X2(PWM5)ʹ��A��SA, X1(PWM3)��X3(PWM7)ʹ��B��SB
        uint8 PWMDrv_idxFuncConfUB_CA[4];  //PWMDrv_idxFuncConfUB_CA, PWMģ���и���������ӦPWM�ܽ�����
    //PWM�͵ͱ���������(��Ľṹ)
        uint8 LSPWMDrv_idxChanSigConfUB_CA[_LSPWM_NUM_CHAN];  //PWM�͵ͱ�����������ͨ������
    //�����͸ߵͱ���������(��Ľṹ)
        uint8 L9825Drv_swtUseUB_CA[2];  //L9825Drv_swtUseUB_CA, L9825����оƬ��ʹ������
        uint8 PSWTDrv_idxChanSigConfUB_CA[_PSWT_NUM_CHAN];  //PSWTDrv_idxChanSigConfUB_CA, �ߵͱ�������������ã�0~7Ϊ�ͱ�������8~15Ϊ�߱�����
    //ϵͳ����(��Ľṹ)
        uint32 SysCon_stIRevShuOffMskUI_C;  //SysCon_stIRevShuOffMaskUI_C
        uint32 SysCon_stRevShuOffMskUI_C;  //SysCon_stRevShuOffMaskUI_C
        uint16 SysCon_stExtWaitMskUW_C;  //SysCon_stExtWaitMskUW_C��PostDrive״̬�ȴ��������¼�Mask
        uint16 SysCon_stExtWait2MskUW_C;  //SysCon_stExtWait2MskUW_C��PostDrive״̬�ȴ��������¼�Mask
        uint32 SysCon_tiPostDriveMaxUI_C;  //SysCon_tiPostDriveMaxUI_C��PostDrive�ʱ�䣬s
        uint32 SysCon_tiShutdownMaxUI_C;  //SysCon_tiShutdownMaxUI_C��shutdown״̬�ִ��ʱ�䣬s
        uint32 SysCon_tiPostDriveMinUI_C;  //SysCon_tiPostDriveMinUI_C��PostDrive���ʱ�䣬s
    //ͨѶ����(��Ľṹ)
        uint16 MSCANDrv_infoBaudrateUW_CA[3];  //MSCANDrv_infoBaudrateUW_CA
        uint16 KWPCAN_tiToBMaxUW_C;  //KWPCAN_tiToBMaxUW_C,ToB �ʱ��50*100ms
        uint16 KWPCAN_tiToCMaxUW_C;  //KWPCAN_tiToCMaxUW_C,ToC �ʱ��50*100ms
        uint16 KWPCAN_numFCWMaxUW_C;  //KWPCAN_numFCWMaxUW_C,����Flow Control Wait Frame��Ŀ16
        uint8 CCP_StationIdUB_CA[10];  //
    //�ײ���������(��Ľṹ)
        uint16 SigTst_bEnUW_C;  //SigTst_bEnUW_C���Ƿ�ֱ�ӿ���9825��9362
        uint8 SigTst_bChanL9825UB_CA[_L9825_NUM_CHAN];  //SigTst_bChanL9825UB_CA����ͨ������
        uint16 SigTst_fChanL9825UW_CA[_L9825_NUM_PWMCHAN];  //SigTst_fChanL9825UW_CA��L9825��ͨ����PWMƵ��
        uint16 SigTst_rChanL9825UW_CA[_L9825_NUM_PWMCHAN];  //SigTst_rChanL9825UW_CA��L9825��ͨ����ռ�ձ�
        uint8 SigTst_bChanBTS724UB_CA[8];  //SigTst_bChanBTS724UB_CA���߱���������
    //��Դ����(��Ľṹ)
        uint16 BattCD_uLogicOnUW_C;  //�߼����ϵ��ѹ��ֵ
        BattCD_uUW_CUR_TYPE BattCD_uUW_CUR;  //���ص�ѹ����
    }HardwareMap_infoMap1S_TYPE;

    typedef struct
    {
    //���ݰ汾
        uint8 VehDataSet_idxSupervisorVerUB_CA[16];  //��׼���ݰ汾��
        uint8 VehDataSet_idxUserVerUB_CA[16];  //�û����ݰ汾��
    //ʾ��
        uint16 Test_datValueUW_C;  //��ֵ�궨����ʾ��
        Test_tTempUW_CUR_TYPE Test_tTempUW_CUR;  //���߱궨����ʾ��
        Test_tiTimeUW_MAP_TYPE Test_tiTimeUW_MAP;  //����궨����ʾ��
        uint8 Test_idxChannelUW_CA[20];  //Test_idxChannelUW_CA
        addrTablePUW_TYPE addrTablePUW;  //
        AccPedCD_rUW_CUR_TYPE AccPedCD_rUW_CUR;  //
    }VehicleMap_infoMap1S_TYPE;

    //�ײ�������ʾ(��Ľṹ)
    #define DataSet_idxUserVerUB_CA HardwareMap_infoMap1SP->DataSet_idxUserVerUB_CA

    //ģ������������(��Ľṹ)
    #define ATDDrv_infoChanUB_CA HardwareMap_infoMap1SP->ATDDrv_infoChanUB_CA
    #define ATDDrv_idxChanSigConfUB_CA HardwareMap_infoMap1SP->ATDDrv_idxChanSigConfUB_CA

    //��������������(��Ľṹ)
    #define DIODrv_idxChanSigConfUB_CA HardwareMap_infoMap1SP->DIODrv_idxChanSigConfUB_CA
    #define DIODrv_idxChanSwtTypeUW_CA HardwareMap_infoMap1SP->DIODrv_idxChanSwtTypeUW_CA
    #define DIODrv_uLevelLowThresUW_C HardwareMap_infoMap1SP->DIODrv_uLevelLowThresUW_C
    #define DIODrv_uLevelHighThresUW_C HardwareMap_infoMap1SP->DIODrv_uLevelHighThresUW_C

    //Ƶ�����������(��Ľṹ)
    #define PWMDrv_idxScaleAUB_C HardwareMap_infoMap1SP->PWMDrv_idxScaleAUB_C
    #define PWMDrv_idxScaleBUB_C HardwareMap_infoMap1SP->PWMDrv_idxScaleBUB_C
    #define PWMDrv_idxScaleSAUB_C HardwareMap_infoMap1SP->PWMDrv_idxScaleSAUB_C
    #define PWMDrv_idxScaleSBUB_C HardwareMap_infoMap1SP->PWMDrv_idxScaleSBUB_C
    #define PWMDrv_idxChanTimerConfUB_CA HardwareMap_infoMap1SP->PWMDrv_idxChanTimerConfUB_CA
    #define PWMDrv_idxFuncConfUB_CA HardwareMap_infoMap1SP->PWMDrv_idxFuncConfUB_CA

    //PWM�͵ͱ���������(��Ľṹ)
    #define LSPWMDrv_idxChanSigConfUB_CA HardwareMap_infoMap1SP->LSPWMDrv_idxChanSigConfUB_CA

    //�����͸ߵͱ���������(��Ľṹ)
    #define L9825Drv_swtUseUB_CA HardwareMap_infoMap1SP->L9825Drv_swtUseUB_CA
    #define PSWTDrv_idxChanSigConfUB_CA HardwareMap_infoMap1SP->PSWTDrv_idxChanSigConfUB_CA

    //ϵͳ����(��Ľṹ)
    #define SysCon_stIRevShuOffMskUI_C HardwareMap_infoMap1SP->SysCon_stIRevShuOffMskUI_C
    #define SysCon_stRevShuOffMskUI_C HardwareMap_infoMap1SP->SysCon_stRevShuOffMskUI_C
    #define SysCon_stExtWaitMskUW_C HardwareMap_infoMap1SP->SysCon_stExtWaitMskUW_C
    #define SysCon_stExtWait2MskUW_C HardwareMap_infoMap1SP->SysCon_stExtWait2MskUW_C
    #define SysCon_tiPostDriveMaxUI_C HardwareMap_infoMap1SP->SysCon_tiPostDriveMaxUI_C
    #define SysCon_tiShutdownMaxUI_C HardwareMap_infoMap1SP->SysCon_tiShutdownMaxUI_C
    #define SysCon_tiPostDriveMinUI_C HardwareMap_infoMap1SP->SysCon_tiPostDriveMinUI_C

    //ͨѶ����(��Ľṹ)
    #define MSCANDrv_infoBaudrateUW_CA HardwareMap_infoMap1SP->MSCANDrv_infoBaudrateUW_CA
    #define KWPCAN_tiToBMaxUW_C HardwareMap_infoMap1SP->KWPCAN_tiToBMaxUW_C
    #define KWPCAN_tiToCMaxUW_C HardwareMap_infoMap1SP->KWPCAN_tiToCMaxUW_C
    #define KWPCAN_numFCWMaxUW_C HardwareMap_infoMap1SP->KWPCAN_numFCWMaxUW_C
    #define CCP_StationIdUB_CA HardwareMap_infoMap1SP->CCP_StationIdUB_CA

    //�ײ���������(��Ľṹ)
    #define SigTst_bEnUW_C HardwareMap_infoMap1SP->SigTst_bEnUW_C
    #define SigTst_bChanL9825UB_CA HardwareMap_infoMap1SP->SigTst_bChanL9825UB_CA
    #define SigTst_fChanL9825UW_CA HardwareMap_infoMap1SP->SigTst_fChanL9825UW_CA
    #define SigTst_rChanL9825UW_CA HardwareMap_infoMap1SP->SigTst_rChanL9825UW_CA
    #define SigTst_bChanBTS724UB_CA HardwareMap_infoMap1SP->SigTst_bChanBTS724UB_CA

    //��Դ����(��Ľṹ)
    #define BattCD_uLogicOnUW_C HardwareMap_infoMap1SP->BattCD_uLogicOnUW_C
    #define BattCD_uUW_CUR HardwareMap_infoMap1SP->BattCD_uUW_CUR

    //���ݰ汾
    #define VehDataSet_idxSupervisorVerUB_CA VehicleMap_infoMap1SP->VehDataSet_idxSupervisorVerUB_CA
    #define VehDataSet_idxUserVerUB_CA VehicleMap_infoMap1SP->VehDataSet_idxUserVerUB_CA

    //ʾ��
    #define Test_datValueUW_C VehicleMap_infoMap1SP->Test_datValueUW_C
    #define Test_tTempUW_CUR VehicleMap_infoMap1SP->Test_tTempUW_CUR
    #define Test_tiTimeUW_MAP VehicleMap_infoMap1SP->Test_tiTimeUW_MAP
    #define Test_idxChannelUW_CA VehicleMap_infoMap1SP->Test_idxChannelUW_CA
    #define addrTablePUW VehicleMap_infoMap1SP->addrTablePUW
    #define AccPedCD_rUW_CUR VehicleMap_infoMap1SP->AccPedCD_rUW_CUR

    /*******************************************************************************************
                             MAPȫ�ֱ������� 
    ********************************************************************************************/
#ifndef AECS_HW
    #pragma CONST_SEG __GPAGE_SEG PARAMETERS_ROM1
#else   
    #pragma CONST_SEG VCU_CAL
#endif   
    extern volatile const HardwareMap_infoMap1S_TYPE HardwareMap_infoMap1S; //�ײ�궨����
    extern volatile const VehicleMap_infoMap1S_TYPE VehicleMap_infoMap1S;  //�����궨����
    #pragma CONST_SEG DEFAULT
    
    typedef struct
    {
        uint16 dataChkSumUW;
        uint16 idxProYearUW;
        uint8 idxProMonthUB;
        uint8 idxProDayUB; //����ˢд����
        uint8 dataReserved1UB;
        uint8 idxHourUW;
        uint8 idxMinuteUB;
        uint8 idxSecondUB; //����ˢдʱ��
        uint8 idxToolIDUBA[60];
    }HardwareMap_infoDataCheck1S_TYPE;

#ifndef AECS_HW    
      #pragma CONST_SEG __GPAGE_SEG DATA_VERIFY1
#else
      #pragma CONST_SEG VDL_HWIOCONFIG_CAL
#endif      
       extern volatile const HardwareMap_infoDataCheck1S_TYPE HardwareMap_infoDataCheck1S;  
    #pragma CONST_SEG DEFAULT
            
              
    extern HardwareMap_infoMap1S_TYPE  * __far HardwareMap_infoMap1SP;   //ָ��GlabalMap��һ��ָ��
    extern VehicleMap_infoMap1S_TYPE  * __far VehicleMap_infoMap1SP;     
  

#endif 
