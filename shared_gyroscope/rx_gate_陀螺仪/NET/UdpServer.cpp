
#include <sys/types.h>
#include <unistd.h>

#include <stdio.h>

#include <string.h>
#include <ifaddrs.h>
#include <linux/ioctl.h>
#include <netdb.h> // gethostbyname, gethostbyname2, gethostbyname_r, gethostbyname_r2

#include "UdpServer.h"
#include "../COMM/sky_trace.h"

struct sockaddr_in m_src_addr;
socklen_t m_src_addr_len;

CUdpServer::CUdpServer()
{
    m_udp_sock_server = 0;
    m_last_error = 0;
    m_status = 0;
    m_connected_status = 0;
    m_thread_read_avtived = 0;
    m_connect_interval = 5;
    m_pReadCallback = NULL;
}

CUdpServer::~CUdpServer()
{
    close_udp();
}

sint32 CUdpServer::init_udp_server(uint32 port)
{
    m_port = port;
    m_status = 1;

    return start_connect_thread();
}

sint32 CUdpServer::init_udp_server(void)
{
    sint32 res;

    if (m_connected_status)
        close_udp(); //函数中m_status清零了
    m_status = 1;
    m_connected_status = 0;

    if ((m_udp_sock_server = socket(PF_INET, SOCK_DGRAM, 0)) < 0)
        SKY_TRACE_ERROR("create socket error %m\n");

    struct sockaddr_in socket_server_addr;
    memset(&socket_server_addr, 0, sizeof(socket_server_addr));
    socket_server_addr.sin_family = AF_INET;
    socket_server_addr.sin_port = htons(m_port);
    socket_server_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    SKY_TRACE_ERROR("[%s], m_port:%d\n", __FUNCTION__, m_port);

    if (bind(m_udp_sock_server, (struct sockaddr *)&socket_server_addr, sizeof(socket_server_addr)) < 0)
        SKY_TRACE_ERROR("bind error, %m\n");

    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_setscope(&attr, PTHREAD_SCOPE_SYSTEM);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);

    res = pthread_create(&tid_recv, &attr, udp_read_thread, (void *)this);
    pthread_attr_destroy(&attr);
    if (res != 0)
    {
        SKY_TRACE_ERROR("create udp_read_thread error");
        m_last_error = -3;
        close(m_udp_sock_server);
        return -5;
    }

    m_thread_read_avtived = 1;

    m_connected_status = 1;
    m_last_error = 0;

    return 0;
}

sint32 CUdpServer::start_connect_thread(void)
{
    uint32 res;

    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_setscope(&attr, PTHREAD_SCOPE_SYSTEM);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);

    res = pthread_create(&tid_recv, &attr, udp_connect_thread, (void *)this);
    pthread_attr_destroy(&attr);
    if (res != 0)
    {
        m_last_error = -1;
        return -1;
    }

    return 0;
}

void CUdpServer::close_udp(void)
{
    if (m_connected_status)
    {
        m_thread_read_avtived = 0;
        close(m_udp_sock_server);
        m_udp_sock_server = 0;
        m_connected_status = 0;
    }
    m_status = 0;
}

void *CUdpServer::udp_read_thread(void *arg)
{
    int policy, priority;
    pthread_getschedparam(pthread_self(), &policy, (struct sched_param *)&priority);
    SKY_TRACE_INFO("udp_read_thread policy=%d, priority=%d\n", policy, priority);

    prctl(PR_SET_NAME, "udp_read_thread");

    SKY_TRACE_INFO("------------Begin UdpServer Read Thread--------------\n");
    PRINTF("------------Begin UdpServer Read Thread--------------\n");
    CUdpServer *p_udp = (CUdpServer *)arg;
    sint32 res;
    struct sockaddr_in src_addr;
    socklen_t addr_len;

    while (1)
    {
        addr_len = sizeof(src_addr);
        memset(p_udp->rev_buf, 0, sizeof(p_udp->rev_buf));

        res = recvfrom(p_udp->m_udp_sock_server, p_udp->rev_buf, sizeof(p_udp->rev_buf), 0, (struct sockaddr *)&src_addr, &addr_len);
        if (res <= 0)
        {
            p_udp->m_last_error = -100 + res;
            p_udp->start_connect_thread(); //自动重连
            SKY_TRACE_INFO("------------Quit UdpServer Read Thread 1--------------\n");
            break;
        }

        memcpy(&(m_src_addr), &src_addr, sizeof(src_addr));
        m_src_addr_len = addr_len;

        SKY_TRACE_INFO("Received data from the client  %s:%d.\n\n", inet_ntoa(src_addr.sin_addr), ntohs(src_addr.sin_port));
        PRINTF("Received data from the client  %s:%d.\n", inet_ntoa(src_addr.sin_addr), ntohs(src_addr.sin_port));
        SKY_TRACE_INFO("p_udp->rev_buf: %s, time:%ld\n", p_udp->rev_buf, time(NULL));
        PRINTF("p_udp->rev_buf: %s, time:%ld\n", p_udp->rev_buf, time(NULL));

        SKY_TRACE_INFO("------------UdpServer is running--------------\n");
        if (p_udp->m_pReadCallback != NULL)
            p_udp->m_pReadCallback(p_udp->rev_buf, res, (struct sockaddr_in *)&src_addr, addr_len);

        if (p_udp->m_thread_read_avtived == 0)
        {
            SKY_TRACE_INFO("------------Quit UdpServer Read Thread 2--------------\n");
            break;
        }
        usleep(5 * 1000);
    }
}

void *CUdpServer::udp_connect_thread(void *arg)
{
    int policy, priority;
    pthread_getschedparam(pthread_self(), &policy, (struct sched_param *)&priority);
    SKY_TRACE_INFO("udp_connect_thread policy=%d, priority=%d\n", policy, priority);

    prctl(PR_SET_NAME, "udp_connect_thread");

    SKY_TRACE_DEBUG("--start Udp Server ---\n");
    CUdpServer *p_udp = (CUdpServer *)arg;

    while (1)
    {
        if (p_udp->init_udp_server() == 0) //UDP连接成功
        {
            SKY_TRACE_INFO("---Quit Udp Server ---\n");
            break;
        }

        SKY_TRACE_INFO("-- Udp Server is running.---\n");
        if (p_udp->m_status == 0)
            break;                        //关闭UDP
        sleep(p_udp->m_connect_interval); //休眠时间单位是秒
    }
}

void CUdpServer::set_read_callback_func(const pFunc_ReadCallback pReadFunc)
{
    m_pReadCallback = pReadFunc;
}

sint32 CUdpServer::get_udp_status()
{
    return m_connected_status;
}
sint32 CUdpServer::get_last_error()
{
    return m_last_error;
}

sint32 CUdpServer::send_udp_data(sint8 *str, uint32 len, struct sockaddr_in *addr, uint32 addr_len)
{
    int ret;

    if (m_connected_status == 0)
    {
        SKY_TRACE_ERROR("[%s]: m_connected_status: %d \n", __FUNCTION__, m_connected_status);
        return -1;
    }

    SKY_TRACE_INFO("[%s]: %s \n", __FUNCTION__, str);
    ret = sendto(m_udp_sock_server, str, len, 0, (struct sockaddr *)addr, addr_len);
    // SKY_TRACE_INFO("%s,line: %d, %m\n", __FUNCTION__, __LINE__);

    if (ret != len)
    {
        SKY_TRACE_ERROR("[%s]--- CUdpServer::send_udp_data error!!!!! ---\n", __FUNCTION__);
        m_last_error = -200;
        close_udp();

        start_connect_thread(); //自动重连
        return -2;
    }

    return ret;
}

sint32 CUdpServer::send_udp_data(sint8 *str, uint32 len)
{
    int ret;

    if (m_connected_status == 0)
        return -1;

    SKY_TRACE_INFO("CUdpServer::[%s]: %s, time:%ld\n", __FUNCTION__, str, time(NULL));
    PRINTF("CUdpServer::[%s]: %s, time:%ld\n", __FUNCTION__, str, time(NULL));
    ret = sendto(m_udp_sock_server, str, len, 0, (struct sockaddr *)&m_src_addr, m_src_addr_len);
    // SKY_TRACE_DEBUG("CUdpServer::[%s], m_src_addr: %s: %d \n", __FUNCTION__, inet_ntoa(m_src_addr.sin_addr), ntohs(m_src_addr.sin_port));
    // SKY_TRACE_INFO("CUdpServer::%s, line: %d, %m\n", __FUNCTION__, __LINE__);

    if (ret != len)
    {
        SKY_TRACE_ERROR("[%s]--- CUdpServer::send_udp_data error!!!!! ---\n", __FUNCTION__);
        m_last_error = -200;
        close_udp();

        start_connect_thread(); //自动重连
        return -2;
    }

    return ret;
}
