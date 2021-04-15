/**
* \file       tcp.h
* \author     yangkun
* \date       2019/05/04
* \version    Version 1.0
* \brief
* \note       Copyright (c) 2000-2020
* \remarks    tcp functions
******************************************************************************/


#ifndef _TCP_H
#define _TCP_H

#ifdef __cplusplus
extern "C"
{
#endif

#include "../COMM/macro_def.h"
#include <arpa/inet.h>

#define LOOPBACK_ADDR			  0x7F000001
#define TCP_TRANS_BUF_LEN         10240
#define MAX_LISTEN_COUNT		  1000

#define MAX_SOCKET_CONN             1024 /* 最大tcp连接数 */


typedef void* (*TCP_PARSE_MSG_CB)(void *);
typedef void* (*TCP_CLIENT_THREAD_FUN)(void *);

typedef struct tTcpServerInfo
{
	SWORD32 swSock;
	WORD16 wPort;
	WORD32 dwClientNum;
	TCP_CLIENT_THREAD_FUN pThreadFun;
	SWORD32 swClientSock[MAX_SOCKET_CONN];
	PTHREAD_T arryClientThread[MAX_SOCKET_CONN];
}T_TCPSERVER_INFO;

typedef struct tTcpServerParam
{
	SWORD32 swSock;
	WORD16 wPort;
	TCP_CLIENT_THREAD_FUN pCallback;
	SWORD32 swClientSock;
	PTHREAD_T pClientThread;
}T_TCPSERVER_PARAM;


SWORD32 tcp_client_process(WORD32 dwRemoteIp, WORD16 wPort, TCP_PARSE_MSG_CB tcpcbFunc);
SWORD32 tcp_server_init(WORD32 dwServerIp, WORD16 wPort);
SWORD32 tcp_set_recv_timeout(SWORD32 swSock,WORD32 timeout);

void * tcp_server_proc(void * pTcpServerInfo);



#ifdef __cplusplus
}
#endif
#endif
