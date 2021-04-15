/*****************************************************************************/
/** 
* \file       sky_time.c
* \author     yangkun
* \date       2015/07/06
* \version    v 1.0
* \brief      系统时间相关函数
* \note       Copyright (c) 2000-2020  金龙客车制造有限公司
* \remarks    输入修改日志
******************************************************************************/

/********************************************************************************
 *                               头文件引用                                     *
 ********************************************************************************/
#include "sky_time.h"

/********************************************************************************
 *                               局部常量定义                                   *
 ********************************************************************************/
#if CODE_PART("局部常量定义")

#endif

/********************************************************************************
 *                               局部宏定义                                     *
 ********************************************************************************/
#if CODE_PART("局部宏定义")
#define TIME_STR_FORMAT_MS    "%04d-%02d-%02d %02d:%02d:%02d.%03d"
#define TIME_STR_FORMAT_S     "%04d-%02d-%02d %02d:%02d:%02d"
#endif

/********************************************************************************
 *                               局部数据类型定义                               *
 ********************************************************************************/
#if CODE_PART("局部数据类型定义")

#endif

/********************************************************************************
 *                               静态变量定义                                   *
 ********************************************************************************/
#if CODE_PART("静态变量定义")

#endif

/********************************************************************************
 *                               全局变量定义                                   *
 ********************************************************************************/
#if CODE_PART("全局变量定义")
/* 分时判断开关,FALSE: 不分时判断 */
BOOL g_ts_switch = TRUE;
#endif

/********************************************************************************
 *                               局部函数声明                                   *
 ********************************************************************************/
#if CODE_PART("局部函数声明")

#endif

/********************************************************************************
 *                               局部函数定义                                   *
 ********************************************************************************/
#if CODE_PART("局部函数定义")

#endif

/********************************************************************************
 *                               全局函数定义                                   *
 ********************************************************************************/
#if CODE_PART("全局函数定义")
/* 
struct tm {
    int tm_sec;   // 秒 – 取值区间为[0,59]
    int tm_min;   // 分 - 取值区间为[0,59]
    int tm_hour;  // 时 - 取值区间为[0,23]
    int tm_mday;  // 一个月中的日期 - 取值区间为[1,31]
    int tm_mon;   // 月份（从一月开始，0代表一月） - 取值区间为[0,11]
    int tm_year;  // 年份 - 其值等于实际年份减去1900
    int tm_wday;  // 星期 - 取值区间为[0,6]，其中0代表星期天，1代表星期一，以此类推
    int tm_yday;  // 从每年的1月1日开始的天数 – 取值区间为[0,365]，其中0代表1月1日，1代表1月2日，以此类推
    int tm_isdst; // 夏令时标识符，实行夏令时的时候，tm_isdst为正。不实行夏令时的进候，tm_isdst为0；不了解情况时，tm_isdst()为负
};

struct timespec {
    time_t tv_sec;                // Seconds
    long   tv_nsec;               // Nanoseconds
};

struct itimerspec {
    struct timespec it_interval;  // Interval for periodic timer
    struct timespec it_value;     // Initial expiration
};

*/
/*****************************************************************************/
/** 
* \author      yangkun
* \date        2015/10/15
* \brief       时间转换字符串
* \param[in]   time_ms     时间(精度:毫秒)
* \param[out]  time_str    时间字符串
* \return      VOID
* \ingroup     sky_time
* \remarks     格式: 2015-09-08 15:52:21.356
******************************************************************************/
VOID time_to_str_ms(WORD64 time_ms, CHAR time_str[TIME_STR_LEN])
{
    TIMEVAL tv = {0};
    TM      tm = {0};
    WORD64  ms = 0;

    if(NULL == time_str)
    {
        //ASSERT(0);
        goto EXIT;
    }

    tv.tv_sec  = time_ms / 1000ull;
    ms         = (time_ms % 1000ull);
    tv.tv_usec = ms * 1000ull;

    /* 可重入的函数 */
    LOCALTIME_R(&tv.tv_sec, &tm);

    SNPRINTF(time_str,
                 TIME_STR_LEN,
                 TIME_STR_FORMAT_MS,
                 tm.tm_year + 1900,
                 tm.tm_mon + 1,
                 tm.tm_mday,
                 tm.tm_hour,
                 tm.tm_min,
                 tm.tm_sec,
                 (WORD32) ms);

EXIT:
    return;
}

/*****************************************************************************/
/** 
* \author      yangkun
* \date        2015/10/15
* \brief       时间转换字符串
* \param[in]   time_s      时间(精度:秒)
* \param[out]  time_str    时间字符串
* \return      VOID
* \ingroup     sky_time
* \remarks     格式: 2015-09-08 15:52:21
******************************************************************************/
VOID time_to_str_s(WORD32 time_s, CHAR time_str[TIME_STR_LEN])
{
    TIMEVAL tv = {0};
    TM      tm = {0};

    if(NULL == time_str)
    {
        //ASSERT(0);
        goto EXIT;
    }

    tv.tv_sec = time_s;

    /* 可重入的函数 */
    LOCALTIME_R(&tv.tv_sec, &tm);

    SNPRINTF(time_str,
                 TIME_STR_LEN,
                 TIME_STR_FORMAT_S,
                 tm.tm_year + 1900,
                 tm.tm_mon + 1,
                 tm.tm_mday,
                 tm.tm_hour,
                 tm.tm_min,
                 tm.tm_sec);

EXIT:
    return;
}

/*****************************************************************************/
/** 
* \author      yangkun
* \date        2015/07/06
* \brief       获取系统当前时间
* \return      当前时间(精度:微秒)
* \ingroup     sky_time
* \remarks     其它信息
******************************************************************************/
WORD64 time_current_us()
{
    TIMEVAL tv;
    WORD64  cur_time = 0;

    MEMSET(&tv, 0, SIZEOF(TIMEVAL));
    GETTIMEOFDAY(&tv, NULL);
    cur_time = ((WORD32)tv.tv_sec) * 1000000ull + (WORD32)tv.tv_usec;

    return cur_time;
}

/*****************************************************************************/
/** 
* \author      yangkun
* \date        2015/07/06
* \brief       获取系统当前时间
* \return      当前时间(精度:毫秒)
* \ingroup     sky_time
* \remarks     其它信息
******************************************************************************/
WORD64 time_current_ms()
{
    TIMEVAL tv;
    WORD64  cur_time = 0;

    MEMSET(&tv, 0, SIZEOF(TIMEVAL));
    GETTIMEOFDAY(&tv, NULL);
    cur_time = ((WORD32)tv.tv_sec) * 1000ull + ((WORD32)tv.tv_usec) / 1000ull;

    return cur_time;
}

/*****************************************************************************/
/** 
* \author      yangkun
* \date        2015/07/06
* \brief       获取系统当前时间
* \return      当前时间(精度:秒)
* \ingroup     sky_time
* \remarks     其它信息
******************************************************************************/
WORD32 time_current_s()
{
    TIMEVAL tv;
    WORD32  cur_time = 0;

    MEMSET(&tv, 0, SIZEOF(TIMEVAL));
    GETTIMEOFDAY(&tv, NULL);
    cur_time = (WORD32)tv.tv_sec;

    return cur_time;
}

/*****************************************************************************/
/** 
* \author      yangkun
* \date        2015/09/08
* \brief       获取系统当前时间字符串
* \param[out]  time_str    系统当前时间字符串
* \return      VOID
* \ingroup     sky_time
* \remarks     格式: 2015-09-08 15:52:21.356
******************************************************************************/
VOID time_current_str(CHAR time_str[TIME_STR_LEN])
{
    TIMEVAL now      = {0};
    TM      time_now = {0};

    if(NULL == time_str)
    {
        //ASSERT(0);
        goto EXIT;
    }

    GETTIMEOFDAY(&now, NULL);

    /* 可重入的函数 */
    LOCALTIME_R(&now.tv_sec, &time_now);

    SNPRINTF(time_str,
                 TIME_STR_LEN,
                 TIME_STR_FORMAT_MS,
                 time_now.tm_year + 1900,
                 time_now.tm_mon + 1,
                 time_now.tm_mday,
                 time_now.tm_hour,
                 time_now.tm_min,
                 time_now.tm_sec,
                 (WORD32) now.tv_usec / 1000);

EXIT:
    return;
}

/*****************************************************************************/
/** 
* \author      yangkun
* \date        2016/04/20
* \brief       设置系统时间
* \param[in]   time_str    待设置的时间字符串
* \return      参见cos_return_code.h
* \ingroup     sky_time
* \remarks     格式: 2015-09-08 15:52:21
******************************************************************************/
WORD32 time_set(const CHAR time_str[TIME_STR_LEN])
{
    TM      tm;
    TIMEVAL tv;
    SWORD32 arg_cnt = -1;
    SWORD32 set_rtn = -1;
    WORD32  rtn     = RTN_SUCCESS;

    if(NULL == time_str)
    {
        rtn = RTN_ERROR;
        //ASSERT(0);
        goto EXIT;
    }

    MEMSET(&tm, 0, SIZEOF(TM));
    arg_cnt = SSCANF(time_str, 
                         TIME_STR_FORMAT_S, 
                         &tm.tm_year,
                         &tm.tm_mon, 
                         &tm.tm_mday, 
                         &tm.tm_hour,
                         &tm.tm_min, 
                         &tm.tm_sec);
    /* 字符串格式错误 */
    if(6 != arg_cnt)
    {
        rtn = RTN_RECV_ERROR;
        //ASSERT(0);
        goto EXIT;
    }

    tm.tm_year = tm.tm_year - 1900;
    tm.tm_mon  = tm.tm_mon - 1;

    MEMSET(&tv, 0, SIZEOF(TIMEVAL));
    tv.tv_sec = mktime(&tm);

    set_rtn = settimeofday(&tv, NULL);
    if(set_rtn < 0)
    {
        rtn = RTN_ERROR;
        goto EXIT;
    }

EXIT:
    return rtn;
}
#endif

