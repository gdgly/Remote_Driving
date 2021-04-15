#include    "Passage.h"
//#include    "Std_Types.h"
#include    <string.h>
#include    <stdlib.h>
#include    <stdio.h> 
#include    <math.h> 
#include "../COMM/sky_trace.h"


#define     MAX_BUF_SIZE_PASSAGE            1024
#define     MIN_PASSAGE_FRAME_SIZE          30
#define		MAX_PASSAGE_FRAME_SIZE			200

sint8 read_buf_passage[MAX_BUF_SIZE_PASSAGE+1];
uint32 position_passage = 0;

void AddStringToBuf_Passage(sint8 * buf, sint32 count)
{
    if(position_passage >= MAX_BUF_SIZE_PASSAGE)        //缓冲区满了
    {
        position_passage = MAX_BUF_SIZE_PASSAGE;
        //printf("Buf is full");
        return;
    }

    if((position_passage + count) >= MAX_BUF_SIZE_PASSAGE)
        count = MAX_BUF_SIZE_PASSAGE - position_passage ;

    memcpy(read_buf_passage + position_passage, buf , count);

    position_passage += count;
}

/*
判断是否是有效的帧
参数：字符串的指针，字符串的长度
返回：1：有效帧， 其他值：无效帧
*/
sint32 passage_frame_check(sint8 *line, uint32 len)
{
    sint8 buf[9]={0};
	sint32 i, sum, sum_verf;

    if(len < MIN_PASSAGE_FRAME_SIZE)
        return -1;

	sum = 0;
	for(i=0;i<len-5;i++)
		sum += line[i];

	sum_verf = line[len-5];
	sum_verf = sum_verf << 8;
	sum_verf += line[len-4];
	sum_verf = sum_verf << 8;
	sum_verf += line[len-3];
	sum_verf = sum_verf << 8;
	sum_verf += line[len-2];

    if(sum_verf != sum)
        return -2;

    return 1;
}


uint32 get_passage_frame(sint8 * buf)
{
	int i, j;

	if (position_passage < MIN_PASSAGE_FRAME_SIZE)
		return 0;

	for (i = 0; i<(position_passage - MIN_PASSAGE_FRAME_SIZE + 2); i++)
	{
		if ((read_buf_passage[i] == '@')&&(read_buf_passage[i+1] == '@'))
			break;
	}


	if (i == (position_passage - MIN_PASSAGE_FRAME_SIZE + 2)) //没有开头@@标记
	{
		memcpy(read_buf_passage, read_buf_passage + i - 1, position_passage - i + 1); //把后面没有检查的数据移到缓冲区头
		position_passage = position_passage - i + 1;
		return -1;
	}

	for (j = i; j < (position_passage - 5); j++)
	{
		if ((read_buf_passage[j] == '%')&&(read_buf_passage[j+5] == '!'))
			break;
	}

	if (j == (position_passage - 5))   //没找到尾
	{
		if ((j - i) >= MAX_PASSAGE_FRAME_SIZE)
		{
			memcpy(read_buf_passage, read_buf_passage + position_passage - MAX_PASSAGE_FRAME_SIZE + 1, MAX_PASSAGE_FRAME_SIZE - 1); //移动剩余数据
			position_passage = MAX_PASSAGE_FRAME_SIZE - 1;
		}
		else
		{
			memcpy(read_buf_passage, read_buf_passage + i, position_passage - i); //移动剩余数据
			position_passage = position_passage - i;
		}
		return -2;
	}

	//找到一帧数据
	if ((j - i) >= MAX_PASSAGE_FRAME_SIZE)     //帧长限制
	{
		position_passage = position_passage - j - 1;
		if (position_passage > 0)
			memcpy(read_buf_passage, read_buf_passage + j + 1, position_passage); //移动剩余数据
		return -3;
	}
	else
	{
		memcpy(buf, read_buf_passage + i, j - i + 6); //COPY一帧数据
		buf[j - i + 6] = 0;
		memcpy(read_buf_passage, read_buf_passage + j + 6, position_passage - j - 6); //去除有效数据头前的无效数据
		position_passage = position_passage - j - 6;

		return (j - i + 6); //返回字符串长度
	}
}

sint8 passage_parse(sint8 *line, sint32 len, Passage_Info *passage)
{
    sint32 i;
    sint8 buf[6];

	//获取通道号（门序号）
	buf[0] = line[len-13];
	if((buf[0] < '0')||(buf[0] > '9'))
		return -1;
	buf[1] = '\0';
	passage->DoorNum = atoi(buf);

	//获取上车人数
    memcpy(buf,line+len-12,3);
	for(i=0;i<3;i++)
		if((buf[i] < '0')||(buf[i] > '9'))
			return -2;
	buf[3] = '\0';
	passage->nUpPassageNum = atoi(buf);

	//获取下车人数
    memcpy(buf,line+len-9,3);
	for(i=0;i<3;i++)
		if((buf[i] < '0')||(buf[i] > '9'))
			return -3;
	buf[3] = '\0';
	passage->nDownPassageNum = atoi(buf);

    return 0;
}

void show_passage(Passage_Info *passage)
{
    SKY_TRACE_INFO("Passage Num is %d\n", passage->nInPassage);
    SKY_TRACE_INFO("Door num is %d\n", passage->DoorNum);
    SKY_TRACE_INFO("Passage up num is %d\n", passage->nUpPassageNum);
    SKY_TRACE_INFO("Passage down num is %d\n", passage->nDownPassageNum);
}


