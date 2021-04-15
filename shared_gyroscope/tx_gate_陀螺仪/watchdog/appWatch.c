#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <assert.h>
#include <sys/syscall.h>
#include <dirent.h>
#include "../COMM/sky_trace.h"

void init_daemon();
void init_sigaction();

char *getcurrentDir()
{
  char *file_path;
  file_path = (char *)malloc(80);
  getcwd(file_path, 80);
  return file_path;
};

const char *appName = "txgate";

int main(int argc, char **argv)
{
  SKY_TRACE_INFO("APK GBWatch start..\n");
  init_sigaction();
  init_daemon();
  char ret[64]; //设置一个合适的长度，以存储每一行输出
  char cmd[255] = {'\0'};
  char pidcmd[255] = {'\0'};
  sprintf(cmd, "%s/%s", getcurrentDir(), appName);
  sprintf(pidcmd, "pidof %s", cmd);
  while (1)
  {

    FILE *pp = popen(pidcmd, "r"); //建立管道
    if (!pp)
    {
      SKY_TRACE_ERROR("pidof Erorr\n");
      exit(0);
      // continue;
    }
    memset(ret, 0, 64);
    fgets(ret, sizeof(ret), pp);
    if (strlen(ret) < 2)
    {
      SKY_TRACE_ERROR("read pipe error, ERROR: %m\n");
      system(cmd);
    }
    pclose(pp); //关闭管道
    if (strlen(ret) < 2)
    {
      SKY_TRACE_ERROR("Rstart %s\n", appName);
      system(cmd);
    }
    sleep(5);
  }
}

void init_daemon()
{
  int pid;
  pid = fork();
  if (pid < 0)
    exit(1);        //创建错误，退出
  else if (pid > 0) //父进程退出
    exit(0);
  setsid(); //使子进程成为组长
  pid = fork();
  if (pid != 0)
    exit(0); //再次退出，使进程不是组长，这样进程就不会打开控制终端
}

void DealRoutineOnExit(int sign)
{
  SKY_TRACE_INFO("sign: 0x%x.\n", sign);
}

///建立信号处理机制
void init_sigaction()
{
  struct sigaction tact;
  /*信号到了要执行的任务处理函数为prompt_info*/
  tact.sa_handler = DealRoutineOnExit;
  tact.sa_flags = 0;
  /*初始化信号集*/
  sigemptyset(&tact.sa_mask);
  /*建立信号处理机制*/
  sigaction(SIGINT, &tact, NULL);
  sigaction(SIGQUIT, &tact, NULL);
  sigaction(SIGTERM, &tact, NULL);
  sigaction(SIGKILL, &tact, NULL);
}
