#include  "SkyCan.h"

//#include <unistd.h>
//#include <stdio.h>
//#include <stdlib.h>

//#include <sys/ioctl.h>
//#include <linux/if.h>
//#include <linux/sockios.h>

//#include <string.h>

#include "../COMM/sky_trace.h"

#define SIOCSCANBAUDRATE        (SIOCDEVPRIVATE+0)
#define SIOCGCANBAUDRATE        (SIOCDEVPRIVATE+1)

#define SOL_CAN_RAW (SOL_CAN_BASE + CAN_RAW)
#define CAN_RAW_FILTER  1
#define CAN_RAW_RECV_OWN_MSGS 0x4

//struct ifreq ifr;


/************************************
 功能：类CSkyCan构造函数，初始化变量，申请缓冲区资源
 参数：无
 返回值： 无
 ***********************************/
CSkyCan::CSkyCan(int num)
{
    m_sock = 0;
    m_Status = 0;

    m_nFrameNum = 0;
    m_nReadPointer =0;
    m_nWritePointer = 0;
    m_Full = 0;
    m_Empty = 1;

    m_thread_avtived = 0;
    tid_recv = 0;
    tid_process = 0;

    m_pReadCallback = NULL;

    m_nFrameNum = num;
	
	m_RecvNums = 0;
	m_ProcessNums = 0;

    m_nLastError = 0;

    m_nFrameSize = sizeof(struct can_frame);
    m_p_buf = new unsigned char [m_nFrameSize * m_nFrameNum];

    //  m_shared_mutex = std::make_shared<std::mutex>();
}

/************************************
 功能：类CSkyCan析构函数，释放资源
 参数：无
 返回值： 无
 ***********************************/
CSkyCan::~CSkyCan()
{
    CloseCan();

    delete m_p_buf;
}

/************************************
 功能：CAN 口初始化函数
 参数：CAN 口的序号，范围0-3，CAN口通讯波特率
 返回值： 成功初始化返回0，不成功返回其他数值
 ***********************************/
unsigned char CSkyCan::init_can(unsigned char can, int baud)
{
    const char * can_name[] = {"can0","can1","can2","can3"};

    struct sockaddr_can addr;
    struct ifreq ifr;

    char str[50]= {0};

    /// set bitrate before socket open can
    sprintf(str, "ip link set %s down", can_name[can]);
    //SKY_TRACE_INFO("%s\n", str);
    system(str);
    sprintf(str, "ip link set %s up type can bitrate %d", can_name[can], baud);
    //SKY_TRACE_INFO("%s\n", str);
    system(str);

    /*检查通讯口是否正确*/
    //if((can <1 )||(can > 4))
    if(can > 3)
    {
        SKY_TRACE_ERROR("invalid can%d\n", can);
        m_nLastError = 1;
        return m_nLastError;
    }

    //创建SOCKET
    m_sock = socket(PF_CAN, SOCK_RAW, CAN_RAW);
    if(m_sock < 0)
    {
        SKY_TRACE_ERROR("can socket opened error\n");
        m_nLastError = 2;
        return m_nLastError;
    }


    addr.can_family = AF_CAN;
    strcpy(ifr.ifr_name, can_name[can]);

    int ret;

    ret = ioctl(m_sock, SIOCGIFINDEX, &ifr);  //get index
    if(ret && ifr.ifr_ifindex == 0)
    {
        SKY_TRACE_ERROR("Can't get interface index for can, code= %d, can0 ifr_ifindex value: %d, name: %s\n", ret, ifr.ifr_ifindex, ifr.ifr_name);
        close(m_sock);
        m_nLastError = 3;
        return m_nLastError;
    }

    //SKY_TRACE_INFO("%s can_ifindex = %x\n",ifr.ifr_name,ifr.ifr_ifindex);
    addr.can_ifindex = ifr.ifr_ifindex;
    ioctl(m_sock,SIOCGIFNAME,&ifr);
    //SKY_TRACE_INFO("ret = %d, %s can_ifname = %s\n",ret,can_name[can],ifr.ifr_name);

    int recv_own_msgs = 0;		///set loop back:  1 enable 0 disable
    setsockopt(m_sock, SOL_CAN_RAW, CAN_RAW_RECV_OWN_MSGS,&recv_own_msgs, sizeof(recv_own_msgs));

    if (bind(m_sock,(struct sockaddr*)&addr,sizeof(addr)) < 0)
    {
        SKY_TRACE_ERROR("bind error\n");
        close(m_sock);
        m_nLastError = 4;
        return m_nLastError;
    }



    //下面开始创建接受线程。
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    // 设置线程绑定属性
    int res = pthread_attr_setscope(&attr, PTHREAD_SCOPE_SYSTEM);
    // 设置线程分离属性
    res += pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);

    //创建数据接收线程
    pthread_create(&tid_recv, &attr, Can_Read_thread, (void *) this);
    //创建数据处理线程
    pthread_create(&tid_process, &attr, ProcessThreadFunction, (void *) this);

	m_thread_avtived = 1;
	
    m_Status = 1;
    return 0;
}

/************************************
 功能：关闭CAN口
 参数：对象指针
 返回值： 无
 ***********************************/
void CSkyCan::CloseCan(void)
{
    m_thread_avtived = 0;

    if(m_Status)
    {
        if(m_sock > 0)
            close(m_sock);
        m_sock = 0;
    }

    m_Status = 0;
    m_nLastError = 0;
    //缓冲区不释放，也不清除，防止数据丢失
}


/************************************
 功能：数据读取的线程函数
 参数：对象指针
 返回值： 无
 ***********************************/
void * CSkyCan::Can_Read_thread(void * arg)
{
    int policy, priority;
    pthread_getschedparam(pthread_self(), &policy, (struct sched_param *)&priority);
    SKY_TRACE_INFO("Can_Read_thread policy=%d, priority=%d\n", policy, priority);

    prctl(PR_SET_NAME, "Can_Read_thread");

    //SKY_TRACE_INFO("------------start Can Read Thread--------------\n");
    CSkyCan *pcan = (CSkyCan*) arg;

    struct can_frame frame;
	int nbytes;

    while(1)
    {
        memset(&frame,0,pcan->m_nFrameSize);
        //nbytes = read(pcan->m_sock,&frame,pcan->m_nFrameSize);
        nbytes = recv(pcan->m_sock,&frame,pcan->m_nFrameSize,MSG_WAITALL);

        //if(frame.can_dlc > 0)
		if(nbytes > 0)
        {
            pcan->WriteOneFrame(&frame);
			pcan->m_RecvNums ++;

			continue;
            //frame.data[frame.can_dlc] = '\0';
            //SKY_TRACE_INFO("%s recv: %s\n",ifr.ifr_name,frame.data);
			/*
            {
                int i;
                //SKY_TRACE_INFO("%s recv:\n", ifr.ifr_name);
                for(i=0; i<frame.can_dlc; i++)
                {
                    SKY_TRACE_INFO("0x%x ", frame.data[i]);
                }
                SKY_TRACE_INFO("\n");
            }
			*/
        }
		else
		{
			usleep(5*100);      //等待0.5ms，1帧数据的时间
		}

        if (!pcan->m_thread_avtived)
            break;
    }
}

/************************************
 功能：数据处理的线程函数
 参数：对象指针
 返回值： 无
 ***********************************/
void * CSkyCan::ProcessThreadFunction(void * arg)//接受数据的线程函数
{
    int policy, priority;
    pthread_getschedparam(pthread_self(), &policy, (struct sched_param *)&priority);
    SKY_TRACE_INFO("ProcessThreadFunction policy=%d, priority=%d\n", policy, priority);

    prctl(PR_SET_NAME, "ProcessThreadFunction");

    //SKY_TRACE_INFO("------------start ProcessThread--------------\n");

    struct can_frame frame;
    MCANDrv_infoMsgS_TYPE infoMsgS;

    int ret,i;
    char buff[20];

    CSkyCan *pcan = (CSkyCan*) arg;

    while(true)
    {
        ret = pcan->ReadOneFrame(&frame);
        if(ret == 0)
        {
            /*
            SKY_TRACE_INFO("[RECEIVE] Len is %d,content is :\n",len);
            for (i = 0; i < len; i++)
            {
                SKY_TRACE_INFO("0x%02x ", buff[i]);
            }
            SKY_TRACE_INFO("\n");
            */

            //数据格式转换
            memset(&infoMsgS,0,sizeof(MCANDrv_infoMsgS_TYPE));

            for (i = 0; i < 8; i++)
                infoMsgS.datMsgUBA[i] = frame.data[i];

            if(frame.can_id & CAN_EFF_FLAG)
            {
                infoMsgS.infoConfS.infoMsgIDUI = frame.can_id & CAN_EFF_MASK;
                infoMsgS.infoConfS.infoMsgParU.B.IDE = 0;   //标准帧
            }
            else
            {
                infoMsgS.infoConfS.infoMsgIDUI = frame.can_id & CAN_SFF_MASK;
                infoMsgS.infoConfS.infoMsgParU.B.IDE = 1;   //扩展帧
            }

            infoMsgS.infoConfS.infoMsgParU.B.DLC = frame.can_dlc;
            infoMsgS.infoConfS.infoMsgParU.B.DIR = 0;   //接收
            
            if(pcan->m_pReadCallback != NULL)
                pcan->m_pReadCallback(infoMsgS);

            //if(pcan->m_pReadCallback != NULL)
            //    pcan->m_pReadCallback((char *)(&frame),pcan->m_nFrameSize);


			pcan->m_ProcessNums ++;
                //SKY_TRACE_INFO("Receive frame is %d, Process frame is %d\n", pcan->m_RecvNums, pcan->m_ProcessNums);
            continue;
        }

        usleep(1*1000);

        //for test
        //SKY_TRACE_INFO("Receive frame is %d, Process frame is %d\n", pcan->m_RecvNums, pcan->m_ProcessNums);

        if (!pcan->m_thread_avtived)
            break;
    }

    //pthread_detach(pthread_self()); //标记为DETACHED状态，完成后释放自己占用的资源。

    return NULL;
}


/************************************
 功能：往缓冲区写一帧数据
 参数：can_frame结构体指针
 返回值：成功写入返回0，没写成功返回1
 ***********************************/
int CSkyCan::WriteOneFrame(struct can_frame * p_can_frame)
{
    if(m_Full)
    {
        m_nLastError = 11;
        return m_nLastError;
		//SKY_TRACE_ERROR("Buffer is full, lost one frame\n");
        //return 1;
    }

    memcpy(m_p_buf + m_nWritePointer * m_nFrameSize, (unsigned char *) p_can_frame, m_nFrameSize);

    m_nWritePointer ++;
    if(m_nWritePointer >= m_nFrameNum)
    {
        m_nWritePointer = 0;
    }

    // std::lock_guard<std::mutex> locker{*m_shared_mutex};
    m_Empty = 0;
    if(m_nWritePointer == m_nReadPointer)
        m_Full = 1;

    return 0;
}

/************************************
 功能：从缓冲区读取一帧数据
 参数：无
 返回值：成功读取返回0，没数据返回1
 ***********************************/
int CSkyCan::ReadOneFrame(struct can_frame * p_can_frame)
{
    if(m_Empty)
    {
        return 1;
    }

    memcpy((unsigned char *) p_can_frame, m_p_buf + m_nReadPointer*m_nFrameSize, m_nFrameSize);

    m_nReadPointer ++;
    if(m_nReadPointer >= m_nFrameNum)
        m_nReadPointer = 0;

    // std::lock_guard<std::mutex> locker{*m_shared_mutex};
    m_Full = 0;
    if(m_nReadPointer == m_nWritePointer)
        m_Empty = 1;

    return 0;
}

/************************************
 功能：设置读CAN数据回调函数
 参数：回调函数指针
 返回值：无
 ***********************************/
void CSkyCan::SetReadCallbackFunc(const pFunc_ReadCallback pReadFunc)
{
    m_pReadCallback = pReadFunc;
}

/************************************
 功能：获取CAN口当前状态
 参数：无
 返回值：CAN的SOCKET状态
 ***********************************/
int CSkyCan::GetCanStatus()
{
    return m_Status;
}

int CSkyCan::GetLastError()
{
    return m_nLastError;
}


/************************************
 功能：通过CAN口发送指定长度的数据
 参数：can_id，要接收数据CAN端的ID
		str，发送数据的指针
		len，发送数据的长度，该长度可以任意长，如长度超过一帧，函数自动分成多帧发送
 返回值：CAN的SOCKET状态
 ***********************************/
int CSkyCan::Write_Can_Data(int can_id,char* str,int len)
{
    struct can_frame frame;
    int i;
    int nbytes = 0;

    if(!m_Status)
    {
        SKY_TRACE_ERROR("can not send , can is not opened!");
        m_nLastError = 21;
        return m_nLastError;
    }

    frame.can_id = can_id; //can device id

    while(len)
    {
        if(len > sizeof(frame.data))
        {
            memset(&frame.data,0,sizeof(frame.data));
            memcpy(&frame.data,str,sizeof(frame.data));
            //SKY_TRACE_INFO("%d,(%s)\r\n",sizeof(frame.data),frame.data);
            frame.can_dlc = sizeof(frame.data);
            str += sizeof(frame.data);
            len -= sizeof(frame.data);
        }
        else
        {
            memset(&frame.data,0,sizeof(frame.data));
            memcpy(&frame.data,str,len);
            //SKY_TRACE_INFO("%d,(%s)\r\n",len,frame.data);
            frame.can_dlc = len;
            str = NULL;
            len = 0;
        }
        write(m_sock, &frame, sizeof(struct can_frame));
        usleep(100);
    }
    return 0;
}

/************************************
 功能：通过CAN口发送一帧数据
 参数：frame_data，一帧数据
 返回值：CAN的SOCKET状态
 ***********************************/
int CSkyCan::SendOneFrameData(const MCANDrv_infoMsgS_TYPE infMsgS)
{
    struct can_frame frame_data;

    if(!m_Status)
    {
        SKY_TRACE_ERROR("can not send , can is not opened!");
        m_nLastError = 21;
        return m_nLastError;
    }

    //格式转换
    memset(&frame_data,0,m_nFrameSize);

    if(infMsgS.infoConfS.infoMsgParU.B.IDE == 0)    //标准帧
    {
        frame_data.can_id = CAN_SFF_MASK & infMsgS.infoConfS.infoMsgIDUI;
    }
    else
    {
        frame_data.can_id = CAN_EFF_MASK & infMsgS.infoConfS.infoMsgIDUI;
        frame_data.can_id = frame_data.can_id | CAN_EFF_FLAG;            //扩展帧
    }
    frame_data.can_dlc = infMsgS.infoConfS.infoMsgParU.B.DLC;
    for(int i=0; i<8; i++)
        frame_data.data[i] = infMsgS.datMsgUBA[i];

    write(m_sock, &frame_data, sizeof(struct can_frame));

    return 0;
}
