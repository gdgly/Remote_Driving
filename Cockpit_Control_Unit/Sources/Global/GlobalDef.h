/********************************************************************************
* 常州易控汽车电子有限公司
* (c) Copyright 2009-2015 ECTEK
* 保留所有权利
********************************************************************************
* 工程名称：全局定义
* 文件名： GlobalDef.h
* 功能描述 ： 全局定义头文件，包含各种全局的宏定义，函数定义和变量、常量定义
* 引用记录：
* 备注： 
* 修订记录：
*		日期		      版本		  作者		描述
*   2009.02.22           00.01.00     方成     第一版
*******************************************************************************/

#ifndef __GLOBALDEF_H_
    #define __GLOBALDEF_H_
    
    //#define AECS_HW   1

    #include <hidef.h>     
    #include <mc9s12xep100.h>  
    #include "HW_select.h"  
    #include "HardwareLib.h"  //底层驱动接口

    #include "Vin_Interface_conversion.h" 
    #include "Map.h"       //Map图管理
    #include "EOLData.h" //EOL数据
    #include "CANNet.h" //CAN总线管理
    #include "VehCo.h" //整车管理
    #include "TypeDef.h"
   
    #pragma LINK_INFO DERIVATIVE "mc9s12xep100"
    
#endif

