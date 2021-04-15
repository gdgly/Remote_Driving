/*****************************************************************************/
/** 
* \file       sky_trace.c
* \author     yangkun
* \date       2017/02/27
* \version    1.0
* \brief      文件描述
* \note       Copyright (c) 2000-2020  金龙客车制造有限公司
* \remarks    修改日志
******************************************************************************/
/*****************************************************************************
*                               头文件引用                                   *
*****************************************************************************/
#include "sky_trace.h"
#include "sky_tavl.h"
#include "sky_time.h"
#include "pseudo_timer_inter.h"

/*****************************************************************************
*                               局部常量定义                                 *
*****************************************************************************/
typedef enum trace_subpath_e
{
    TRACE_SUBPATH_USR = 1,
} TRACE_SUBPATH;

/*****************************************************************************
*                                局部宏定义                                  *
*****************************************************************************/
#define TRACE_LOG_HEAD_DIVIDE "====================================================================================="
#define TRACE_LOG_HEAD_TIME_BEGIN "+                              "
#define TRACE_LOG_HEAD_TIME_END "                              +"
#define TRACE_LOG_HEAD_DESC(dp) (dp) ? "[time                   ] [module      ] function                       line [level]: information" : "[time                   ] [pid  |tid  ] function                       line [level]: information"
#define TRACE_LOG_HEAD_FORMAT "\n%s\n%s%23s%s\n%s\n%s\n"
#define TRACE_LOG_FORMAT "[%s] [%-5d|%-5u] %-30.30s %4d [%-5s]: "
#define TRACE_LOG_FORMAT_DP "[%s] [%-12s] %-30.30s %4d [%-5s]: "

// #define TRACE_LOG_DIR             "/var/log/bus"
#define TRACE_LOG_DIR "./rx_log"
#define TRACE_LOG_SIZE (20 * 1024 * 1024)
#define TRACE_FILE_NAME_LENGTH 256

#define TRACE_LEVEL_STR_LENGTH 32
#define TRACE_BUF_SIZE 1048576 /*!< 1MB */
#define TRACE_SINGLE_SIZE 512  /*!< 一条TRACE最大长度 */
#define TRACE_FLUSH_TIME 100   /* TRACE刷新时间间隔(单位:ms) */

#define PROC_PATH_LEN 128 /* 业务进程全路径长度 */
#define PROC_NAME_LEN 32  /* 进程名字符串最大长度 */
#define FUNC_NAME_LEN 64  /* 函数名字符串最大长度 */

#define TRACE_LOG_SNPRINTF(expect_len)                                       \
    do                                                                       \
    {                                                                        \
        expect_len = SNPRINTF((CHAR *)module->trace_info + module->used_len, \
                              TRACE_BUF_SIZE - module->used_len,             \
                              TRACE_LOG_FORMAT,                              \
                              time_str,                                      \
                              pid,                                           \
                              tid,                                           \
                              func_name,                                     \
                              line,                                          \
                              level_str);                                    \
    } while (0)

/*****************************************************************************
*                             局部数据类型定义                               *
*****************************************************************************/
typedef CHAR (*MODULE_NAME)[TRACE_MAX_MODULE_NAME_LEN];

typedef struct trace_node_s
{
    TAVL_NODE avl_node;
    CHAR set_file[TRACE_FILE_NAME_LENGTH];
    LONG trace_level;
} TRACE_NODE;

typedef struct trace_dbg_s
{
    CHAR time[TIME_STR_LEN];
    WORD32 line;
    SWORD32 error;
} TRACE_DBG;

typedef struct trace_module_s
{
    BYTE subpath;  /*!< TRACE_SUBPATH */
    BOOL init;     /*!< 已初始化 */
    BOOL trace_on; /*!< trace开关 */
    BYTE pad[5];
    WORD32 default_level;
    WORD32 used_len;        /*!< trace_info已使用长度 */
    WORD64 last_trace_time; /*!< 最近一条trace时间 */
    CHAR *trace_info;
    CHAR trace_file[TRACE_FILE_NAME_LENGTH];
} TRACE_MODULE;

typedef struct trace_hdl
{
    BOOL init_comm; /*!< common init */
    BYTE pad[7];
    TRACE_MODULE usr; /*!< 业务进程trace */
    TAVL_HEAD trace_tree;
    PTHREAD_MUTEX_T fp_mutex;
    TRACE_DBG fopen_err; /*!< fopen失败信息 */
} TRACE_HDL;

/*****************************************************************************
*                               局部变量定义                                 *
*****************************************************************************/

/*****************************************************************************
*                               全局变量定义                                 *
*****************************************************************************/
TRACE_HDL g_trace_hdl = {.init_comm = FALSE};

/*****************************************************************************
*                               局部函数声明                                 *
*****************************************************************************/
static WORD32 trace_log_file_set(TRACE_MODULE *module);

/*****************************************************************************
*                               全局函数定义                                 *
*****************************************************************************/
void trace_comm_clear_logfile(void)
{
    char cmd_buf[256] = {'\0'};
    // sprintf(cmd_buf, "find %s -mtime 2 -type f -exec rm {} \\; &", TRACE_LOG_DIR);

    //删除120分钟前的日志文件
    sprintf(cmd_buf, "find %s -mmin +120 -type f -exec rm {} \\; &", TRACE_LOG_DIR);
    system(cmd_buf);
    // PRINTF("%s\n\n", cmd_buf);
}
/*****************************************************************************
*                               局部函数定义                                 *
*****************************************************************************/
#if CODE_PART("数据管理")
/*****************************************************************************/
/** 
* \author      yangkun
* \date        2015/09/09
* \brief       查找设置的trace节点
* \param[in]   file_name    设置的文件名
* \return      trace节点
* \ingroup     sky_trace
* \remarks     其它信息
******************************************************************************/
static TRACE_NODE *trace_node_find(const CHAR *file_name)
{
    TAVL_NODE *avl_node = NULL;
    TRACE_NODE *trace_node = NULL;
    INT result = 0;

    //ASSERT(NULL != file_name);

    avl_node = tavl_find(&g_trace_hdl.trace_tree,
                         (VOID *)file_name,
                         __avl_key_cmp_string,
                         &result);

    if (avl_node && !result)
    {
        trace_node = (TRACE_NODE *)avl_node->owner;
    }

    return trace_node;
}

static TAVL_NODE *__trace_node_create_func(TAVL_HEAD *avl_head, VOID *key)
{
    TRACE_NODE *trace_node = NULL;

    if ((NULL == avl_head) || (NULL == key))
    {
        PRINTF("__trace_node_create_func: input parameters NULL!\n");
        goto error;
    }

    trace_node = (TRACE_NODE *)MALLOC(SIZEOF(TRACE_NODE));
    if (NULL == trace_node)
    {
        PRINTF("__trace_node_create_func: memory alloc fail!\n");
        goto error;
    }

    MEMSET(trace_node, 0, SIZEOF(TRACE_NODE));
    STRNCPY(trace_node->set_file, (CHAR *)key, TRACE_FILE_NAME_LENGTH);
    trace_node->avl_node.key = trace_node->set_file;
    trace_node->avl_node.owner = trace_node;

    return &trace_node->avl_node;

error:
    //ASSERT(0);
    return NULL;
}

/*****************************************************************************/
/** 
* \author      yangkun
* \date        2015/09/09
* \brief       创建设置的trace节点
* \param[in]   file_name    设置的文件名
* \return      trace节点
* \ingroup     sky_trace
* \remarks     其它信息
******************************************************************************/
static TRACE_NODE *trace_node_create(const CHAR *file_name)
{
    TAVL_NODE *avl_node = NULL;
    TRACE_NODE *trace_node = NULL;
    INT is_same = 0;

    //ASSERT(NULL != file_name);

    avl_node = tavl_insert(&g_trace_hdl.trace_tree,
                           (VOID *)file_name,
                           __trace_node_create_func,
                           __avl_key_cmp_string,
                           &is_same);
    if ((is_same) ||
        (NULL == avl_node))
    {
        //ASSERT(0);
        goto EXIT;
    }

    trace_node = (TRACE_NODE *)avl_node->owner;

EXIT:
    return trace_node;
}

/*****************************************************************************/
/** 
* \author      yangkun
* \date        2015/09/08
* \brief       销毁设置的trace节点
* \param[out]  trace_node    trace节点
* \return      VOID
* \ingroup     sky_trace
* \remarks     其它信息
******************************************************************************/
static VOID trace_node_destroy(TRACE_NODE *trace_node)
{
    SKY_TRACE_DEBUG("Param: %p\n", trace_node);

    //ASSERT(NULL != trace_node);

    /* 从线索avl树中移除 */
    tavl_remove_node(&g_trace_hdl.trace_tree,
                     &trace_node->avl_node,
                     __avl_key_cmp_string);

    /* 释放内存 */
    FREE(trace_node);
    trace_node = NULL;

    return;
}

static VOID trace_tree_destroy()
{
    TRACE_NODE *trace_node = NULL;

    trace_node = (TRACE_NODE *)tavl_get_1st_node(&g_trace_hdl.trace_tree);
    while (trace_node)
    {
        trace_node_destroy(trace_node);

        trace_node = (TRACE_NODE *)tavl_get_1st_node(&g_trace_hdl.trace_tree);
    }

    return;
}
#endif

#if CODE_PART("mutex")
/*****************************************************************************/
/** 
* \author      yangkun
* \date        2016/03/11
* \brief       加锁
* \return      VOID
* \ingroup     sky_trace
* \remarks     其它信息
******************************************************************************/
VOID trace_mutex_lock()
{
    PTHREAD_MUTEX_LOCK(&g_trace_hdl.fp_mutex);
    return;
}

/*****************************************************************************/
/** 
* \author      yangkun
* \date        2016/03/11
* \brief       解锁
* \return      VOID
* \ingroup     sky_trace
* \remarks     job模板卸载时调用,避免trace死锁
******************************************************************************/
VOID trace_mutex_unlock()
{
    PTHREAD_MUTEX_UNLOCK(&g_trace_hdl.fp_mutex);
    return;
}
#endif

/*****************************************************************************/
/** 
* \author      yangkun
* \date        2015/09/11
* \brief       公共初始化
* \param[out]  module           trace模块
* \param[in]   subpath          子路径类型
* \param[in]   default_level    默认级别
* \return      参见return_code.h
* \ingroup     sky_trace
* \remarks     其它信息
******************************************************************************/
static WORD32 trace_comm_init(TRACE_MODULE *module,
                              TRACE_SUBPATH subpath,
                              WORD32 default_level)
{
    WORD32 rtn = RTN_SUCCESS;

    //ASSERT(NULL != module);

    if (module->init)
    {
        PRINTF("trace_inter_init: [%s] has init!\n", module->trace_file);
        goto EXIT;
    }

    if (!g_trace_hdl.init_comm)
    {
        tavl_init_head(&g_trace_hdl.trace_tree, NULL);

        rtn = pseudo_timer_create();
        if (RTN_SUCCESS != rtn)
        {
            //ASSERT(0);
            goto EXIT;
        }

        g_trace_hdl.init_comm = TRUE;
    }

    module->subpath = subpath;
    /* trace缓存 */
    if (NULL == module->trace_info)
    {
        module->trace_info = (CHAR *)MALLOC(TRACE_BUF_SIZE);
        if (NULL == module->trace_info)
        {
            rtn = RTN_ERROR;
            //ASSERT(0);
            goto EXIT;
        }
    }
    module->used_len = 0;
    module->trace_info[0] = '\0';

    /* 设置默认属性 */
    module->trace_on = TRUE;
    module->default_level = default_level;

    /* 设置trace log文件 */
    rtn = trace_log_file_set(module);
    if (RTN_SUCCESS != rtn)
    {
        rtn = RTN_ERROR;
        //ASSERT(0);
        goto EXIT;
    }

    /* 初始化ok */
    module->init = TRUE;

EXIT:
    return rtn;
}

/*****************************************************************************/
/** 
* \author      yangkun
* \date        2015/09/11
* \brief       公共卸载
* \return      VOID
* \ingroup     sky_trace
* \remarks     其它信息
******************************************************************************/
VOID trace_comm_uninit()
{
    if (!g_trace_hdl.init_comm)
    {
        goto EXIT;
    }

    trace_tree_destroy();
    PTHREAD_MUTEX_DESTROY(&g_trace_hdl.fp_mutex);
    MEMSET(&g_trace_hdl, 0, SIZEOF(TRACE_HDL));

EXIT:
    return;
}

/*****************************************************************************/
/** 
* \author      yangkun
* \date        2015/09/09
* \brief       向文件中写入trace
* \param[out]  module    trace模块
* \return      VOID
* \ingroup     sky_trace
* \remarks     其它信息
******************************************************************************/
static VOID trace_write(TRACE_MODULE *module)
{
    FILE *fp = NULL;
    CHAR bak_file[TRACE_FILE_NAME_LENGTH] = {0};
    LONG file_size = 0;
    SWORD32 result = 0;

    //(NULL != module);
    if (module->init == FALSE) //add by zhj 2019-09-24
        goto EXIT;

    if (0 == module->used_len)
    {
        goto EXIT;
    }

    if (NULL == module->trace_info)
    {
        PRINTF("code error:%s %u\n", __FILE__, __LINE__);
        goto EXIT;
    }

    /* 文件存在就打开,不存在则创建(因为文件可能被手动删除,
       所以每次写trace时都重新打开) */
    fp = FOPEN(module->trace_file, "a");
    if (NULL == fp)
    {
        g_trace_hdl.fopen_err.error = errno;
        g_trace_hdl.fopen_err.line = __LINE__;
        time_current_str(g_trace_hdl.fopen_err.time);

        PRINTF("code error:%s %u,  module->trace_file: %s\n", __FILE__, __LINE__, module->trace_file);
        goto EXIT;
    }

    /* 若本次需要记录的内容超过剩余空间，则把当前文件重命名为*.bak */
    FSEEK(fp, 0, SEEK_END);
    file_size = FTELL(fp);
    if ((file_size + module->used_len) >= TRACE_LOG_SIZE)
    {
        trace_comm_clear_logfile();

        FCLOSE(fp);
        fp = NULL;

        time_t timep;
        struct tm *p;
        time(&timep);
        p = localtime(&timep);

        /* 重命名为*.bak */
        SNPRINTF(bak_file,
                 TRACE_FILE_NAME_LENGTH,
                 "%s-%04d-%02d-%02d-%02d-%02d-%02d.log", module->trace_file, 1900 + p->tm_year, 1 + p->tm_mon, p->tm_mday,
                 p->tm_hour, p->tm_min, p->tm_sec);
        // PRINTF("module->trace_file:%s, bak_file:%s\n", module->trace_file, bak_file);
        // REMOVE(bak_file);
        RENAME(module->trace_file, bak_file);

        fp = FOPEN(module->trace_file, "a");
        if (NULL == fp)
        {
            g_trace_hdl.fopen_err.error = errno;
            g_trace_hdl.fopen_err.line = __LINE__;
            time_current_str(g_trace_hdl.fopen_err.time);

            PRINTF("code error:%s %u\n", __FILE__, __LINE__);
            goto EXIT;
        }
    }

    result = FPUTS(module->trace_info, fp);

    FFLUSH(fp);

    FCLOSE(fp);
    fp = NULL;

    /* 清空缓存 */
    module->used_len = 0;
    module->trace_info[0] = '\0';

EXIT:
    return;
}

/*****************************************************************************/
/** 
* \author      yangkun
* \date        2015/10/20
* \brief       获取进程执行路径
* \param[out]  exe_path    执行路径
* \return      VOID
* \ingroup     sky_debug
* \remarks     其它信息
******************************************************************************/
VOID exe_path_get(CHAR exe_path[PROC_PATH_LEN])
{
    LONG path_len = -1;

    if (NULL == exe_path)
    {
        goto EXIT;
    }

    /* 初始化 */
    MEMSET(exe_path, 0, PROC_PATH_LEN);

    path_len = READLINK("/proc/self/exe", exe_path, PROC_PATH_LEN);
    if ((path_len <= 0) ||
        (path_len >= PROC_PATH_LEN))
    {
        goto EXIT;
    }

    /* 返回的字符串没有以'\0'结尾 */
    exe_path[path_len] = '\0';

EXIT:
    return;
}

/*****************************************************************************/
/** 
* \author      yangkun
* \date        2015/09/11
* \brief       获取进程名
* \param[out]  proc_name    进程名
* \return      VOID
* \ingroup     sky_debug
* \remarks     从进程执行路径获取进程名(prctl(PR_GET_NAME,proc_name,0,0,0)获取
*              的进程名会被截断)
******************************************************************************/
VOID proc_name_get(CHAR proc_name[PROC_NAME_LEN])
{
    CHAR proc_exe_path[PROC_PATH_LEN] = {0};
    CHAR *proc_name_pos = NULL;

    if (NULL == proc_name)
    {
        goto EXIT;
    }

    /* 初始化 */
    MEMSET(proc_name, 0, PROC_NAME_LEN);

    exe_path_get(proc_exe_path);

    /* 从后开始搜索'/'位置 */
    proc_name_pos = STRRCHR(proc_exe_path, '/');
    if (NULL == proc_name_pos)
    {
        goto EXIT;
    }

    /* 越过'/' */
    proc_name_pos++;

    /* 输出 */
    STRNCPY(proc_name, proc_name_pos, PROC_NAME_LEN);
    proc_name[PROC_NAME_LEN - 1] = '\0';

EXIT:
    return;
}

/*****************************************************************************/
/** 
* \author      yangkun
* \date        2015/09/10
* \brief       设置trace log文件(全路径)
* \param[in]   module    trace模块
* \return      参见return_code.h
* \ingroup     sky_trace
* \remarks     其它信息
******************************************************************************/
static WORD32 trace_log_file_set(TRACE_MODULE *module)
{
    CHAR time_str[TIME_STR_LEN] = {0};
    CHAR proc_name[PROC_NAME_LEN] = {0};
    WORD32 len = 0;
    WORD32 rtn = RTN_SUCCESS;

    //   if(module->init == FALSE)      //add by zhj 2019-09-24
    //        goto EXIT;

    switch (module->subpath)
    {
    case TRACE_SUBPATH_USR:
    {
        SNPRINTF(module->trace_file,
                 TRACE_FILE_NAME_LENGTH,
                 "%s",
                 TRACE_LOG_DIR);
        break;
    }

    default:
    {
        //ASSERT(0);
        goto EXIT;
    }
    }

    /* 判断log目录是否存在，若不存在则创建 */
    if (NULL == OPENDIR(module->trace_file))
    {
        struct stat st;
        mkdirs(module->trace_file);
        if (stat(module->trace_file, &st) < 0 || !S_ISDIR(st.st_mode))
        {
            perror(module->trace_file);
            rtn = RTN_ERROR;
            goto EXIT;
        }
    }

    /* 拼装trace文件名 */
    proc_name_get(proc_name);
    len = STRLEN(module->trace_file);
    //ASSERT(len < TRACE_FILE_NAME_LENGTH);

    SNPRINTF(module->trace_file + len,
             TRACE_FILE_NAME_LENGTH - len,
             "/0_%s",
             proc_name);

    PRINTF("[%s] module->trace_file:%s\n", __FUNCTION__, module->trace_file);

    if (NULL == module->trace_info)
    {
        rtn = RTN_ERROR;
        //ASSERT(0);
        goto EXIT;
    }

    /* 写入trace起始头部 */
    trace_mutex_lock();

    time_current_str(time_str);
    len = SNPRINTF(module->trace_info,
                   TRACE_BUF_SIZE,
                   TRACE_LOG_HEAD_FORMAT,
                   TRACE_LOG_HEAD_DIVIDE,
                   TRACE_LOG_HEAD_TIME_BEGIN,
                   time_str,
                   TRACE_LOG_HEAD_TIME_END,
                   TRACE_LOG_HEAD_DIVIDE,
                   TRACE_LOG_HEAD_DESC(0));
    if (len > 0)
    {
        module->used_len = len;
        trace_write(module);
    }

    trace_mutex_unlock();

EXIT:
    return rtn;
}

/*****************************************************************************/
/** 
* \author      yangkun
* \date        2015/09/08
* \brief       获取trace level字符串
* \param[in]   level        trace level
* \param[out]  level_str    level字符串
* \return      VOID
* \ingroup     sky_trace
* \remarks     其它信息
******************************************************************************/
static VOID trace_level_str_get(WORD32 level, CHAR level_str[TRACE_LEVEL_STR_LENGTH])
{
    switch (level)
    {
    case TRACE_LEVEL_DEBUG:
    {
        STRCPY(level_str, "DEBUG");
        break;
    }
    case TRACE_LEVEL_INFO:
    {
        STRCPY(level_str, "INFO");
        break;
    }
    case TRACE_LEVEL_WARNING:
    {
        STRCPY(level_str, "WARNING");
        break;
    }
    case TRACE_LEVEL_ERROR:
    {
        STRCPY(level_str, "ERROR");
        break;
    }
    default:
    {
        STRCPY(level_str, "UNKNOWN");
        break;
    }
    }

    return;
}

/*****************************************************************************/
/** 
* \author      yangkun
* \date        2015/10/29
* \brief       获取本线程id(LWP)
* \return      tid
* \ingroup     sky_debug
* \remarks     其它信息
******************************************************************************/
WORD32 tid_get()
{
    PID_T tid;
    WORD32 rtid = 0;

    tid = syscall(SYS_gettid);
    MEMCPY(&rtid, &tid, SIZEOF(WORD32));

    return rtid;
}

VOID trace_assemble(TRACE_MODULE *module, const CHAR *func_name,
                    WORD32 line, WORD32 level, const CHAR *format, va_list arg)
{
    CHAR time_str[TIME_STR_LEN] = {0};
    CHAR level_str[TRACE_LEVEL_STR_LENGTH] = {0};
    WORD64 cur_time = 0;
    PID_T pid = GETPID;
    WORD32 tid = tid_get();
    SWORD32 expect_len = 0;
    SWORD32 used_len = 0;

    if (module->init == FALSE) //add by zhj 2019-09-24
        goto EXIT;

    /* 开始向文件写trace */
    trace_mutex_lock();

    //ASSERT(NULL != module);
    if (NULL == module->trace_info)
    {
        PRINTF("code error:%s %u\n", __FILE__, __LINE__);
        goto EXIT;
    }

    if (module->used_len > TRACE_BUF_SIZE - TRACE_SINGLE_SIZE)
    {
        PRINTF("code error:%s %u\n", __FILE__, __LINE__);
        module->used_len = 0;
    }

    /* 获取系统当前时间 */
    cur_time = time_current_ms();
    time_to_str_ms(cur_time, time_str);

    /* 获取trace级别 */
    trace_level_str_get(level, level_str);

    /* 拼装打印log内容 */
    TRACE_LOG_SNPRINTF(expect_len);

    if (expect_len > TRACE_BUF_SIZE - module->used_len)
    {
        trace_write(module);

        /* 重新拼装 */
        TRACE_LOG_SNPRINTF(expect_len);
        //ASSERT(expect_len <= TRACE_BUF_SIZE);

        module->used_len = expect_len;
    }
    else
    {
        module->used_len += expect_len;
    }

    used_len = VSNPRINTF((CHAR *)module->trace_info + module->used_len,
                         TRACE_BUF_SIZE - module->used_len,
                         format,
                         arg);
    if (used_len < 0)
    {
        PRINTF("code error:%s %u\n", __FILE__, __LINE__);
        used_len = 0;
    }

    module->used_len += used_len;

    /* 预留一条TRACE的缓存 */
    if ((module->used_len > TRACE_BUF_SIZE - TRACE_SINGLE_SIZE) ||
        /* 预防伪定时器有assert,若当前有trace,1s后也能flush */
        (module->last_trace_time + TRACE_FLUSH_TIME <= cur_time))
    {
        trace_write(module);
    }

    /* 更新时间 */
    module->last_trace_time = cur_time;

EXIT:
    trace_mutex_unlock();
    return;
}

/*****************************************************************************/
/** 
* \author      yangkun
* \date        2016/11/21
* \brief       周期性刷新TRACE缓存
* \return      VOID
* \ingroup     sky_trace
* \remarks     其它信息
******************************************************************************/
VOID trace_flush()
{
    trace_mutex_lock();
    trace_write(&g_trace_hdl.usr);
    trace_mutex_unlock();
    return;
}

void mkdirs(char *muldir)
{
    int i, len;
    char str[512];
    strncpy(str, muldir, 512);
    len = strlen(str);
    for (i = 0; i < len; i++)
    {
        if (str[i] == '/')
        {
            str[i] = '\0';
            if (access(str, 0) != 0)
            {
                mkdir(str, 0777);
            }
            str[i] = '/';
        }
    }
    if (len > 0 && access(str, 0) != 0)
    {
        mkdir(str, 0777);
    }
    return;
}

VOID *trace_init(WORD32 default_level)
{
    WORD32 rtn = RTN_SUCCESS;

    rtn = trace_comm_init(&g_trace_hdl.usr, TRACE_SUBPATH_USR, default_level);
    if (RTN_SUCCESS != rtn)
    {
        PRINTF("FAILED: trace_init() \n");
        //ASSERT(0);
        goto EXIT;
    }

    PRINTF("SUCCESS: trace_init() \n");

    return &g_trace_hdl;

EXIT:
    return NULL;
}

/*****************************************************************************/
/** 
* \author      yangkun
* \date        2015/09/12
* \brief       从文件路径中提取文件名
* \param[in]   file_path    文件路径(可以为全路径,非全路径,文件名)
* \return      文件名字符串起始地址
* \ingroup     sky_debug
* \remarks     入参字符串要求有'\0'结尾
******************************************************************************/
CHAR *file_name_get(const CHAR *file_path)
{
    CHAR *file_name = NULL;

    if (NULL == file_path)
    {
        goto EXIT;
    }

    /* 从后开始搜索'/'位置 */
    file_name = STRRCHR((CHAR *)file_path, '/');
    if (NULL != file_name)
    {
        /* 越过'/' */
        file_name++;
    }
    else
    {
        /* 没有'/',则认为是文件名 */
        file_name = (CHAR *)file_path;
    }

EXIT:
    return file_name;
}

VOID trace_log(const CHAR *file_path, const CHAR *func_name, WORD32 line, WORD32 level, const CHAR *format, ...)
{
    TRACE_MODULE *module = NULL;
    CHAR *file_name = NULL;
    TRACE_NODE *trace_node = NULL;
    WORD32 file_trace_level = 0;

    /* 提取文件名 */
    file_name = file_name_get(file_path);
    if (NULL == file_name)
    {
        //ASSERT(0);
        goto EXIT;
    }

    module = &g_trace_hdl.usr;

    if (!module->trace_on)
    {
        goto EXIT;
    }

    /* 若.c文件已设置trace级别,则使用真实值,否则使用模块的默认trace级别 */
    trace_node = trace_node_find(file_name);
    if (NULL != trace_node)
    {
        file_trace_level = trace_node->trace_level;
    }
    else
    {
        file_trace_level = module->default_level;
    }

    if (file_trace_level < level)
    {
        goto EXIT;
    }

    va_list arg;
    va_start(arg, format);
    trace_assemble(module, func_name, line, level, format, arg);
    va_end(arg);

EXIT:
    return;
}
