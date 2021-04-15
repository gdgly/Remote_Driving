#ifndef __udp_server__
#define __udp_server__

#include <pthread.h>  

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>


#include "../Std_Types.h"

#define BUFF_LEN 2048

class CUdpServer
{
 	public:

    CUdpServer();
    virtual ~CUdpServer();

	sint32 init_udp_server(void);					//供线程使用
	sint32 init_udp_server(uint32 port);			//供外部调用
	sint32 start_connect_thread(void);

	void close_udp(void);

	sint32 send_udp_data(sint8* str, uint32 len, struct sockaddr_in *addr, uint32 addr_len);
	sint32 send_udp_data(sint8* str, uint32 len);
    typedef void (*pFunc_ReadCallback)(sint8 * data, uint32 len, struct sockaddr_in *addr, uint32 addr_len);
	
    void set_read_callback_func(const pFunc_ReadCallback pReadFunc);

	sint32 get_udp_status();
	sint32 get_last_error();

	uint32 m_connect_interval;	//自动重连间隔时间，单位秒

	private:

	static void * udp_read_thread(void * arg);
	static void * udp_connect_thread(void * arg);

    sint32 m_udp_sock_server;
	struct sockaddr_in socket_server_addr;

	sint32 m_last_error;
	
	pthread_t tid_recv;			//接受数据线程的Id
	
	sint8 m_status;				//是否需要连接
	sint8 m_connected_status;	//连接状态

    sint32 m_thread_read_avtived;
	
	sint8 rev_buf[BUFF_LEN];
	sint8 send_buf[BUFF_LEN];

	uint32 m_port;

	pFunc_ReadCallback m_pReadCallback ;
};
	
	
#endif /* defined(__udp_server__) */
