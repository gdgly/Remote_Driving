#include    "NMEA.h"
//#include    "Std_Types.h"
#include    <string.h>
#include    <stdlib.h>
#include    <stdio.h> 
#include    <math.h> 
#include "../COMM/sky_trace.h"


#define     MAX_SIZE_F9P        1024

#define     MAX_GPS_FRAME_SIZE      96
#define     MIN_GPS_FRAME_SIZE      10

sint8 read_buf[MAX_SIZE_F9P+1];
uint32 position = 0;

void AddStringToBuf(sint8 * buf, sint32 count)
{
    if(position >= MAX_SIZE_F9P)        //缓冲区满了
    {
        position = MAX_SIZE_F9P;
        //printf("Buf is full");
        return;
    }

    if((position + count) >= MAX_SIZE_F9P)
        count = MAX_SIZE_F9P - position ;

    memcpy(read_buf + position, buf , count);

    position += count;
    //read_buf[position] = 0;
    //printf("Buf is %s\n", read_buf);
}


uint32 Get_GPSData(sint8 * buf)
{
	int i, j;

	if (position < MIN_GPS_FRAME_SIZE)
		return 0;

	for (i = 0; i<position; i++)
	{
		if (read_buf[i] == '$')
			break;
	}

	if (i == position) //没有开头$标记
	{
		position = 0;   //丢弃数据
		return -1;
	}

	if ((position - i)< MIN_GPS_FRAME_SIZE)
	{
		memcpy(read_buf, read_buf + i, position - i); //把头移动到最开始
		position = position - i;
		return -2;
	}

	for (j = i; j < position; j++)
	{
		if (read_buf[j] == '*')
			break;
	}

	if (j == position)   //没找到尾*
	{
		if ((j - i) >= MAX_GPS_FRAME_SIZE)
		{
			memcpy(read_buf, read_buf + position - MAX_GPS_FRAME_SIZE + 1, MAX_GPS_FRAME_SIZE - 1); //移动剩余数据
			position = MAX_GPS_FRAME_SIZE - 1;
		}
		else
		{
			memcpy(read_buf, read_buf + i, position - i); //移动剩余数据
			position = position - i;
		}
		return -3;
	}

	//找到一帧GPS数据
	if ((j - i) >= MAX_GPS_FRAME_SIZE)     //要求最长帧小于96
	{
		position = position - j - 1;
		if (position > 0)
			memcpy(read_buf, read_buf + j + 1, position); //移动剩余数据,*不复制
		return -4;
	}
	else
	{
		if ((j + 2) < position)
		{
			memcpy(buf, read_buf + i, j - i + 3);   //COPY一帧GPS数据
			buf[j - i + 3] = 0;
			memcpy(read_buf, read_buf + j + 3, position - j - 3);   //去除有效数据头前的无效数据
			position = position - j - 3;

            return (j - i + 3);       //返回字符串长度
			//return 1;
		}
		else    //还差校验和两字节
		{
			memcpy(read_buf, read_buf + i, position - i);   //去除有效数据头前的无效数据
			position = position - i;
			return -5;
		}
	}
}


/**
 * \brief Calculate control sum of binary buffer
 */
uint8 nmea_calc_crc(const sint8 *buff, uint32 buff_sz)
{
    uint32 chsum = 0,  i;

    for(i = 0; i < buff_sz; ++i)
        chsum ^= (uint32)buff[i];

    return (uint8) chsum;
}


static uint32 GetComma(uint32 num, sint8 *str)
{
    uint32 i, j = 0;
    uint32 len = strlen(str);
    for (i = 0; i < len; i++)
    {
        if (str[i] == ',')
            j++;
        if (j == num)
            return i + 1; /*返回当前找到的逗号位置的下一个位置*/
    }
    return 0;
}

static float64 get_double_number(sint8 *s)
{
    sint8 buf[128];
    uint32 i;
    float64 rev;
    i = GetComma(1, s); /*得到数据长度 */
    strncpy(buf, s, i);     //字符串最后多个逗号，atof函数自动丢弃
    buf[i] = 0;      /*加字符串结束标志*/
    rev = atof(buf); /*字符串转float，一定要包含头文件<stdlib.h> */
    return rev;
}

static float64 get_double_latitude(sint8 *s)    //纬度计算
{
    sint8 buf[128];
    uint32 i, deg;
    float64 rev;
    i = GetComma(1, s);     /*得到数据长度 */
    if(i < 8)
        return 0;

    strncpy(buf, s, 2);     //复制度，2位
    buf[2] = 0;
    deg = atoi(buf);

    strncpy(buf, s+2, i);   //字符串最后多个逗号，atof函数自动丢弃
    buf[i-2] = 0;           /*加字符串结束标志*/
    rev = atof(buf);        /*字符串转float，一定要包含头文件<stdlib.h> */
    rev = rev /60 + deg;

    return rev;
}

static float64 get_double_longitude(sint8 *s)   //经度计算
{
    sint8 buf[128];
    uint32 i, deg;
    float64 rev;
    i = GetComma(1, s);     /*得到数据长度 */
    if(i < 9)
        return 0;

    strncpy(buf, s, 3);     //复制度，3位
    buf[3] = 0;
    deg = atoi(buf);

    strncpy(buf, s+3, i);   //字符串最后多个逗号，atof函数自动丢弃
    buf[i-3] = 0;           /*加字符串结束标志*/
    rev = atof(buf);        /*字符串转float，一定要包含头文件<stdlib.h> */
    rev = rev /60 + deg;

    return rev;
}

static uint32 get_int_number(sint8 *s)
{
    sint8 buf[128];
    uint32 i;
    float64 rev;
    i = GetComma(1, s); /*得到数据长度*/
    strncpy(buf, s, i);
    buf[i] = 0;      /*加字符串结束标志 */
    rev = atoi(buf); /*字符串转int */
    return rev;
}

static void UTC2BTC(date_time *GPS)
{
    /*如果秒号先出,再出时间数据,则将时间数据+1秒 */
    GPS->second++; /*加一秒*/
    if (GPS->second > 59)
    {
        GPS->second = 0;
        GPS->minute++;
        if (GPS->minute > 59)
        {
            GPS->minute = 0;
            GPS->hour++;
        }
    }
    GPS->hour += 8; /*北京时间跟UTC时间相隔8小时 */
    if (GPS->hour > 23)
    {
        GPS->hour -= 24;
        GPS->day += 1;
        if (GPS->month == 2 || GPS->month == 4 || GPS->month == 6 || GPS->month == 9 || GPS->month == 11)
        {
            if (GPS->day > 30)
            { /*上述几个月份是30天每月，2月份还不足30*/
                GPS->day = 1;
                GPS->month++;
            }
        }
        else
        {
            if (GPS->day > 31)
            { /*剩下的几个月份都是31天每月 */
                GPS->day = 1;
                GPS->month++;
            }
        }
        if (GPS->year % 4 == 0)
        {
            if (GPS->day > 29 && GPS->month == 2)
            { /*闰年的二月是29天*/
                GPS->day = 1;
                GPS->month++;
            }
        }
        else
        {
            if (GPS->day > 28 && GPS->month == 2)
            { /*其他的二月是28天每月*/
                GPS->day = 1;
                GPS->month++;
            }
        }
        if (GPS->month > 12)
        {
            GPS->month -= 12;
            GPS->year++;
        }
    }
}


sint8 gps_parse(sint8 *line, GPS_INFO *GPS)
{
    sint32 tmp;
    sint8 head_buf[6];
    sint8 *buf = line;

    memcpy(head_buf,line+1,5);
    head_buf[5] = '\0';

    if (strcmp(head_buf,"GPGGA") == 0) /*"$GPGGA" */
    {
        GPS->latitude = get_double_latitude(&buf[GetComma(2, buf)]);        /*纬度*/
        GPS->NS = buf[GetComma(3, buf)];                                    /*南北纬 */
        GPS->longitude = get_double_longitude(&buf[GetComma(4, buf)]);      /*经度*/
        GPS->EW = buf[GetComma(5, buf)];                                    /*东西经 */
        GPS->calu_status = buf[GetComma(6, buf)] - '0';                     /*定位质量指示*/
        GPS->num = get_int_number(&buf[GetComma(7, buf)]);                  /*可用卫星数目*/
        GPS->high = get_double_number(&buf[GetComma(9, buf)]);              /*天线离海平面的高度*/

        return 0;
    }
    return -1;
}

void show_gps(GPS_INFO *GPS)
{
    SKY_TRACE_INFO("year     : %d-%02d-%02d\n", GPS->D.year, GPS->D.month, GPS->D.day);
    SKY_TRACE_INFO("time     : %02d:%02d:%02d\n", GPS->D.hour, GPS->D.minute, GPS->D.second);
    SKY_TRACE_INFO("latitude     : %s %10.6f\n", (GPS->NS == 'N') ? "N" : "S", GPS->latitude);
    SKY_TRACE_INFO("longitude     : %s %10.6f\n", (GPS->EW == 'W') ? "W" : "E", GPS->longitude);
    SKY_TRACE_INFO("high     : %.4f\n", GPS->high);
    SKY_TRACE_INFO("status     : %s\n", (GPS->status == 'A') ? "P" : "A");
    SKY_TRACE_INFO("NUM   : %02d\n", GPS->num);
    SKY_TRACE_INFO("calculate status     : %d\n", GPS->calu_status);
    SKY_TRACE_INFO("--------------------\n");
}

/*
判断是否是有效的NMEA帧
参数：字符串的指针，字符串的长度
返回：1：有效帧， 0：无效帧
*/
sint32 nmea_frame_check(sint8 *line, uint32 len)
{
    uint8 sum;
    sint8 buf[4]={0};
    sint8 c;

    if(len < MIN_GPS_FRAME_SIZE)
        return -1;

    sum = nmea_calc_crc(line + 1, len - 4);
    sprintf(buf,"%02X",sum);            //把校验和转换为16进制

    if((buf[0] != line[len-2])||(buf[1] != line[len-1]))
        return -2;

    c = line[5];

    //根据帧长，判断接收状态
    if (c == 'A') /* "GPGGA" */
    {
        if(len < 45)
            return -4;
    }

    return 1;
}

typedef struct tagCRDCARTESIAN
{
	double x;
	double y;
	double z;
} CRDCARTESIAN, *PCRDCARTESIAN;
//大地坐标系
typedef struct tagCRDGEODETIC
{
	double longitude; //经度
	double latitude;  //纬度
	double height;    //大地高,可设为0
} CRDGEODETIC;
typedef CRDGEODETIC *PCRDGEODETIC;

void GeodeticToCartesian(PCRDCARTESIAN pcc, PCRDGEODETIC pcg)
{
	double B;  //纬度度数
	double L;  //经度度数
	double L0; //中央经线度数
	double l;  //L-L0
	double t;  //tanB
	double m;  //ltanB
	double N;  //卯酉圈曲率半径
	double q2;
	double x;  //高斯平面纵坐标
	double y;  //高斯平面横坐标
	double s;  //赤道至纬度B的经线弧长
	double f;  //参考椭球体扁率
	double e2; //椭球第一偏心率
	double a;  //参考椭球体长半轴
			   //double b; //参考椭球体短半轴
	double a1;
	double a2;
	double a3;
	double a4;
	double b1;
	double b2;
	double b3;
	double b4;
	double c0;
	double c1;
	double c2;
	double c3;
	double LLL;
	int Datum = 84; //投影基准面类型：北京54基准面为54，西安80基准面为80，WGS84基准面为84  中国2000基准面2000
	int prjno = 0;  //投影带号
	int zonewide = 3;//时区宽度
	double IPI = 0.0174532925199433333333; //3.1415926535898/180.0
	B = pcg->latitude;                     //纬度
	L = pcg->longitude;                    //经度

	//LLL = L;
	LLL = 120; //固定度带号,中央子午线
	if (zonewide == 6)
	{
		prjno = (int)(LLL / zonewide) + 1;
		L0 = prjno * zonewide - 3;
	}
	else
	{
		prjno = (int)((LLL - 1.5) / 3) + 1;
		L0 = prjno * 3;
	}

	if (Datum == 54)
	{
		a = 6378245;
		f = 1 / 298.3;
	}
	else if (Datum == 84)
	{
		a = 6378137;
		f = 1 / 298.257223563;
	}
	else if (Datum == 2000)
	{
		a = 6378137;
		f = 1 / 298.257222101;
	}

	L0 = L0 * IPI;
	L = L * IPI;
	B = B * IPI;

	e2 = 2 * f - f * f; //(a*a-b*b)/(a*a);
	l = L - L0;
	t = tan(B);
	m = l * cos(B);
	N = a / sqrt(1 - e2 * sin(B) * sin(B));
	q2 = e2 / (1 - e2) * cos(B) * cos(B);
	a1 = 1 + (double)3 / 4 * e2 + (double)45 / 64 * e2 * e2 + (double)175 / 256 * e2 * e2 * e2 + (double)11025 / 16384 * e2 * e2 * e2 * e2 + (double)43659 / 65536 * e2 * e2 * e2 * e2 * e2;
	a2 = (double)3 / 4 * e2 + (double)15 / 16 * e2 * e2 + (double)525 / 512 * e2 * e2 * e2 + (double)2205 / 2048 * e2 * e2 * e2 * e2 + (double)72765 / 65536 * e2 * e2 * e2 * e2 * e2;
	a3 = (double)15 / 64 * e2 * e2 + (double)105 / 256 * e2 * e2 * e2 + (double)2205 / 4096 * e2 * e2 * e2 * e2 + (double)10359 / 16384 * e2 * e2 * e2 * e2 * e2;
	a4 = (double)35 / 512 * e2 * e2 * e2 + (double)315 / 2048 * e2 * e2 * e2 * e2 + (double)31185 / 13072 * e2 * e2 * e2 * e2 * e2;
	b1 = a1 * a * (1 - e2);
	b2 = (double)-1 / 2 * a2 * a * (1 - e2);
	b3 = (double)1 / 4 * a3 * a * (1 - e2);
	b4 = (double)-1 / 6 * a4 * a * (1 - e2);
	c0 = b1;
	c1 = 2 * b2 + 4 * b3 + 6 * b4;
	c2 = -(8 * b3 + 32 * b4);
	c3 = 32 * b4;
	s = c0 * B + cos(B) * (c1 * sin(B) + c2 * sin(B) * sin(B) * sin(B) + c3 * sin(B) * sin(B) * sin(B) * sin(B) * sin(B));
	x = s + (double)1 / 2 * N * t * m * m + (double)1 / 24 * (5 - t * t + 9 * q2 + 4 * q2 * q2) * N * t * m * m * m * m + (double)1 / 720 * (61 - 58 * t * t + t * t * t * t) * N * t * m * m * m * m * m * m;
	y = N * m + (double)1 / 6 * (1 - t * t + q2) * N * m * m * m + (double)1 / 120 * (5 - 18 * t * t + t * t * t * t - 14 * q2 - 58 * q2 * t * t) * N * m * m * m * m * m;
	y = y + 1000000 * prjno + 500000;
	pcc->x = x;
	//pcc->y = y - 38000000;		//子午线114
	pcc->y=y-40000000;		//子午线120
	//pcc->y = y;
	pcc->z = 0;
}

void gps_bl_xy(double b, double l,double *x, double *y)
{
	CRDCARTESIAN pcc;
	CRDGEODETIC pcg;
	pcg.longitude = l;
	pcg.latitude = b;
	pcg.height = 0;

	GeodeticToCartesian(&pcc, &pcg);
	*x = pcc.x;
	*y = pcc.y;
}
