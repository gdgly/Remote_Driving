/*****************************************************************************/
/**
* \file       macro_def.h
* \author     yangkun
* \date       2019/05/04
* \version    version 1.0
* \brief
* \note       Copyright (c) 2000-2020
* \remarks    macro definitions
******************************************************************************/
#ifndef _MACRO_DEF_H_
#define _MACRO_DEF_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/queue.h>
#include <sys/syscall.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <dirent.h>
#include <fcntl.h>
#include <semaphore.h>
#include <pthread.h>
#include <time.h>
#include <errno.h>
#include <assert.h>
#include <math.h>
#include <stdint.h>
#include <inttypes.h>
#include <setjmp.h>
#include <stdarg.h>
#include <getopt.h>
#include <linux/if_tun.h>
#include <linux/xfrm.h>

#include <netinet/in.h>
#include <netinet/in_systm.h>
#include <netinet/ip.h>
#include <netinet/tcp.h>
#include <netinet/udp.h>
#include <net/if.h>

#include <arpa/inet.h>

#define CODE_PART(x)                      1

#if CODE_PART("data type")
typedef void							  VOID;
typedef unsigned char					  BOOL;
typedef unsigned char                     BYTE;
typedef char                              SBYTE;
typedef char                              CHAR;

typedef unsigned short                    WORD16;
typedef signed short                      SWORD16;
typedef unsigned int                      WORD32;
typedef signed int                        SWORD32;
typedef int                               INT;       /* SWORD32 */
typedef unsigned long                     ULONG;
typedef long                              LONG;
typedef unsigned long long                WORD64;
typedef long long                         SWORD64;
typedef float                             FLOAT;
typedef double                            DOUBLE;
typedef signed   long int                 SWORDPTR;  /* */
typedef unsigned long int                 WORDPTR;   /* */

/* ��չ�������� */
typedef size_t                            SIZE_T;    /* unsigned long*/
typedef ssize_t                           SSIZE_T;   /* long int*/
typedef off_t                             OFF_T;
typedef pid_t                             PID_T;

typedef struct epoll_event                EPOLL_EVENT;

typedef sem_t                             SEM_T;
typedef pthread_mutex_t                   PTHREAD_MUTEX_T;
typedef pthread_t                         PTHREAD_T;

typedef time_t                            TIME_T;
typedef struct tm                         TM;
typedef struct timeval                    TIMEVAL;
typedef struct timespec                   TIMESPEC;

/* BOOL */
#undef  TRUE
#define TRUE                              1
#undef  FALSE
#define FALSE                             0
#endif

#if CODE_PART("operations")
#define SIZEOF(x)                     sizeof(x)
#define ABS(x)                        (((x) > 0) ? (x) : -(x))
#define MAX(A, B)                     (((A) > (B)) ? (A) : (B))
#define MIN(A, B)                     (((A) < (B)) ? (A) : (B))

#if CODE_PART("bit")
#define BYTE_BITS                     8
#define BYTE_MAX                      0xFF
#define WORD16_MAX					  0xFFFF
#define WORD32_MAX                    0xFFFFFFFF

#define BIT_SET(F, B)                 ((F) |= (B))
#define BIT_RESET(F, B)               ((F) &= ~(B))
#define BIT_TEST(F, B)                ((F) & (B))
#define BIT_MATCH(F, B)               (((F) & (B)) == (B))

#define BIT_AND(A, B)                 ((A) & (B))
#define BIT_OR(A, B)                  ((A) | (B))
#define BIT_XOR(A, B)                 ((A) ^ (B))
#define BIT_ANTI(A)                   (~(A))

#define BUILD_64(H, L)                (((WORD64)(H) << 32) | (L))
#define PARSE_64(C, A, B) \
do{ \
    *A = (C) >> 32; \
    *B = ((C) << 32) >> 32; \
}while(0)
#endif

#if CODE_PART("rand")
#define RAND_NOZERO()                 ((((WORD32)RAND()) % RAND_MAX) + 1)
#define RAND()                        rand()
#define SRAND(seed)                   srand(seed)
#endif

#if CODE_PART("struct offset")
#define VAROFFSET(type, member)               ((ULONG)(&((type *)0)->member))
#define GET_STRUCT_PTR(node, type, member)    ((type *)((BYTE*)(node) - VAROFFSET(type,member)))
#endif

#if CODE_PART("�ַ�����")
#define IS_SPACE(c)                   ((c) == ' ' || ((c) >= '\t' && (c) <= '\r'))
#define IS_SLASH(c)                   ((c) == '/')
#define IS_POINT(c)                   ((c) == '.')
#define IS_ASCII(c)                   (((c) & ~0x7f) == 0)
#define IS_UPPER(c)                   ((c) >= 'A' && (c) <= 'Z')
#define IS_LOWER(c)                   ((c) >= 'a' && (c) <= 'z')
#define IS_ALPHA(c)                   (COS_IS_UPPER(c) || COS_IS_LOWER(c))
#define IS_DIGIT(c)                   ((c) >= '0' && (c) <= '9')
#define IS_ALNUM(c)                   (COS_IS_ALPHA(c) || COS_IS_DIGIT(c))
#define IS_XDIGIT(c)                  (COS_IS_DIGIT(c) \
                                           || ((c) >= 'A' && (c) <= 'F') \
                                           || ((c) >= 'a' && (c) <= 'f'))
#define IS_PRINT(c)                   ((c) >= ' ' && (c) <= '~')
#define TO_UPPER(c)                   ((c) - 0x20 * (((c) >= 'a') && ((c) <= 'z')))
#define TO_LOWER(c)                   ((c) + 0x20 * (((c) >= 'A') && ((c) <= 'Z')))
#endif

#endif

#if CODE_PART("string operations")
#define STRLEN(dst)                   strlen(dst)
#define STRCPY(dst, src)              strcpy(dst, src)
#define STRNCPY(dst, src, n)          strncpy(dst, src, n)
#define STRCAT(dst, src)              strcat(dst, src)
#define STRNCAT(dst, src, n)          strncat(dst, src, n)
#define STRCMP(dst, src)              strcmp(dst, src)
#define STRNCMP(dst, src, n)          strncmp(dst, src, n)
#define STRCASECMP(dst, src)          strcasecmp(dst, src)
#define STRNCASECMP(dst, src, n)      strncasecmp(dst, src, n)
#define STRCHR(str, chr)              strchr(str, chr)
#define STRRCHR(str, chr)             strrchr(str, chr)
#define STRSTR(str1, str2)            strstr(str1, str2)
#define STRTOK(str1, str2)            strtok(str1, str2)
#define ATOI(str)                     atoi(str)
#define STRTOUL(str, endchar, base)   strtoul(str, endchar, base)
#define STRTOULL(str, endchar, base)  strtoull(str, endchar, base)

#endif

#if CODE_PART("memory operations")
#define MALLOC(buflen)                malloc(buflen)
#define FREE(buf)                     free(buf)
#define MEMSET(dst, var, n)           memset(dst, var, n)
#define MEMCMP(dst, src, n)           memcmp(dst, src, n)
#define MEMCPY(dst, src, n)           memcpy(dst, src, n)
#define MEMCHR(dst, src, n)           memchr(dst,src,n)
#define MEMMOVE(dst, src, n)          memmove(dst,src,n)
#define MEMCPY64(key, arg1, arg2) \
do{ \
    if(SIZEOF(key) >= SIZEOF(arg1) + SIZEOF(arg2)) \
    { \
        MEMCPY(&key, &arg1, SIZEOF(arg1)); \
        MEMCPY((CHAR*)&key + SIZEOF(arg1), &arg2, SIZEOF(arg2)); \
    } \
}while(0)

#endif

#if CODE_PART("print")
#define PRINTF                        printf
#define SPRINTF                       sprintf
#define SNPRINTF                      snprintf
#define VSNPRINTF                     vsnprintf
#define PUTCHAR                       putchar
#define SSCANF                        sscanf
#endif

#if CODE_PART("file")
/* std in out */
#define FD_INVALID                    -1
#define FD_STDIN                      STDIN_FILENO
#define FD_STDOUT                     STDOUT_FILENO
#define FD_STDERR                     STDERR_FILENO

#define REMOVE(path)                  remove(path)
#define RENAME(old, new)              rename(old, new)

/* system */
#define OPENDIR(path)                 opendir(path)
#define MKDIR(path, mode)             mkdir(path, mode)

/* file operation */
#define OPEN(path, flags, ...)        open(path, flags, __VA_ARGS__)
#define CLOSE(fd)                     close(fd)
#define SEND(fd, buff, size, flags)   send(fd, buff, size, flags)
#define RECV(fd, buff, size, flags)   recv(fd, buff, size, flags)
#define READ(fd, buff, size)          read(fd, buff, size)
#define WRITE(fd, buff, size)         write(fd, buff, size)
#define FCNTL(fd, cmd, ...)           fcntl(fd, cmd, __VA_ARGS__)
#define LSEEK(fd, offset, pos)        lseek(fd, offset, pos)
#define DUP(oldfd)                    dup(oldfd)
#define DUP2(oldfd, targetfd)         dup2(oldfd, targetfd)
#define ISATTY(fd)                    isatty(fd)
#define TTYNAME(fd)                   ttyname(fd)

#define FOPEN(path, mode)             fopen(path, mode)
#define FCLOSE(fp)                    fclose(fp)
#define FGETS(str, num, fp)           fgets(str, num, fp)
#define FPUTS(str, fp)                fputs(str, fp)
#define FSEEK(fp, offset, pos)        fseek(fp, offset, pos)
#define FTELL(fp)                     ftell(fp)
#define FPRINTF                       fprintf
#define FFLUSH(fp)                    fflush(fp)
#endif


#if CODE_PART("unistd.h")
#define GETOPT(argc, argv, optstr)    getopt(argc, argv, optstr)
#define GETPID                        getpid()
#define READLINK(path, buff, size)    readlink(path, buff, size)
#define GETCWD(buff, size)            getcwd(buff, size)
#define SLEEP(sec)                    sleep(sec)
#define USLEEP(usec)                  usleep(usec)
#endif

#define SYSTEM(cmd_string)            system(cmd_string)

#if CODE_PART("execinfo.h")

#define BACKTRACE(buff, size)                   backtrace(buff, size)

#define BACKTRACE_SYMBOLS(buff, size)           backtrace_symbols(buff, size)

#define BACKTRACE_SYMBOLS_FD(buff, size, fd)    backtrace_symbols_fd(buff, size, fd)

#endif

#if CODE_PART("signal.h")
#define SIGNAL(sig, func)             signal(sig, func)
#define KILL(pid, sig)                kill(pid, sig)
#endif


#if CODE_PART("threads operations")
#define PTHREAD_MUTEX_INIT(mutex, attr)         pthread_mutex_init(mutex, attr)
#define PTHREAD_MUTEX_DESTROY(mutex)            pthread_mutex_destroy(mutex)
#define PTHREAD_MUTEX_LOCK(mutex)               pthread_mutex_lock(mutex)
#define PTHREAD_MUTEX_UNLOCK(mutex)             pthread_mutex_unlock(mutex)

#define PTHREAD_CREATE(thd, attr, start, arg)   pthread_create(thd, attr, start, arg)
#define PTHREAD_DETACH(tid)                     pthread_detach(tid)
#define PTHREAD_SELF                            pthread_self()
#endif

#if CODE_PART("time")
#define GETTIMEOFDAY(tv, tz)          gettimeofday(tv, tz)
#define LOCALTIME_R(time, tm)         localtime_r(time, tm)
#define CLOCK_GETTIME(clock, tp)      clock_gettime(clock, tp)
#endif

#if CODE_PART("errno")
#define ERRNO                         errno
#define STRERROR                      strerror(errno)
#endif

#if CODE_PART("return values")
enum {
    RTN_SUCCESS = 0,
    RTN_ERROR = -1,
    RTN_CONNECT_ERROR = -2,
    RTN_FDOPEN_ERROR = -3,
    RTN_CMD_ERROR = -4,
    RTN_OPEN_ERROR = -5,
    RTN_SOCKET_ERROR = -6,
    RTN_SEND_ERROR = -7,
    RTN_RECV_ERROR = -8,
    RTN_GET_RESP_ERROR = -9,
    RTN_COPY_ERROR = -100,
    RTN_ERROR_LOGIN =250,
};
#endif

#define SIGASSERT    -1
#define ABORT()      abort()
void proc_assert_entry(const CHAR *file, const CHAR *func, WORD32 line,
                           const CHAR *exp_str, SWORD32 sign);

#define ASSERT(x)                     ((x) ? (void)0 : proc_assert_entry(__FILE__,    \
                                           __FUNCTION__, __LINE__, #x, SIGASSERT))

#ifdef __cplusplus
}
#endif
#endif


