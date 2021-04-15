/********************************************************************************
* �����׿������������޹�˾
* (c) Copyright 2009-2015 ECTEK
* ��������Ȩ��
********************************************************************************
* �������ƣ�ȫ�ֶ���
* �ļ����� SysConf.h
* �������� �� ϵͳ����ͷ�ļ�
* ���ü�¼�����ļ�������.h�ļ�����
* ��ע�� 
* �޶���¼��
*		����		      �汾		  ����		����
*   2009.02.22           00.01.00     ����     ��һ��
*******************************************************************************/

#ifndef __SYSCONF_H
    #define __SYSCONF_H
    

    /*******************************************************************************************
                                  KWP2000 ȫ������
    ********************************************************************************************/  
    #define _KWP2000_LEN_K2REI_VMCS0 10     //Vehicle Manufacturer Customer String 0
    #define _KWP2000_LEN_K2REI_VMCS1 10     //Vehicle Manufacturer Customer String 1    
    #define _KWP2000_LEN_K2REI_VMECUSWVER 64   //Vehicle Manufacturer ECU Software  Version        
         
    #define _KWP2000_LEN_K2REI_EMDSID 32    //ECU Manufacturer Dataset Identification,FLASH    
    
    #define _KWP2000_LEN_K2REI_VMVIN 17     //Vehicle Manufacturer Vehicle Identification Number
    #define _KWP2000_LEN_K2REI_VMECUHWNR 13   //Vehicle Manufacturer ECU Hardware Number
    
    #define _KWP2000_LEN_K2REI_BOOTBLOCKID 8    //ECU Manufacturer Bootblock Identification,EEPROM     
    #define _KWP2000_LEN_K2REI_EMEPID 20    //ECU Manufacturer EEPROM Identification,EEPROM    
    #define _KWP2000_LEN_K2REI_EMECUHWNR 16 //ECU Manufacturer ECU Hardware Number, EEPROM
    #define _KWP2000_LEN_K2REI_EMECUHWVER 10 //ECU Manufacturer ECU Hardware Version Number, EEPROM
    #define _KWP2000_LEN_K2REI_EMECUSWNR 10 //ECU Manufacturer ECU Software Number,EEPROM 
    #define _KWP2000_LEN_K2REI_EMECUSWVER 10 //ECU Manufacturer ECU Software Version Number,EEPROM
    
    #define _KWP2000_LEN_K2REI_VMSYSNM 16  //Vehicle Manufacturer System Name or Engine Type
    #define _KWP2000_LEN_K2REI_VMTESTERSN 10 //Vehicle Manufacturer Repair Shop Code or Tester Serial Number
    #define _KWP2000_LEN_K2REI_VMPMDATE 3  //Vehicle Manufacturer Program Data
    #define _KWP2000_LEN_K2REI_VMEGTSTDATE 3  //Vehicle Manufacturer Engine Test Date
    #define _KWP2000_LEN_K2REI_VMDSID 30  //Vehicle Manufacturer Dataset Identification
    #define _KWP2000_LEN_K2REI_VMCS2 10     //Vehicle Manufacturer Customer String 2
    #define _KWP2000_LEN_K2REI_VMCS3 10     //Vehicle Manufacturer Customer String 3
    #define _KWP2000_LEN_K2REI_VMCS4 10     //Vehicle Manufacturer Customer String 4
    #define _KWP2000_LEN_K2REI_VMCS5 10     //Vehicle Manufacturer Customer String 5  
    
    /*******************************************************************************************
                                  �ײ�������������
    ********************************************************************************************/          
    #define  _HARDWAREDRV_NUM_RESETTYPE 7
    #define  _HARDWAREDRV_INDEX_RESET_POWERON 1       //�ϵ縴λ
    #define  _HARDWAREDRV_INDEX_RESET_LOWVOLTAGE 2    //�͵�ѹ��λ
    #define  _HARDWAREDRV_INDEX_RESET_CLOCKMONITOR 3  //Clock Monitor��λ
    #define  _HARDWAREDRV_INDEX_RESET_WATCHDOG 4      //���Ź���λ
    #define  _HARDWAREDRV_INDEX_RESET_ILLEGALADDRESS 5  //�Ƿ���ַ���ʸ�λ
    #define  _HARDWAREDRV_INDEX_RESET_MANUAL 6  //�ֶ���λ���ɳ������
    #define  _HARDWAREDRV_INDEX_RESET_UNIMPLEMENTED 7 //�޷�ʶ��ĸ�λ����

    
#endif