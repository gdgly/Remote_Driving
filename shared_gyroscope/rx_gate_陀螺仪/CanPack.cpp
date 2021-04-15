#include "CanPack.h"
#include "./NET/CloudComm.h"

extern CloudComm g_CloudComm;

//1、网络中数据传输采用 Intel 格式（小端模式）进行传输，超过 1 字节数据时先发送低字节后发送高 字节。 
 
//2、CAN 消息中协议值（Raw Value）、实际物理值（Physical Value）、比例尺（Factor）、偏移值（Offset） 的计算方法如下： 

//		实际物理值 = （协议值*比例尺）+ 偏移值 

//		协议值 =（实际物理值 - 偏移值）/比例尺 

//例：比例尺为 0.1A/bit，偏移值为-100A，实际电流值为 500A，则实际发送的协议值为：

//		Raw Value = (500+100)/0.1 = 6000 =0x1770 

//比例尺为 1℃/bit，偏移值为-40℃，发送协议值为 70，则实际对应物理值为：

//		Physical Value = (70*1)-40 = 30℃ 

//0x1806A0B0 行车状态1

int VehicleState1(uint8 *data)
{
	// 车速 bit16 ~ bit23  == data[2]
	g_CloudComm.AddKeyValue("ms",(((data[0]&0x03)> 0 ) ? 1:0));
	g_CloudComm.AddKeyValue("cmzt",(data[0]>>2)&0x01);
	g_CloudComm.AddKeyValue("zzxd",(data[0]&0x10)>>4);
	g_CloudComm.AddKeyValue("yzxd",(data[0]&0x20)>>5);
	g_CloudComm.AddKeyValue("skd",(data[0]&0x80)>>7);
	g_CloudComm.AddKeyValue("jgd",data[1]&0x01);
	g_CloudComm.AddKeyValue("ygd",(data[1]&0x02) >> 1);
	g_CloudComm.AddKeyValue("cs",data[2]-50);
	g_CloudComm.AddKeyValue("soc",(int)(data[3] * 0.5));
	g_CloudComm.AddKeyValue("cdzt",(data[4]>>2)&0x03);

	return 0;
};
// 0x1804A0B0 行车状态
int DrivingState(uint8 *data)
{
	//实际物理值 = （协议值*比例尺）+ 偏移值 
	g_CloudComm.AddKeyValue("p",data[0]&0x03);
	g_CloudComm.AddKeyValue("dw",(data[0]>>2)&0x0F);
	g_CloudComm.AddKeyValue("zs",(data[1]|(data[2]<<8))-15000);
	return 0;
};
// 0x1802A0B0
int VCUEPSState(uint8 *data)
{
	g_CloudComm.AddKeyValue("zj",(int)((data[3]|(data[4]<<8))*0.1) - 1080);
	g_CloudComm.AddKeyValue("eps",data[7]);
	return 0;
};

// 0x1812A0B0 行车状态4
int VehicleState4(uint8 *data)
{
	g_CloudComm.AddKeyValue("wd",((int)(data[1]*0.5) - 30));
	g_CloudComm.AddKeyValue("kt",(((data[2]&0x0F)> 0 ) ? 1:0));
	g_CloudComm.AddKeyValue("xhlc",(data[3]|((data[4]&0x0F)<<8)));
	return 0;
};
