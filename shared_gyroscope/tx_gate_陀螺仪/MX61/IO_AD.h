#ifndef __IO_AD__
#define __IO_AD__

int TurnOnLed(unsigned char led_index, unsigned char lamp_status);
int SetOutputIO(unsigned char io_index, unsigned char io_status);
int GetInputIO(unsigned char io_index);

int GetADC(int ch);

#endif /* defined(__IO_AD__) */
