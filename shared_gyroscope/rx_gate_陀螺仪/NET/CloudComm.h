#ifndef _CLOUD_COMM_H_
#define _CLOUD_COMM_H_

#include <iostream>
#include <string.h>
#include <list>
#include <map>
#include <pthread.h>
#include "./../CANNet.h"
#include "TcpClient.h"
#include "../COMM/sky_trace.h"

#define CABIN_ID_FOLDER_PATH "./rx_gate"
#define CABIN_ID_FILE_PATH "./rx_gate/cabid"

using namespace std;
typedef map<string, string> TStrStrMap;
typedef pair<string, string> TStrStrPair;

#define DEVICE_ID_LEN 17
#define FLAG_COMMAND_NEED_RESPONSE 0xFE
#define FLAG_COMMAND_NO_NEED_RESPONSE 0xA0
#define EPS_STEP_THRESHOLD 3
#define COUNT_MARK_STRONG 100
#define COUNT_MARK_WEAK 5
#define REMOTE_SENSING_DATA_LENTG 80

class canpack
{
public:
    char buf[13];
};

typedef struct stDashboards
{
    bool refresh;
    int cs;
    int soc;
    int p;
    int dw;
    int ms;
    int zzxd;
    int yzxd;
    int zs;
    int zj;
    int xhlc;
    int jgd;
    int ygd;
    int skd;
    int ygq;
    int aqd;
    int xhqd;
    int kt;
    int wd;
    int cmzt;
    int cdzt;
    int eps;
} tstDashboards;

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
    int Status(void);
    int AddSendData(int dataid, char *data);
    int AddDriverModeData(string vin, uint8 mode);
    int SendDriverModeData(string vin);
    void SendDriverModeToCentreconsole();
    int SendToCloud(void);
    int CabinTakeover(char *vin);
    int CabinUnTakeover(char *vin);
    int RequestBindingRelation(void);
    int GetVehName(string vin, string &vname);
    void CloudUntakeoverPostProcess(void);

public:
    int FrameCheck(void);
    void Analysis(int cmd, int flag, unsigned int packlen, uint8 *data, unsigned int datalen);
    void GetBindingRelation(int cmd, unsigned int packlen, uint8 *data, unsigned int datalen); 

private:
    void GetSendData(char *packdata, int &packlen);
    void ClearSendData(void);
    void DestroySendData(void);
    int PackHead(char *pack, int &packlen, sint8 cmd, sint8 flag);
    int PackData(char *pack, int &packlen, int devtype, char *devid);
    int PackData(char *pack, int &packlen, char *src, char *des, char *data, unsigned int datalen);
    int PackData(char *pack, int &packlen, char *src, char *des);
    int PackTail(char *pack, int &packlen);
    int PackData(char *pack, int &packlen, char *devid);

public:
    int AddKeyValue(string name, int value);

private:
    void InitDashboards(void);
    string GetString(void);

private:
    bool m_bLogin_Status;
    bool m_bHeart_Success;
    int m_nLoginNum;
    int m_nHeartSpace;
    time_t m_nHeartSendDt;
    time_t m_nHeartAckRecvDt;
    CTcpClient m_TcpClient;
    bool m_bCabinTakeover_status;

private:
    char m_DeviceId[DEVICE_ID_LEN + 1];
    int m_DeviceType;
    char m_BindId[DEVICE_ID_LEN + 1];
    map<unsigned int, canpack *> m_SendMap;
    map<string, uint8> m_DriverModeMap;
    list<string> m_staticBindIds;
    TStrStrMap m_VehInfoMap;

private:
    pthread_mutex_t thread_mutex;
    clock_t m_nReportDt;
    time_t m_nStatusDt;
    tstDashboards m_DashboardsData;
    int m_nCountStrong;
    int m_nCountWeak;
    int m_nOldEps;

public:
    uint8 m_RecvBuf[BUFF_LEN * 2];
    unsigned int m_RecvLen;
    int m_nSeqCount;
    uint8 m_TakeoverFlag;
    uint8 m_unTakeoverFlag;
    uint8 m_unTakeoverFlag_cloud;
};

void RecvFromCloud(uint8 *data, unsigned int len, void *arg);

#endif