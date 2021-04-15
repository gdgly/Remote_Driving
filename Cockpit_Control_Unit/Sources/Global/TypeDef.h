/********************************************************************************
* �����׿������������޹�˾
* (c) Copyright 2009-2015 ECTEK
* ��������Ȩ��
********************************************************************************
* �������ƣ�ȫ�ֶ���
* �ļ����� TypeDef.h
* �������� �� �������Ͷ��弰ȫ�ֺ궨��ͷ�ļ�
* ���ü�¼�����ļ�������.h�ļ�����
* ��ע�� 
* �޶���¼��
*		����		      �汾		  ����		����
*   2009.02.22           00.01.00     ����     ��һ��
*******************************************************************************/

#ifndef __TYPEDEF_H
    #define __TYPEDEF_H


    #include "SysConf.h"
    
    /*******************************************************************************************
                                  �����������Ͷ���
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

    //NULLָ��
    #ifndef NULL
    	#define NULL    (0)
    #endif
 */
    //�õ�32λ�����ĸ�16λ������16λ�����ĸ�8λ
    #define getHighPart_32(x) ((x)>>16)
    #define getHighPart_16(x) ((x)>>8)

    //min,max�궨��
    #ifndef min
    	#define min(a, b)        (((a) < (b)) ? (a) : (b))
    #endif
    #ifndef max
    	#define max(a, b)        (((a) > (b)) ? (a) : (b))
    #endif

    //���Ͷ���
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
                //64λ���������������Ƿ�֧��
                typedef unsigned long long uint64;
                typedef signed long long   sint64;
            #else
                typedef unsigned long uint64;
                typedef signed long   sint64;
            #endif
        #endif
    #endif
                     
 

#endif 
