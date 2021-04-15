/*****************************************************************************/
/** 
* \file       cos_trace.h
* \author     wangkz
* \date       2017/02/27
* \version    V1.0
* \brief      文件描述
* \note       Copyright (c) 2000-2020  SKYWELL
* \remarks    修改日志
******************************************************************************/
#ifndef _SKY_TRACE_H_
#define _SKY_TRACE_H_
/*****************************************************************************
*                                头文件引用                                  *
*****************************************************************************/
#ifdef __cplusplus
extern "C"
{
#endif

#include <stdarg.h>
#include "macro_def.h"
#include <sys/prctl.h>
/*****************************************************************************
*                                  宏定义                                    *
*****************************************************************************/
#define TRACE_LEVEL_DEBUG 4   /**<  打开debug及级别小于debug的trace功能*/
#define TRACE_LEVEL_INFO 3    /**<  打开Info及级别小于info的trace功能*/
#define TRACE_LEVEL_WARNING 2 /**<  打开warning及级别小于warning的trace功能*/
#define TRACE_LEVEL_ERROR 1   /**<  打开error及级别小于error的trace功能*/

#define TRACE_MAX_MODULE_NUM 24
#define TRACE_MAX_MODULE_NAME_LEN 32

#ifndef DEBUG_METHOD
#define DEBUG_METHOD 3
#endif

#if DEBUG_METHOD == 3
#define SKY_TRACE_DEBUG(...) trace_log(__FILE__, __FUNCTION__, __LINE__, TRACE_LEVEL_DEBUG, __VA_ARGS__)
#define SKY_TRACE_INFO(...) trace_log(__FILE__, __FUNCTION__, __LINE__, TRACE_LEVEL_INFO, __VA_ARGS__)
#define SKY_TRACE_WARNING(...) trace_log(__FILE__, __FUNCTION__, __LINE__, TRACE_LEVEL_WARNING, __VA_ARGS__)
#define SKY_TRACE_ERROR(...) trace_log(__FILE__, __FUNCTION__, __LINE__, TRACE_LEVEL_ERROR, __VA_ARGS__)

#elif DEBUG_METHOD == 2
#define SKY_TRACE_DEBUG(...) printf(__VA_ARGS__)
#define SKY_TRACE_INFO(...) printf(__VA_ARGS__)
#define SKY_TRACE_WARNING(...) printf(__VA_ARGS__)
#define SKY_TRACE_ERROR(...) printf(__VA_ARGS__)

#else
#define SKY_TRACE_DEBUG(...) (VOID)0
#define SKY_TRACE_INFO(...) (VOID)0
#define SKY_TRACE_WARNING(...) (VOID)0
#define SKY_TRACE_ERROR(...) trace_log(__FILE__, __FUNCTION__, __LINE__, TRACE_LEVEL_ERROR, __VA_ARGS__)

#endif

    /*****************************************************************************
*                               全局函数声明                                 *
*****************************************************************************/
    void mkdirs(char *muldir);
    VOID *trace_init(WORD32 default_level);
    VOID trace_log(const CHAR *file_path, const CHAR *func_name, WORD32 line, WORD32 level, const CHAR *format, ...);
    void trace_comm_clear_logfile(void);
#ifdef __cplusplus
}
#endif
#endif
