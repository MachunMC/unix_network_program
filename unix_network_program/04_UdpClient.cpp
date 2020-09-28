//#include "common.h"
//
//int main(void)
//{
//	s32 nRet = 0;
//	s32 nSockFd = 0;
//	struct sockaddr_in tServerAddr;
//	s8 achSendBuf[UDP_CLIENT_SEND_BUF_LEN + 1] = { 0 };
//	s8 achRecvBuf[UDP_CLIENT_RECV_BUF_LEN + 1] = { 0 };
//
//	memset(&tServerAddr, 0, sizeof(tServerAddr));
//
//	// 创建udp socket
//	nSockFd = socket(AF_INET, SOCK_DGRAM, 0);
//	if (-1 == nSockFd)
//	{
//		printf("create udp socket failed, error:%s\n", strerror(errno));
//		return -1;
//	}
//
//	// 绑定目的地址
//	tServerAddr.sin_family = AF_INET;
//	inet_pton(AF_INET, UDP_SERVER_ADDR, &tServerAddr.sin_addr.s_addr);
//	tServerAddr.sin_port = htons(UDP_SERVER_PORT);
//
//	// 发送和接收数据
//	while (1)
//	{
//		memset(achSendBuf, 0, sizeof(achSendBuf));
//
//		printf("Please input:");
//		if (NULL == fgets(achSendBuf, UDP_CLIENT_SEND_BUF_LEN, stdin))
//		{
//			printf("fgets failed, error:%s\n", strerror(errno));
//			continue;
//		}
//
//		if (0 == strncmp(achSendBuf, "send stop", 9))
//		{
//			printf("send stop, return\n");
//			break;
//		}
//		else
//		{
//			nRet = sendto(nSockFd, achSendBuf, UDP_CLIENT_SEND_BUF_LEN, 0, (struct sockaddr*) &tServerAddr, sizeof(tServerAddr));
//			if (-1 == nRet)
//			{
//				printf("send failed, error:%s\n", strerror(errno));
//				continue;
//			}
//		}
//
//		nRet = recvfrom(nSockFd, achRecvBuf, UDP_CLIENT_SEND_BUF_LEN, 0, NULL, NULL);
//		if (-1 == nRet)
//		{
//			printf("recv failed, error:%s\n", strerror(errno));
//		}
//		else
//		{
//			printf("[RECV] %s\n", achRecvBuf);
//			if (0 == strncmp(achSendBuf, "bye", 3))
//			{
//				printf("server say bye, close socket and return\n");
//				break;
//			}
//		}
//	}
//	
//	close(nSockFd);
//	return 0;
//}