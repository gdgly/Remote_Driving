#ifndef __RS232_H__
#define __RS232_H__

#include <pthread.h>

#include "CCycleBuffer.h"

	
class CRS232
{
public:
    CRS232();
    virtual ~CRS232();

    int OpenUart(int port);
    int set_speed(int speed);
    int set_Parity(int databits,int stopbits,int parity);
    int ComWrite(char *WriteBuff,const int WriteLen);	//com口写数据
    void CloseUart();//关闭com口

    typedef void (*pFunc_ReadCallback)(char* buf, int count);

    void SetReadCallbackFunc(const pFunc_ReadCallback pReadFunc);
	
private:
    int fd;//文件描述符
    int thread_avtived;
	
	pthread_t tid_recv, tid_process;//接受数据线程的Id
	
    static void * ReadThreadFunction(void * arg);//接受数据的线程函数
    static void * ProcessThreadFunction(void * arg);//接受数据的线程函数
	
	CCycleBuffer * p_cycle_buf;
	
	pFunc_ReadCallback pReadCallback;
};

#endif /* __RS232_H__ */
