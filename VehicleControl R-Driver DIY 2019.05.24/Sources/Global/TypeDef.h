/********************************************************************************
* 常州易控汽车电子有限公司
* (c) Copyright 2009-2015 ECTEK
* 保留所有权利
********************************************************************************
* 工程名称：全局定义
* 文件名： TypeDef.h
* 功能描述 ： 基本类型定义及全局宏定义头文件
* 引用记录：该文件被各个.h文件包含
* 备注： 
* 修订记录：
*		日期		      版本		  作者		描述
*   2009.02.22           00.01.00     方成     第一版
*******************************************************************************/

#ifndef __TYPEDEF_H
    #define __TYPEDEF_H


    #include "SysConf.h"
    
    /*******************************************************************************************
                                  基本变量类型定义
    ********************************************************************************************/
/*    
    #ifndef false
        #define false (0)
    #endif
    #ifndef true
        #define true  (!false)
    #endif
    #ifndef FALSE
    	#define FALSE   (0)
    #endif
    #ifndef TRUE
    	#define TRUE    (!FALSE)
    #endif

    //NULL指针
    #ifndef NULL
    	#define NULL    (0)
    #endif
 */
    //得到32位整数的高16位，或者16位整数的高8位
    #define getHighPart_32(x) ((x)>>16)
    #define getHighPart_16(x) ((x)>>8)

    //min,max宏定义
    #ifndef min
    	#define min(a, b)        (((a) < (b)) ? (a) : (b))
    #endif
    #ifndef max
    	#define max(a, b)        (((a) > (b)) ? (a) : (b))
    #endif

    //类型定义
    #ifndef _STD_TYPE_H
    	typedef unsigned char  bool;

    	typedef unsigned char  bit8;
    	typedef unsigned short bit16;
    	typedef unsigned long  bit32;

    	typedef unsigned int   uint;
    	typedef signed int     sint;
    	typedef double         real;

    	typedef unsigned char  uint8;
    	typedef signed char    sint8;
    	typedef unsigned short uint16;
    	typedef signed short   sint16;
    	typedef unsigned long  uint32;
    	typedef signed long    sint32;
    	typedef float          real32;
    	typedef double         real64;

    	#if !defined(_C166)		
    		typedef unsigned char  bit;
    	#endif

    	#if defined(__DCC__)  || defined(__use64integers__)
            #ifndef __QA_C_CHECKER
                //64位整数，看编译器是否支持
                typedef unsigned long long uint64;
                typedef signed long long   sint64;
            #else
                typedef unsigned long uint64;
                typedef signed long   sint64;
            #endif
        #endif
    #endif
                     
 

#endif 
