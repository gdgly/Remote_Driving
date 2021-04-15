/********************************************************************************
* �������ƣ�����������
* �ļ����� Map.h
* �������� �� MAPͼ����ģ��ͷ�ļ�
* ���ü�¼��
* ��ע��
* �޶���¼��
*		����		      �汾		  ����		����
*******************************************************************************/


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
        uint16 xDist[7];
        uint16 values[7];
    }AccPedCD_rUW_CUR_T01_TYPE;  //

    typedef struct
    {
        uint16 xSize;
        uint16 xDist[7];
        uint16 values[7];
    }AccPedCD_rUW_CUR2_T01_TYPE;  //

    typedef struct
    {
        uint16 xSize;
        uint16 xDist[11];
        uint16 values[11];
    }AccPed_Rip_T01_TYPE;  //

    typedef struct
    {
        uint16 xSize;
        uint16 xDist[7];
        uint16 values[7];
    }Breakk_rUW_CUR_T01_TYPE;  //

  typedef struct
    {
        uint16 xSize;
        uint16 xDist[7];
        sint16 values[7];
    }YaoGan_CUR_T01_TYPE;  //  ҡ��


    typedef struct
    {
        uint16 xSize;
        uint16 xDist[11];
        sint16 values[11];
    }AccPed_Acceleration_T01_Type;  //

    typedef struct
    {
        uint16 xSize;
        uint16 xDist[11];
        sint16 values[11];
    }Break_Acceleration_T01_Type;  //

    typedef struct
    {
        uint16 xSize;
        uint16 xDist[11];
        uint16 values[11];
    }AccPed_Rip_T02_TYPE;  //

    typedef struct
    {
        uint16 xSize;
        uint16 xDist[11];
        sint16 values[11];
    }AccPed_Acceleration_T02_TYPE;  //

    typedef struct
    {
        uint16 xSize;
        uint16 xDist[11];
        sint16 values[11];
    }Break_Acceleration_T02_TYPE;  //

    typedef struct
    {
        uint16 xSize;
        uint16 xDist[7];
        uint16 values[7];
    }AccPedCD_CUR_T02_TYPE;  //

    typedef struct
    {
        uint16 xSize;
        uint16 xDist[7];
        uint16 values[7];
    }Breakk_CUR_T02_TYPE;  //

    typedef struct
    {
        uint16 xSize;
        uint16 xDist[7];
        uint16 values[7];
    }Clutch_CUR_T02_TYPE;  //

    typedef struct
    {
        uint16 xSize;
        sint16 xDist[9];
        sint16 values[9];
    }Speed_To_Pwm_Add_TYPE;  //

    typedef struct
    {
        uint16 xSize;
        sint16 xDist[13];
        uint16 values[13];
    }BackZeroForce_Angle_TYPE;  //

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
        uint16 DIODrv_idxChanSwtTypeUW_CA[_DIO_NUM_CHAN];  //�ߵ���Ч��������������������ͨ���ߵ���Ч����
        uint16 DIODrv_uLevelHighThresUW_C;  //�ߵ���Ч��������ͨ���ĸߵ�ƽ��ѹ����
        uint16 DIODrv_uLevelLowThresUW_C;  //�ߵ���Ч����������ͨ���ĵ͵�ƽ��ѹ����
    //Ƶ�����������(��Ľṹ)
        uint8 PWMDrv_idxScaleAUB_C;  //PWMDrv_idxScaleAUB_C, PWMģ����ʱ��ԴA�ķ�Ƶ��
        uint8 PWMDrv_idxScaleBUB_C;  //PWMDrv_idxScaleBUB_C, PWMģ����ʱ��ԴB�ķ�Ƶ��
        uint8 PWMDrv_idxScaleSAUB_C;  //PWMDrv_idxScaleSAUB_C, PWMģ����ʱ��ԴSA�ķ�Ƶ��, fSA=fA/(2*N)
        uint8 PWMDrv_idxScaleSBUB_C;  //PWMDrv_idxScaleSBUB_C, PWMģ����ʱ��ԴSB�ķ�Ƶ��, fSB=fB/(2*N)
        uint8 PWMDrv_idxChanTimerConfUB_CA[4];  //PWMDrv_idxChanTimerConfUB_CA, PWM��ͨ������ʱ��Դ, X0(PWM1)��X2(PWM5)ʹ��A��SA, X1(PWM3)��X3(PWM7)ʹ��B��SB
        uint8 PWMDrv_stChanPrecTypeUB_CA[4];  //PWMͨ���ľ������ͣ�0ΪҪ���Ƶ�ʷ�Χ,1ΪҪ���ռ�ձȷ�Χ
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
        uint32 SigTst_fChanL9825UI_CA[_L9825_NUM_PWMCHAN];  //SigTst_fChanL9825UI_CA��L9825��ͨ����PWMƵ��
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
    //ͨ�ó��ͻ����궨
        uint8 Test_bSaveCmdUW_C;  //��ֵ�궨����ʾ�� ����EEPROM�Ĵ洢
        Test_tTempUW_CUR_TYPE Test_tTempUW_CUR;  //���߱궨����ʾ��
        Test_tiTimeUW_MAP_TYPE Test_tiTimeUW_MAP;  //����궨����ʾ��
        uint8 Test_strVersionUB_CA[16];  //�ַ����궨����ʾ��
        uint16 Percent_Filter_And_All[4];  //�����˲��ٷֱ�-0����-1����ռ��-2����ռ��
        uint8 VCU_C_1[7];  //��������֡-�汾-����-�ͺ�H������
        uint8 Vehicle_Type;  //���Ͷ��� 1 ʵ���Ҽ�ʻ�� 2 �ּܼ�ʻ��
        uint16 AccPed_Differ_Limit;  //˫���Ų�ֵ���� 100�� %
        uint16 AccPed_BreakErr_Max;  //����ɲ��ͬʱ�ȹ��� �������ֵ 0~100.00%
        uint16 Error_PowOn_Delay_Max;  //�ϵ���ϼ���ӳ� *10ms
        uint8 Err_AccPed_Index_Max;  //���Ź��ϼ���ӳ� *10ms
        uint8 LostEPSCom_Index_Max;  //EPSͨѶ��ʧ��ʱ *10ms
        uint8 LostVehcoCom_Index_Max;  //�г�״̬ͨѶ��ʧ��ʱ *10ms
        uint8 LostVehcoStateCom_Index_Max;  //����״̬ͨѶ��ʧ��ʱ *10ms
        uint8 LostVehcoBreakCom_Index_Max;  //�����ƶ�״̬ͨѶ��ʧ��ʱ *10ms
        uint8 LostVehcoSteering_Wheel_AngleCom_Index_Max;  //ģ�ⷽ����ͨѶ��ʧ��ʱ *10ms
        sint16 EPS_Angle_Max;  //EPS�Ƕ��������ֵ 0.1��
        sint16 EPS_Angle_Min;  //EPS�Ƕ�������Сֵ 0.1��
        uint16 R_Break_Min;  //Զ���ƶ�j������С����
        sint16 Wheel_Offset;  //�����̱�����ƫ��
        sint16 EPS_Offset;  //EPS�Ƕ�ƫ��
        uint8 DriveModeDebug;  //Զ�̼�ʻģʽDEBUG����
        uint16 D_Dang_Change_Delay_Index_Max;  //
        uint8 Break_Acceleration_Min_Step;  //�ƶ�������С����
        uint8 Eps_AngleMaxSpeed;  //
        uint16 Auto_Hold_Delay_Max;  //Auto_Hold_Delay_Max
        sint16 Auto_Hold_Release_Torque;  //
    //ʵ���Ҽ�ʻ��
        uint16 Motor_Vehicle_Speed_Scale_T01;  //�����ٱ�
        uint8 AccPedCD_TYPE_T01;  //�������� 1 ˫�� 2 ����
        uint8 Breakk_Type_T01;  //�ƶ�����  0����·������ 1��˫·����������  2����·ģ���� 3��˫·ģ����
        AccPedCD_rUW_CUR_T01_TYPE AccPedCD_rUW_CUR_T01;  //���ſ���
        AccPedCD_rUW_CUR2_T01_TYPE AccPedCD_rUW_CUR2_T01;  //���ſ���2
        AccPed_Rip_T01_TYPE AccPed_Rip_T01;  //��������
        Breakk_rUW_CUR_T01_TYPE Breakk_rUW_CUR_T01;  //�ƶ�̤�����
        AccPed_Acceleration_T01_Type AccPed_Acceleration_T01;  //���ſ���-���ٶ�
        YaoGan_CUR_T01_TYPE YaoGan_CUR_T01; // ҡ������

        Break_Acceleration_T01_Type Break_Acceleration_T01;  //�ƶ����-�ƶ�������%
    //���Ƽ�ʻ��
        uint16 Steering_Whee_Auto_Check_Pwm;  //�������Լ�PWMռ�ձ�
        uint8 Steering_Whee_Auto_Check_Still_Time_Max;  //�������Լ��תʱ������ 10ms
        uint8 Die_Angle;  //�����������Ƕ�
        uint16 Motor_Vehicle_Speed_Scale_T02;  //�����ٱ�
        uint8 AccPedCD_TYPE_T02;  //�������� 1 ˫�� 2 ���� 4 CAN
        uint8 Breakk_Type_T02;  //�ƶ�����  0����·������ 1��˫·����������  2����·ģ���� 3��˫·ģ����    4:CAN
        AccPed_Rip_T02_TYPE AccPed_Rip_T02;  //��������
        AccPed_Acceleration_T02_TYPE AccPed_Acceleration_T02;  //���ſ���-���ٶ�
        Break_Acceleration_T02_TYPE Break_Acceleration_T02;  //�ƶ����-�ƶ�������%
        AccPedCD_CUR_T02_TYPE AccPedCD_CUR_T02;  //��������
        Breakk_CUR_T02_TYPE Breakk_CUR_T02;  //�ƶ�̤������
        Clutch_CUR_T02_TYPE Clutch_CUR_T02;  //���̤������
        uint16 Level_Low;  //������
        uint16 Level_High;  //������
        uint8 Rtest_whee;  //
        sint16 Tag_Speed;  //
        Speed_To_Pwm_Add_TYPE Speed_To_Pwm_Add;  //Speed_To_Pwm_Add
        uint32 Fsetpwm;  //Fsetpwm
        BackZeroForce_Angle_TYPE BackZeroForce_Angle;  //����������
    }VehicleMap_infoMap1S_TYPE;

    //�ײ�������ʾ(��Ľṹ)
    #define DataSet_idxUserVerUB_CA HardwareMap_infoMap1SP->DataSet_idxUserVerUB_CA

    //ģ������������(��Ľṹ)
    #define ATDDrv_infoChanUB_CA HardwareMap_infoMap1SP->ATDDrv_infoChanUB_CA
    #define ATDDrv_idxChanSigConfUB_CA HardwareMap_infoMap1SP->ATDDrv_idxChanSigConfUB_CA

    //��������������(��Ľṹ)
    #define DIODrv_idxChanSigConfUB_CA HardwareMap_infoMap1SP->DIODrv_idxChanSigConfUB_CA
    #define DIODrv_idxChanSwtTypeUW_CA HardwareMap_infoMap1SP->DIODrv_idxChanSwtTypeUW_CA
    #define DIODrv_uLevelHighThresUW_C HardwareMap_infoMap1SP->DIODrv_uLevelHighThresUW_C
    #define DIODrv_uLevelLowThresUW_C HardwareMap_infoMap1SP->DIODrv_uLevelLowThresUW_C

    //Ƶ�����������(��Ľṹ)
    #define PWMDrv_idxScaleAUB_C HardwareMap_infoMap1SP->PWMDrv_idxScaleAUB_C
    #define PWMDrv_idxScaleBUB_C HardwareMap_infoMap1SP->PWMDrv_idxScaleBUB_C
    #define PWMDrv_idxScaleSAUB_C HardwareMap_infoMap1SP->PWMDrv_idxScaleSAUB_C
    #define PWMDrv_idxScaleSBUB_C HardwareMap_infoMap1SP->PWMDrv_idxScaleSBUB_C
    #define PWMDrv_idxChanTimerConfUB_CA HardwareMap_infoMap1SP->PWMDrv_idxChanTimerConfUB_CA
    #define PWMDrv_stChanPrecTypeUB_CA HardwareMap_infoMap1SP->PWMDrv_stChanPrecTypeUB_CA
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
    #define SigTst_fChanL9825UI_CA HardwareMap_infoMap1SP->SigTst_fChanL9825UI_CA
    #define SigTst_rChanL9825UW_CA HardwareMap_infoMap1SP->SigTst_rChanL9825UW_CA
    #define SigTst_bChanBTS724UB_CA HardwareMap_infoMap1SP->SigTst_bChanBTS724UB_CA

    //��Դ����(��Ľṹ)
    #define BattCD_uLogicOnUW_C HardwareMap_infoMap1SP->BattCD_uLogicOnUW_C
    #define BattCD_uUW_CUR HardwareMap_infoMap1SP->BattCD_uUW_CUR

    //���ݰ汾
    #define VehDataSet_idxSupervisorVerUB_CA VehicleMap_infoMap1SP->VehDataSet_idxSupervisorVerUB_CA
    #define VehDataSet_idxUserVerUB_CA VehicleMap_infoMap1SP->VehDataSet_idxUserVerUB_CA

    //ͨ�ó��ͻ����궨
    #define Test_bSaveCmdUW_C VehicleMap_infoMap1SP->Test_bSaveCmdUW_C
    #define Test_tTempUW_CUR VehicleMap_infoMap1SP->Test_tTempUW_CUR
    #define Test_tiTimeUW_MAP VehicleMap_infoMap1SP->Test_tiTimeUW_MAP
    #define Test_strVersionUB_CA VehicleMap_infoMap1SP->Test_strVersionUB_CA
    #define Percent_Filter_And_All VehicleMap_infoMap1SP->Percent_Filter_And_All
    #define VCU_C_1 VehicleMap_infoMap1SP->VCU_C_1
    #define Vehicle_Type VehicleMap_infoMap1SP->Vehicle_Type
    #define AccPed_Differ_Limit VehicleMap_infoMap1SP->AccPed_Differ_Limit
    #define AccPed_BreakErr_Max VehicleMap_infoMap1SP->AccPed_BreakErr_Max
    #define Error_PowOn_Delay_Max VehicleMap_infoMap1SP->Error_PowOn_Delay_Max
    #define Err_AccPed_Index_Max VehicleMap_infoMap1SP->Err_AccPed_Index_Max
    #define LostEPSCom_Index_Max VehicleMap_infoMap1SP->LostEPSCom_Index_Max
    #define LostVehcoCom_Index_Max VehicleMap_infoMap1SP->LostVehcoCom_Index_Max
    #define LostVehcoStateCom_Index_Max VehicleMap_infoMap1SP->LostVehcoStateCom_Index_Max
    #define LostVehcoBreakCom_Index_Max VehicleMap_infoMap1SP->LostVehcoBreakCom_Index_Max
    #define LostVehcoSteering_Wheel_AngleCom_Index_Max VehicleMap_infoMap1SP->LostVehcoSteering_Wheel_AngleCom_Index_Max
    #define EPS_Angle_Max VehicleMap_infoMap1SP->EPS_Angle_Max
    #define EPS_Angle_Min VehicleMap_infoMap1SP->EPS_Angle_Min
    #define R_Break_Min VehicleMap_infoMap1SP->R_Break_Min
    #define Wheel_Offset VehicleMap_infoMap1SP->Wheel_Offset
    #define EPS_Offset VehicleMap_infoMap1SP->EPS_Offset
    #define DriveModeDebug VehicleMap_infoMap1SP->DriveModeDebug
    #define D_Dang_Change_Delay_Index_Max VehicleMap_infoMap1SP->D_Dang_Change_Delay_Index_Max
    #define Break_Acceleration_Min_Step VehicleMap_infoMap1SP->Break_Acceleration_Min_Step
    #define Eps_AngleMaxSpeed VehicleMap_infoMap1SP->Eps_AngleMaxSpeed
    #define Auto_Hold_Delay_Max VehicleMap_infoMap1SP->Auto_Hold_Delay_Max
    #define Auto_Hold_Release_Torque VehicleMap_infoMap1SP->Auto_Hold_Release_Torque

    //ʵ���Ҽ�ʻ��
    #define Motor_Vehicle_Speed_Scale_T01 VehicleMap_infoMap1SP->Motor_Vehicle_Speed_Scale_T01
    #define AccPedCD_TYPE_T01 VehicleMap_infoMap1SP->AccPedCD_TYPE_T01
    #define Breakk_Type_T01 VehicleMap_infoMap1SP->Breakk_Type_T01
    #define AccPedCD_rUW_CUR_T01 VehicleMap_infoMap1SP->AccPedCD_rUW_CUR_T01
    #define AccPedCD_rUW_CUR2_T01 VehicleMap_infoMap1SP->AccPedCD_rUW_CUR2_T01
    #define AccPed_Rip_T01 VehicleMap_infoMap1SP->AccPed_Rip_T01
    #define Breakk_rUW_CUR_T01 VehicleMap_infoMap1SP->Breakk_rUW_CUR_T01
    #define AccPed_Acceleration_T01 VehicleMap_infoMap1SP->AccPed_Acceleration_T01
    #define YaoGan_CUR_T01 VehicleMap_infoMap1SP->YaoGan_CUR_T01  //ҡ��
    #define Break_Acceleration_T01 VehicleMap_infoMap1SP->Break_Acceleration_T01

    //���Ƽ�ʻ��
    #define Steering_Whee_Auto_Check_Pwm VehicleMap_infoMap1SP->Steering_Whee_Auto_Check_Pwm
    #define Steering_Whee_Auto_Check_Still_Time_Max VehicleMap_infoMap1SP->Steering_Whee_Auto_Check_Still_Time_Max
    #define Die_Angle VehicleMap_infoMap1SP->Die_Angle
    #define Motor_Vehicle_Speed_Scale_T02 VehicleMap_infoMap1SP->Motor_Vehicle_Speed_Scale_T02
    #define AccPedCD_TYPE_T02 VehicleMap_infoMap1SP->AccPedCD_TYPE_T02
    #define Breakk_Type_T02 VehicleMap_infoMap1SP->Breakk_Type_T02
    #define AccPed_Rip_T02 VehicleMap_infoMap1SP->AccPed_Rip_T02
    #define AccPed_Acceleration_T02 VehicleMap_infoMap1SP->AccPed_Acceleration_T02
    #define Break_Acceleration_T02 VehicleMap_infoMap1SP->Break_Acceleration_T02
    #define AccPedCD_CUR_T02 VehicleMap_infoMap1SP->AccPedCD_CUR_T02
    #define Breakk_CUR_T02 VehicleMap_infoMap1SP->Breakk_CUR_T02
    #define Clutch_CUR_T02 VehicleMap_infoMap1SP->Clutch_CUR_T02
    #define Level_Low VehicleMap_infoMap1SP->Level_Low
    #define Level_High VehicleMap_infoMap1SP->Level_High
    #define Rtest_whee VehicleMap_infoMap1SP->Rtest_whee
    #define Tag_Speed VehicleMap_infoMap1SP->Tag_Speed
    #define Speed_To_Pwm_Add VehicleMap_infoMap1SP->Speed_To_Pwm_Add
    #define Fsetpwm VehicleMap_infoMap1SP->Fsetpwm
    #define BackZeroForce_Angle VehicleMap_infoMap1SP->BackZeroForce_Angle

    /*******************************************************************************************
                             MAPȫ�ֱ������� 
    ********************************************************************************************/
       
    #pragma CONST_SEG __GPAGE_SEG PARAMETERS_ROM1    
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
    
    #pragma CONST_SEG __GPAGE_SEG DATA_VERIFY1
        extern volatile const HardwareMap_infoDataCheck1S_TYPE HardwareMap_infoDataCheck1S;  
    #pragma CONST_SEG DEFAULT
            
                
    extern HardwareMap_infoMap1S_TYPE  * __far HardwareMap_infoMap1SP;   //ָ��GlabalMap��һ��ָ��
    extern VehicleMap_infoMap1S_TYPE  * __far VehicleMap_infoMap1SP;     
    

#endif 