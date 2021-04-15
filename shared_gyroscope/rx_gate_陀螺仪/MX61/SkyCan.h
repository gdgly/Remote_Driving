#ifndef __SKYCAN__
#define __SKYCAN__

#include <sys/socket.h>

//#include <linux/can.h>
#include "can.h"

#include <pthread.h>  

//#include "Std_Types.h"
#include "../Std_Types.h"

#include "SkyHardwareMX61Lib.h"


class CSkyCan
{
 	public:
    CSkyCan(int num);
    virtual ~CSkyCan();

	unsigned char init_can(unsigned char can, int baud);
	void CloseCan(void);

	int Write_Can_Data(int can_id,char* str,int len);

	int SendOneFrameData(const MCANDrv_infoMsgS_TYPE infMsgS);

    //typedef void (*pFunc_ReadCallback)(char* buf, int count);
    typedef void (*pFunc_ReadCallback)(MCANDrv_infoMsgS_TYPE infoMsgS);
	
    void SetReadCallbackFunc(const pFunc_ReadCallback pReadFunc);

	int GetCanStatus();
	int GetLastError();

	private:

	static void * Can_Read_thread(void * arg);
    static void * ProcessThreadFunction(void * arg);//接受数据的线程函数


	int WriteOneFrame(struct can_frame * p_can_frame);
	int ReadOneFrame(struct can_frame * p_can_frame);

    int m_sock;
	int m_nFrameNum;
	int m_nFrameSize;

	int m_nLastError;
	
	unsigned char * m_p_buf;
	
	pthread_t tid_recv, tid_process;//接受数据线程的Id
	
	int m_Status;
    int m_thread_avtived;

	int m_nReadPointer;
	int m_nWritePointer;
	int m_Full;
	int m_Empty;

	int m_RecvNums;
	int m_ProcessNums;
	
	pFunc_ReadCallback m_pReadCallback ;
};
	
	
#endif /* defined(__SKYCAN__) */
