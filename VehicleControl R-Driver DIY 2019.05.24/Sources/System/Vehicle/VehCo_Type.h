/*******************************************************************************
* 工程名称：整车管理
* 文件名： VehCo_Type.h
* 功能描述 ： 车辆类型定义头文件
* 引用记录：
* 备注： 
* 修订记录：
*		日期		      版本		  作者		描述
*                                             
*******************************************************************************/

#ifndef __VehCo_Type_H
    #define __VehCo_Type_H

    #include "TypeDef.h"
      #ifndef AECS_HW
      #pragma CODE_SEG DEFAULT
    #else  
      #pragma CODE_SEG VCU_CODE 
    #endif 
    void  Vehicle_Single_Variable_Set(void);  //单值变量映射
    void  Vehicle_Type_Set (void); //曲线表映射
    
    

    
    
     
#endif    