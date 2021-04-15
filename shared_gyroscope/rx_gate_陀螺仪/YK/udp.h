/**
* \file       udp.h
* \author     yangkun
* \date       2019/05/09
* \version    Version 1.0
* \brief
* \note       Copyright (c) 2000-2020
* \remarks    tcp functions
******************************************************************************/


#ifndef _UDP_H
#define _UDP_H

#ifdef __cplusplus
extern "C"
{
#endif

#include "../COMM/macro_def.h"
#include <arpa/inet.h>

SWORD32 udp_send(CHAR *dst_ip, WORD16 dst_port, CHAR *data, WORD32 data_len);

#ifdef __cplusplus
}
#endif
#endif

