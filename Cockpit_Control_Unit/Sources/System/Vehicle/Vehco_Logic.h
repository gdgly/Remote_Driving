/*******************************************************************************
* �������ƣ������߼�
* �ļ����� Vehco_Logic.h
* �������� �� 
* ���ü�¼��
* ��ע�� 
* �޶���¼��
*		����		      �汾		  ����		����
*                                             
*******************************************************************************/

#ifndef __Vehco_Logic_H
    #define __Vehco_Logic_H

    #include "TypeDef.h"
    
    
        
    
    #ifndef AECS_HW
      #pragma CODE_SEG DEFAULT
    #else  
      #pragma CODE_SEG VCU_CODE 
    #endif
    
    void Vehco_Logic(void);
    void Vehco_Error (void);
    void Vehco_Errpr_Judge(void);
    void Beep_5050 (void);  
    void Lock_Vehicle_Active_Init(void);
    void Lock_Box_Vehicle(void);
    void Electric_Consume_Calculation(uint8 Pow_Down);
    
#endif    