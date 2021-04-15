/********************************************************************************
�Զ��庯������ ͷ�ļ�                                          
*******************************************************************************/

#ifndef __Custom_Func_H
    #define __Custom_Func_H

    #include "TypeDef.h"
    
    
    //#define _VEHCO_TIME_MONITOR_PERIOD  5
    //#define _AD_fliter_Cycles 10 //���Ź��˲�������
    #ifndef AECS_HW
      #pragma CODE_SEG DEFAULT
    #else  
      #pragma CODE_SEG VCU_CODE 
    #endif

    //����ģʽ�ж�
    void Torque_Mode_Judge(void);   
    //�ٶȻ��崦��
    void Motor_Speed_Buffer_FIFO (void); 
    //D���䶯ת�ؼ���
    void Crawl_D_Torque (void);     
    //����ת�ظ���ֵ����   Demand_Torque
    void AccPed_Torque_Calculation (void);    
    //D�Զ�פ��ת�ؼ���
    void Auto_Parking(void);
    //R�Զ�פ��ת�ؼ��� 
    void Auto_Parking_R(void);
    //��ع�������ת��
    void Torque_Power_Limit(void); 
    //D�����л���ת�ؼ���
    void FeedBack_Coast (void); 
    //D���ƶ�����ת�ؼ���
    void FeedBack_Break (void); 
    //�����繦�ʼ���
    void Driver_Power_Func(void);
    //���Ч���Զ�����ϵ������
    void  Efficieny_Fix(void); 
    //�ϲ�8bitΪByter
    uint8 SetBit8(uint8 *Bitt);
    //���byterΪ8��bit
    void Get8bit(uint8 *Bitt,uint8 Bytee);
    //������Ť��ƽ�����ɴ���
    void Gears_Shoft_Torque(void);
    
          
#endif    