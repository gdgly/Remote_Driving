/**
* \file       tcp.c
* \author     yangkun
* \date       2019/05/04
* \version    Version 1.0
* \brief
* \note       Copyright (c) 2000-2020
* \remarks    tcp functions
******************************************************************************/

#include "tcp.h"
#include "../COMM/sky_trace.h"

#define TCP_KEEP_ALIVE_ENABLE       1   /* 使能tcp keepalive */
#define TCP_KEEP_ALIVE_IDLE         10  /* keepalive idle 时长 */
#define TCP_KEEP_ALIVE_INTVL        5   /* keepalive 包发送间隔 */
#define TCP_KEEP_ALIVE_CNT          6   /* keepalive 包发送次数 */

#define TCP_USER_TIMEOUT_MS         10000    /* (户超时时长，单位毫秒) */

#define SOCKET_RECV_TIMEOUT         60  /* tcp 收包函数超时时长 */

typedef struct tTcpParam
{
	WORD32 dwIpaddr;
	WORD16 wPort;
	TCP_PARSE_MSG_CB pCallback;
}T_TCPCLIENT_PARAM;

T_TCPCLIENT_PARAM tTcpClientParam;


/*****************************************************************************/
/**
* \author      yangkun
* \date        2019/05/04
* \brief       tcp_keep_alive_sockopt
* \param[in]   socket_fd
* \return      SWORD32
* \remarks     设置tcp keepalive的socket 选项
******************************************************************************/
SWORD32 tcp_keep_alive_sockopt(SWORD32 socket_fd)
{
    WORD32 keepAlive = TCP_KEEP_ALIVE_ENABLE;
    WORD32 keepIdle = TCP_KEEP_ALIVE_IDLE;
    WORD32 keepInterval = TCP_KEEP_ALIVE_INTVL;
    WORD32 keepCount = TCP_KEEP_ALIVE_CNT;
    WORD32 tcpUserTimeout = TCP_USER_TIMEOUT_MS;
    struct timeval revTimeout = {SOCKET_RECV_TIMEOUT,0};

    if(-1 == setsockopt(socket_fd, SOL_SOCKET, SO_KEEPALIVE, (void *)&keepAlive, SIZEOF(keepAlive)))
    {
        return -1;
    }

    if(-1 == setsockopt(socket_fd, SOL_TCP, TCP_KEEPIDLE, (void *)&keepIdle, SIZEOF(keepIdle)))
    {
        return -1;
    }

    if(-1 == setsockopt(socket_fd, SOL_TCP, TCP_KEEPINTVL, (void *)&keepInterval, SIZEOF(keepInterval)))
    {
        return -1;
    }

    if(-1 == setsockopt(socket_fd, SOL_TCP, TCP_KEEPCNT, (void *)&keepCount, SIZEOF(keepCount)))
    {
        return -1;
    }

    if(-1 == setsockopt(socket_fd, IPPROTO_TCP, TCP_USER_TIMEOUT, (void *)&tcpUserTimeout, SIZEOF(tcpUserTimeout)))
    {
        return -1;
    }

    if(-1 == setsockopt(socket_fd, SOL_SOCKET, SO_RCVTIMEO, (void *)&revTimeout, SIZEOF(struct timeval)))
    {
        return -1;
    }

    return 0;
}

/*****************************************************************************/
/**
* \author      yangkun
* \date        2019/05/04
* \brief       tcp_set_recv_timeout
* \param[in]   socket_fd
* \param[in]   timeout
* \return      SWORD32
* \remarks     设置tcp recv 超时时间
******************************************************************************/
SWORD32 tcp_set_recv_timeout(SWORD32 swSock,WORD32 timeout)
{
	struct timeval revTimeout = {timeout,0};

	if (swSock < 0)
	{
		return -1;
	}

	if(-1 == setsockopt(swSock, SOL_SOCKET, SO_RCVTIMEO, (void *)&revTimeout, SIZEOF(struct timeval)))
    {
        return -1;
    }
}

/*****************************************************************************/
/**
* \author      yangkun
* \date        2019/05/04
* \brief       tcp_server_init
* \param[in]   wPort
* \param[in]   dwServerIp
* \return      SWORD32 -1-->fail
* \remarks     tcp 服务端初始化函数
******************************************************************************/
SWORD32 tcp_server_init(WORD32 dwServerIp, WORD16 wPort)
{
    SWORD32 ret = 0;
    SWORD32 swConnected = 0;
    SWORD32 swSock = -1;
    SWORD32 swSelect = 0;
    struct sockaddr_in tcpServerAddr = {0};
    fd_set fdWriteSet;
    fd_set fdReadSet;
    struct timeval connectTimeout = {2, 0};
    SWORD32 swRetryTimes = 0;
    SWORD32 err = 0;

    /* create a socket */
    swSock = socket(AF_INET, SOCK_STREAM, 0);
    if(swSock < 0)
    {
        return swSock;
    }

    /* connect to server */
    MEMSET(&tcpServerAddr, 0, SIZEOF(struct sockaddr_in));
    tcpServerAddr.sin_family = AF_INET;
    tcpServerAddr.sin_port = htons(wPort);
    tcpServerAddr.sin_addr.s_addr = htonl(dwServerIp);

    if (bind(swSock, (struct sockaddr *)&tcpServerAddr, sizeof(struct sockaddr_in)) == -1)
    {
    	SKY_TRACE_ERROR("socket bind error!");
        goto fail;
    }

    if (listen(swSock, MAX_LISTEN_COUNT) == -1)
    {
    	SKY_TRACE_ERROR("socket listen error!");
        goto fail;
    }

    return swSock;

fail:
    if (swSock > -1) {
		SKY_TRACE_ERROR("socket create failed!");
        close(swSock);
    }

    return -1;
}

void *tcp_server_proc(void * pTcpServerInfo)
{
	SWORD32 swClientSock = -1;
    PTHREAD_T pServerCtrlThread;
    int policy, priority;
    pthread_getschedparam(pthread_self(), &policy, (struct sched_param *)&priority);
    SKY_TRACE_INFO("StartServer policy=%d, priority=%d\n", policy, priority);

    prctl(PR_SET_NAME, "StartServer");
	T_TCPSERVER_PARAM tTcpServerParam = {0};
	TIMESPEC timereq = {0};
	TIMESPEC timeret = {0};
	timereq.tv_nsec = 50000000; //50ms
	for(;;)
	{
		struct sockaddr_in client_addr;
		socklen_t length = sizeof(client_addr);

		if (((T_TCPSERVER_INFO *)pTcpServerInfo)->dwClientNum >= MAX_SOCKET_CONN)
		{
			SLEEP(180);
			SKY_TRACE_ERROR("max connections!\n");
			continue;
		}

		swClientSock = accept(((T_TCPSERVER_INFO *)pTcpServerInfo)->swSock, (struct sockaddr*)&client_addr,&length);
		if(swClientSock < 0)
		{
			SKY_TRACE_ERROR("acception error!\n");
			continue;
		}

		((T_TCPSERVER_INFO *)pTcpServerInfo)->swClientSock[((T_TCPSERVER_INFO *)pTcpServerInfo)->dwClientNum] = swClientSock;

		if (NULL != ((T_TCPSERVER_INFO *)pTcpServerInfo)->pThreadFun)
		{
			tTcpServerParam.swSock = ((T_TCPSERVER_INFO *)pTcpServerInfo)->swSock;
			tTcpServerParam.wPort = ((T_TCPSERVER_INFO *)pTcpServerInfo)->wPort;
			tTcpServerParam.pCallback = ((T_TCPSERVER_INFO *)pTcpServerInfo)->pThreadFun;
			tTcpServerParam.swClientSock = swClientSock;

			PTHREAD_CREATE(&tTcpServerParam.pClientThread, 0, tTcpServerParam.pCallback, (void *)&tTcpServerParam);
			((T_TCPSERVER_INFO *)pTcpServerInfo)->arryClientThread[((T_TCPSERVER_INFO *)pTcpServerInfo)->dwClientNum] = tTcpServerParam.pClientThread;
		}
		((T_TCPSERVER_INFO *)pTcpServerInfo)->dwClientNum++;

		nanosleep(&timereq,&timeret);
	}
}

/*****************************************************************************/
/**
* \author      yangkun
* \date        2019/05/04
* \brief       tcp_client_init
* \param[in]   wPort
* \param[in]   dwRemoteIp
* \return      SWORD32 -1-->fail
* \remarks     tcp 客户端初始化函数
******************************************************************************/
SWORD32 tcp_client_init(WORD16 wPort,WORD32 dwRemoteIp)
{
    SWORD32 ret = 0;
    SWORD32 swConnected = 0;
    SWORD32 swSock = -1;
    SWORD32 swSelect = 0;
    struct sockaddr_in tcpServerAddr = {0};
    fd_set fdWriteSet;
    fd_set fdReadSet;
    struct timeval connectTimeout = {2, 0};
    SWORD32 swRetryTimes = 0;
    SWORD32 err = 0;

    /* create a socket */
    swSock = socket(AF_INET, SOCK_STREAM, 0);
    if(swSock < 0)
    {
        return swSock;
    }

    /* connect to server */
    MEMSET(&tcpServerAddr, 0, SIZEOF(struct sockaddr_in));
    tcpServerAddr.sin_family = AF_INET;
    tcpServerAddr.sin_port = htons(wPort);
    tcpServerAddr.sin_addr.s_addr = htonl(dwRemoteIp);

    swConnected = connect(swSock, (struct sockaddr *)&tcpServerAddr, SIZEOF(struct sockaddr_in));
    if(0 == swConnected)
    {
    	SKY_TRACE_ERROR("socket bind error!");
        ret = 0;
    }

    if(0 != ret)
    {
        CLOSE(swSock);
        swSock = -1;
    }
    else
    {
        /* set keep alive on socket */
        if(tcp_keep_alive_sockopt(swSock) < 0)
        {
            CLOSE(swSock);
            swSock = -1;
        }
    }

    return swSock;
}

/*****************************************************************************/
/**
* \author      yangkun
* \date        2019/05/04
* \brief       tcp_client_proc
* \param[in]   swSock      socket 句柄
* \param[in]   parse_cb    tcp收包回调函数
* \return      -: fail 0: success
* \remarks     tcp 客户端收包函数
******************************************************************************/
SWORD32 tcp_client_proc(SWORD32 swSock, TCP_PARSE_MSG_CB tcpcbFunc)
{
	CHAR *cBuf = NULL;
	WORD32 dwMsgID = 0;
	WORD32 dwSwapTmp = 0;
	WORD64 ullVehicleID = 0;
    SWORD32 swRecvDataLen = 0;

    cBuf = (CHAR *)MALLOC(TCP_TRANS_BUF_LEN);
    if (NULL == cBuf)
    {
        goto Exit;
    }

    while(TRUE)
    {
        MEMSET(cBuf, 0, TCP_TRANS_BUF_LEN);
        swRecvDataLen = RECV(swSock, cBuf, TCP_TRANS_BUF_LEN,0);

        if (swRecvDataLen == 0)
        {
            goto Exit;
        }

        if(swRecvDataLen < 0)
        {
            if ((ERRNO == EINTR) || (ERRNO == EWOULDBLOCK) || (ERRNO == EAGAIN))
            {
                continue;
            }
            else
            {
                goto Exit;
            }
        }

		if (*((WORD32*)cBuf) != LOOPBACK_ADDR)
		{
			continue;
		}

		dwSwapTmp = ntohl(*((WORD32*)&cBuf[4]));
		ullVehicleID = (WORD64)dwSwapTmp<<32;
		dwSwapTmp = ntohl(*((WORD32*)&cBuf[8]));
		ullVehicleID += dwSwapTmp;
		dwMsgID = ntohl(*((WORD32*)&cBuf[12])); //目前为0，不区分消息类型
		
        //if (tcpcbFunc(ullVehicleID,0,(void *)&cBuf[16], swRecvDataLen-16) < 0)
        {
            goto Exit;
        }
    }


Exit:
    if (NULL != cBuf)
    {
        FREE(cBuf);
    }

    if(-1 != swSock)
    {
        CLOSE(swSock);
        swSock = -1;
    }

    return -1;
}

void tcp_client_thread_fun(void *pParam)
{
	SWORD32 swSock = 0;
	if (NULL == pParam)
	{
		return;
	}
	while (1)
	{
		swSock = tcp_client_init(((T_TCPCLIENT_PARAM*)pParam)->wPort, ((T_TCPCLIENT_PARAM*)pParam)->dwIpaddr);
		if (swSock < 0)
		{
			SLEEP(1);
			continue;
		}

		tcp_client_proc(swSock, ((T_TCPCLIENT_PARAM*)pParam)->pCallback);
	}
}

SWORD32 tcp_client_process(WORD32 dwRemoteIp, WORD16 wPort, TCP_PARSE_MSG_CB tcpcbFunc)
{
	PTHREAD_T pThread;

	tTcpClientParam.dwIpaddr = dwRemoteIp;
	tTcpClientParam.wPort = wPort;
	tTcpClientParam.pCallback = tcpcbFunc;

	//PTHREAD_CREATE(&pThread, 0, (void *)tcp_client_thread_fun, &tTcpClientParam);

	return RTN_SUCCESS;
}

