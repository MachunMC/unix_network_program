#include "common.h"

int main(void)
{
	s32 nSockFd = 0;
	ssize_t nRetVal = 0;
	struct sockaddr_in tServerAddr;
	s8 achSendBuf[TCP_SEND_BUF_LEN] = { 0 };
	s8 achRecvBuf[TCP_RECV_BUF_LEN] = { 0 };

	memset(&tServerAddr, 0, sizeof(tServerAddr));

	do
	{
		// 创建ipv4 tcp socket
		nSockFd = socket(AF_INET, SOCK_STREAM, 0);
		if (-1 == nSockFd)
		{
			printf("creat ipv4 tcp socket failed, error:%s\n", strerror(errno));
			break;
		}

		// connect 连接服务器
		tServerAddr.sin_family = AF_INET;
		inet_pton(AF_INET, TCP_SERVER_ADDR, &tServerAddr.sin_addr.s_addr);
		tServerAddr.sin_port = htons(TCP_SERVER_PORT);
		
		if (0 != connect(nSockFd, (sockaddr*)&tServerAddr, sizeof(tServerAddr)))
		{
			printf("connect server %s:%d failed, error:%s\n", TCP_SERVER_ADDR, TCP_SERVER_PORT, strerror(errno));
			break;
		}

		while (1)
		{
			printf("please input message to send:");
			scanf("%s", achSendBuf);

			// send 发送数据
			nRetVal = send(nSockFd, achSendBuf, sizeof(achSendBuf), 0);
			if (-1 == nRetVal)
			{
				printf("send msg to %s:%d failed, error:%s\n", TCP_SERVER_ADDR, TCP_SERVER_PORT, strerror(errno));
				break;
			}

			// recv 接收数据
			nRetVal = recv(nSockFd, achRecvBuf, sizeof(achRecvBuf), 0);
			if (-1 == nRetVal)
			{
				printf("recv msg to %s:%d failed, error:%s\n", TCP_SERVER_ADDR, TCP_SERVER_PORT, strerror(errno));
				continue;
			}
			else
			{
				printf("[RECV]%s %d:%s, nRetVal:%d\n", TCP_SERVER_ADDR, TCP_SERVER_PORT, achRecvBuf, nRetVal);
			}
			
			if (0 == strncmp(achSendBuf, "bye", 3) || 0 == strncmp(achSendBuf, "bye", 3))
			{
				printf("session end, close the socket\n");
				break;
			}
		}
	} while (0);

	// 关闭socket
	if (nSockFd > 0)
	{
		close(nSockFd);
	}

	return 0;
}