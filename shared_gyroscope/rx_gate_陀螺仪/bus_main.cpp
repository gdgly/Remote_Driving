#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <signal.h>

#include "./NTU_F9P/NMEA.h"
#include "./NTU_F9P/Passage.h"
#include "./MX61/SkyHardwareMX61Lib.h"
#include "./NET/UdpClient.h"
#include "./NET/UdpServer.h"
#include "COMM/sky_trace.h"
#include "bus_pack.h"
#include "CANNet.h"
#include "./NET/CloudComm.h"
//#include "./YK/can_msg_process.h"

using namespace std;

CUdpClient g_Udp2Client_Dashboard;
CUdpServer g_Udp2Server_Dashboard; // the Dashboard server

CUdpClient g_Udp2Client_Centreconsole;
CUdpServer g_Udp2Server_Centreconsole; // the Centreconsole server

CUdpClient g_Udp2Client_RemoteSensing;

CloudComm g_CloudComm;

sint8 g_imea[20] = "855585021100001";
char *vin_untakeover;

extern uint32 gComNumber;
extern uint32 gComBaud;
extern uint32 gCanNumber;
extern uint32 gCanBaud;
extern sint8 gCloudIpStr[16];
extern uint32 gCloudIp;
extern uint32 gCloudPort;
extern sint8 gDashboardClientIpStr[16];
extern sint32 gDashboardClientIp;
extern uint32 gDashboardClientPort;
extern sint8 gRemoteIpStr[16];
extern uint32 gRemoteIp;
extern uint32 gRemotePort;
extern sint8 gVehicleIDStr[32];
extern sint8 gIMEINum[16];

extern sint8 gCentreconsoleClientIpStr[16];
extern uint32 gCentreconsoleClientIp;
extern uint32 gCentreconsoleClientPort;
extern uint32 gCentreconsoleServerPort;
extern uint32 gDashboardServerPort;
extern uint32 gComNumberPassage;
extern uint32 gComBaudPassage;
uint32 gDashboardClientPort_default_value;
//////////////////////////////////////////////////////////////////////////////

#define MAINVER 1
#define SUBVER1 0
#define SUBVER2 5
#define STR(s) #s
#define VERSION(a, b, c) "rx_gate V" STR(a) "." STR(b) "." STR(c) " "__DATE__ \
                                                                  " "__TIME__

CUdpClient udp_sky_cloud;
FILE *pFile_vlist = NULL;

extern void Udp2Client_Dashboard_RecvCallBack(sint8 *buf, uint32 len, struct sockaddr *src_addr, uint32 addr_len);

// void Udp2Client_Dashboard_RecvCallBack(sint8 * buf, uint32 len, struct sockaddr * src_addr, uint32 addr_len)
// {
//     int i;
//     i=1;

// }

// void Udp2Client_Centreconsole_RecvCallBack(sint8 * buf, uint32 len, struct sockaddr * src_addr, uint32 addr_len)
// {
//     return;
// }

void start_Centreconsole_client(struct sockaddr_in *src_addr)     //保存作为TCP server接受数据时获取对端client的ip
{
    gDashboardClientPort = gDashboardClientPort_default_value;    //防止Port被已经启动过的Dashboard改动过，先用二合一时，仪表盘的端口会被更改，

    strcpy(gCentreconsoleClientIpStr, inet_ntoa(src_addr->sin_addr));
    SKY_TRACE_INFO("Centreconsole ip is [%s] %s:%d\n\n", __FUNCTION__, gCentreconsoleClientIpStr, gCentreconsoleClientPort);
    g_Udp2Client_Centreconsole.set_read_callback_func(NULL);
    g_Udp2Client_Centreconsole.init_udp_client(gCentreconsoleClientIpStr, gCentreconsoleClientPort);
}

void start_Dashboard_client(struct sockaddr_in *src_addr)
{
    strcpy(gDashboardClientIpStr, inet_ntoa(src_addr->sin_addr));
    if (strcmp(gDashboardClientIpStr, gCentreconsoleClientIpStr) == 0)   //二合一版本中，先按照中控的协议请求Vlist
    {
        gDashboardClientPort = gCentreconsoleClientPort;
    }
    SKY_TRACE_INFO("[%s] Dashboard ip is %s:%d\n", __FUNCTION__, gDashboardClientIpStr, gDashboardClientPort);
    g_Udp2Client_Dashboard.set_read_callback_func(Udp2Client_Dashboard_RecvCallBack);
    g_Udp2Client_Dashboard.init_udp_client(gDashboardClientIpStr, gDashboardClientPort);
}

void Udp2Server_RecvCallBack(sint8 *buf, uint32 len, struct sockaddr_in *src_addr, uint32 addr_len)
{
    size_t count;
    size_t nmemb = 1;
    sint8 send_buf[BUFF_LEN] = {0};
    int ret = 0;
    char head[] = "cmd:vlist;data$";

    SKY_TRACE_INFO("[%s] buf:%s \n", __FUNCTION__, buf);
    memset(send_buf, 0, sizeof(send_buf));
    if (strcmp(buf, "cmd:vlist") == 0)
    {
        start_Centreconsole_client(src_addr);
        pFile_vlist = fopen(CABIN_ID_FILE_PATH, "rb");
        SKY_TRACE_INFO("pFile_vlist:%p\n", pFile_vlist);
        if (NULL != pFile_vlist)
        {
            strcpy(send_buf, head);
            count = fread(send_buf + strlen(head), sizeof(send_buf) - strlen(head), nmemb, pFile_vlist);
            SKY_TRACE_INFO("read %d bytes from %s .\n", strlen(send_buf), CABIN_ID_FILE_PATH);
            ret = g_Udp2Server_Centreconsole.send_udp_data(send_buf, strlen(send_buf), src_addr, addr_len);
            if (count < nmemb)
            {
                fclose(pFile_vlist);
                pFile_vlist = NULL;
            }
            g_CloudComm.SendDriverModeToCentreconsole();
        }
    }
    else if (strcmp(buf, "cmd:vlist:rev_ok") == 0)
    {
        memset(send_buf, 0, sizeof(send_buf));

        if (pFile_vlist != NULL)
        {
            count = fread(send_buf, BUFF_LEN, nmemb, pFile_vlist);
            if (count < nmemb)
            {
                fclose(pFile_vlist);
                pFile_vlist = NULL;
            }
        }
        else
        {
            strcpy(send_buf, "cmd:vlist:done");
        }

        g_Udp2Server_Centreconsole.send_udp_data(send_buf, strlen(send_buf), src_addr, addr_len);
    }
    else if (strstr(buf, "cmd:takeover;vin:"))
    {
        char *vin;
        vin = strrchr(buf, ':');
        vin++;
        g_CloudComm.CabinTakeover(vin);
        g_CloudComm.m_unTakeoverFlag = 0;
    }
    else if (strstr(buf, "cmd:untakeover;vin:"))
    {
        // char *vin;
        vin_untakeover = strrchr(buf, ':');
        vin_untakeover++;
        g_CloudComm.m_TakeoverFlag = 0;
        g_CloudComm.m_unTakeoverFlag = 1;
        // g_CloudComm.CabinUnTakeover(vin);
    }
    else if (strcmp(buf, "cmd:dashboard_start") == 0)
    {
        char head[] = "cmd:dashboard_start;data";
        sprintf(send_buf, "%s:%s", head, "cabin_recv");
        g_Udp2Server_Dashboard.send_udp_data(send_buf, strlen(send_buf), src_addr, addr_len);

        start_Dashboard_client(src_addr);
    }
    else
    {
        SKY_TRACE_INFO("[%s] unknow command: %s\n", __FUNCTION__, buf);
    }

    SKY_TRACE_INFO("exit from %s.\n\n", __FUNCTION__);
}

void Udp2Server_Dashboard_RecvCallBack(sint8 *buf, uint32 len, struct sockaddr_in *src_addr, uint32 addr_len)
{
    sint8 send_buf[BUFF_LEN] = {0};
    char head[] = "cmd:dashboard_start;data";

    SKY_TRACE_INFO("[%s] buf: %s \n", __FUNCTION__, buf);
    PRINTF("[%s] buf: %s \n", __FUNCTION__, buf);
    memset(send_buf, 0, sizeof(send_buf));
    if (strcmp("cmd:dashboard_start", buf) == 0)
    {
        sprintf(send_buf, "%s:%s", head, "cabin_recv");
        g_Udp2Server_Dashboard.send_udp_data(send_buf, strlen(send_buf), src_addr, addr_len);

        start_Dashboard_client(src_addr);
    }
    else
    {
        SKY_TRACE_INFO("[%s] unknow command: %s\n", __FUNCTION__, buf);
    }

    SKY_TRACE_INFO("exit from Udp_2_CentreconsoleSer_RecvCallBack.\n\n");
}

void Can2CallBack(MCANDrv_infoMsgS_TYPE infMsgS)
{
    sint32 i;

    SKY_TRACE_INFO("[RECEIVE] Can data len is %d\n", infMsgS.infoConfS.infoMsgParU.B.DLC);

    for (i = 0; i < infMsgS.infoConfS.infoMsgParU.B.DLC; i++)
    {
        SKY_TRACE_INFO("0x%02x ", infMsgS.datMsgUBA[i]);
    }
    SKY_TRACE_INFO("\n");

    if (infMsgS.infoConfS.infoMsgParU.B.IDE)
    {
        SKY_TRACE_INFO("can frame is SFF\n");
    }
    else
    {
        SKY_TRACE_INFO("can frame is EFF\n");
    }
    SKY_TRACE_INFO("Can id is 0x%08x \n", infMsgS.infoConfS.infoMsgIDUI);
}

void can_send_test(void)
{
    int i;
    MCANDrv_infoMsgS_TYPE infMsgS;

    memset(&infMsgS, 0, sizeof(MCANDrv_infoMsgS_TYPE));
    for (i = 0; i < 8; i++)
        infMsgS.datMsgUBA[i] = (i + 1) * 0x11;
    infMsgS.infoConfS.infoMsgIDUI = 0x123;
    infMsgS.infoConfS.infoMsgParU.B.CAN = gCanNumber;
    infMsgS.infoConfS.infoMsgParU.B.DLC = 8;
    infMsgS.infoConfS.infoMsgParU.B.IDE = 0;
    MCANDrv_SendMsg(infMsgS);
}

void init_device(void)
{
    sint8 buf[30] = {0};

    load_config_file();

    SKY_TRACE_INFO("gComNumber = %d\n", gComNumber);
    SKY_TRACE_INFO("gComBaud = %d\n", gComBaud);
    SKY_TRACE_INFO("gCanNumber = %d\n", gCanNumber);
    SKY_TRACE_INFO("gCanBaud = %d\n", gCanBaud);
    SKY_TRACE_INFO("gCloudIpStr = %s\n", gCloudIpStr);
    SKY_TRACE_INFO("gCloudIp = %08X\n", gCloudIp);
    SKY_TRACE_INFO("gCloudPort = %d\n", gCloudPort);
    SKY_TRACE_INFO("gVehicleIDStr = %s\n", gVehicleIDStr);
    SKY_TRACE_INFO("gIMEINum = %s\n", gIMEINum);
    SKY_TRACE_INFO("gComNumberPassage = %d\n", gComNumberPassage);
    SKY_TRACE_INFO("gComBaudPassage = %d\n", gComBaudPassage);
    SKY_TRACE_INFO("gDashboardServerPort = %d\n", gDashboardServerPort);
    SKY_TRACE_INFO("gDashboardClientIpStr = %s\n", gDashboardClientIpStr);
    SKY_TRACE_INFO("gDashboardClientIp = %08X\n", gDashboardClientIp);
    SKY_TRACE_INFO("gDashboardClientPort = %d\n", gDashboardClientPort);
    SKY_TRACE_INFO("gCentreconsoleServerPort = %d\n", gCentreconsoleServerPort);
    SKY_TRACE_INFO("gCentreconsoleClientIpStr = %s\n", gCentreconsoleClientIpStr);
    SKY_TRACE_INFO("gCentreconsoleClientIp = %08X\n", gCentreconsoleClientIp);
    SKY_TRACE_INFO("gCentreconsoleClientPort = %d\n", gCentreconsoleClientPort);

    gDashboardClientPort_default_value = gDashboardClientPort;
    g_Udp2Server_Dashboard.set_read_callback_func(Udp2Server_RecvCallBack);
    g_Udp2Server_Dashboard.init_udp_server(gDashboardServerPort);

    g_Udp2Server_Centreconsole.set_read_callback_func(Udp2Server_RecvCallBack);
    g_Udp2Server_Centreconsole.init_udp_server(gCentreconsoleServerPort);

    // 云端通信
    memset(buf, 0, 30);
    strcpy(buf, gCloudIpStr);
    g_CloudComm.init(gVehicleIDStr, buf, gCloudPort);

    //CAN2初始化
    MCANDrv_SetBaudrate(gCanNumber, gCanBaud);
    MCANDrv_SetCallbackFunc(gCanNumber, CANNet_RecvMsgCAN_2, NULL);
}

int main()
{
    trace_init(TRACE_LEVEL_DEBUG);
    struct sigaction sa;
    sa.sa_handler = SIG_IGN;
    sigaction(SIGPIPE, &sa, 0);

    printf("%s\n\n", VERSION(MAINVER, SUBVER1, SUBVER2));
    SKY_TRACE_INFO("%s\n", VERSION(MAINVER, SUBVER1, SUBVER2));
    init_device();

    trace_comm_clear_logfile();

    cout << "Running ..." << endl;
    while (1)
    {
        //状态指示灯
        DORrv_TurnOnLed(0, 1);
        usleep(1000 * 1000);
        DORrv_TurnOnLed(0, 0);
        usleep(1000 * 1000);
    }
}
