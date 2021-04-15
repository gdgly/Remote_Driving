#include <iostream>
#include <stdio.h>
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
#include "COMM/sky_trace.h"
#include "bus_pack.h"
#include "CANNet.h"
#include "HWT905.h"
#include "./NET/CloudComm.h"
//#include "./YK/can_msg_process.h"

using namespace std;

CUdpClient g_Udp2Client_Dashboard;
CloudComm g_CloudComm;

sint8 g_imea[20] = "855585021100001";

extern uint32 gComNumber;
extern uint32 gComBaud;
extern uint32 gCanNumber;
extern uint32 gCanBaud;
extern sint8 gCloudIpStr[16];
extern uint32 gCloudIp;
extern uint32 gCloudPort;
extern sint8 gRemoteIpStr[16];
extern uint32 gRemoteIp;
extern uint32 gRemotePort;
extern sint8 gVehicleIDStr[32];
extern sint8 gIMEINum[16];

extern uint32 gComNumberPassage;
extern uint32 gComBaudPassage;
//////////////////////////////////////////////////////////////////////////////

CUdpClient udp_sky_cloud;

#define MAINVER 1
#define SUBVER1 0
#define SUBVER2 5
#define STR(s) #s
#define VERSION(a, b, c) "tx_gate V" STR(a) "." STR(b) "." STR(c) " "__DATE__ \
                                                                  " "__TIME__

void Udp2Client_Dashboard_RecvCallBack(sint8 *buf, uint32 len, struct sockaddr *src_addr, uint32 addr_len)
{
    int i;
    i = 1;
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
    if (!trace_init(TRACE_LEVEL_DEBUG))
    {
        PRINTF("bus trace init failed!\n");
    }
    //for test
    SKY_TRACE_INFO("gComNumber = %d\n", gComNumber);
    SKY_TRACE_INFO("gComBaud = %d\n", gComBaud);
    SKY_TRACE_INFO("gCanNumber = %d\n", gCanNumber);
    SKY_TRACE_INFO("gCanBaud = %d\n", gCanBaud);
    SKY_TRACE_INFO("gCloudIpStr = %s\n", gCloudIpStr);
    SKY_TRACE_INFO("gCloudIp = %08X\n", gCloudIp);
    SKY_TRACE_INFO("gCloudPort = %d\n", gCloudPort);
    SKY_TRACE_INFO("gVehicleIDStr = %s\n", gVehicleIDStr);
    SKY_TRACE_INFO("gIMEINum = %s\n", gIMEINum);

    // 云端通信
    memset(buf, 0, 30);
    strcpy(buf, gCloudIpStr);
    g_CloudComm.init(gVehicleIDStr, buf, gCloudPort);

    //CAN2初始化
    MCANDrv_SetBaudrate(gCanNumber, gCanBaud);
    //MCANDrv_SetCallbackFunc(2,CANNet_RecvMsgCAN_2,NULL);
    MCANDrv_SetCallbackFunc(gCanNumber, CANNet_RecvMsgCAN_2, NULL);

     //串口初始化
    MRS232Drv_SetBaudrate(gComNumber, gComBaud);
    MRS232Drv_SetCallbackFunc(gComNumber, RS232_HWT905_CallBack);
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
    //send_test();

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
