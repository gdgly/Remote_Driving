/*******************************************************************************
* �������ƣ���������
* �ļ����� VehCo.h
* �������� �� ��������Э��ͷ�ļ�
* ���ü�¼��
* ��ע�� 
* �޶���¼��
*		����		      �汾		  ����		����
*                                             
*******************************************************************************/

#ifndef __VEHCO_H
    #define __VEHCO_H

    #include "TypeDef.h"
    
    
    #define _VEHCO_TIME_MONITOR_PERIOD  10
    #define _AD_fliter_Cycles 10 //���Ź��˲�������
    
    #define _VEHCO_AFTERRUN_STATUS_INIT     0
    #define _VEHCO_AFTERRUN_STATUS_BEGIN    1
    #define _VEHCO_AFTERRUN_STATUS_FINISH   2    
    //������ʷ�����趨������������
    //#pragma DATA_SEG __GPAGE_SEG RUNPAR_EEPROM

    #ifndef AECS_HW
      #pragma DATA_SEG __GPAGE_SEG RUNPAR_EEPROM
    #else  
      #pragma DATA_SEG  NVRAM 
    #endif
        extern uint16 VehCo_ctTestUW;
        extern uint8 Vehco_State_Stop_Step;



    #ifndef AECS_HW
      #pragma CODE_SEG DEFAULT
    #else  
      #pragma CODE_SEG VCU_CODE 
    #endif
    //���������ʼ��
    void VehCo_Init(void);
    
    //��������ѭ��
    void VehCo_Manage10ms(void);
    void VehCo_Manage1ms(void);
    
    //�������
    void VehCo_UpdateSig(void);
    void VehCo_Monitor(void);
    
    //״̬����
    void VehCo_StateMachine(void);
    void VehCo_Afterrun(void);
    
    //void Vehco_AC_Control(void);
    //void VehCo_Test(void);
    void Vehco_Key_Status(void);
    void VehCo_DOset (uint8 Do_off) ;

    //CAN����ɲ������źŴ���
    void  CAN_AccPed_Breakk(void) ;
    //�Զ�EPB
    void Auto_Hold_EPB (void) ;

    //�Զ�����
    void  Steering_Whee_Back_Zero (void); 
    //����������
    void BackZeroForce_Angle_Calc (void) ;
    
    //ģ�ⷽ�����Զ�У��
    void Steering_Whee_Auto_Check (void); 

    //Զ�̼��ٶȿ���
    void R_Vehco_Acceleration (void) ;
    
    //�����̽Ƕȿ���
    void  Steering_Wheel (void); 
    
    //Զ�̵�λ����
    void Vehco_Dang (void);
    
    //��ʻģʽ�ж�
    void Vehco_Driver_Mode(void);
    //�����ж�
    void Vehco_Errpr_Judge(void);     
     
#endif    