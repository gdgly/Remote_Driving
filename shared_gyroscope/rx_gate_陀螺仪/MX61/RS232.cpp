#include     <stdio.h> 
#include     <unistd.h>
#include     <fcntl.h>  
#include     <termios.h>
#include     <pthread.h> 

#include "RS232.h"

#include "../COMM/sky_trace.h"

/************************************
 功能：构造函数
 参数：无
 返回值：无
 ***********************************/
CRS232::CRS232()
{
    fd = 0;
    thread_avtived = 0;
	
	p_cycle_buf = new CCycleBuffer(1024);
	
	pReadCallback = NULL;
}

/************************************
 功能：析构函数
 参数：无
 返回值：无
 ***********************************/
CRS232::~CRS232()
{
    CloseUart();
	delete p_cycle_buf;
}

/**********************************************************************
*函数,	OpenUart
*作用:	打开串口设备
*参数：	fd：串口文件指针的指针
*		port:通讯口，范围是1-4
*返回值:整型,
*		0-表示串口打开成功，
*		1-表示tcsetattr获取失败，
*		2-无法打开设备
**********************************************************************/
int CRS232::OpenUart(int port)
{
    /*设备驱动文件，分别对应COM1,COM2,COM3,COM4*/
    const char * com[] = {"/dev/ttyS0","/dev/ttyS1","/dev/ttyS2","/dev/ttyS3"};

    /*检查通讯口是否正确*/
    if((port <1 )||(port > 4))
    {
        SKY_TRACE_ERROR("invalid port %d\n", port);
        return 1;
    }


    fd = open(com[port-1], O_RDWR);
    if (fd < 0)
    {
		//cout <<"Open Com " << port << " faild" << endl;
        SKY_TRACE_ERROR("open COM%d faild\n", port);
        return 2;
    }

//下面开始创建接受线程。
    pthread_attr_t attr;
    pthread_attr_init(&attr);
// 设置线程绑定属性
    int res = pthread_attr_setscope(&attr, PTHREAD_SCOPE_SYSTEM);
// 设置线程分离属性
    res += pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
//创建数据接收线程
    pthread_create(&tid_recv, &attr, ReadThreadFunction, (void *) this);
//创建数据处理线程
    pthread_create(&tid_process, &attr, ProcessThreadFunction, (void *) this);

    thread_avtived = 1;

    return 0;
}

/**********************************************************************
*函数,	set_speed
*作用:	设置串口波特率
*参数：	fd：文件指针
*		speed:波特率，
*返回值:整型,
*		0-表示设置成功，
*		1-表示tcsetattr获取失败，
*		2-文件指针无效
*		3-speed非有效值
**********************************************************************/
int CRS232::set_speed(int speed)
{
    int   i;
    int   status;
    struct termios   Opt;

    int name_arr[] = {115200, 57600, 38400,  19200,  9600,	4800,  2400, 1200};
    int speed_arr[] = {B115200, B57600, B38400, B19200, B9600, B4800, B2400, B1200};

    if(fd <= 0)
    {
        SKY_TRACE_ERROR("invalid file pointer!\n");
        return 2;
    }

    for(i=0; i< 8; i++)
        if(speed == name_arr[i])break;

    if(i == 8)	//没找到标准的波特率
    {
        SKY_TRACE_ERROR("baud error!\n");
        return 3;
    }

    tcgetattr(fd, &Opt);
    for ( i= 0;  i < sizeof(speed_arr) / sizeof(int);  i++)
    {
        if (speed == name_arr[i])
        {
            tcflush(fd, TCIOFLUSH);
            cfsetispeed(&Opt, speed_arr[i]);
            cfsetospeed(&Opt, speed_arr[i]);
            status = tcsetattr(fd, TCSANOW, &Opt);
            if (status != 0)
            {
                SKY_TRACE_ERROR("invalid tcsetattr!\n");
                return 1;
            }
        }
        tcflush(fd,TCIOFLUSH);
    }
    return 0;
}


/**********************************************************************
*函数,	set_Parity
*作用:	设置串口数据位、停止位、奇偶校验
*参数：	fd：文件指针
*		databits:数据位，支持7位和8位两种
*		stopbits：停止位，支持1位和2位两种
*		parity：奇偶校验，n,N表示无奇偶校验，o,O表示偶校验，n,N表示奇校验
*返回值:整型,
*		0-表示设置成功，
*		1-表示tcsetattr获取失败，
*		2-文件指针无效
*		3-数据位设置错误
*		4-停止位设置错误
*		5-奇偶位设置错误
**********************************************************************/
int CRS232::set_Parity(int databits,int stopbits,int parity)
{
    struct termios options;
	
    if  ( tcgetattr( fd,&options)  !=  0)
    {
        SKY_TRACE_ERROR("invalid tcsetattr 1!\n");
        return 1;
    }

    if(fd <= 0)
    {
        SKY_TRACE_ERROR("invalid file pointer!\n");
        return 2;
    }

    options.c_cflag &= ~CSIZE;

    /*检查数据位*/
    switch (databits)
    {
    case 7:
        options.c_cflag |= CS7;
        break;
    case 8:
        options.c_cflag |= CS8;
        break;
    default:
        SKY_TRACE_ERROR("invalid databits!\n");
        return 3;
    }

    /*检查奇偶校验位*/
    switch (parity)
    {
    case 'n':
    case 'N':
        options.c_cflag &= ~PARENB;
        options.c_iflag &= ~INPCK;
        break;
    case 'o':
    case 'O':
        options.c_cflag |= (PARODD | PARENB);
        options.c_iflag |= INPCK;
        break;
    case 'e':
    case 'E':
        options.c_cflag |= PARENB;
        options.c_cflag &= ~PARODD;
        options.c_iflag |= INPCK;
        break;
    case 'S':
    case 's':
        options.c_cflag &= ~PARENB;
        options.c_cflag &= ~CSTOPB;
        break;
    default:
        SKY_TRACE_ERROR("invalid parity!\n");
        return 4;
    }

    /*检查停止位*/
    switch (stopbits)
    {
    case 1:
        options.c_cflag &= ~CSTOPB;
        break;
    case 2:
        options.c_cflag |= CSTOPB;
        break;
    default:
        SKY_TRACE_ERROR("invalid parity!\n");
        return 5;
    }


    options.c_iflag &= ~(IGNBRK|BRKINT|PARMRK|ISTRIP|INLCR|IGNCR|ICRNL|IXON);
    options.c_oflag &= ~OPOST;
    options.c_lflag &= ~(ECHO|ECHONL|ICANON|ISIG|IEXTEN);

    /* Set input parity option */

    if (parity != 'n')
        options.c_iflag |= INPCK;
    options.c_cc[VTIME] = 150; // 15 seconds
    options.c_cc[VMIN] = 0;


    tcflush(fd,TCIFLUSH); /* Update the options and do it NOW */
    if (tcsetattr(fd,TCSANOW,&options) != 0)
    {
        SKY_TRACE_ERROR("invalid tcsetattr 2!\n");
        return 1;
    }

    return 0;
}


void CRS232::SetReadCallbackFunc(const pFunc_ReadCallback pReadFunc)
{
	pReadCallback = pReadFunc;
}


/************************************
 功能：数据接收线程的函数
 参数：对象指针
 返回值： 无
 ***********************************/
void * CRS232::ReadThreadFunction(void *arg)
{
    //SKY_TRACE_INFO("------------start ReadThread--------------\n");
    CRS232 *com = (CRS232*) arg;

    int nread,i=0;
    char buff[102];

    while(true)
    {

        if((nread = read(com->fd,buff,100))>0) //接收数据
        {
            com->p_cycle_buf->Write(buff,nread);
        }

        usleep(5*1000);

        if(! com->thread_avtived)break;
    }

    //pthread_detach(pthread_self()); //标记为DETACHED状态，完成后释放自己占用的资源。

    return NULL;
}

/************************************
 功能：数据处理的线程函数
 参数：对象指针
 返回值： 无
 ***********************************/
void * CRS232::ProcessThreadFunction(void * arg)//接受数据的线程函数
{
    //SKY_TRACE_INFO("------------start ProcessThread--------------\n");
    CRS232 *com = (CRS232*) arg;

    int len,i;
    char buff[102];

    while(true)
    {
        len = com->p_cycle_buf->GetLength();
        if(len > 0)
        {
            len = len > 100 ? 100 : len ;
            com->p_cycle_buf->Read(buff,len);

			/*
            SKY_TRACE_INFO("[RECEIVE] Len is %d,content is :\n",len);
            for (i = 0; i < len; i++)
            {
                SKY_TRACE_INFO("0x%02x ", buff[i]);
            }
            SKY_TRACE_INFO("\n");
			*/
			if(com->pReadCallback != NULL)
				com->pReadCallback(buff,len);
			
            continue;
        }

        usleep(10*1000);

        if(! com->thread_avtived)break;
    }

    //pthread_detach(pthread_self()); //标记为DETACHED状态，完成后释放自己占用的资源。

    return NULL;
}



/************************************
 功能：Com口写数据
 参数：char * WriteBuff 写入的数据，const int WriteLen 写入的长度
 返回值：写入长度,-1表示写入失败
 ***********************************/
int CRS232::ComWrite(char *WriteBuff, const int WriteLen)
{
    if (fd <= 0)   //判断操作符是否打开
    {
        SKY_TRACE_ERROR("Com error!\n");
        return -1;
    }
    int wlen = write(fd, WriteBuff, WriteLen);
    return wlen;
}

/************************************
 功能：Com关闭
 参数：无
 返回值：无
 ***********************************/
void CRS232::CloseUart()
{
    thread_avtived = 0;		//接收线程退出

    if (fd > 0)   //判断文件描述符是否存在
    {
        close(fd);
        fd = 0;
    }
}


