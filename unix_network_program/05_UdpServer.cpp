#include "common.h"

int main(void)
{
	s32 nRet = 0;
	s32 nSockFd = 0;
	s32 nAddrLen = 0;
	struct sockaddr_in tLocalAddr;
	struct sockaddr_in tPeerAddr;
	s8 achPeerAddr[UDP_SERVER_PEER_ADDR_LEN + 1] = { 0 };
	s8 achSendBuf[UDP_SERVER_SEND_BUF_LEN + 1] = { 0 };
	s8 achRecvBuf[UDP_SERVER_RECV_BUF_LEN + 1] = { 0 };

	memset(&tLocalAddr, 0, sizeof(tLocalAddr));
	memset(&tPeerAddr, 0, sizeof(tPeerAddr));

	// 创建接收socket
	nSockFd = socket(AF_INET, SOCK_DGRAM, 0);
	if (-1 == nSockFd)
	{
		printf("create udp socket failed, error:%s\n", strerror(errno));
		return -1;
	}

	// 绑定本地地址端口
	tLocalAddr.sin_family = AF_INET;
	tLocalAddr.sin_port = htons(UDP_SERVER_PORT);
	tLocalAddr.sin_addr.s_addr = INADDR_ANY; //表示本机所有地址

	nRet = bind(nSockFd, (struct sockaddr*) &tLocalAddr, sizeof(tLocalAddr));
	if (-1 == nRet)
	{
		printf("bind local addr failed, error:%s\n", strerror(errno));
		close(nSockFd);
		return -1;
	}

	// 接收发送数据
	while (1)
	{
		memset(&achRecvBuf, 0, sizeof(achRecvBuf));

		nAddrLen = sizeof(tPeerAddr);
		nRet = recvfrom(nSockFd, achRecvBuf, UDP_SERVER_RECV_BUF_LEN, 0, (struct sockaddr*)&tPeerAddr, (socklen_t * )&nAddrLen);
		if (-1 == nRet)
		{
			printf("recvfrom failed, error:%s\n", strerror(errno));
			continue;
		}
		else // 接收成功
		{
			inet_ntop(AF_INET, &tPeerAddr.sin_addr.s_addr, achPeerAddr, sizeof(achPeerAddr)-1);
			printf("recv from %s %d:%s\n", achPeerAddr, htons(tPeerAddr.sin_port), achRecvBuf);
		}		

		// 回复数据
		snprintf(achSendBuf, sizeof(achSendBuf)-1, "Ok, %s %d I got it.", achPeerAddr, htons(tPeerAddr.sin_port));

		nAddrLen = sizeof(tPeerAddr);
		nRet = sendto(nSockFd, achSendBuf, sizeof(achSendBuf)-1, 0, (struct sockaddr*) &tPeerAddr, (socklen_t)nAddrLen);
		if (-1 == nRet)
		{
			printf("send to peer failed, error:%s\n", strerror(errno));
			continue;
		}

		if (0 == strncmp(achRecvBuf, "bye", 3))
		{
			printf("peer said 'bye', close the socket\n");
			break;
		}
	}

	close(nSockFd);
	return 0;
}