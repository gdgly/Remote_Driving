#ifndef __CCycleBuffer__
#define __CCycleBuffer__


//#define		USE_MUTEX	1	//使用线程锁
#define		USE_MUTEX	0	//不使用线程锁


#if USE_MUTEX
#include <pthread.h>  
#endif


class CCycleBuffer
{
public:
    bool isFull();
    bool isEmpty();
    void Empty();
    int GetLength();
    CCycleBuffer(int size);
    virtual ~CCycleBuffer();
    int Write(char* buf, int count);
    int Read(char* buf, int count);

private:
    bool m_bEmpty, m_bFull;
    char * m_pBuf;
    int m_nBufSize;
    int m_nReadPos;
    int m_nWritePos;
	
	//线程安全
	#if USE_MUTEX
	pthread_mutex_t queueMutex;  
	#endif

public:
    int GetReadPos() { return m_nReadPos; }
    int GetWritePos() { return m_nWritePos; }

};

#endif /* defined(__CCycleBuffer__) */
