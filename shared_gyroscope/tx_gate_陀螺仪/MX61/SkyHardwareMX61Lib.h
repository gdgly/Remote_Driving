#ifndef __SKYHARDWARE_MX61_LIB__
#define __SKYHARDWARE_MX61_LIB__



	//#include "Std_Types.h"
	#include "../Std_Types.h"

    //模拟量输入
	//输入范围0-5V
    #define _ATD_NUM_CHAN			2
    
    #define _ATD_INDEX_CHAN1 		0   //通道1，对应接插件PIN34
    #define _ATD_INDEX_CHAN2 		1   //通道2, 对应接插件PIN35

    uint16 ATDDrv_GetChanResult(uint8 idxChanUB); //得到上述通道的电压值，电压值精度1/4095V/bit


    //数字量输入
    #define _DIN_NUM_CHAN 			6

    #define _DIN_OPENLVL_LOW_STATE  0
    #define _DIN_OPENLVL_HIGH_STATE 1

    #define _DIN_INDEX_BAK0_CHAN    0   //开关1信号，对应接插件PIN2
    #define _DIN_INDEX_BAK1_CHAN    1   //开关2信号，对应接插件PIN3
    #define _DIN_INDEX_BAK2_CHAN    2   //开关3信号，对应接插件PIN4
    #define _DIN_INDEX_BAK3_CHAN    3   //开关4信号，对应接插件PIN5
    #define _DIN_INDEX_BAK4_CHAN    4   //开关5信号，对应接插件PIN6
    #define _DIN_INDEX_BAK5_CHAN    5   //开关6信号，对应接插件PIN7

    //void DINDrv_SetChanThres(uint8 idxChanUB, uint8 stOpenLvlUB, uint16 uThresUW);		//不需要设置
    uint8 DINDrv_GetChanState(uint8 idxChanUB); //得到上述通道开关状态

    //数字量输出
    #define _DO_NUM_CHAN  			4

    #define _DO_INDEX_BAK1_CHAN    	1 	//输出信号，对应接插件PIN8
    #define _DO_INDEX_BAK2_CHAN    	2 	//输出信号，对应接插件PIN9
    #define _DO_INDEX_BAK3_CHAN    	3 	//输出信号，对应接插件PIN22
    #define _DO_INDEX_BAK4_CHAN    	4 	//输出信号，对应接插件PIN23

    void DODrv_SetChanState(uint8 idxChanUB, uint8 bLvlUB); //bLvlUB表示电平状态，0为低电平，1为高电平

	//LED灯控制
	uint8 DORrv_TurnOnLed(uint8 led_index, uint8 lamp_status);


    //CAN通讯接口
    #define _MCAN_NUM_CHAN     4
    /*
     * CAN0具备64个收发缓存,带电源隔离,终端电阻通过线束选择
	 * CAN1具备64个收发缓存,带电源隔离,终端电阻通过线束选择
     * CAN2具备2个收发缓存,其中MO24~31由底层CCP、UDS等协议占用
	 * CAN3具备2个收发缓存
     */

    #define _MCAN_DIR_RX    0
    #define _MCAN_DIR_TX    1

    #define _MCAN_IDE_STD    0
    #define _MCAN_IDE_EXT    1

    typedef enum
    {
        MCAN_Status_ok                = 0x00000000,
        MCAN_Status_notInitialised    = 0x00000001,
        MCAN_Status_wrongParam        = 0x00000002,
        MCAN_Status_wrongPin          = 0x00000004,
        MCAN_Status_busHeavy          = 0x00000008,
        MCAN_Status_busOff            = 0x00000010,
        MCAN_Status_notSentBusy       = 0x00000020,
        MCAN_Status_receiveEmpty      = 0x00000040,
        MCAN_Status_messageLost       = 0x00000080,
        MCAN_Status_newData           = 0x00000100,
        MCAN_Status_newDataButOneLost = MCAN_Status_messageLost | MCAN_Status_newData
    } MCAN_stRetE_TYPE;

    typedef enum
    {
        MCAN_NODEMODE_MO = 0,  //MO模式下，每个MO对应一个具体的报文
        MCAN_NODEMODE_BUF = 1, //BUF模式，接收使用一个MO处理，其它MO成为发送缓存
        MCAN_NODEMODE_UNKNOWN = 0xFF,
    } MCANDrv_idxNodeModeE_TYPE;

    typedef union
    {
        uint32 U;
        struct
        {
            uint32 DLC       : 8; //数据长度
            uint32 DIR       : 1; //0为接收，1为发送
            uint32 IDE       : 1; //0为标准帧，1为扩展帧
            uint32 Reserved  : 6;
            uint32 CAN       : 8; //CAN通道号
            uint32 MO        : 8; //MO序号
        } B;
    } MCANDrv_infoMsgParU_TYPE;

    typedef struct 
    {
        MCANDrv_infoMsgParU_TYPE infoMsgParU;
        uint32 infoMsgIDUI;
    } MCANDrv_infoMsgConfS_TYPE;

    typedef struct
    {
        MCANDrv_infoMsgConfS_TYPE infoConfS;
        uint8  datMsgUBA[8];
    } MCANDrv_infoMsgS_TYPE;

    typedef void (*pFunc_TxMsgCallback)(void);
    typedef void (*pFunc_RxMsgCallback)(MCANDrv_infoMsgS_TYPE infoMsgS);

    /*CAN设置函数*/
    uint8 MCANDrv_SetBaudrate(uint8 idxCANUB, uint32 baudrateUI);
    uint8 MCANDrv_SetCANMode(uint8 idxCANUB, MCANDrv_idxNodeModeE_TYPE idxNodeModeE);
    uint8 MCANDrv_SetMsgObj(const MCANDrv_infoMsgConfS_TYPE infoMsgConfS, void *pFunc);
    uint8 MCANDrv_SetCallbackFunc(uint8 idxCANUB, const pFunc_RxMsgCallback pRxFunc, const pFunc_TxMsgCallback pTxFunc);
    
    /*CAN操作函数*/
    MCAN_stRetE_TYPE MCANDrv_SendMsg(const MCANDrv_infoMsgS_TYPE infMsgS);

    uint8 MCANDrv_CheckIfBusOff(uint8 idxCANUB); //返回1表示有busoff错误



    //RS232通讯接口
    #define _MRS232_NUM_CHAN     4

    typedef void (*pFunc_ReadCallback)(sint8 * buf, sint32 count);

	uint8 MRS232Drv_SetBaudrate(uint8 idxRS232UB, uint32 baudrateUI);
    uint8 MRS232Drv_SetCallbackFunc(uint8 idxRS232UB, const pFunc_ReadCallback pRxFunc);

    uint8 MRS232Drv_SendData(uint8 idxRS232UB, sint8 * RS232Data, const sint32 DataLength);

    uint8 MRS232Drv_ClosePort(uint8 idxRS232UB);
	

#endif /* defined(__SKYHARDWARE_MX61_LIB__) */