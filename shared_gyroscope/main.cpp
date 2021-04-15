#include <iostream>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <signal.h>
#include <time.h>
#include <sys/time.h>


int main()
{
    struct timeval start, end;
    float timeuse; 
    gettimeofday(&start,NULL); // 开始时间

    // SKY_TRACE_INFO("Start of Send : %s \n", start.tv_sec*1000000+start.tv_usec);
    printf("Start of Main:%ld \n",start.tv_sec*1000000+start.tv_usec);
    
    for(long int i =0; i<10000 ; i++)
    {
        // SKY_TRACE_INFO("车端->舱端的HWT905数据\n");
         printf("车端->舱端的HWT905数据\n" );
    }

    gettimeofday(&end,NULL);
    timeuse = 1000000*(end.tv_sec-start.tv_sec)+end.tv_usec-start.tv_usec;       
    timeuse /= 1000;       
    printf("used time:%f msec\n",timeuse);
}