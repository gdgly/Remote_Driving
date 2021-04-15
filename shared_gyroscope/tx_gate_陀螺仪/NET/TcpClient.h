#ifndef __TCP_CLIENT__
#define __TCP_CLIENT__

#include <pthread.h>  

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <unistd.h>



#include "../Std_Types.h"

#define BUFF_LEN 2048
#define FRAME_HEADER_LEN 11
#define FRAME_START_HEADER_LEN 2

class CTcpClient
{
	public:

		CTcpClient();
		virtual ~CTcpClient();

		sint32 init_tcp_client(const sint8 * hostname, uint32 port);				//供外部调用
		int tcp_connect(void);
		int tcp_disconnect(void);



		sint32 send_tcp_data(sint8* str, uint32 len);

    typedef void (*pFunc_ReadCallback)(uint8 *data, uint32 len, void *arg);

    void set_read_callback_func(const pFunc_ReadCallback pReadFunc, void *arg);

		sint32 get_tcp_status();
		sint32 get_last_error();

		//uint32 m_connect_interval;	//自动重连间隔时间，单位秒

	private:
		int SetNonBlocking(int fd);

    static void *tcp_read_thread_new(void *arg);
    static void *tcp_read_thread(void *arg);
    sint8 get_local_ip();

		sint32 m_tcp_sock_client;
		struct sockaddr_in socket_server_addr;

    sint32 m_last_error;
    pthread_attr_t attr;
    pthread_t tid_recv; //接受数据线程的Id
    bool m_recvThreadFlag;
    uint8 rev_buf[BUFF_LEN];

		sint8 m_ip_address[31];
		uint32 m_port;

		void *m_arg;
		pFunc_ReadCallback m_pReadCallback ;
};
	
	
#endif /* defined(__TCP_CLIENT__) */
