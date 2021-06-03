#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <arpa/inet.h>
#include <unistd.h>

typedef long long int	       s64;   // long long int为64位，long和int都是32位
typedef unsigned long long int u64;
typedef int			           s32;
typedef unsigned int           u32;
typedef short int              s16;
typedef unsigned short int     u16;
typedef char                   s8;
typedef unsigned int	       u8;


#define UDP_SERVER_SEND_BUF_LEN   256
#define UDP_SERVER_RECV_BUF_LEN   256
#define UDP_SERVER_ADDR           "127.0.0.1"
#define UDP_SERVER_PORT           5678
#define UDP_SERVER_PEER_ADDR_LEN  32
#define UDP_CLIENT_SEND_BUF_LEN   256
#define UDP_CLIENT_RECV_BUF_LEN   256
#define TCP_SERVER_ADDR		     "127.0.0.1"
#define TCP_SERVER_PORT			  6789
#define TCP_SEND_BUF_LEN	      256
#define TCP_RECV_BUF_LEN	      256
#define TCP_LISTEN_QUEUE_LEN	  128