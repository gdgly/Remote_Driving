#include <stdio.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include <sys/ioctl.h>

#include "../COMM/sky_trace.h"

#define GPIO_IOC_MAGIC   'G'

#define IOCTL_GPIO_SETOUTPUT              _IOW(GPIO_IOC_MAGIC, 0, int)       
#define IOCTL_GPIO_SETINPUT               _IOW(GPIO_IOC_MAGIC, 1, int)
#define IOCTL_GPIO_SETVALUE               _IOW(GPIO_IOC_MAGIC, 2, int) 
#define IOCTL_GPIO_GETVALUE   	 		  _IOR(GPIO_IOC_MAGIC, 3, int)

typedef struct
{
    int pin;
    int data;
    int usepullup;
} gpio_arg;

#define DEV_GPIO 	"/dev/user_gpio"
#define DEV_ADC		"/dev/tlc2543"

/*
GPIOn m				index = (n-1)*32 + m
LED1, GPIO1 3	,	index is 3
LED2, GPIO5 18	,	index is 146
LED3, GPIO5 19	,	index is 147
*/


#define		GPIO_TO_PIN(bank, gpio)  	(32*(bank - 1)+ (gpio))
//LED
#define		GPIO_LED1			GPIO_TO_PIN(1,3)
#define		GPIO_LED2			GPIO_TO_PIN(5,18)
#define		GPIO_LED3			GPIO_TO_PIN(5,19)

//输入
#define  	VACC_BUS_PIN		GPIO_TO_PIN(7,8)
#define  	BUS_IN1_PIN			GPIO_TO_PIN(7,3)
#define  	BUS_IN2_PIN			GPIO_TO_PIN(7,7)
#define  	BUS_IN3_PIN			GPIO_TO_PIN(7,0)
#define  	BUS_IN4_PIN			GPIO_TO_PIN(7,2)
#define  	BUS_IN5_PIN			GPIO_TO_PIN(7,1)

//输出
#define  	BUS_OUT1_PIN		GPIO_TO_PIN(6,31)
#define  	BUS_OUT2_PIN		GPIO_TO_PIN(7,6)
#define  	BUS_OUT3_PIN		GPIO_TO_PIN(7,4)
#define  	BUS_OUT4_PIN		GPIO_TO_PIN(7,5)

unsigned char led_channel[3] = {GPIO_LED1, GPIO_LED2, GPIO_LED3};
unsigned char input_channel[6] = {VACC_BUS_PIN, BUS_IN1_PIN, BUS_IN2_PIN, BUS_IN3_PIN, BUS_IN4_PIN, BUS_IN5_PIN};
unsigned char output_channel[4] = {BUS_OUT1_PIN, BUS_OUT2_PIN, BUS_OUT3_PIN, BUS_OUT4_PIN};


/*
函数作用：	控制外面LED灯
函数参数：	led_index，LED灯IO的序号
			lamp_status，灯的状态，0表示关，1表示开
返回参数：	0，函数执行成功
*/
int TurnOnLed(unsigned char led_index, unsigned char lamp_status)
{
	int fd = -1;
	unsigned char pin_number;
	gpio_arg arg;
	
	if(led_index >= 3)
	{
		SKY_TRACE_ERROR("GPIO led output index is error");
		return 1;
	}

	pin_number = led_channel[led_index];

	switch(pin_number) 
	{
		case GPIO_LED1:
		case GPIO_LED2:
		case GPIO_LED3:
			break;
		default:
			SKY_TRACE_ERROR("GPIO led output pin number is error");
			return 1;
	}

	if((lamp_status != 0)&&(lamp_status != 1))
	{
        SKY_TRACE_ERROR("lamp_status must between 0-1, 0:off, 1:on");
		return 2;
	}

    arg.pin = pin_number;
    arg.data = lamp_status;
    arg.usepullup = 1;
	
    fd = open(DEV_GPIO, O_RDWR);
    if (fd < 0)
    {
        SKY_TRACE_ERROR("Error device open fail \n");
        return 3;
    }
	
    ioctl(fd, IOCTL_GPIO_SETOUTPUT, &arg);
    ioctl(fd, IOCTL_GPIO_SETVALUE, &arg);

    close(fd);
	
	return 0;
}

int SetOutputIO(unsigned char io_index, unsigned char io_status)
{
	int fd = -1;
	unsigned char pin_number;
	gpio_arg arg;

	if(io_index >= 4)
	{
		SKY_TRACE_ERROR("GPIO output index is error");
		return 1;
	}
	
	pin_number = output_channel[io_index];

	switch(pin_number) 
	{
		case VACC_BUS_PIN:
		case BUS_IN1_PIN:
		case BUS_IN2_PIN:
		case BUS_IN3_PIN:
		case BUS_IN4_PIN:
		case BUS_IN5_PIN:
			break;
		default:
			SKY_TRACE_ERROR("GPIO output pin number is error");
			return 1;
	}
	
	if((io_status != 0)&&(io_status != 1))
	{
        SKY_TRACE_ERROR("lamp_status must between 0-1, 0:off, 1:on");
		return 2;
	}
	
    arg.pin = pin_number;
    arg.data = io_status;
    arg.usepullup = 1;
	
    fd = open(DEV_GPIO, O_RDWR);
    if (fd < 0)
    {
        SKY_TRACE_ERROR("Error device open fail \n");
        return 3;
    }
	
    ioctl(fd, IOCTL_GPIO_SETOUTPUT, &arg);
    ioctl(fd, IOCTL_GPIO_SETVALUE, &arg);

    close(fd);
	
	return 0;
}

int GetInputIO(unsigned char io_index)
{
	int fd = -1;
	unsigned char pin_number;
	gpio_arg arg;

	if(io_index >= 6)
	{
		SKY_TRACE_ERROR("GPIO input index is error");
		return 1;
	}
	
	pin_number = input_channel[io_index];

	switch(pin_number) 
	{
		case BUS_OUT1_PIN:
		case BUS_OUT2_PIN:
		case BUS_OUT3_PIN:
		case BUS_OUT4_PIN:
			break;
		default:
			SKY_TRACE_ERROR("GPIO input pin number is error");
			return -1;
	}
	
    arg.pin = pin_number;
    arg.data = 0;
	
    fd = open(DEV_GPIO, O_RDWR);
    if (fd < 0)
    {
        SKY_TRACE_ERROR("Error device open fail \n");
        return -2;
    }
	
	ioctl(fd, IOCTL_GPIO_SETINPUT, &arg);
	ioctl(fd, IOCTL_GPIO_GETVALUE, &arg);

    close(fd);
	
	return arg.data;
}

int GetADC(int ch)
{
	int fd = -1;
	int value = 0,rst;

	if(ch > 1)
	{
		SKY_TRACE_ERROR("ADC index is error");
		return 1;
	}

	fd = open(DEV_ADC, O_RDWR);
	if (fd < 0) 
	{
		SKY_TRACE_ERROR("Error Device OPEN!!!\n");
		return -2;
	}

	lseek(fd, ch, SEEK_SET);
	rst = read(fd, &value, 2);
	if(rst < 0)
	{
		SKY_TRACE_ERROR("ADC read error!!!\n");
		return -3;
	}
	close(fd);

	return value;
}
