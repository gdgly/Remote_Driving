/********************************************************************************
* �����׿������������޹�˾
* (c) Copyright 2009-2015 ECTEK
* ��������Ȩ��
********************************************************************************
* �������ƣ�ȫ�ֶ���
* �ļ����� GlobalDef.h
* �������� �� ȫ�ֶ���ͷ�ļ�����������ȫ�ֵĺ궨�壬��������ͱ�������������
* ���ü�¼��
* ��ע�� 
* �޶���¼��
*		����		      �汾		  ����		����
*   2009.02.22           00.01.00     ����     ��һ��
*******************************************************************************/

#ifndef __GLOBALDEF_H_
    #define __GLOBALDEF_H_
    
    //#define AECS_HW   1

    #include <hidef.h>     
    #include <mc9s12xep100.h>  
    #include "HW_select.h"  
    #include "HardwareLib.h"  //�ײ������ӿ�

    #include "Vin_Interface_conversion.h" 
    #include "Map.h"       //Mapͼ����
    #include "EOLData.h" //EOL����
    #include "CANNet.h" //CAN���߹���
    #include "VehCo.h" //��������
    #include "TypeDef.h"
   
    #pragma LINK_INFO DERIVATIVE "mc9s12xep100"
    
#endif

