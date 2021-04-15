/********************************************************************************
* 常州易控汽车电子有限公司
* (c) Copyright 2009-2015 ECTEK
* 保留所有权利
********************************************************************************
* 工程名称：全局定义
* 文件名： SysConf.h
* 功能描述 ： 系统配置头文件
* 引用记录：该文件被各个.h文件包含
* 备注： 
* 修订记录：
*		日期		      版本		  作者		描述
*   2009.02.22           00.01.00     方成     第一版
*******************************************************************************/

#ifndef __SYSCONF_H
    #define __SYSCONF_H
    

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
                                  底层驱动其它配置
    ********************************************************************************************/          
    #define  _HARDWAREDRV_NUM_RESETTYPE 7
    #define  _HARDWAREDRV_INDEX_RESET_POWERON 1       //上电复位
    #define  _HARDWAREDRV_INDEX_RESET_LOWVOLTAGE 2    //低电压复位
    #define  _HARDWAREDRV_INDEX_RESET_CLOCKMONITOR 3  //Clock Monitor复位
    #define  _HARDWAREDRV_INDEX_RESET_WATCHDOG 4      //看门狗复位
    #define  _HARDWAREDRV_INDEX_RESET_ILLEGALADDRESS 5  //非法地址访问复位
    #define  _HARDWAREDRV_INDEX_RESET_MANUAL 6  //手动复位，由程序产生
    #define  _HARDWAREDRV_INDEX_RESET_UNIMPLEMENTED 7 //无法识别的复位类型

    
#endif