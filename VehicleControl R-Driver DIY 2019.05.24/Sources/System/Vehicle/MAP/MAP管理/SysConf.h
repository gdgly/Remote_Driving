/********************************************************************************
* �����׿��販�������Ӽ������޹�˾
* (c) Copyright 2005-2009 ECTEK
* ��������Ȩ��
********************************************************************************
* �������ƣ�ȫ�ֶ���
* �ļ����� SysConf.h
* �������� �� ϵͳ����ͷ�ļ�
* ���ü�¼�����ļ�������.h�ļ�����
* ��ע�� 
* �޶���¼��
*		����		      �汾		  ����		����
*   2009.02.22       00.01.00     ����     ��һ��
*   2009.03.17       00.01.01     ����     ������CCP���ʵ��ڴ��ַ��Χ�궨�� 
*   2009.12.03       00.01.02     ����     �޸����ݰ汾�ֽ���
*   2011.01.07       00.01.03     ����     �޸�OBD֧�ֵ��ź���ĿΪ24
*******************************************************************************/

#ifndef __SYSCONF_H
    #define __SYSCONF_H
    
    /*******************************************************************************************
                                  ����ϵͳ�汾������
    ********************************************************************************************/
   
 
    /*******************************************************************************************
                                  �궨�汾������
    ********************************************************************************************/

    #define __CALIBRATION_VERSION

    #ifdef __CALIBRATION_VERSION
        #ifdef __DISABLE_CCP
            #undef __DISABLE_CCP
        #endif
    #else
        #ifndef __DISABLE_CCP
            #define __DISABLE_CCP
        #endif    
    #endif


    /*******************************************************************************************
                                  RAM�汾������
    ********************************************************************************************/

    #define __EXTRAM_NOT_USED     //�ⲿRAMû�У�Ҳ����ʹ��CCP��ֻ���޷��л�ҳ����
                                    //ֻ���ڶ�����__DISABLE_CCP�󣬲��޷�ʹ��CCP





    /*******************************************************************************************
                                  �жϴ��������
    ********************************************************************************************/

    //����INT_CFADDR���ж�������ַ����8λ����ȡ����4λ
    //�ж�������ַ����8λ����ȡ����4λ/2���õ�INT_CFDATA0~INT_CFDATA7�ж�Ӧ���Ǹ�
    //���øö�Ӧ�ļĴ����������ж���XGATE����CPU��������ǰ�жϵ����ȼ�
    #define ROUTE_INTERRUPT(vec_adr, cfdata)                \
      INT_CFADDR= (vec_adr) & 0xF0;                         \
      INT_CFDATA_ARR[((vec_adr) & 0x0F) >> 1]= (cfdata)    
      
    /* �ж�������ַ��Vector Address��=XGATE��ͨ���ţ�Channel ID��*2 */
    #define __INT_INDEX_RTIINTERRUPT_VEC 0xF0    
    
    #define __INT_INDEX_ECTFIRSTCHAN_VEC 0xEE  //ECT ͨ��0
    #define __INT_INDEX_ECTTIMEROVERFLOW_VEC 0xDE  //ECT timer overflow->ECT��ʱ������ж�
    
    
    #define __INT_INDEX_FLASHERRINTERRUPT_VEC 0xBA //Flash �����ж�
    
    #define __INT_INDEX_CANERRINTERRUPT_VEC 0xB4  //CAN0 �����ж�
    #define __INT_INDEX_CANREVINTERRUPT_VEC 0xB2  //CAN0 �����ж�    
    #define __INT_INDEX_CAN1ERRINTERRUPT_VEC 0xAC  //CAN1 �����ж�
    #define __INT_INDEX_CAN1REVINTERRUPT_VEC 0xAA  //CAN1 �����ж�
    
    #define __INT_INDEX_SPI0INTERRUPT_VEC 0xD8  //SPI0�ж�
    
    #define __INT_INDEX_ATD0INTERRUPT_VEC 0xD2  //ATD1�ж�
    
    #define __INT_INDEX_PLLLOSTINTERRUPT_VEC 0xC6  //PLL Lost�ж�
    
    #define __INT_INDEX_PIT0INTERRUPT_VEC 0x7A
    #define __INT_INDEX_PIT1INTERRUPT_VEC 0x78
    #define __INT_INDEX_PIT2INTERRUPT_VEC 0x76
    
    #define __INT_INDEX_TIMFIRSTCHAN_VEC 0x54  //TIM ͨ��0
    #define __INT_INDEX_TIMTIMEROVERFLOW_VEC 0x44  //TIM timer overflow->TIM��ʱ������ж�    
    
    #define __INT_INDEX_XGATESOFTWAREERR_VEC  0x16 

    /*******************************************************************************************
                                    ����Ϣ������
    ********************************************************************************************/

    #define __SEG_START_REF(a) __SEG_START_ ## a
    #define __SEG_END_REF(a) __SEG_END_ ## a
    #define __SEG_SIZE_REF(a) __SEG_SIZE_ ## a

    #define __SEG_START_DEF(a) extern char __SEG_START_REF(a) []
    #define __SEG_END_DEF(a) extern char __SEG_END_REF( a) []
    #define __SEG_SIZE_DEF(a) extern char __SEG_SIZE_REF( a) []

    

    __SEG_START_DEF(BOOTLOADER_PIC_FLASH); 
    __SEG_SIZE_DEF(BOOTLOADER_PIC_FLASH);
    
    __SEG_START_DEF(TEST_RAM);
    
    
    __SEG_START_DEF(RUNINFO_COPY_EEPROM);
    __SEG_SIZE_DEF(RUNINFO_COPY_EEPROM);
    __SEG_START_DEF(RUNINFO_EEPROM);
    __SEG_SIZE_DEF(RUNINFO_EEPROM);
    __SEG_START_DEF(DSM_EEPROM);
    __SEG_SIZE_DEF(DSM_EEPROM);
    __SEG_START_DEF(RUNINFO_COPY_VERIFY_EEPROM);
    
    __SEG_START_DEF(BOOTLOADER_PIC_RAM);
    
    __SEG_SIZE_DEF(KWP2000_PIC_FLASH);
        
    
                                  
    /*******************************************************************************************
                                  �����ƺ�����
    ********************************************************************************************/

    #define SET_SEM(x) (XGSEM = 0x0101 << (x))
    #define TST_SEM(x) (XGSEM & 0x0001 << (x))
    #define REL_SEM(x) (XGSEM = 0x0100 << (x))  
    
    /*******************************************************************************************
                                  ȫ�ֵ�ַ������
    ********************************************************************************************/ 
    #define __RAM_ADDRESS_BEGIN 0
    #define __RAM_ADDRESS_END 0x0FFFFF
    
    #define __EEPROM_ADDRESS_BEGIN 0x13F000
    #define __EEPROM_ADDRESS_END 0x13FFFF   //4K
    
    #define __EEPROM_ADDRESS_CCP_BEGIN 0x13F000
    #define __EEPROM_ADDRESS_CCP_END   0x13FFFF
    
    #define __FLASH_ADDRESS_BEGIN 0x700000
    #define __FLASHD_ADDRESS_END  0x7FFFFF  //1M
    
    #define __CALFLASH_ADDRESS_BEGIN 0x7D0000
    #define __CALFLASH_ADDRESS_END 0x7EFFFF   //128K
    
    #define __CALFLASH_ADDRESS_ENGINE_BEGIN 0x7D0000
    #define __CALFLASH_ADDRESS_ENGINE_END 0x7EFFFF   //128K
    
    #define __CALFLASH_ADDRESS_VEHICLE_BEGIN 0x7D0000
    #define __CALFLASH_ADDRESS_VEHICLE_END 0x7EFFFF   //128K
    
    #define __EOLROM_ADDRESS_BEGIN 0x7CE800                
    #define __EOLROM_ADDRESS_END   0x7CFFFF //6K
    
    #define __MAP_SIZE_SECTOR 0x4000 //16K
    
    #define __FLASH_ADDRESS_INIT_EEPROM_BEGIN 0x700000  //����EEPROM��ֵ��Flash��ʼ��ַ
    #define __FLASH_SIZE_INIT_EEPROM 0x1000  //����EEPROM��ֵ��Flash �ռ��С,4K
    
    
    /*******************************************************************************************
                                  DSMȫ������
    ********************************************************************************************/ 
    #define _DSM_NUM_CONF_DFC_CHECKS 150
    #define _DSM_NUM_CONF_DFES_CLASSES 20
    #define _DSM_NUM_CONF_DINH_FIDS 50

    #define _DSM_NUM_CONF_DFES_ENTRIES 20 
    #define _DSM_NUM_CONF_SIGNALS 24   
    #define _DSM_NUM_CONF_FRFR_SIGNALS 12
    #define _DSM_NUM_CONF_FRFR_BYTES 13
    #define _DSM_NUM_CONF_EXFRFR_SIGNALS 2
    #define _DSM_NUM_CONF_EXFRFR_BYTES 2
    #define _DSM_NUM_CONF_ENV_BLOCKS 3
    #define _DSM_NUM_CONF_ENV_BLK_SIGNALS 2
    #define _DSM_NUM_CONF_ENV_BLK_BYTES 3
    #define _DSM_NUM_CONF_ENV_BLK_CLASSES 10

    #define _DSM_MASK_CONF_DFC_GLOBAL_DISBL 0
    
    #define _DFES_NUM_FAULTENTRY_STATUS 16
    
    #define _DINH_NUM_DFC_FID_MAX 6
    
    #define _DSMRDY_NUM_DFCGROUP_MAX 20 
    #define _DSMRDY_NUM_GROUP 11   
    
    #define _DVAL_NUM_PRIMARY_DFC_MAX 4
    
    /*******************************************************************************************
                                  Bootloader ȫ������
    ********************************************************************************************/
    #define _BOOTLOADER_INDEX_RESETTYPE_NORMAL 0    
    #define _BOOTLOADER_INDEX_RESETTYPE_COP 1
    #define _BOOTLOADER_INDEX_RESETTYPE_CLOCKMONITOR 2     
    
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
                                  ģ��ͨ������
    ********************************************************************************************/  
    #define _ECT_NUM_CHAN 8
    #define _ECT_NUM_TASK 8
    #define _L9825_NUM_CHAN 16
    #define _L9825_NUM_PWMCHAN 4
    
    #define _ATD_NUM_CHAN 24
    #define _DIO_NUM_CHAN 18 
    #define _LSPWM_NUM_CHAN 4
    #define _PSWT_NUM_CHAN 16

                                                                                         
    /*******************************************************************************************
                                  �ײ�������������
    ********************************************************************************************/  
    
    //��λ����
    #define  _HARDWAREDRV_NUM_RESETTYPE 7
    #define  _HARDWAREDRV_INDEX_RESET_POWERON 1       //�ϵ縴λ
    #define  _HARDWAREDRV_INDEX_RESET_LOWVOLTAGE 2    //�͵�ѹ��λ
    #define  _HARDWAREDRV_INDEX_RESET_CLOCKMONITOR 3  //Clock Monitor��λ
    #define  _HARDWAREDRV_INDEX_RESET_WATCHDOG 4      //���Ź���λ
    #define  _HARDWAREDRV_INDEX_RESET_ILLEGALADDRESS 5  //�Ƿ���ַ���ʸ�λ
    #define  _HARDWAREDRV_INDEX_RESET_MANUAL 6  //�ֶ���λ���ɳ������
    #define  _HARDWAREDRV_INDEX_RESET_UNIMPLEMENTED 7 //�޷�ʶ��ĸ�λ����
    

    //���Ź���λ����
    #define  _HARDWAREDRV_INDEX_RESETMANUAL_NOTMANUAL  0  //���Զ���λ
    #define  _HARDWAREDRV_INDEX_RESETMANUAL_RAMTEST 1     //RAM����ʧ��
    
    #define  _HARDWAREDRV_INDEX_RESETMANUAL_PLLLOST 10
    #define  _HARDWAREDRV_INDEX_RESETMANUAL_CRGERR 11
    #define  _HARDWAREDRV_INDEX_RESETMANUAL_PITERR 12     //ʱ�Ӵ���
    
    #define  _HARDWAREDRV_INDEX_RESETMANUAL_EEPROMACCERR 20
    #define  _HARDWAREDRV_INDEX_RESETMANUAL_EEPROMMGSTAT 21
    #define  _HARDWAREDRV_INDEX_RESETMANUAL_EEPROMPARTITIONERR 22 //EEPROM����
    #define  _HARDWAREDRV_INDEX_RESETMANUAL_DFDERR 23 //Double Bit Fault    
    
    
    #define  _HARDWAREDRV_INDEX_RESETMANUAL_KWP2000 30
    #define  _HARDWAREDRV_INDEX_RESETMANUAL_FLASHPROGRAM 31 //�ⲿ�豸����
    
    #define  _HARDWAREDRV_INDEX_RESETMANUAL_ATDERR 40  //AD���ж���
    
    
    #define  _HARDWAREDRV_INDEX_RESETMANUAL_INVALIDINT 50 //���������ж�����
    
    #define  _HARDWAREDRV_INDEX_RESETMANUAL_XGATEERR 60  //XGATE�������
    
    #define  _HARDWAREDRV_INDEX_RESETMANUAL_MPUERR 70 //MPU����
           
    
    
    
    
    
    
#endif