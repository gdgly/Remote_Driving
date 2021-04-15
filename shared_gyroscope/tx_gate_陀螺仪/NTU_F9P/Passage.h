#ifndef __PASSAGE_H__
#define __PASSAGE_H__

//#include "Std_Types.h"
#include "../Std_Types.h"

#define BUFLEN                  512
#define PASSAGE_FRAME_LEN       100       /*一帧数据长度*/ 

typedef struct Passage
{
    uint8  DoorNum;
    sint32 nUpPassageNum;
    sint32 nDownPassageNum;
    sint32 nInPassage;
}Passage_Info;


void AddStringToBuf_Passage(sint8 * buf, sint32 count);
uint32 get_passage_frame(sint8 * buf);
sint32 passage_frame_check(sint8 *line, uint32 len);
sint8 passage_parse(sint8 *line, sint32 len, Passage_Info *passage);
void show_passage(Passage_Info *passage);

#endif /* __PASSAGE_H__ */
