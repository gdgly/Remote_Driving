
#include <sys/types.h>
#include <unistd.h>

#include <stdio.h>

#include <string.h>
#include <ifaddrs.h>
#include <linux/ioctl.h>
#include <netdb.h>      // gethostbyname, gethostbyname2, gethostbyname_r, gethostbyname_r2


#include  "UdpClient.h"
#include "../COMM/sky_trace.h"

CUdpClient::CUdpClient()
{
    m_udp_sock_client = 0;
    m_last_error = 0;
    m_status = 0;
    m_connected_status = 0;
    m_thread_read_avtived = 0;
    m_connect_interval = 5;
    m_pReadCallback = NULL;
}

CUdpClient::~CUdpClient()
{
    close_udp();
}

sint32 CUdpClient::init_udp_client_hostname(sint8 * hostname, uint32 port)
{
    int len;

    memset(m_hostname,0,sizeof(m_hostname));        //域名清零
    len = strlen(hostname);
    if(len > 100)len = 100;
    memcpy(m_hostname,hostname,len);

    m_port = port;

    m_status = 1;

    return start_connect_thread();
}

sint8 CUdpClient::get_local_ip()
{
    struct ifaddrs *ifaddr, *ifa;
    int family, s;

    if (getifaddrs(&ifaddr) == -1)
    {
        return -1;
    }
    int ret_value = -2;
    /* Walk through linked list, maintaining head pointer so we
    *              can free list later */
    for (ifa = ifaddr; ifa != NULL; ifa = ifa->ifa_next)
    {
        if (ifa->ifa_addr == NULL)
            continue;

        family = ifa->ifa_addr->sa_family;

        if (strcmp(ifa->ifa_name, "eth0") == 0) //检测eth0的类型
        {
            if (family == AF_INET)
            {
                ret_value = 0;
                break;
            }
        }
    }
    freeifaddrs(ifaddr);

    return ret_value;
}

sint32 CUdpClient::init_udp_client(sint8 * ip, uint32 port)
{
    int len;

    memset(m_hostname,0,sizeof(m_hostname));        //域名清零
    memset(m_ip_address,0,sizeof(m_ip_address));    //IP清零
    len = strlen(ip);
    if(len > 30)len = 30;
    memcpy(m_ip_address,ip,len);
    m_port = port;

    m_status = 1;

    return start_connect_thread();
}

sint32 CUdpClient::init_udp_client(void)
{
   sint32 res;

    if(m_connected_status)
        close_udp();    //函数中m_status清零了
    m_status = 1;
    m_connected_status = 0;

    if(get_local_ip() != 0)
    {
        SKY_TRACE_ERROR("RJ45 failure\n");
        return -1;
    }

    //检查是否是域名模式，如果是则先进行域名转换
    if(strlen(m_hostname) > 0)
    {
        SKY_TRACE_INFO("hostname to ip\n");

        struct hostent *h;
        if ((h = gethostbyname(m_hostname)) == NULL)
        { // get the host info error,域名错误或网络有问题
            return -2;
        }
        const char *hostip = inet_ntoa(*((struct in_addr*)h->h_addr));
        SKY_TRACE_INFO("ip is %s\n", hostip);                   //for test
        memset(m_ip_address,0,sizeof(m_ip_address));    //IP清零
        memcpy(m_ip_address,hostip,strlen(hostip));
    }

    m_udp_sock_client = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if(m_udp_sock_client == -1)
    {
        m_last_error = -1;
        return -3;
    }

    memset(&socket_server_addr, 0, sizeof(socket_server_addr));
	socket_server_addr.sin_family = AF_INET;
	socket_server_addr.sin_port = htons(m_port);  /* host to net, short */
    socket_server_addr.sin_addr.s_addr = inet_addr(m_ip_address);
    //socket_server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
 	if (0 == inet_aton(m_ip_address, &socket_server_addr.sin_addr)) 
 	{                                              //IP address error
        m_last_error = -2;
        close(m_udp_sock_client);
		return -4;
	}

/*  //这次UDP客户端只发数据，不收，因此不起接收数据的线程
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_setscope(&attr, PTHREAD_SCOPE_SYSTEM);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);

    res = pthread_create(&tid_recv, &attr, udp_read_thread, (void *) this);
    pthread_attr_destroy(&attr);
    if(res != 0)
    {
        m_last_error = -3;
        close(m_udp_sock_client);
		return -5;
    }

    m_thread_read_avtived = 1;
 */
    m_connected_status = 1;
    m_last_error = 0;

    return 0;
}

sint32 CUdpClient::start_connect_thread(void)
{
    uint32 res;

    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_setscope(&attr, PTHREAD_SCOPE_SYSTEM);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);

    res = pthread_create(&tid_recv, &attr, udp_connect_thread, (void *) this);
    pthread_attr_destroy(&attr);
    if(res != 0)
    {
        m_last_error = -1;
		return -1;
    }

    return 0;
}

void CUdpClient::close_udp(void)
{
    if(m_connected_status)
    {
        m_thread_read_avtived = 0;
        close(m_udp_sock_client);
        m_udp_sock_client = 0;
        m_connected_status = 0;
    }
    m_status = 0;
}

void * CUdpClient::udp_read_thread(void * arg)
{
    //SKY_TRACE_INFO("------------start Udp Read Thread--------------\n");
    CUdpClient *p_udp = (CUdpClient*) arg;

    struct sockaddr src_addr;
	uint32 addr_len;
    sint32 res;

    while(1)
    {
        res = recvfrom(p_udp->m_udp_sock_client, p_udp->rev_buf, BUFF_LEN, 0, (struct sockaddr*)&src_addr, &addr_len);
        if(res <= 0)
        {
            //p_udp->close_udp();
            p_udp->m_last_error = -100 + res;
            p_udp->start_connect_thread(); //自动重连
            //SKY_TRACE_INFO("------------Quit Udp Read Thread 1--------------\n");
            break;
        }
        
        if(p_udp->m_pReadCallback != NULL)
            p_udp->m_pReadCallback(p_udp->rev_buf,res,(struct sockaddr*)&src_addr,addr_len);

        if(p_udp->m_thread_read_avtived == 0)
        {
            //SKY_TRACE_INFO("------------Quit Udp Read Thread 2--------------\n");
            break;
        }
    }
}

void * CUdpClient::udp_connect_thread(void * arg)
{
    int policy, priority;
    pthread_getschedparam(pthread_self(), &policy, (struct sched_param *)&priority);
    SKY_TRACE_INFO("udp_connect_thread policy=%d, priority=%d\n", policy, priority);

    prctl(PR_SET_NAME, "udp_connect_thread");

    //SKY_TRACE_INFO("------------start Udp connected Thread--------------\n");
    CUdpClient *p_udp = (CUdpClient*) arg;

    while(1)
    {
        if(p_udp->init_udp_client() == 0)   //UDP连接成功
        {
            //SKY_TRACE_INFO("------------Quit Udp connected Thread--------------\n");
            break;
        }

        if(p_udp->m_status == 0)break;           //关闭UDP
        sleep(p_udp->m_connect_interval);        //休眠时间单位是秒
    }
}

void CUdpClient::set_read_callback_func(const pFunc_ReadCallback pReadFunc)
{
    m_pReadCallback = pReadFunc;
}

sint32 CUdpClient::get_udp_status()
{
    return m_connected_status;
}
sint32 CUdpClient::get_last_error()
{
    return m_last_error;
}

// sint32 CUdpClient::send_udp_data(sint8* str, uint32 len)
sint32 CUdpClient::send_udp_data(void* str, uint32 len)
{
    if(m_connected_status == 0)
        return -1;
    
    if(sendto(m_udp_sock_client, str, len, 0, (struct sockaddr*)&socket_server_addr, sizeof(socket_server_addr)) != len)
    {
        m_last_error = -200;
        close_udp();

        start_connect_thread(); //自动重连

        return -2;
    }
    return 0;
}

