#ifndef _CLOUD_COMM_H_
#define _CLOUD_COMM_H_

#include <iostream>
#include <string.h>
#include <map>
#include <pthread.h>
#include "./../CANNet.h"
#include "TcpClient.h"
#include "../COMM/sky_trace.h"
#include "../HWT905.h"

#define DEVICE_ID_LEN 17
#define FLAG_COMMAND_NEED_RESPONSE 0xFE
#define FLAG_COMMAND_NO_NEED_RESPONSE 0xA0

// #define CABIN_ID_FILE_PATH "/var/tx_gate/cabid"
#define CABIN_ID_FOLDER_PATH "./tx_gate"
#define CABIN_ID_FILE_PATH "./tx_gate/cabid"

using namespace std;

class canpack
{
public:
    char buf[13];
};
class Thread
{
public:
    Thread();
    virtual ~Thread();
    int StartThread(void);
    void StopThread(void);
    virtual void Process(void) = 0;

protected:
    bool m_bRun;
    pthread_t m_thread;
};

class CloudComm : public Thread
{
public:
    CloudComm();
    ~CloudComm();

public:
    void init(const char *deviceid, const char *hostname, int port);
    void destroy(void);
    virtual void Process(void);

public:
    int Login(void);
    int LogOut(void);
    int Heart(void);
    int AddSendData(int dataid, char *data);
    // int SendToCloud(void);
    void SendToCloud(void);
    // int SendHWT905ToCloud(void);
    void SendHWT905ToCloud(void);
    int SendDrivingmode(uint8 mode);
    int RequestBindingRelation(void);

public:
    int FrameCheck(void);
    void Analysis(int cmd, int flag, uint8 *data, unsigned int datalen);

private:
    void GetSendData(char *packdata, int &packlen);
    void ClearSendData(void);
    void DestroySendData(void);
    int PackHead(char *pack, int &packlen, int cmd, int flag);
    int PackData(char *pack, int &packlen, int devtype, char *devid);
    // int PackData(char *pack, int &packlen, char *src, char *des, char *data, unsigned int datalen);
    int PackData(char *pack, int &packlen, char *src, char *des, void *data, unsigned int datalen);
    int PackTail(char *pack, int &packlen);
    int PackData(char *pack, int &packlen, char *devid);

private:
    bool m_bLogin_Status;
    bool m_bHeart_Success;
    int m_nLoginNum;
    int m_nHeartSpace;
    time_t m_nHeartSendDt;
    time_t m_nHeartAckRecvDt;
    CTcpClient m_TcpClient;

private:
    char m_DeviceId[DEVICE_ID_LEN + 1]; //车的ID
    int m_DeviceType;
    char m_BindId[DEVICE_ID_LEN + 1];       // 远程驾驶过程中绑定的舱id
    char m_StaticBindId[DEVICE_ID_LEN + 1]; // 下发的静态绑定信息
    map<unsigned int, canpack *> m_SendMap;

public:
    uint8 m_RecvBuf[BUFF_LEN * 2];
    uint8 m_hwt_buf[HWT_FRAME_SIZE];
    unsigned int m_RecvLen;
    int m_nSeqCount;
};

void RecvFromCloud(uint8 *data, unsigned int len, void *arg);

#endif