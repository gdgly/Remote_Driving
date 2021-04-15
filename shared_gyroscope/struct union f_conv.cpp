#include <stdio.h>


//struct 

union f_conv
{
  float f;
  char c[4];
};


int main()
{
    f_conv f1;
    f_conv f2;
    f_conv f3;
    //  f1.f =   14; 
    //  printf("%02x,%02x,%02x,%02x\n",f1.c[0],f1.c[1],f1.c[2],f1.c[3]);
    f1.c[0] = 0x34;
    f1.c[1] = 0x1b;
    f1.c[2] = 0x1d;
    f1.c[3] = 0xc1;

    printf("f1.f = %f\n",f1.f);

    f2.c[0] = 0xfa;
    f2.c[1] = 0x81;
    f2.c[2] = 0x19;
    f2.c[3] = 0xc1;

    printf("f2.f = %f\n",f2.f);

    f3.c[0] = 0x00;
    f3.c[1] = 0xe8;
    f3.c[2] = 0x17;
    f3.c[3] = 0xbc;

    printf("f3.f = %f\n",f3.f);


}
