/********************************************************************************
* 北京易控凌博汽车电子技术有限公司
* (c) Copyright 2005-2009 ECTEK
* 保留所有权利
********************************************************************************
* 工程名称：全局定义
* 文件名： SysConf.h
* 功能描述 ： 系统配置头文件
* 引用记录：该文件被各个.h文件包含
* 备注： 
* 修订记录：
*		日期		      版本		  作者		描述
*   2009.02.22       00.01.00     方成     第一版
*   2009.03.17       00.01.01     方成     增加了CCP访问的内存地址范围宏定义 
*   2009.12.03       00.01.02     方成     修改数据版本字节数
*   2011.01.07       00.01.03     方成     修改OBD支持的信号数目为24
*******************************************************************************/

#ifndef __SYSCONF_H
    #define __SYSCONF_H
    
    /*******************************************************************************************
                                  适用系统版本宏设置
    ********************************************************************************************/
   
 
    /*******************************************************************************************
                                  标定版本宏设置
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
                                  RAM版本宏设置
    ********************************************************************************************/

    #define __EXTRAM_NOT_USED     //外部RAM没有，也可以使用CCP，只是无法切换页而已
                                    //只有在定义了__DISABLE_CCP后，才无法使用CCP





    /*******************************************************************************************
                                  中断处理宏设置
    ********************************************************************************************/

    //设置INT_CFADDR，中断向量地址（后8位），取出高4位
    //中断向量地址（后8位），取出低4位/2，得到INT_CFDATA0~INT_CFDATA7中对应的那个
    //设置该对应的寄存器，设置中断由XGATE还是CPU来处理，以前中断的优先级
    #define ROUTE_INTERRUPT(vec_adr, cfdata)                \
      INT_CFADDR= (vec_adr) & 0xF0;                         \
      INT_CFDATA_ARR[((vec_adr) & 0x0F) >> 1]= (cfdata)    
      
    /* 中断向量地址（Vector Address）=XGATE的通道号（Channel ID）*2 */
    #define __INT_INDEX_RTIINTERRUPT_VEC 0xF0    
    
    #define __INT_INDEX_ECTFIRSTCHAN_VEC 0xEE  //ECT 通道0
    #define __INT_INDEX_ECTTIMEROVERFLOW_VEC 0xDE  //ECT timer overflow->ECT的时钟溢出中断
    
    
    #define __INT_INDEX_FLASHERRINTERRUPT_VEC 0xBA //Flash 错误中断
    
    #define __INT_INDEX_CANERRINTERRUPT_VEC 0xB4  //CAN0 错误中断
    #define __INT_INDEX_CANREVINTERRUPT_VEC 0xB2  //CAN0 接收中断    
    #define __INT_INDEX_CAN1ERRINTERRUPT_VEC 0xAC  //CAN1 错误中断
    #define __INT_INDEX_CAN1REVINTERRUPT_VEC 0xAA  //CAN1 接收中断
    
    #define __INT_INDEX_SPI0INTERRUPT_VEC 0xD8  //SPI0中断
    
    #define __INT_INDEX_ATD0INTERRUPT_VEC 0xD2  //ATD1中断
    
    #define __INT_INDEX_PLLLOSTINTERRUPT_VEC 0xC6  //PLL Lost中断
    
    #define __INT_INDEX_PIT0INTERRUPT_VEC 0x7A
    #define __INT_INDEX_PIT1INTERRUPT_VEC 0x78
    #define __INT_INDEX_PIT2INTERRUPT_VEC 0x76
    
    #define __INT_INDEX_TIMFIRSTCHAN_VEC 0x54  //TIM 通道0
    #define __INT_INDEX_TIMTIMEROVERFLOW_VEC 0x44  //TIM timer overflow->TIM的时钟溢出中断    
    
    #define __INT_INDEX_XGATESOFTWAREERR_VEC  0x16 

    /*******************************************************************************************
                                    段信息宏设置
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
                                  锁机制宏设置
    ********************************************************************************************/

    #define SET_SEM(x) (XGSEM = 0x0101 << (x))
    #define TST_SEM(x) (XGSEM & 0x0001 << (x))
    #define REL_SEM(x) (XGSEM = 0x0100 << (x))  
    
    /*******************************************************************************************
                                  全局地址宏设置
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
    
    #define __FLASH_ADDRESS_INIT_EEPROM_BEGIN 0x700000  //保存EEPROM初值的Flash起始地址
    #define __FLASH_SIZE_INIT_EEPROM 0x1000  //保存EEPROM初值的Flash 空间大小,4K
    
    
    /*******************************************************************************************
                                  DSM全局设置
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
                                  Bootloader 全局设置
    ********************************************************************************************/
    #define _BOOTLOADER_INDEX_RESETTYPE_NORMAL 0    
    #define _BOOTLOADER_INDEX_RESETTYPE_COP 1
    #define _BOOTLOADER_INDEX_RESETTYPE_CLOCKMONITOR 2     
    
    /*******************************************************************************************
                                  KWP2000 全局设置
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
                                  模块通道配置
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
                                  底层驱动其它配置
    ********************************************************************************************/  
    
    //复位类型
    #define  _HARDWAREDRV_NUM_RESETTYPE 7
    #define  _HARDWAREDRV_INDEX_RESET_POWERON 1       //上电复位
    #define  _HARDWAREDRV_INDEX_RESET_LOWVOLTAGE 2    //低电压复位
    #define  _HARDWAREDRV_INDEX_RESET_CLOCKMONITOR 3  //Clock Monitor复位
    #define  _HARDWAREDRV_INDEX_RESET_WATCHDOG 4      //看门狗复位
    #define  _HARDWAREDRV_INDEX_RESET_ILLEGALADDRESS 5  //非法地址访问复位
    #define  _HARDWAREDRV_INDEX_RESET_MANUAL 6  //手动复位，由程序产生
    #define  _HARDWAREDRV_INDEX_RESET_UNIMPLEMENTED 7 //无法识别的复位类型
    

    //看门狗复位类型
    #define  _HARDWAREDRV_INDEX_RESETMANUAL_NOTMANUAL  0  //非自动复位
    #define  _HARDWAREDRV_INDEX_RESETMANUAL_RAMTEST 1     //RAM测试失败
    
    #define  _HARDWAREDRV_INDEX_RESETMANUAL_PLLLOST 10
    #define  _HARDWAREDRV_INDEX_RESETMANUAL_CRGERR 11
    #define  _HARDWAREDRV_INDEX_RESETMANUAL_PITERR 12     //时钟错误
    
    #define  _HARDWAREDRV_INDEX_RESETMANUAL_EEPROMACCERR 20
    #define  _HARDWAREDRV_INDEX_RESETMANUAL_EEPROMMGSTAT 21
    #define  _HARDWAREDRV_INDEX_RESETMANUAL_EEPROMPARTITIONERR 22 //EEPROM错误
    #define  _HARDWAREDRV_INDEX_RESETMANUAL_DFDERR 23 //Double Bit Fault    
    
    
    #define  _HARDWAREDRV_INDEX_RESETMANUAL_KWP2000 30
    #define  _HARDWAREDRV_INDEX_RESETMANUAL_FLASHPROGRAM 31 //外部设备请求
    
    #define  _HARDWAREDRV_INDEX_RESETMANUAL_ATDERR 40  //AD队列堵塞
    
    
    #define  _HARDWAREDRV_INDEX_RESETMANUAL_INVALIDINT 50 //不正常的中断请求
    
    #define  _HARDWAREDRV_INDEX_RESETMANUAL_XGATEERR 60  //XGATE软件错误
    
    #define  _HARDWAREDRV_INDEX_RESETMANUAL_MPUERR 70 //MPU错误
           
    
    
    
    
    
    
#endif