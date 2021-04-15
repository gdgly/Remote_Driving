/********************************************************************************
* �����׿������������޹�˾
* (c) Copyright 2009-2015 ECTEK
* ��������Ȩ��
********************************************************************************
* �������ƣ�����������
* �ļ����� HardwareLib.h
* �������� �� �ײ�Ӳ�����ܿ�
* ���ü�¼�����ļ���GlobalDef.h����
* ��ע�� 
* �޶���¼��
*		����		      �汾		  ����		����
*   2015.11.17           00.01.00     ����     ���EC3202V00Ӳ���޸Ľӿ�
*******************************************************************************/

#ifndef __HARDWARELIB_H
#define __HARDWARELIB_H


    #include "TypeDef.h"
    
    
    /*******************************************************************************************
                                 ���ú�����
    ********************************************************************************************/
    
    //һά����ֵ
    uint16 Intpol_u16u16(uint16 * __far addrTablePUW, uint16 dataXUW); //1ά����ֵ��X,Y����16λ�޷�������
    uint16 Intpol_s16u16(uint16 * __far addrTablePUW, sint16 dataXW); //1ά����ֵ��X���з���������Y���޷�������
    sint16 Intpol_u16s16(uint16 * __far addrTablePUW, uint16 dataXUW);//1ά����ֵ��X���޷���������Y���з�������
    sint16 Intpol_s16s16(uint16 * __far addrTablePUW, sint16 dataXW); //1ά����ֵ��X,Y����16λ�з�������
    
    //��ά����ֵ
    uint16 Intpol_u16u16u16(uint16 * __far addrTablePUW, uint16 dataXUW,uint16 dataYUW); //2ά����ֵ��X,Y,Z����16λ�޷�������  
    sint16 Intpol_u16u16s16(uint16 * __far addrTablePUW, uint16 dataXUW,uint16 dataYUW);//2ά����ֵ��X,Y����16λ�޷��ţ�Z���з���
    uint16 Intpol_u16s16u16(uint16 * __far addrTablePUW, uint16 dataXUW,sint16 dataYW); //2ά����ֵ��X,Z����16λ�޷��ţ�Y���з���
    sint16 Intpol_s16u16s16(uint16 * __far addrTablePUW, sint16 dataXW,uint16 dataYUW); //2ά����ֵ��X,Z����16λ�з��ţ�Y���޷���
    

    /*******************************************************************************************
                                 �ײ������ӿ�
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
    
    
    //ģ��������
    #define _ATD_VOLT_REFERENCE 5
    
    #define _ATD_INDEX_UBATTERY_CHAN 0   //���ص�ѹ���ڲ�����
    #define _ATD_INDEX_ACCPED1_CHAN  1   //����̤��1����Ӧ�Ӳ��X61
    #define _ATD_INDEX_ACCPED2_CHAN  2   //����̤��2����Ӧ�Ӳ��X62
    #define _ATD_INDEX_BRAKE1_CHAN   3   //�ƶ�1����Ӧ�Ӳ��X57
    #define _ATD_INDEX_BRAKE2_CHAN   4   //�ƶ�2����Ӧ�Ӳ��X58
    #define _ATD_INDEX_LOGICP_CHAN   23 //�߼����źţ���Ӧ�Ӳ��X73
    
    uint16 ATDDrv_GetChanResult(uint8 idxChanUB); //�õ�����ͨ����ADֵ 
    
    
    //����������
    //ǰ9·ͨ����ͨ��������øߵ͵�ƽ��Ч
    #define _DIO_INDEX_BAK1_CHAN    0   //����1�źţ���Ӧ�Ӳ��X10
    #define _DIO_INDEX_BAK2_CHAN    1   //����2�źţ���Ӧ�Ӳ��X1
    #define _DIO_INDEX_BAK3_CHAN    2   //����3�źţ���Ӧ�Ӳ��X2
    #define _DIO_INDEX_BAK4_CHAN    3   //����4�źţ���Ӧ�Ӳ��X17
    #define _DIO_INDEX_BAK5_CHAN    4   //����5�źţ���Ӧ�Ӳ��X18
    #define _DIO_INDEX_BAK6_CHAN    5   //����6�źţ���Ӧ�Ӳ��X13
    #define _DIO_INDEX_BAK7_CHAN    6   //����7�źţ���Ӧ�Ӳ��X14
    #define _DIO_INDEX_BAK8_CHAN    7   //����8�źţ���Ӧ�Ӳ��X9
    #define _DIO_INDEX_BAK9_CHAN    8   //����9�źţ���Ӧ�Ӳ��X5
    #define _DIO_INDEX_BAK10_CHAN   9   //����10�źţ���Ӧ�Ӳ��X6
    #define _DIO_INDEX_BAK11_CHAN   10  //����11�źţ���Ӧ�Ӳ��X3
    #define _DIO_INDEX_BAK12_CHAN   11  //����12�źţ���Ӧ�Ӳ��X4
    #define _DIO_INDEX_BAK13_CHAN   12  //����13�źţ���Ӧ�Ӳ��X7
    #define _DIO_INDEX_BAK14_CHAN   13  //����14�źţ���Ӧ�Ӳ��X11
    #define _DIO_INDEX_BAK15_CHAN   14  //����15�źţ���Ӧ�Ӳ��X15
    #define _DIO_INDEX_BAK16_CHAN   15  //����16�źţ���Ӧ�Ӳ��X19
    #define _DIO_INDEX_BAK17_CHAN   16  //����17�źţ���Ӧ�Ӳ��X22
    #define _DIO_INDEX_BAK18_CHAN   17  //����18�źţ���Ӧ�Ӳ��X23
    
    uint8 DIODrv_GetChanState(uint8 idxChanUB); //�õ�����ͨ����ADֵ
    
    
    //������Ϸ���ֵ
    #define _DSM_INDEX_OK_STATUS    0x0
    #define _DSM_INDEX_ERR_STATUS   0x1
    #define _DSM_INDEX_NVA_STATUS   0xFF
    
    
    //PWM���ź�������ͱ�������
    #define _LSPWM_INDEX_BAK1_CHAN  0  //���1����Ӧ�Ӳ��X45
    #define _LSPWM_INDEX_BAK2_CHAN  1  //���2����Ӧ�Ӳ��X38
    #define _LSPWM_INDEX_BAK3_CHAN  2  //���3����Ӧ�Ӳ��X46
    #define _LSPWM_INDEX_BAK4_CHAN  3  //���4����Ӧ�Ӳ��X40
    
    #define _PWM_DUTYCYCLE_FULL 10000  //10000��ʾռ�ձ�Ϊ100.00%
    #define _PWM_DUTYCYCLE_FACTOR   100
    
    //�ú��������10ms�����е���
    void LSPWMDrv_Interface(uint8 idxChanUB,uint32 fSetUI,uint16 rSetUW); //�������ͨ���ĵͱ�����Ƶ�ʼ�ռ�ձȣ�Ƶ�ʾ���0.01Hz�����Ŵ�100����ռ�ձȾ���Ϊ0.01%�����Ŵ�100��
    
    /*Ƶ��������ͨ������״̬*/
    #define _PPWM_DIAG_INDEX_OPENLOAD_STATUS        0
    #define _PPWM_DIAG_INDEX_SHORTCIRCUIT_STATUS    1
    
    /*��ȡƵ��������ͨ������*/
    uint8 LSPWMDrv_GetDiagStatus(uint8 idxChanUB,uint8 idxDiagTypeUB); /*idxDiagTypeUB����������������,����ֵΪ0��1,1��ʾ����������ͷ����˹���*/
    
    
    //�ߵͱ�����
    #define _PSWT_INDEX_BAK1_CHAN    0    //���̵�����������Ӧ�Ӳ��X33
    #define _PSWT_INDEX_BAK2_CHAN    1    //����2����Ӧ�Ӳ��X44   
    #define _PSWT_INDEX_BAK3_CHAN    2    //����3����Ӧ�Ӳ��X41
    #define _PSWT_INDEX_BAK4_CHAN    3    //����4����Ӧ�Ӳ��X43
    #define _PSWT_INDEX_BAK5_CHAN    4    //����5����Ӧ�Ӳ��X37
    #define _PSWT_INDEX_BAK6_CHAN    5    //����6����Ӧ�Ӳ��X42
    #define _PSWT_INDEX_BAK7_CHAN    6    //����7����Ӧ�Ӳ��X47
    #define _PSWT_INDEX_BAK8_CHAN    7    //����8����Ӧ�Ӳ��X48
    #define _PSWT_INDEX_BAK9_CHAN    8    //����9����Ӧ�Ӳ��X20
    #define _PSWT_INDEX_BAK10_CHAN   9    //����10����Ӧ�Ӳ��X16
    #define _PSWT_INDEX_BAK11_CHAN   10   //����11����Ӧ�Ӳ��X12
    #define _PSWT_INDEX_BAK12_CHAN   11   //����12����Ӧ�Ӳ��X8
    #define _PSWT_INDEX_BAK13_CHAN   12   //����13����Ӧ�Ӳ��X36
    #define _PSWT_INDEX_BAK14_CHAN   13   //����14����Ӧ�Ӳ��X32
    #define _PSWT_INDEX_BAK15_CHAN   14   //����15����Ӧ�Ӳ��X28
    #define _PSWT_INDEX_BAK16_CHAN   15   //����16����Ӧ�Ӳ��X24
    
    //�ú��������10ms�����е���
    void PSWTDrv_Interface(uint8 idxChanUB,uint8 bOpenUB); //�������ͨ���ĵͱ�����״̬, bOpen=1ʱΪ��, ����Ϊ�ر�
    
    /*�����͹�������ͨ������״̬*/
    #define _PSWT_DIAG_INDEX_OPENLOAD_STATUS        0 /*��·*/
    #define _PSWT_DIAG_INDEX_SHORTCIRCUIT_STATUS    1 /*��·*/
    
    /*��ȡ�����͹�������ͨ������*/
    uint8 PSwtDrv_GetDiagStatus(uint8 idxChanUB,uint8 idxDiagTypeUB); /*idxDiagTypeUB����������������,����ֵΪ0��1,1��ʾ����������ͷ����˹���*/
    
    
    //CANͨѶ�ӿ�
    typedef struct 
    {
        uint32 infoMsgIDUI;
        uint8  bExtIDUB;
        uint8  datMsgUBA[8];
        uint8  numLenUB;
        uint8  idxMsgPriorityUB;
        uint16 tiTimeStampUW;
    } MSCANDrv_infoMSGS_TYPE;
    
    //���ͽӿڣ�����1msCAN���������е���
    void MSCANDrv_SendMsgCAN0(MSCANDrv_infoMSGS_TYPE infoMsgS);
    void MSCANDrv_SendMsgCAN1(MSCANDrv_infoMSGS_TYPE infoMsgS);
    void MSCANDrv_SendMsgCAN2(MSCANDrv_infoMSGS_TYPE infoMsgS);
    
    //��ȡCAN�ӿڹ���״̬
    uint8 CANNet_GetErrStatus(uint8 idxCANUB); //idxCANUBΪCANͨ���ţ�0~2��������ֵΪ0��1,1��ʾ�й��ϣ�0��ʾ�޹���
    
    
    //EEPROM�洢
    extern uint8 EEPROMDrv_bSaveInstantUB; //������Ϊ����EEE�洢
    
    
    /*******************************************************************************************
                                 EOL���ݹ���
    ********************************************************************************************/
    #define _EOLDATA_INDEX_ERR_VEHICLE 0
    #define _EOLDATA_INDEX_ERR_ENGINE 1
    #define _EOLDATA_INDEX_ERR_OTHER 2
    
    extern uint8 EOLData_stChkErrUB;
    
    
    /*******************************************************************************************
                                 �ײ��Ѵ���������
    ********************************************************************************************/
    extern uint8 BattCD_bLogicOnUB; //�߼��翪��
    extern uint16 BattCD_uUW;  //���ص�ѹ
    
    
    /*******************************************************************************************
                                ��������״̬
    *******************************************************************************************/
    #define _VEHCO_INDEX_STATUS_INIT 0
    #define _VEHCO_INDEX_STATUS_RUNNING 1
    #define _VEHCO_INDEX_STATUS_AFTERRUN 2
    #define _VEHCO_INDEX_STATUS_FINISH 3
    
    extern uint8 VehCo_stNewUB; //��������״̬
    

#endif
