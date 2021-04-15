#include "CloudComm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

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
    memset(m_DeviceId, '1', DEVICE_ID_LEN);
    m_DeviceType = 1;
    m_nHeartSpace = 5;
    m_nLoginNum = 0;
    m_nSeqCount = 0;
};

CloudComm::~CloudComm(){

};

void CloudComm::init(const char *deviceid, const char *hostname, int port)
{
    m_bLogin_Status = false;
    m_bHeart_Success = false;
    memset(m_RecvBuf, 0x00, 2 * BUFF_LEN);
    m_RecvLen = 0;
    m_DeviceType = 1;
    m_nHeartSendDt = 0;
    m_nHeartAckRecvDt = time(NULL);
    memcpy(m_DeviceId, deviceid, DEVICE_ID_LEN);
    memset(m_BindId, 0, DEVICE_ID_LEN + 1);
    memset(m_StaticBindId, 0, DEVICE_ID_LEN + 1);
    memset(m_hwt_buf, 0, HWT_FRAME_SIZE);

    FILE *pFile;
    if ((pFile = fopen(CABIN_ID_FILE_PATH, "r")) != NULL) // 文件读取
    {
        char tempID[18] = {0};
        int numread = fread(tempID, sizeof(char), 17, pFile);
        SKY_TRACE_INFO("read %s  %s\n", CABIN_ID_FILE_PATH, tempID);
        if (numread == 17)
        {
            memcpy(m_StaticBindId, tempID, 17);
        }
        fclose(pFile);
    }
    else
    {
        SKY_TRACE_INFO("%s not exist \n", CABIN_ID_FILE_PATH);
    }

    m_TcpClient.init_tcp_client(hostname, port);
    m_TcpClient.set_read_callback_func(RecvFromCloud, this);
    StartThread();
};
void CloudComm::destroy(void)
{
    DestroySendData();
    StopThread();
};

void CloudComm::Process(void)
{
    static int ctl_num = 0;
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
            SendToCloud();

            // SendHWT905ToCloud();

            usleep(10 * 1000);
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

// int CloudComm::SendToCloud(void)
void CloudComm::SendToCloud(void)
{
    struct timeval start, end;
    float timeuse; 
    gettimeofday(&start,NULL); // 开始时间

    char canpack[1024] = {0};
    int packlen = 0;
    GetSendData(canpack, packlen);
    if (packlen != 0)
    {
        char databuf[1024] = {0};
        int datalen = 0;
        PackHead(databuf, datalen, 0x23, FLAG_COMMAND_NO_NEED_RESPONSE);
        PackData(databuf, datalen, m_DeviceId, m_BindId, canpack, packlen);
        PackTail(databuf, datalen);
        // SKY_TRACE_INFO("车端->舱端的CAN,0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x\n",
        //                databuf[0], databuf[1], databuf[2], databuf[45], databuf[46],
        //                databuf[47], databuf[48], databuf[49], databuf[50], databuf[51],
        //                databuf[52], databuf[53], databuf[54], databuf[55], databuf[56], databuf[57]);
        SKY_TRACE_INFO("车端->舱端的CAN, m_BindId:%s, datalen:%d\n", m_BindId, datalen);
        // return m_TcpClient.send_tcp_data(databuf, datalen);
        m_TcpClient.send_tcp_data(databuf, datalen);
    }
    gettimeofday(&end,NULL);
    timeuse = 1000000*(end.tv_sec-start.tv_sec)+end.tv_usec-start.tv_usec;       
    timeuse /= 1000;       
    //  printf("used time:%f msec\n",timeuse);
    SKY_TRACE_INFO("控制指令占用时长: %f msec\n", timeuse);
    // return 0;
}

// int CloudComm::SendHWT905ToCloud(void)
void CloudComm::SendHWT905ToCloud(void)
{
    // printf("Vehicle recive HWT905 data:[%s]: \n",gettimestamp().c_str()); 
    int packlen = 0;
    char databuf[1024] = {0};
    int datalen = 0;
    PackHead(databuf, datalen, 0x3e, FLAG_COMMAND_NO_NEED_RESPONSE);
    PackData(databuf, datalen, m_DeviceId, m_BindId, m_hwt_buf, HWT_FRAME_SIZE);
    // PackData(databuf, datalen, m_DeviceId, m_BindId, m_hwt_buf, 33);
    PackTail(databuf, datalen);
    SKY_TRACE_INFO("车端->舱端的HWT905数据, m_BindId:%s, datalen:%d\n", m_BindId, datalen);
    // return m_TcpClient.send_tcp_data(databuf, datalen);
    m_TcpClient.send_tcp_data(databuf, datalen); 
    printf("Vehicle Send HWT905 Data To Cloud:[%s]:",gettimestamp().c_str());   
    for (int i = 0; i < datalen; i++)
    {
        printf("%02X ",databuf[i]);
    }  
    printf("\n");  
}

int CloudComm::RequestBindingRelation(void)
{
    char databuf[1024] = {0};
    int datalen = 0, ret = 0;
    PackHead(databuf, datalen, 0x2d, FLAG_COMMAND_NEED_RESPONSE);
    PackData(databuf, datalen, m_DeviceId);
    PackTail(databuf, datalen);
    ret = m_TcpClient.send_tcp_data(databuf, datalen);
    SKY_TRACE_INFO("[%s], m_DeviceId: %s, ret:  %d\n", __FUNCTION__, m_DeviceId, ret);
    return ret;
}

int CloudComm::SendDrivingmode(uint8 mode)
{
    char databuf[1024] = {0};
    int datalen = 0, ret = 0;
    PackHead(databuf, datalen, 0x2f, FLAG_COMMAND_NO_NEED_RESPONSE);
    PackData(databuf, datalen, m_DeviceId, m_BindId, (char *)&mode, 1);
    PackTail(databuf, datalen);
    ret = m_TcpClient.send_tcp_data(databuf, datalen);
    SKY_TRACE_INFO("[%s], mode: %d, ret:  %d, m_nSeqCount: %d\n", __FUNCTION__, mode, ret, m_nSeqCount);
    return ret;
}

int CloudComm::PackHead(char *pack, int &packlen, int cmd, int flag)
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
//int CloudComm::PackData(char *pack, int &packlen, char *src, char *des, char *data, unsigned int datalen)
int CloudComm::PackData(char *pack, int &packlen, char *src, char *des, void *data, unsigned int datalen)
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
int CloudComm::PackData(char *pack, int &packlen, char *devid)
{
    pack[packlen++] = (DEVICE_ID_LEN >> 8) & 0xFF;
    pack[packlen++] = DEVICE_ID_LEN & 0xFF;
    memcpy(&pack[packlen], devid, DEVICE_ID_LEN);
    packlen = packlen + DEVICE_ID_LEN;
    return packlen;
}
int CloudComm::PackTail(char *pack, int &packlen)
{
    pack[packlen++] = 0x24;
    pack[packlen++] = 0x24;

    /*printf("Send:");
    for (int i = 0; i < packlen ; i++)
    {
        printf("%02X ",(unsigned char)pack[i]);
    }
    printf("\n");*/
    return packlen;
};

void RecvFromCloud(uint8 *data, unsigned int len, void *arg)
{
#if 0
    printf("Recv[%d]:",len);
    for (unsigned int i = 0; i < len ; i++)
    {
        printf("%02X ",(unsigned char)data[i]);
    }
    printf("\n");
#endif

    //printf("[%s]: 0x%02x \n", __FUNCTION__, data[2]);

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
                SKY_TRACE_INFO("cmd:0x%x, flag:%d, m_RecvLen: %d, packlen: %d\n", cmd, flag, m_RecvLen, packlen);
                if (m_RecvLen > (packlen + 12))
                {
                    if ((m_RecvBuf[packlen + 11] == 0x24) && (m_RecvBuf[packlen + 12] == 0x24))
                    {
                        Analysis(cmd, flag, m_RecvBuf, m_RecvLen);
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
                    if (packlen > 73)
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

void CloudComm::Analysis(int cmd, int flag, uint8 *data, unsigned int datalen)
{
    //    SKY_TRACE_INFO("TX [%s], 0x%x\n", __FUNCTION__, data[2]);
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
    case 0x24: // 车收到云端转发驾驶舱的数据
        // 2个字节帧头##，2个字节命令，4个字节时间，2个字节长度 + 16个字节地址，16个字节地址
        // 获取车辆ID,舱ID
        {
            char t_vehid[DEVICE_ID_LEN + 1] = {0};
            char t_cabid[DEVICE_ID_LEN + 1] = {0};
            memcpy(t_cabid, &data[11], DEVICE_ID_LEN);
            memcpy(t_vehid, &data[28], DEVICE_ID_LEN);
            if ((memcmp(t_cabid, m_BindId, DEVICE_ID_LEN) == 0) && (memcmp(t_vehid, m_DeviceId, DEVICE_ID_LEN) == 0))
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
    case 0x25: // 云端发出的接管指令
    {
        char t_vehid[DEVICE_ID_LEN + 1] = {0};
        char t_cabid[DEVICE_ID_LEN + 1] = {0};
        memcpy(t_vehid, &data[11], DEVICE_ID_LEN);
        memcpy(t_cabid, &data[28], DEVICE_ID_LEN);
        SKY_TRACE_INFO("收到云端发出的接管指令，车辆id:%s 驾驶舱id:%s \n", t_vehid, t_cabid);
        // 校验车辆ID是否匹配
        if (memcmp(t_vehid, m_DeviceId, DEVICE_ID_LEN) == 0)
        {
            memcpy(m_BindId, t_cabid, DEVICE_ID_LEN);
        }
    }
    break;
    case 0x26: // 舱端发出接管指令
    {
        char t_vehid[DEVICE_ID_LEN + 1] = {0};
        char t_cabid[DEVICE_ID_LEN + 1] = {0};
        memcpy(t_cabid, &data[11], DEVICE_ID_LEN);
        memcpy(t_vehid, &data[28], DEVICE_ID_LEN);
        SKY_TRACE_INFO("收到舱端发出接管指令，车辆id:%s, 驾驶舱id:%s \n", t_vehid, t_cabid);
        // PRINTF("收到舱端发出接管指令，车辆id:%s, 驾驶舱id:%s \n", t_vehid, t_cabid);
        // 校验车辆ID是否匹配
        if (memcmp(t_vehid, m_DeviceId, DEVICE_ID_LEN) == 0)
        {
            memcpy(m_BindId, t_cabid, DEVICE_ID_LEN + 1);
        }
    }
    break;
    case 0x2c: //驾驶舱取消接管指令
    {
        char t_vehid[DEVICE_ID_LEN + 1] = {0};
        char t_cabid[DEVICE_ID_LEN + 1] = {0};
        memcpy(t_vehid, &data[11], DEVICE_ID_LEN);
        memcpy(t_cabid, &data[28], DEVICE_ID_LEN);
        SKY_TRACE_INFO("收到舱端取消接管指令，车辆id:%s, 驾驶舱id:%s \n", t_vehid, t_cabid);
        // 校验车辆ID是否匹配
        if (memcmp(t_vehid, m_DeviceId, DEVICE_ID_LEN) == 0)
        {
            memset(m_BindId, 0, DEVICE_ID_LEN + 1);
        }
    }
    break;
    case 0x2d: //车端从云端获取车舱绑定关系
    {
        char t_cabid[DEVICE_ID_LEN + 1] = {0};

        memcpy(t_cabid, &data[11], DEVICE_ID_LEN);
        SKY_TRACE_INFO("车端从云端获取车舱绑定关系，车辆id:%s, 驾驶舱id:%s \n", m_DeviceId, t_cabid);
        // memcpy(m_BindId, t_cabid, DEVICE_ID_LEN + 1);
    }
    break;
    case 0x2b: // 云端发出的取消接管指令
    {
        char t_vehid[DEVICE_ID_LEN + 1] = {0};
        char t_cabid[DEVICE_ID_LEN + 1] = {0};
        memcpy(t_vehid, &data[11], DEVICE_ID_LEN);
        memcpy(t_cabid, &data[28], DEVICE_ID_LEN);
        SKY_TRACE_INFO("收到云端发出的取消接管指令，车辆id:%s 驾驶舱id:%s \n", t_vehid, t_cabid);
        // 校验车辆ID是否匹配
        if (memcmp(t_vehid, m_DeviceId, DEVICE_ID_LEN) == 0)
        {
            memset(m_BindId, 0, DEVICE_ID_LEN + 1);
        }
    }
    break;
    case 0x29: // 云端下发车舱绑定关系到车
    {
        SKY_TRACE_INFO("云端下发车舱绑定关系到车 \n");
        char t_vehid[DEVICE_ID_LEN + 1] = {0};
        char t_cabid[DEVICE_ID_LEN + 1] = {0};
        memcpy(t_vehid, &data[11], DEVICE_ID_LEN);
        memcpy(t_cabid, &data[28], DEVICE_ID_LEN);

        SKY_TRACE_INFO("t_vehid:%s,t_cabid:%s\n", t_vehid, t_cabid);

        if (memcmp(t_vehid, m_DeviceId, DEVICE_ID_LEN) == 0)
        {
            mkdirs((char *)CABIN_ID_FOLDER_PATH);
            FILE *pFile;
            pFile = fopen(CABIN_ID_FILE_PATH, "w+");
            if (NULL != pFile)
            {
                fwrite(t_cabid, strlen(t_cabid), 1, pFile);
                fclose(pFile);
                memcpy(m_StaticBindId, t_cabid, DEVICE_ID_LEN);
            }
        }
    }
    break;
    default:
        break;
    }
};

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
