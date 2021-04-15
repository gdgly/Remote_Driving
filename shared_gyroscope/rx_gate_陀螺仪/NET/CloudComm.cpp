#include "CloudComm.h"
#include "UdpClient.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <vector>
#include "../CANNet.h"
#include "CJsonObject/CJsonObject.hpp"
#include "UdpServer.h"
#include "UdpClient.h"
#include "../HWTPack.h"


string remote_sensing_ip = "192.168.1.150";

extern CUdpClient g_Udp2Client_Dashboard;
extern CUdpServer g_Udp2Server_Centreconsole; // the centre console
extern CUdpClient g_Udp2Client_Centreconsole;
extern CUdpClient g_Udp2Client_RemoteSensing;

extern sint8 gCentreconsoleClientIpStr[16];
extern uint32 gCentreconsoleClientPort;

extern sint8 gDashboardClientIpStr[16];
extern uint32 gDashboardClientPort;

extern sint8 gRemoteSensingClientIpStr[16];
extern uint32 gRemoteSensingClientPort;

string gettimestamp(void)
{
 struct timeval start;
 gettimeofday(&start,NULL);

 struct tm _tm = {0};
 localtime_r(&start.tv_sec,&_tm);
 char szTmp[50] = {0};
 strftime(szTmp,50,"%G-%m-%d %H:%M:%S",&_tm);
 sprintf(&szTmp[strlen(szTmp)],".%ld",start.tv_usec/1000);
 string str(szTmp);
 return str;
}

//extern struct typFNT_GB16 code GB_16[];

void Udp2Client_Centreconsole_RecvCallBack(sint8 *buf, uint32 len, struct sockaddr *src_addr, uint32 addr_len)
{
    return;
}

static void *ProcessProc(void *lParam)
{
    int policy, priority;
    pthread_getschedparam(pthread_self(), &policy, (struct sched_param *)&priority);
    SKY_TRACE_INFO("ProcessProc policy=%d, priority=%d\n", policy, priority);

    prctl(PR_SET_NAME, "ProcessProc");

    Thread *pthis = (Thread *)lParam;
    if (!pthis)
    {
        return 0;
    }
    pthis->Process();
    return 0;
};

Thread::Thread()
    : m_thread(0)
{
    m_bRun = false;
};

Thread::~Thread()
{
    StopThread();
};

int Thread::StartThread(void)
{
    m_bRun = true;
    int errcode = pthread_create(&m_thread, 0, ProcessProc, this);
    if (errcode)
    {
        m_bRun = false;
        perror("pthread_create");
    }
    return errcode;
};

void Thread::StopThread(void)
{
    m_bRun = false;
    if (m_thread)
    {
        void *retVal;
        pthread_join(m_thread, &retVal);
        m_thread = 0;
    }
};

CloudComm::CloudComm()
{
    memset(m_DeviceId, '0', DEVICE_ID_LEN);
    m_DeviceType = 2;
    m_nHeartSpace = 5;
    m_nLoginNum = 0;
    m_nSeqCount = 0;
    m_TakeoverFlag = 0;
    m_unTakeoverFlag = 0;
    m_unTakeoverFlag_cloud = 0;
};

CloudComm::~CloudComm(){

};

void CloudComm::init(const char *deviceid, const char *hostname, int port)
{
    SKY_TRACE_INFO("CloudComm init ....\n");
    m_bLogin_Status = false;
    m_bHeart_Success = false;
    memset(m_RecvBuf, 0x00, 2 * BUFF_LEN);
    m_RecvLen = 0;
    m_DeviceType = 2;
    m_nHeartSendDt = 0;
    m_nHeartAckRecvDt = time(NULL);
    m_nReportDt = 0;
    m_nStatusDt = 0;
    m_nCountStrong = 0;
    m_nCountWeak = 0;
    m_nOldEps = 0;
    m_unTakeoverFlag = 0;
    m_unTakeoverFlag_cloud = 0;

    memcpy(m_DeviceId, deviceid, DEVICE_ID_LEN);
    memset(m_BindId, 0, DEVICE_ID_LEN + 1);
    InitDashboards();
    pthread_mutex_init(&thread_mutex, NULL);
    m_TcpClient.init_tcp_client(hostname, port);
    m_TcpClient.set_read_callback_func(RecvFromCloud, this);
    StartThread();
};
void CloudComm::destroy(void)
{
    pthread_mutex_destroy(&thread_mutex);
    DestroySendData();
    StopThread();
};

void Udp2Client_Dashboard_RecvCallBack(sint8 *buf, uint32 len, struct sockaddr *src_addr, uint32 addr_len)
{
    int i;
    i = 1;
}

void CloudComm::Process(void)
{
    SKY_TRACE_INFO("process thread start .....\n ");
    while (m_bRun)
    {
        if (m_TcpClient.get_tcp_status() < 0) // 非连接状态
        {
            SKY_TRACE_INFO("get tcp status < 0 \n");
            //发送数据清除
            ClearSendData();
            m_RecvLen = 0;
            if (m_TcpClient.tcp_connect() == 0) //进行连接
            {
                SKY_TRACE_INFO("tcpserver connetct success ## ...\n");
                Login(); // 连上后，第一次登录
                sleep(2);
                RequestBindingRelation();
            }
            else
            {
                SKY_TRACE_ERROR("tcpserver connetct failed ...\n");
                PRINTF("tcpserver connetct failed ...\n");
                SLEEP(2); // 连接失败，两秒重连
                continue;
            }
        }

        if (!m_bLogin_Status)
        {
            if (m_nLoginNum < 3) //登录失败连续登录三次
            {
                sleep(2);
                m_nLoginNum++;
                Login();
            }
            else // 连续登录失败，断开重新连接
            {
                SKY_TRACE_ERROR("login failed many times, disconnet and reconnect \n");
                PRINTF("login failed many times, disconnet and reconnect \n");
                m_nLoginNum = 0;
                m_TcpClient.tcp_disconnect();
                m_RecvLen = 0;
            }
        }
        else // 登录成功
        {
            if (abs(time(NULL) - m_nHeartSendDt) >= m_nHeartSpace)
            {
                m_nHeartSendDt = time(NULL);
                m_nSeqCount++;
                Heart();
                m_bHeart_Success = false;
            }
            if ((abs(time(NULL) - m_nHeartAckRecvDt) >= 40))
            {
                m_nHeartAckRecvDt = time(NULL);
                SKY_TRACE_ERROR("above 20 sec no recv heart ack,logout and disconnect... second: %ld\n", m_nHeartAckRecvDt);
                PRINTF("above 20 sec no recv heart ack,logout and disconnect... second: %ld\n", m_nHeartAckRecvDt);
                LogOut();
                m_TcpClient.tcp_disconnect();
                m_bLogin_Status = false;
                sleep(1);
                continue;
            }
            //TCP 数据发送
            SendToCloud();
            //UDP 数据发送
            // SKY_TRACE_INFO("[%s], abs(clock() - m_nReportDt):%d\n",__FUNCTION__, abs(clock() - m_nReportDt));
            if (abs(clock() - m_nReportDt) > 200)
            {
                string t_reportdata = GetString();
                if (t_reportdata != "")
                {
                    SKY_TRACE_INFO("[%s] Dashboard ip is %s:%d\n", __FUNCTION__, gDashboardClientIpStr, gDashboardClientPort);

                    if (g_Udp2Client_Dashboard.get_udp_status() == 0)
                    {
                        if (strcmp(gDashboardClientIpStr, gCentreconsoleClientIpStr) == 0)
                        {
                            gDashboardClientPort = gCentreconsoleClientPort;
                        }
                        g_Udp2Client_Dashboard.set_read_callback_func(Udp2Client_Dashboard_RecvCallBack);
                        g_Udp2Client_Dashboard.init_udp_client(gDashboardClientIpStr, gDashboardClientPort);
                    }
                    g_Udp2Client_Dashboard.send_udp_data((char *)t_reportdata.c_str(), t_reportdata.size());
                    // SKY_TRACE_INFO("send to dashboard data: %s \n",(char*)t_reportdata.c_str());
                    m_nReportDt = clock();
                }
            }
            // SendDriverModeData(m_BindId);

            if (abs(time(NULL) - m_nStatusDt) >= 1)
            { // 1s汇报一次驾驶舱状态
                Status();
                m_nStatusDt = time(NULL);
            }

            // usleep(20 * 1000);
            usleep(5 * 1000);
        }
    }
};

int CloudComm::Login(void)
{
    char databuf[100] = {0};
    int datalen = 0;
    PackHead(databuf, datalen, 0x20, FLAG_COMMAND_NEED_RESPONSE);
    PackData(databuf, datalen, m_DeviceType, m_DeviceId);
    PackTail(databuf, datalen);
    SKY_TRACE_ERROR("CloudComm::Login\n");
    return m_TcpClient.send_tcp_data(databuf, datalen);
};

int CloudComm::CabinTakeover(char *vin)
{
    char databuf[100] = {0};
    int datalen = 0;
    PackHead(databuf, datalen, 0x26, FLAG_COMMAND_NEED_RESPONSE);
    PackData(databuf, datalen, m_DeviceId, vin);
    PackTail(databuf, datalen);
    SKY_TRACE_INFO("[%s] , time:%ld\n", __FUNCTION__, time(NULL));
    PRINTF("[%s] , time:%ld\n", __FUNCTION__, time(NULL));
    return m_TcpClient.send_tcp_data(databuf, datalen);
};

int CloudComm::CabinUnTakeover(char *vin)
{
    char databuf[100] = {0};
    int datalen = 0, ret;
    PackHead(databuf, datalen, 0x2c, FLAG_COMMAND_NEED_RESPONSE);
    PackData(databuf, datalen, vin, m_DeviceId);
    PackTail(databuf, datalen);
    SKY_TRACE_INFO("[%s] databuf: %s, datalen: %d\n", __FUNCTION__, databuf, datalen);
    ret = m_TcpClient.send_tcp_data(databuf, datalen);
    return ret;
};

int CloudComm::LogOut(void)
{
    char databuf[100] = {0};
    int datalen = 0;
    PackHead(databuf, datalen, 0x21, FLAG_COMMAND_NEED_RESPONSE);
    PackData(databuf, datalen, m_DeviceType, m_DeviceId);
    PackTail(databuf, datalen);
    return m_TcpClient.send_tcp_data(databuf, datalen);
};
int CloudComm::Heart(void)
{
    char databuf[100] = {0};
    int datalen = 0;
    PackHead(databuf, datalen, 0x22, FLAG_COMMAND_NEED_RESPONSE);
    databuf[datalen++] = 0x00;
    databuf[datalen++] = 0x04;

    SKY_TRACE_INFO("cabin Heart()\n");

    int h_seqcount = htonl(m_nSeqCount);
    memcpy(databuf + datalen, &h_seqcount, 4);
    datalen += 4;
    PackTail(databuf, datalen);
    return m_TcpClient.send_tcp_data(databuf, datalen);
};

int CloudComm::Status(void)
{
    char databuf[100] = {0};
    int datalen = 0;
    PackHead(databuf, datalen, 0x2A, FLAG_COMMAND_NO_NEED_RESPONSE);
    databuf[datalen++] = 0x00;
    databuf[datalen++] = 35;

    memcpy(databuf + datalen, m_DeviceId, DEVICE_ID_LEN);
    datalen += 17;
    memcpy(databuf + datalen, m_BindId, DEVICE_ID_LEN);
    datalen += 17;
    if (m_DashboardsData.ms == 0)
    {
        // 本地模式
        databuf[datalen++] = 0;
    }
    else
    {
        databuf[datalen++] = 1;
    }

    PackTail(databuf, datalen);
    return m_TcpClient.send_tcp_data(databuf, datalen);
};

int CloudComm::SendToCloud(void)
{
    if (0 == strlen(m_BindId))
    { // 未处于接管状态
        SKY_TRACE_INFO("未处于接管状态, m_BindId:%s\n", m_BindId);
        // return 0;
    }

    char canpack[100] = {0};
    int packlen = 0;
    GetSendData(canpack, packlen);
    if (packlen != 0)
    {
        char databuf[100] = {0};
        int datalen = 0;
        PackHead(databuf, datalen, 0x24, FLAG_COMMAND_NO_NEED_RESPONSE);
        PackData(databuf, datalen, m_DeviceId, m_BindId, canpack, packlen);
        PackTail(databuf, datalen);
        // SKY_TRACE_INFO("舱端->车端 CAN, datalen:%d, packlen:%d, 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x\n",
        //                datalen, packlen, databuf[0], databuf[1], databuf[2], databuf[45], databuf[46],
        //                databuf[47], databuf[48], databuf[49], databuf[50], databuf[51],
        //                databuf[52], databuf[53], databuf[54], databuf[55], databuf[56], databuf[57]);
        SKY_TRACE_INFO("舱端->车端CAN, datalen:%d, packlen:%d\n", datalen, packlen);
        return m_TcpClient.send_tcp_data(databuf, datalen);
    }
    else
    {
        SKY_TRACE_ERROR("no can data, packlen=0\n");
        PRINTF("no can data, packlen=0\n");
    }

    return 0;
}

int CloudComm::RequestBindingRelation(void)
{
    char databuf[100] = {0};
    int datalen = 0, ret;
    PackHead(databuf, datalen, 0x2e, FLAG_COMMAND_NEED_RESPONSE);
    databuf[datalen++] = 0x00;
    databuf[datalen++] = DEVICE_ID_LEN;

    memcpy(databuf + datalen, m_DeviceId, DEVICE_ID_LEN);
    datalen += DEVICE_ID_LEN;
    PackTail(databuf, datalen);

    ret = m_TcpClient.send_tcp_data(databuf, datalen);

    SKY_TRACE_INFO("[%s], time: %ld\n", __FUNCTION__, time(NULL));
    PRINTF("[%s], time: %ld\n", __FUNCTION__, time(NULL));
    sleep(2);
    return ret;
}

int CloudComm::PackHead(char *pack, int &packlen, sint8 cmd, sint8 flag)
{
    packlen = 0;
    pack[packlen++] = 0x23;
    pack[packlen++] = 0x23;
    pack[packlen++] = cmd & 0xFF;
    pack[packlen++] = flag & 0xFF;
    // 时间戳
    time_t t_stampdt = time(NULL);
    pack[packlen++] = (t_stampdt >> 24) & 0xFF;
    pack[packlen++] = (t_stampdt >> 16) & 0xFF;
    pack[packlen++] = (t_stampdt >> 8) & 0xFF;
    pack[packlen++] = t_stampdt & 0xFF;
    pack[packlen++] = 0; // 加密方式 默认不加密
    return packlen;
};
int CloudComm::PackData(char *pack, int &packlen, int devtype, char *devid)
{
    pack[packlen++] = (19 >> 8) & 0xFF;
    pack[packlen++] = 19 & 0xFF;
    pack[packlen++] = 0x00;
    pack[packlen++] = devtype & 0xFF;
    memcpy(&pack[packlen], devid, DEVICE_ID_LEN);
    packlen = packlen + DEVICE_ID_LEN;
    return packlen;
}
int CloudComm::PackData(char *pack, int &packlen, char *src, char *des, char *data, unsigned int datalen)
{

    pack[packlen++] = ((datalen + 34) >> 8) & 0xFF;
    pack[packlen++] = (datalen + 34) & 0xFF;

    memcpy(&pack[packlen], src, DEVICE_ID_LEN);
    packlen = packlen + DEVICE_ID_LEN;
    memcpy(&pack[packlen], des, DEVICE_ID_LEN);
    packlen = packlen + DEVICE_ID_LEN;

    memcpy(&pack[packlen], data, datalen);
    packlen = packlen + datalen;

    return packlen;
};

int CloudComm::PackData(char *pack, int &packlen, char *src, char *des)
{

    pack[packlen++] = (34 >> 8) & 0xFF;
    pack[packlen++] = 34 & 0xFF;

    memcpy(&pack[packlen], src, DEVICE_ID_LEN);
    packlen = packlen + DEVICE_ID_LEN;
    memcpy(&pack[packlen], des, DEVICE_ID_LEN);
    packlen = packlen + DEVICE_ID_LEN;

    return packlen;
};

int CloudComm::PackData(char *pack, int &packlen, char *devid)
{
    pack[packlen++] = (17 >> 8) & 0xFF;
    pack[packlen++] = 17 & 0xFF;
    memcpy(&pack[packlen], devid, DEVICE_ID_LEN);
    packlen = packlen + DEVICE_ID_LEN;
    return packlen;
}

int CloudComm::PackTail(char *pack, int &packlen)
{
    pack[packlen++] = 0x24;
    pack[packlen++] = 0x24;

    return packlen;
};

void RecvFromCloud(uint8 *data, unsigned int len, void *arg)
{
    CloudComm *p_cloud = (CloudComm *)arg;
    // 如何确保剩余字节数够用
    memcpy(&p_cloud->m_RecvBuf[p_cloud->m_RecvLen], data, len);
    p_cloud->m_RecvLen = p_cloud->m_RecvLen + len;
#if 0
    printf("\nRecvFromCloud[%d]: ", len);
    SKY_TRACE_INFO("\nRecvFromCloud[%d]: \n", len);
    for (unsigned int i = 0; i < 4; i++)
    {
        printf("[%d]:0x%02x [%d]:0x%02x [%d]:0x%02x [%d]:0x%02x [%d]:0x%02x [%d]:0x%02x [%d]:0x%02x [%d]:0x%02x\n",
               i * 8, p_cloud->m_RecvBuf[i * 8], i * 8 + 1, p_cloud->m_RecvBuf[i * 8 + 1], i * 8 + 2, p_cloud->m_RecvBuf[i * 8 + 2], i * 8 + 3, p_cloud->m_RecvBuf[i * 8 + 3],
               i * 8 + 4, p_cloud->m_RecvBuf[i * 8 + 4], i * 8 + 5, p_cloud->m_RecvBuf[i * 8 + 5], i * 8 + 6, p_cloud->m_RecvBuf[i * 8 + 6], i * 8 + 7, p_cloud->m_RecvBuf[i * 8 + 7]);
        SKY_TRACE_INFO("[%d]:0x%02x [%d]:0x%02x [%d]:0x%02x [%d]:0x%02x [%d]:0x%02x [%d]:0x%02x [%d]:0x%02x [%d]:0x%02x\n",
                       i * 8, p_cloud->m_RecvBuf[i * 8], i * 8 + 1, p_cloud->m_RecvBuf[i * 8 + 1], i * 8 + 2, p_cloud->m_RecvBuf[i * 8 + 2], i * 8 + 3, p_cloud->m_RecvBuf[i * 8 + 3],
                       i * 8 + 4, p_cloud->m_RecvBuf[i * 8 + 4], i * 8 + 5, p_cloud->m_RecvBuf[i * 8 + 5], i * 8 + 6, p_cloud->m_RecvBuf[i * 8 + 6], i * 8 + 7, p_cloud->m_RecvBuf[i * 8 + 7]);
    }
    printf("\n");
    SKY_TRACE_INFO("\n");
#endif
    p_cloud->FrameCheck();
};

int CloudComm::FrameCheck(void)
{
    // usleep(2 * 1000);
    // SKY_TRACE_INFO("------------[%s]->m_RecvLen: %d\n", __FUNCTION__, m_RecvLen);
    if (m_RecvLen > 2)
    {
        SKY_TRACE_ERROR("m_RecvBuf[0]: 0x%x, [1]: 0x%x,[2]: 0x%x, [3]: 0x%x, \n", m_RecvBuf[0], m_RecvBuf[1], m_RecvBuf[2], m_RecvBuf[3]);
        if ((m_RecvBuf[0] == 0x23) && (m_RecvBuf[1] == 0x23))
        {
            if (m_RecvLen > 11)
            {
                int cmd = m_RecvBuf[2];
                int flag = m_RecvBuf[3];
                unsigned int packlen = (m_RecvBuf[9] << 8) | m_RecvBuf[10];
                SKY_TRACE_INFO("cmd:0x%x, flag:0x%x, m_RecvLen: %d, packlen: %d\n", cmd, flag, m_RecvLen, packlen);
                if (m_RecvLen > (packlen + 12))
                {
                    if ((m_RecvBuf[packlen + 11] == 0x24) && (m_RecvBuf[packlen + 12] == 0x24))
                    {
                        Analysis(cmd, flag, packlen, m_RecvBuf, m_RecvLen);
                    }
                    memcpy(m_RecvBuf, &m_RecvBuf[packlen + 13], m_RecvLen - packlen - 13);
                    m_RecvLen = m_RecvLen - packlen - 13;
                    FrameCheck();
                }
                else
                {
                    // SKY_TRACE_ERROR("m_RecvBuf[0]:0x%x, [1]:0x%x,, [2]:0x%x,, [3]:0x%x\n", m_RecvBuf[0], m_RecvBuf[1], m_RecvBuf[2], m_RecvBuf[3]);
                    // SKY_TRACE_ERROR("Buf[8]:0x%x,[9]:0x%x,[10]:0x%x,[11]:0x%x,[12]:0x%x,[13]:0x%x,[14]:0x%x,[15]:0x%x\n",
                    //                 m_RecvBuf[8], m_RecvBuf[9], m_RecvBuf[10], m_RecvBuf[11], m_RecvBuf[12], m_RecvBuf[13], m_RecvBuf[14], m_RecvBuf[15]);
                    if (packlen > 138)
                    {
                        SKY_TRACE_ERROR("fatal error: packlen: 0x%x, m_RecvLen is reset to 0.\n", packlen);
                        m_RecvLen = 0;
                    }

                    return 0;
                }
            }
            else
            {
                return 0;
            }
        }
        else
        {
            memcpy(m_RecvBuf, &m_RecvBuf[2], m_RecvLen - 2);
            m_RecvLen = m_RecvLen - 2;
            FrameCheck();
        }
    }
    return 0;
};

void CloudComm::Analysis(int cmd, int flag, unsigned int packlen, uint8 *data, unsigned int datalen)
{
    //   SKY_TRACE_INFO("RX [%s], 0x%x\n", __FUNCTION__, data[2]);
    /*printf("\nRecv:");
    for (unsigned int i = 0; i < datalen ; i++)
    {
        printf("%02X ",(unsigned char)data[i]);
    }
    printf("\n");*/
    switch (cmd)
    {
    case 0x20:
        if ((flag & 0xFF) == 0x00)
        {
            SKY_TRACE_INFO("login success .... \n");
            PRINTF("login success .... \n");
            m_bLogin_Status = true;
        }
        else
        {
            SKY_TRACE_INFO("login failed .... \n");
            PRINTF("login failed .... \n");
            m_bLogin_Status = false;
        }
        break;
    case 0x21:
        m_bLogin_Status = false;
        break;
    case 0x22:
        m_nHeartAckRecvDt = time(NULL);
        SKY_TRACE_INFO("receive heartbeat ack, second: %ld\n", m_nHeartAckRecvDt);
        m_bHeart_Success = true;
        break;
    case 0x23: //驾驶舱收到云端转发车的数据
        // 判断舱端地址是否与绑定地址相等
        // 2个字节帧头##，2个字节命令，4个字节时间，2个字节长度 + 16个字节地址，16个字节地址
        // 获取车辆ID,舱ID
        {
            char t_vehid[DEVICE_ID_LEN + 1] = {0};
            char t_cabid[DEVICE_ID_LEN + 1] = {0};
            memcpy(t_vehid, &data[11], DEVICE_ID_LEN);
            memcpy(t_cabid, &data[28], DEVICE_ID_LEN);
            // SKY_TRACE_INFO("驾驶舱收到云端转发车的数据，车辆id:%s t_cabid:%s \n", t_vehid, t_cabid);
            if ((memcmp(t_vehid, m_BindId, DEVICE_ID_LEN) == 0) && (memcmp(t_cabid, m_DeviceId, DEVICE_ID_LEN) == 0))
            {
                // 多帧分解
                int packlen = (data[10] | (data[9] << 8)) - 34;
                int packnum = packlen / 13;
                for (int i = 0; i < packnum; i++)
                {
                    TCP_To_CAN_Send(&data[45 + i * 13], 13);
                    // usleep(10); // 10ms 延迟，是否加根据调试情况
                }
            }
        }
        break;
    case 0x25: // 云端发出接管指令
    {
        char t_vehid[DEVICE_ID_LEN + 1] = {0};
        char t_cabid[DEVICE_ID_LEN + 1] = {0};
        memcpy(t_vehid, &data[11], DEVICE_ID_LEN);
        memcpy(t_cabid, &data[28], DEVICE_ID_LEN);
        SKY_TRACE_INFO("收到云端发出的接管指令，车辆id:%s 驾驶舱id:%s \n", t_vehid, t_cabid);
        // 校验舱地址是否匹配
        if (memcmp(t_cabid, m_DeviceId, DEVICE_ID_LEN) == 0)
        {
            m_TakeoverFlag = 1;
            m_unTakeoverFlag_cloud = 0;
            memcpy(m_BindId, t_vehid, DEVICE_ID_LEN);
        }
    }
    break;
    case 0x26: //驾驶舱收到云端回复的舱端接管指令执行结果
    {
        sint8 send_buf[BUFF_LEN] = {0};
        uint8 status;
        char t_vehid[DEVICE_ID_LEN + 1] = {0};
        char t_cabid[DEVICE_ID_LEN + 1] = {0};

        memcpy(t_cabid, &data[11], DEVICE_ID_LEN);
        //cmd:takeover;vin:11111111111111111;status:1
        memcpy(t_vehid, &data[28], DEVICE_ID_LEN);
        SKY_TRACE_INFO("驾驶舱收到云端回复的舱端接管指令执行结果, flag:%d, time:%ld\n", flag, time(NULL));
        PRINTF("驾驶舱收到云端回复的舱端接管指令执行结果, flag:%d, time:%ld\n", flag, time(NULL));
        if ((flag & 0xFF) == 0x00)
        {
            SKY_TRACE_INFO("cmd:takeover success : t_cabid :%s, t_vehid:%s.\n", t_cabid, t_vehid);
            // 校验舱地址是否匹配
            if (memcmp(t_cabid, m_DeviceId, DEVICE_ID_LEN) == 0)
            {
                m_TakeoverFlag = 1;
                m_unTakeoverFlag_cloud = 0;
                memcpy(m_BindId, t_vehid, DEVICE_ID_LEN);
            }
        }
        else
        {
            SKY_TRACE_INFO("cmd:takeover failed .... \n");
        }

        status = m_TakeoverFlag == 1 ? 0 : 4;
        sprintf(send_buf, "cmd:takeover;vin:%s;status:%d", t_vehid, status);
        g_Udp2Server_Centreconsole.send_udp_data(send_buf, strlen(send_buf));
    }
    break;
    case 0x2c: //驾驶舱收到云端回复的驾驶舱取消接管指令执行结果
    {
        sint8 send_buf[BUFF_LEN] = {0};
        uint8 status;
        const char *msg;
        char t_vehid[DEVICE_ID_LEN + 1] = {0};
        char t_cabid[DEVICE_ID_LEN + 1] = {0};

        memcpy(t_cabid, &data[28], DEVICE_ID_LEN);
        //cmd:untakeover;vin:11111111111111111;status:1;msg:错误消息
        memcpy(t_vehid, &data[11], DEVICE_ID_LEN);
        if ((flag & 0xFF) == 0x00)
        {
            SKY_TRACE_INFO("cmd:untakeover success .... \n");
            if (memcmp(t_cabid, m_DeviceId, DEVICE_ID_LEN) == 0)
            {
                m_TakeoverFlag = 0;
                memset(m_BindId, 0, DEVICE_ID_LEN);
            }
        }
        else
        {
            SKY_TRACE_INFO("cmd:untakeover failed .... \n");
        }

        AddDriverModeData(t_vehid, 0);

        status = m_TakeoverFlag == 0 ? 0 : 1;
        msg = m_TakeoverFlag == 0 ? "success" : "failed";
        sprintf(send_buf, "cmd:untakeover;vin:%s;;status:%d;msg:%s", t_vehid, status, msg);

        g_Udp2Server_Centreconsole.send_udp_data(send_buf, strlen(send_buf));

        SendDriverModeToCentreconsole();
    }
    break;
    case 0x2e: //驾驶舱从云端获取车舱绑定关系
        if ((flag & 0xFF) == 0x00)
        {
            SKY_TRACE_INFO("驾驶舱从云端获取车舱绑定关系\n");
            PRINTF("驾驶舱从云端获取车舱绑定关系\n");
            GetBindingRelation(cmd, packlen, data, datalen);
        }
        else
        {
            SKY_TRACE_INFO("获取车舱绑定关系失败 .... \n");
            PRINTF("获取车舱绑定关系失败 .... \n");
        }
    break;
    case 0x28: //云端下发车舱绑定关系到驾驶舱
    {
        if ((flag & 0xFF) == 0xFE) 
        {
            SKY_TRACE_INFO("云端下发车舱绑定数据到驾驶舱 ... datalen:%d \n", datalen);
            GetBindingRelation(cmd, packlen, data, datalen);
        }
        else
        {
            SKY_TRACE_INFO("云端下发车舱绑定关系失败 .... \n");
        }
    }
    break;
    case 0x2b: // 云端发出的取消接管指令
    {
        char t_vehid[DEVICE_ID_LEN + 1] = {0};
        char t_cabid[DEVICE_ID_LEN + 1] = {0};
        memcpy(t_vehid, &data[11], DEVICE_ID_LEN);
        memcpy(t_cabid, &data[28], DEVICE_ID_LEN);
        SKY_TRACE_INFO("收到云端发出的取消接管指令，车辆id:%s 驾驶舱id:%s \n", t_vehid, t_cabid);

        // 校验舱地址是否匹配
        if (memcmp(t_cabid, m_DeviceId, DEVICE_ID_LEN) == 0)
        {
            m_unTakeoverFlag_cloud = 1;
            // m_TakeoverFlag = 0;
            // memset(m_BindId, 0, DEVICE_ID_LEN);
            AddDriverModeData(t_vehid, 0);
        }
    }

    case 0x2f: //车端发送驾驶模式状态到驾驶舱
    {
        uint8 t_vehid[DEVICE_ID_LEN + 1] = {0};
        uint8 t_cabid[DEVICE_ID_LEN + 1] = {0};
        unsigned char drivingmode;
        memcpy(t_vehid, &data[11], DEVICE_ID_LEN);
        memcpy(t_cabid, &data[28], DEVICE_ID_LEN);
        memcpy(&drivingmode, &data[45], sizeof(drivingmode));
        SKY_TRACE_INFO("收到车端发送驾驶模式状态指令，车辆id:%s 驾驶舱id:%s drivingmode: %d\n", t_vehid, t_cabid, drivingmode);
        // 校验舱地址是否匹配
        if (memcmp(t_cabid, m_DeviceId, DEVICE_ID_LEN) == 0)
        {
            SKY_TRACE_INFO("\n[%s] %s:%d\n\n", __FUNCTION__, gCentreconsoleClientIpStr, gCentreconsoleClientPort);
            if (g_Udp2Client_Centreconsole.get_udp_status() == 0)
            {
                g_Udp2Client_Centreconsole.set_read_callback_func(Udp2Client_Centreconsole_RecvCallBack);
                g_Udp2Client_Centreconsole.init_udp_client(gCentreconsoleClientIpStr, gCentreconsoleClientPort);
            }
            //cmd:status;vin:11111111111111111;drivingmode:3;
            sint8 send_buf[BUFF_LEN] = {0};
            AddDriverModeData((char *)t_vehid, drivingmode);
            sprintf(send_buf, "cmd:status;vin:%s;drivingmode:%d;", t_vehid, drivingmode);
            g_Udp2Client_Centreconsole.send_udp_data(send_buf, strlen(send_buf));
        }
    }
    break;

    case 0x3e: //驾驶舱收到车HWT905的数据
    // 判断舱端地址是否与绑定地址相等
    // 2个字节帧头##，2个字节命令，4个字节时间，2个字节长度 + 16个字节地址，16个字节地址
    // 获取车辆ID,舱ID
    {
        struct timeval receive;
        float timeuse; 
        gettimeofday(&receive,NULL); // 开始时间
        UdpData_t udp_frame_data;
        uint8 udp_send_frame[100] = {0};
        char t_vehid[DEVICE_ID_LEN + 1] = {0};
        char t_cabid[DEVICE_ID_LEN + 1] = {0};
        memcpy(t_vehid, &data[11], DEVICE_ID_LEN);
        memcpy(t_cabid, &data[28], DEVICE_ID_LEN);
        // printf("Receive HWT905 Data From Cloud:[%s]:",gettimestamp().c_str());  
        // for (int i = 0; i < datalen; i++)
        // {
        //     printf("%02X ",data[i]);
        // }
        // printf("\n");  

        // SKY_TRACE_INFO("[%s], 驾驶舱收到车HWT905的第一帧数据[0]:0x%x, [1]:0x%x, [2]:0x%x, [3]:0x%x, [4]:0x%x, [5]:0x%x, [6]:0x%x, [7]:0x%x, [8]:0x%x, [9]:0x%x, [10]:0x%x\n", __FUNCTION__, 
        // data[45+0], data[45+1], data[45+2], data[45+3], data[45+4], data[45+5], data[45+6], data[45+7], 
        // data[45+8], data[45+9], data[45+10]);

        // SKY_TRACE_INFO("[%s], 驾驶舱收到车HWT905的第二帧数据[0]:0x%x, [1]:0x%x, [2]:0x%x, [3]:0x%x, [4]:0x%x, [5]:0x%x, [6]:0x%x, [7]:0x%x, [8]:0x%x, [9]:0x%x, [10]:0x%x\n", __FUNCTION__, 
        // data[45+11], data[45+12], data[45+13], data[45+14], data[45+15], data[45+16], data[45+17], data[45+18], 
        // data[45+19], data[45+20], data[45+21]);

        // SKY_TRACE_INFO("[%s], 驾驶舱收到车HWT905的第三帧数据[0]:0x%x, [1]:0x%x, [2]:0x%x, [3]:0x%x, [4]:0x%x, [5]:0x%x, [6]:0x%x, [7]:0x%x, [8]:0x%x, [9]:0x%x, [10]:0x%x\n", __FUNCTION__, 
        // data[45+22], data[45+23], data[45+24], data[45+25], data[45+26], data[45+27], data[45+28], data[45+29], 
        // data[45+30], data[45+31], data[45+32]);

        if ((memcmp(t_vehid, m_BindId, DEVICE_ID_LEN) == 0) && (memcmp(t_cabid, m_DeviceId, DEVICE_ID_LEN) == 0))
        {
            // SKY_TRACE_INFO("frame check success! \n");
            // 单帧解析
            if ((data[45+0 ] == 0x55)&&(data[45+1 ]=0x51) && \
               (data[45+11] == 0x55)&&(data[45+12]=0x52) && \
               (data[45+22] == 0x55)&&(data[45+23]=0x53))
            // if ((data[45+22] == 0x55)&&(data[45+23]=0x53))
            { 
                //检查校验和
                if( (hwt_calc_sum(&data[45+0], 10) == data[45+10]) && \
                    (hwt_calc_sum(&data[45+11], 10) == data[45+21]) && \
                    (hwt_calc_sum(&data[45+22], 10) == data[45+32]) )
                // if (hwt_calc_sum(&data[45+22], 10) == data[45+32]) 
                {
                    SKY_TRACE_INFO("checksum success! \n");
                    SearchOneFrameData(udp_frame_data, data);
                    // SKY_TRACE_INFO("[%s], udp_frame_data.angular_a_x = %f\n", __FUNCTION__, udp_frame_data.angular_a_x );
                    WriteSensorOneFrame(udp_send_frame, udp_frame_data);
                    //遥感udp客户端
                    if (g_Udp2Client_RemoteSensing.get_udp_status() == 0)
                    {
                        g_Udp2Client_RemoteSensing.set_read_callback_func(NULL);
                        g_Udp2Client_RemoteSensing.init_udp_client("192.168.1.150", 8080);   //"192.168.1.150"
                        // g_Udp2Client_RemoteSensing.init_udp_client(gRemoteSensingClientIpStr, gRemoteSensingClientPort);  
                    }
                    //g_Udp2Client_RemoteSensing.send_udp_data(udp_send_frame, strlen(udp_send_frame));
                    g_Udp2Client_RemoteSensing.send_udp_data(udp_send_frame, REMOTE_SENSING_DATA_LENTG);
                    // printf("\n Cab Send HWT905 Data To Yaogan:[%s]:",gettimestamp().c_str()); 
                } 
            }
        }
    }
    break;

    default:
        break;
    }
};

void CloudComm::CloudUntakeoverPostProcess(void)
{
    SKY_TRACE_INFO("The cabin untakeover %s!\n", m_BindId);
    memset(m_BindId, 0, DEVICE_ID_LEN);
    m_TakeoverFlag = 0;
}


int CloudComm::AddSendData(int dataid, char *data)
{
    map<unsigned int, canpack *>::iterator iter = m_SendMap.find(dataid);
    if (iter == m_SendMap.end())
    {
        canpack *pack = new canpack;
        memcpy(pack->buf, data, 13);
        m_SendMap.insert(make_pair(dataid, pack));
    }
    else
    {
        memcpy(iter->second->buf, data, 13);
    }
    return 0;
};

void CloudComm::GetSendData(char *packdata, int &packlen)
{
    map<unsigned int, canpack *>::iterator iter;
    int packnum = 0;
    packlen = 0;
    for (iter = m_SendMap.begin(); iter != m_SendMap.end(); iter++)
    {
        memcpy(&packdata[13 * packnum], iter->second->buf, 13);
        packlen = packlen + 13;
        packnum++;
    }
    return;
}

int CloudComm::AddDriverModeData(string vin, uint8 mode)
{
    if (vin == "0")
    {
        vin = m_BindId;
    }

    SKY_TRACE_INFO("Add vin:%s, DriverMode: %d\n", vin.c_str(), mode);
    if (strlen(vin.c_str()) == DEVICE_ID_LEN)
    {
        // m_DriverModeMap.insert(make_pair(vin, mode));
        m_DriverModeMap[vin] = mode;
    }

    return 0;
}

int CloudComm::SendDriverModeData(string vin)
{
    map<string, uint8>::iterator iter = m_DriverModeMap.find(vin);
    uint8 mode = 0;

    if (iter == m_DriverModeMap.end())
    {
        //No found the relative mode.
    }
    else
    {
        mode = iter->second;
    }

    if (g_Udp2Client_Centreconsole.get_udp_status() == 0)
    {
        g_Udp2Client_Centreconsole.set_read_callback_func(Udp2Client_Centreconsole_RecvCallBack);
        g_Udp2Client_Centreconsole.init_udp_client(gCentreconsoleClientIpStr, gCentreconsoleClientPort);
    }

    sint8 send_buf[BUFF_LEN] = {0};
    sprintf(send_buf, "cmd:status;vin:%s;drivingmode:%d;", vin.c_str(), mode);
    g_Udp2Client_Centreconsole.send_udp_data(send_buf, strlen(send_buf));

    return mode;
}

int CloudComm::GetVehName(string vin, string &vname)
{
    int ret = -1;
    vname = m_VehInfoMap[vin];

    if (vname != "")
    {
        // SKY_TRACE_INFO("[%s], enter1\n", __FUNCTION__);
        ret = 0;
    }
    else
    {
        // SKY_TRACE_INFO("[%s], enter2\n", __FUNCTION__);
        TStrStrMap::iterator iter2 = m_VehInfoMap.begin();
        while (iter2 != m_VehInfoMap.end())
        {
            SKY_TRACE_INFO("vin:%s, vname:%s\n", iter2->first.c_str(), iter2->second.c_str());
            iter2++;
        }
    }

    return ret;
}

void CloudComm::SendDriverModeToCentreconsole()
{
    map<string, uint8>::iterator iter;
    sint8 send_buf[BUFF_LEN] = {0};

    SKY_TRACE_INFO("[%s]\n", __FUNCTION__);
    for (iter = m_DriverModeMap.begin(); iter != m_DriverModeMap.end(); iter++)
    {
        if (g_Udp2Client_Centreconsole.get_udp_status() == 0)
        {
            g_Udp2Client_Centreconsole.set_read_callback_func(Udp2Client_Centreconsole_RecvCallBack);
            g_Udp2Client_Centreconsole.init_udp_client(gCentreconsoleClientIpStr, gCentreconsoleClientPort);
        }
        SKY_TRACE_INFO("vin:%s, mode:%d\n", iter->first.c_str(), iter->second);
        sprintf(send_buf, "cmd:status;vin:%s;drivingmode:%d;", iter->first.c_str(), iter->second);
        g_Udp2Client_Centreconsole.send_udp_data(send_buf, strlen(send_buf));
    }
}

void CloudComm::ClearSendData(void)
{
    map<unsigned int, canpack *>::iterator iter;
    for (iter = m_SendMap.begin(); iter != m_SendMap.end(); iter++)
    {
        memset(iter->second->buf, 0x00, 13);
    }
}

void CloudComm::DestroySendData(void)
{
    canpack *pack;
    while (!m_SendMap.empty())
    {
        pack = (*m_SendMap.begin()).second;
        m_SendMap.erase(m_SendMap.begin());
        if (pack != NULL)
        {
            delete pack;
            pack = NULL;
        }
    }
}

int CloudComm::AddKeyValue(string name, int value)
{
    //if (0 == pthread_mutex_lock(&thread_mutex))
    //{
    m_DashboardsData.refresh = true;
    if (name == "cs")
    {
        m_DashboardsData.cs = value;
    }
    else if (name == "soc")
    {
        m_DashboardsData.soc = value;
    }
    else if (name == "p")
    {
        m_DashboardsData.p = value;
    }
    else if (name == "dw")
    {
        m_DashboardsData.dw = value;
    }
    else if (name == "ms")
    {
        m_DashboardsData.ms = value;
    }
    else if (name == "zzxd")
    {
        m_DashboardsData.zzxd = value;
    }
    else if (name == "yzxd")
    {
        m_DashboardsData.yzxd = value;
    }
    else if (name == "zs")
    {
        m_DashboardsData.zs = (value > 0 ? value : 0 - value);
    }
    else if (name == "zj")
    {
        m_DashboardsData.zj = value;
    }
    else if (name == "xhlc")
    {
        m_DashboardsData.xhlc = value;
    }
    else if (name == "jgd")
    {
        m_DashboardsData.jgd = value;
    }
    else if (name == "ygd")
    {
        m_DashboardsData.ygd = value;
    }
    else if (name == "skd")
    {
        m_DashboardsData.skd = value;
    }
    else if (name == "ygq")
    {
        m_DashboardsData.ygq = value;
    }
    else if (name == "aqd")
    {
        m_DashboardsData.aqd = value;
    }
    else if (name == "xhqd")
    {
        m_DashboardsData.xhqd = value;
    }
    else if (name == "kt")
    {
        m_DashboardsData.kt = value;
    }
    else if (name == "wd")
    {
        m_DashboardsData.wd = value;
    }
    else if (name == "cmzt")
    {
        m_DashboardsData.cmzt = value;
    }
    else if (name == "cdzt")
    {
        m_DashboardsData.cdzt = value;
    }
    else if (name == "eps")
    {
        m_DashboardsData.eps = value;
    }
    //	pthread_mutex_unlock(&thread_mutex);
    //}
    return 0;
};

void CloudComm::InitDashboards(void)
{
    m_DashboardsData.refresh = false;
    m_DashboardsData.cs = 0;
    m_DashboardsData.soc = 0;
    m_DashboardsData.p = 0;
    m_DashboardsData.dw = 0;
    m_DashboardsData.ms = 0;
    m_DashboardsData.zzxd = 0;
    m_DashboardsData.yzxd = 0;
    m_DashboardsData.zs = 0;
    m_DashboardsData.zj = 0;
    m_DashboardsData.xhlc = 0;
    m_DashboardsData.jgd = 0;
    m_DashboardsData.ygd = 0;
    m_DashboardsData.skd = 0;
    m_DashboardsData.ygq = 0;
    m_DashboardsData.aqd = 0;
    m_DashboardsData.xhqd = 0;
    m_DashboardsData.kt = 0;
    m_DashboardsData.wd = 0;
    m_DashboardsData.cmzt = 0;
    m_DashboardsData.cdzt = 0;
    m_DashboardsData.eps = 0;
}
string CloudComm::GetString(void)
{
    string temstr = "";
    //if (0 == pthread_mutex_lock(&thread_mutex))
    //{
    if (m_DashboardsData.refresh)
    {
        m_DashboardsData.refresh = false;
        char tempbuf[2048] = {0};
        char dw = 'P';
        if ((m_DashboardsData.p == 0x02) || (m_DashboardsData.p == 0x00))
        {
            if (m_DashboardsData.dw == 0x00)
            {
                dw = 'N';
            }
            else if (m_DashboardsData.dw == 0x01)
            {
                dw = 'D';
            }
            else if (m_DashboardsData.dw == 0x02)
            {
                dw = 'R';
            }
        }

        string t_vehname;
        GetVehName(m_BindId, t_vehname);

        m_DashboardsData.xhlc = m_DashboardsData.soc * 2;
        // SKY_TRACE_INFO("[%s] vin:%s, t_vehname:%s\n", __FUNCTION__, m_BindId, t_vehname.c_str());
        if (abs(m_DashboardsData.eps - m_nOldEps) <= EPS_STEP_THRESHOLD)
        {
            m_nCountStrong++;
            if (m_nCountStrong >= COUNT_MARK_STRONG)
            {
                m_DashboardsData.xhqd = 2; //Signal is strong.
                m_nCountWeak = 0;
            }
        }
        else
        {
            /* code */
            m_nCountWeak++;
            if (m_nCountWeak >= COUNT_MARK_WEAK)
            {
                m_DashboardsData.xhqd = 1; //Signal is weak.
                m_nCountStrong = 0;
            }
        }
        SKY_TRACE_INFO("m_nOldEps:%d, m_nCountWeak:%d, m_nCountStrong:%d\n", m_nOldEps, m_nCountWeak, m_nCountStrong);
        m_nOldEps = m_DashboardsData.eps;

        sprintf(tempbuf, "cs:%d;soc:%d;dw:%c;ms:%d;zzxd:%d;yzxd:%d;zs:%d;zj:%d;xhlc:%d;jgd:%d;ygd:%d;skd:%d;ygq:%d;aqd:%d;xhqd:%d;kt:%d;wd:%d;cmzt:%d;cdzt:%d;eps:%d;vname:%s",
                m_DashboardsData.cs, m_DashboardsData.soc, dw, m_DashboardsData.ms, m_DashboardsData.zzxd, m_DashboardsData.yzxd, m_DashboardsData.zs, m_DashboardsData.zj, m_DashboardsData.xhlc, m_DashboardsData.jgd, m_DashboardsData.ygd, m_DashboardsData.skd, m_DashboardsData.ygq, m_DashboardsData.aqd, m_DashboardsData.xhqd, m_DashboardsData.kt, m_DashboardsData.wd, m_DashboardsData.cmzt, m_DashboardsData.cdzt, m_DashboardsData.eps, t_vehname.c_str());
        temstr = string(tempbuf);
    }
    //	pthread_mutex_unlock(&thread_mutex);
    //}
    // SKY_TRACE_INFO("[%s] temstr:%s\n", __FUNCTION__, temstr.c_str());
    return temstr;
};

void CloudComm::GetBindingRelation(int cmd, unsigned int packlen, uint8 *data, unsigned int datalen)    //(int cmd, int flag, uint8 *data, unsigned int datalen)
{
    char t_cabid[DEVICE_ID_LEN + 1] = {0}; // 车舱id
    std::string s_cabid;
    strchr((const char*) data, 0x24 );
    uint8 pstr[packlen+1];
    memcpy(pstr, &data[11], packlen);

    SKY_TRACE_DEBUG("The json data is %s\n ", pstr);
    // PRINTF("The json data is %s \n", &data[11]);
    PRINTF("The json data is %s \n", pstr);

    const char *s = (const char *)&data[11];
    const std::string data_str(s);
    neb::CJsonObject dataJson(data_str);

    dataJson.Get("hin", s_cabid);
    strcpy(t_cabid, s_cabid.c_str());
    // SKY_TRACE_DEBUG("The t_cabid is %s\n ", t_cabid);
    // SKY_TRACE_DEBUG("The m_DeviceId is %s\n ", m_DeviceId);
    if (strcmp(t_cabid, m_DeviceId) == 0) // 车舱id与下发的id 匹配
    {
        FILE *pFile;
        mkdirs((char *)CABIN_ID_FOLDER_PATH);
        if(cmd == 0x28)
            remove(CABIN_ID_FILE_PATH);
        pFile = fopen(CABIN_ID_FILE_PATH, "wb+");
        // SKY_TRACE_DEBUG("pFile : %p\n ", pFile);
        if (NULL != pFile)
        {
            fwrite(&data[11], datalen - 13, 1, pFile);
            fclose(pFile);

            SKY_TRACE_DEBUG("&data[11]: %s, datalen:%d\n", &data[11], datalen);
            // SKY_TRACE_DEBUG("GetArraySize() : %d\n ", dataJson["vlist"].GetArraySize());
            for (int i = 0; i < dataJson["vlist"].GetArraySize(); ++i)
            {
                std::string strVin, strVname;

                dataJson["vlist"][i].Get("vin", strVin);
                dataJson["vlist"][i].Get("vname", strVname);
                m_staticBindIds.push_back(strVin);

                m_VehInfoMap.insert(TStrStrPair(strVin, strVname));
                AddDriverModeData(strVin, 0);
                SKY_TRACE_INFO("[%s] vin:%s, vname:%s\n\n", __FUNCTION__, strVin.c_str(), strVname.c_str());
            }

            TStrStrMap::iterator iter2 = m_VehInfoMap.begin();
            while (iter2 != m_VehInfoMap.end())
            {
                SKY_TRACE_INFO("2. vin:%s, vname:%s\n",
                                iter2->first.c_str(), iter2->second.c_str());
                // cout<<iter2->first<<iter2->second<<endl;
                iter2++;
            }
        }
        else
        {
            fclose(pFile);
            SKY_TRACE_ERROR("---------FATAL ERROR: pFile is NULL!!!!!!!!!!!!!\n ");
        }
    }
};

