
#include <sys/types.h>
#include <unistd.h>

#include <stdio.h>

#include <string.h>
#include <ifaddrs.h>
#include <linux/ioctl.h>
#include <netdb.h>

#include "../COMM/sky_trace.h"
#include "TcpClient.h"

CTcpClient::CTcpClient()
{
    m_tcp_sock_client = -1;
    m_last_error = 0;
    m_pReadCallback = NULL;
    tid_recv = 0;
    //tid_conn = 0;
    //m_connThreadFlag = false;
    m_recvThreadFlag = false;
}

CTcpClient::~CTcpClient()
{
    if (tid_recv)
    {
        void *ret;
        pthread_attr_destroy(&attr);
        pthread_join(tid_recv, &ret);
        tid_recv = 0;
    }
    m_recvThreadFlag = false;
    tcp_disconnect();
}

sint32 CTcpClient::init_tcp_client(const sint8 *hostname, uint32 port)
{
    unsigned long ret = inet_addr(hostname);
    if (ret == INADDR_NONE)
    {
        SKY_TRACE_INFO("hostname to ip\n");
        struct hostent *h;
        if ((h = gethostbyname(hostname)) == NULL)
        { // get the host info error,域名错误或网络有问题
            return -2;
        }
        const char *hostip = inet_ntoa(*((struct in_addr *)h->h_addr));
        SKY_TRACE_INFO("ip is %s\n", hostip);          //for test
        memset(m_ip_address, 0, sizeof(m_ip_address)); //IP清零
        memcpy(m_ip_address, hostip, strlen(hostip));
    }
    else
    {
        memset(m_ip_address, 0, sizeof(m_ip_address)); //IP清零
        int len = strlen(hostname);
        if (len > 30)
            len = 30;
        memcpy(m_ip_address, hostname, len);
    }
    m_port = port;
    if (tid_recv == 0) //没起接收线程
    {
        pthread_attr_init(&attr);
        pthread_attr_setscope(&attr, PTHREAD_SCOPE_SYSTEM);
        pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
        m_recvThreadFlag = true;
        int res = pthread_create(&tid_recv, &attr, tcp_read_thread, this);
        if (res != 0)
        {
            m_last_error = -3;
            return -6;
        }
    }
    return 0;
}
int CTcpClient::tcp_connect(void)
{
    if (m_tcp_sock_client > 0) //实际连接状态
    {
        return 0;
    }
    m_tcp_sock_client = socket(AF_INET, SOCK_STREAM, 0);
    if (m_tcp_sock_client <= 0)
    {
        m_last_error = -1;
        return -3;
    }
    int on = 1;
    setsockopt(m_tcp_sock_client, SOL_SOCKET, SO_KEEPALIVE, (void *)&on, sizeof(on));
    setsockopt(m_tcp_sock_client, IPPROTO_TCP, TCP_NODELAY, (void *)&on, sizeof(on));
    memset(&socket_server_addr, 0, sizeof(socket_server_addr));
    socket_server_addr.sin_family = AF_INET;
    socket_server_addr.sin_port = htons(m_port); /* host to net, short */
    socket_server_addr.sin_addr.s_addr = inet_addr(m_ip_address);
    if (0 == inet_aton(m_ip_address, &socket_server_addr.sin_addr))
    { //IP address error
        m_last_error = -2;
        tcp_disconnect();
        return -4;
    }

    if (connect(m_tcp_sock_client, (struct sockaddr *)&socket_server_addr, sizeof(socket_server_addr)) < 0)
    {
        SKY_TRACE_ERROR("connect %s:%d failed \n", inet_ntoa(socket_server_addr.sin_addr), ntohs(socket_server_addr.sin_port));
        PRINTF("connect %s:%d failed \n", inet_ntoa(socket_server_addr.sin_addr), ntohs(socket_server_addr.sin_port));
        m_last_error = -3;
        tcp_disconnect();
        return -5;
    }

    SKY_TRACE_INFO("connect %s:%d success \n", inet_ntoa(socket_server_addr.sin_addr), ntohs(socket_server_addr.sin_port));
    SetNonBlocking(m_tcp_sock_client);
    m_last_error = 0;
    return 0;
}
int CTcpClient::tcp_disconnect(void)
{
    if (m_tcp_sock_client > 0)
    {
        close(m_tcp_sock_client);
        m_tcp_sock_client = -1;
        SKY_TRACE_INFO("tcp_disconnect %s:%d\n", inet_ntoa(socket_server_addr.sin_addr), ntohs(socket_server_addr.sin_port));
    }
    return 0;
};

sint8 CTcpClient::get_local_ip()
{
    struct ifaddrs *ifaddr, *ifa;
    int family;

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

void *CTcpClient::tcp_read_thread_new(void *arg)
{
    int policy, priority;
    pthread_getschedparam(pthread_self(), &policy, (struct sched_param *)&priority);
    SKY_TRACE_INFO("tcp_read_thread policy=%d, priority=%d\n", policy, priority);

    prctl(PR_SET_NAME, "tcp_read_thread");

    SKY_TRACE_INFO("------------start TCP Read Thread--------------\n");
    CTcpClient *p_tcp = (CTcpClient *)arg;

    ssize_t res = 0;
    uint8 frameLen = 255;
    uint8 remainedLen = FRAME_START_HEADER_LEN; //FRAME_HEADER_LEN;
    uint8 dataLen = 0;
    uint8 readLen = 0;

    while (p_tcp->m_recvThreadFlag)
    {
        while (p_tcp->m_tcp_sock_client > 0)
        {
            res = recv(p_tcp->m_tcp_sock_client, p_tcp->rev_buf + readLen, remainedLen - readLen, MSG_NOSIGNAL);
            SKY_TRACE_INFO("1.remainedLen:%d, res:%d, [0]:0x%x [1]:0x%x [2]:0x%x [3]:0x%x [4]:0x%x [5]:0x%x [6]:0x%x [7]:0x%x [8]:0x%x [9]:0x%x [10]:0x%x [11]:0x%x [12]:0x%x [13]:0x%x [14]:0x%x [15]:0x%x\n",
                           remainedLen, res, p_tcp->rev_buf[0], p_tcp->rev_buf[1], p_tcp->rev_buf[2], p_tcp->rev_buf[3], p_tcp->rev_buf[4],
                           p_tcp->rev_buf[5], p_tcp->rev_buf[6], p_tcp->rev_buf[7], p_tcp->rev_buf[8], p_tcp->rev_buf[9],
                           p_tcp->rev_buf[10], p_tcp->rev_buf[11], p_tcp->rev_buf[12], p_tcp->rev_buf[13], p_tcp->rev_buf[14], p_tcp->rev_buf[15]);
            // PRINTF("remainedLen:%d, res:%d, [0]:0x%x [1]:0x%x [2]:0x%x [3]:0x%x [4]:0x%x [5]:0x%x [6]:0x%x [7]:0x%x [8]:0x%x [9]:0x%x [10]:0x%x [11]:0x%x [12]:0x%x [13]:0x%x [14]:0x%x [15]:0x%x\n",
            //        remainedLen, res, p_tcp->rev_buf[0], p_tcp->rev_buf[1], p_tcp->rev_buf[2], p_tcp->rev_buf[3], p_tcp->rev_buf[4],
            //        p_tcp->rev_buf[5], p_tcp->rev_buf[6], p_tcp->rev_buf[7], p_tcp->rev_buf[8], p_tcp->rev_buf[9],
            //        p_tcp->rev_buf[10], p_tcp->rev_buf[11], p_tcp->rev_buf[12], p_tcp->rev_buf[13], p_tcp->rev_buf[14], p_tcp->rev_buf[15]);

            if (res > 0)
                readLen += res;
            SKY_TRACE_INFO("2.Now res:%d ,readLen:%d\n", res, readLen);
            // PRINTF("Now readLen: %d\n", readLen);
            if (res <= 0)
            {
                res = readLen;
                if (!(errno == EAGAIN || errno == EWOULDBLOCK || errno == EINTR))
                {
                    SKY_TRACE_INFO("errno:%d ,%m\n", errno);
                    p_tcp->m_last_error = -100 + res;
                    break;
                }
            }

            if (readLen == FRAME_START_HEADER_LEN)
            {
                if ((p_tcp->rev_buf[0] == 0x23) && (p_tcp->rev_buf[1] == 0x23))
                {
                    remainedLen = FRAME_HEADER_LEN;
                    readLen = 2;
                }
                else
                {
                    remainedLen = FRAME_START_HEADER_LEN;
                    readLen = 0;
                }

                SKY_TRACE_INFO("3.0 remainedLen:%d\n", remainedLen);
            }
            else if (readLen == FRAME_HEADER_LEN)
            {
                if ((p_tcp->rev_buf[0] == 0x23) && (p_tcp->rev_buf[1] == 0x23))
                {
                    dataLen = (p_tcp->rev_buf[9] << 8) | p_tcp->rev_buf[10];
                    frameLen = FRAME_HEADER_LEN + dataLen + 2;
                    remainedLen = frameLen;
                }
                else
                {
                    remainedLen = FRAME_START_HEADER_LEN;
                    readLen = 0;
                }
                SKY_TRACE_INFO("3.dataLen:%d, remainedLen:%d\n", dataLen, remainedLen);
                // PRINTF("2.dataLen:%d, remainedLen:%d\n", dataLen, remainedLen);
            }
            else if (readLen == frameLen)
            {
                // SKY_TRACE_INFO("------------recv()->res: %d\n", res);
                if (p_tcp->m_pReadCallback != NULL)
                    p_tcp->m_pReadCallback(p_tcp->rev_buf, readLen, p_tcp->m_arg);

                SKY_TRACE_INFO("4.send frameLen:%d\n", frameLen);
                // PRINTF("send frameLen:%d\n", frameLen);
                remainedLen = FRAME_HEADER_LEN;
                readLen = 0;
                frameLen = 255;
            }

            usleep(3000); //3ms=cpu 5% ;10ms=cpu 4%
            // usleep(10);  // cpu 38%
        }
        usleep(20000);
    }
    return NULL;
}

void *CTcpClient::tcp_read_thread(void *arg)
{
    int policy, priority;
    pthread_getschedparam(pthread_self(), &policy, (struct sched_param *)&priority);
    SKY_TRACE_INFO("tcp_read_thread policy=%d, priority=%d\n", policy, priority);

    prctl(PR_SET_NAME, "tcp_read_thread");

    SKY_TRACE_INFO("------------tcp_read_thread--------------\n");
    CTcpClient *p_tcp = (CTcpClient *)arg;
    while (p_tcp->m_recvThreadFlag)
    {
        while (p_tcp->m_tcp_sock_client > 0)
        {
            sint32 res;
            res = recv(p_tcp->m_tcp_sock_client, p_tcp->rev_buf, BUFF_LEN, MSG_NOSIGNAL);
            if (res <= 0)
            {
                if (!(errno == EAGAIN || errno == EWOULDBLOCK || errno == EINTR))
                {
                    p_tcp->m_last_error = -100 + res;
                    break;
                }
            }
            else
            {
                //SKY_TRACE_INFO("------------recv()->res: %d\n", res);
                if (p_tcp->m_pReadCallback != NULL)
                    p_tcp->m_pReadCallback(p_tcp->rev_buf, res, p_tcp->m_arg);
            }
            usleep(5000);
        }
        usleep(20000);
    }
    return NULL;
}

void CTcpClient::set_read_callback_func(const pFunc_ReadCallback pReadFunc, void *arg)
{
    m_pReadCallback = pReadFunc;
    m_arg = arg;
}

sint32 CTcpClient::get_tcp_status()
{
    //printf("m_tcp_sock_client : %d \n",m_tcp_sock_client);
    if (m_tcp_sock_client > 0)
    {
        return 0;
    }
    return -1;
}
sint32 CTcpClient::get_last_error()
{
    return m_last_error;
}

sint32 CTcpClient::send_tcp_data(sint8 *str, uint32 len)
{
    unsigned char *pstr = (unsigned char *)str;
    // for (int i = 0; i < len; i++)
    // {
    //     printf("%02X ", pstr[i]);
    //     // SKY_TRACE_DEBUG("%02x ", pstr[i]);
    // }
    // printf("\n");
    // SKY_TRACE_DEBUG("\n");

    if (m_tcp_sock_client <= 0)
    {
        SKY_TRACE_ERROR("m_tcp_sock_client <= 0, cannot send_tcp_data");
        return -1;
    }
    int res = 0;
    int pos = 0;
    int slen = 0;
    while (pos < len)
    {
        slen = len - pos;
        res = send(m_tcp_sock_client, (pstr + pos), slen, 0);
        //       printf("send(), res=%d, ERROR: %m\n", res);
        if (res > 0)
        {
            pos = pos + res;
        }
        else
        {
            SKY_TRACE_ERROR("[%s]: errno=%d, erro info: %m\n", __FUNCTION__, errno);
            if (!(errno == EAGAIN || errno == EWOULDBLOCK || errno == EINTR))
            {
                PRINTF("tcp_disconnect(), ERROR: %m\n");
                SKY_TRACE_ERROR("tcp_disconnect(), ERROR: %m\n");
                tcp_disconnect();
                return -1;
            }
        }
    }
    return len;
}

int CTcpClient::SetNonBlocking(int fd)
{
    int t_opts;
    t_opts = fcntl(fd, F_GETFL);

    if (t_opts < 0)
    {
        return -1;
    }
    t_opts = t_opts | O_NONBLOCK;
    if (fcntl(fd, F_SETFL, t_opts) < 0)
    {
        return -1;
    }
    return 0;
};
