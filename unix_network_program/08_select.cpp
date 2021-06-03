#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include "common.h"


int new_connect_handle()
{
	struct sockaddr_in tClientAddr;
	memset(&tClientAddr, 0, sizeof(tClientAddr));

	nAddrLen = sizeof(tClientAddr);
	nAcceptFd = accept(nListenFd, &tClientAddr, &nAddrLen);
	if (-1 == nAcceptFd)
	{
		printf("accept failed, error:%s\n", strerror(errno));
		return -1;
	}

	inet_ntop(AF_INET, &tClientAddr.sin_addr, achAddr, sizeof(achAddr));
	printf("new client comes. peer addr %s:%d\n", achAddr, htons(tClientAddr.sin_port));
	return 0;
}

int recv_handle()
{

	return 0;
}

int main()
{
	s32 nRet = 0;
	s32 nListenFd = 0;
	s32 nAcceptFd = 0;
	s32 nMaxFd = 0;
	s32 nOpt = 0;
	s32 nAddrLen = 0;
	fd_set tReadFdSet;
	fd_set tOldFdSet;
	s8  achAddr[64] = { 0 };
	struct sockaddr_in tServerAddr;
	struct timeval tTimeOut;

	memset(&tTimeOut, 0, sizeof(tTimeOut));
	memset(&tReadFdSet,  0, sizeof(tReadFdSet));
	memset(&tOldFdSet,   0, sizeof(tOldFdSet));
	memset(&tServerAddr, 0, sizeof(tServerAddr));
	

	nListenFd = socket(AF_INET, SOCK_STREAM, 0);
	if (-1 == nListenFd)
	{
		printf("create ipv4 tcp socket failed, error:%s\n", strerror(errno));
		return -1;
	}

	// 端口复用
	nOpt = 1;
	setsockopt(nListenFd, SOL_SOCKET, SO_REUSEADDR, &nOpt, sizeof(nOpt));

	tServerAddr.sin_family = AF_INET;
	tServerAddr.sin_addr = INADDR_ANY;
	tServerAddr.sin_port = htons(TCP_SERVER_PORT);
	nRet = bind(nListenFd, (struct sockaddr*)&tServerAddr, sizeof(tServerAddr));
	if (-1 == nRet)
	{
		printf("bind 0.0.0.0:%d failed, error:%s\n", TCP_SERVER_PORT, strerror(errno));
		return -1;
	}

	nRet = listen(nListenFd, TCP_LISTEN_QUEUE_LEN);
	if (-1 == nRet)
	{
		printf("listen failed, error:%s\n", strerror(errno));
		return -1;
	}

	// 超时时间 3s
	tTimeOut.tv_sec = 3;
	tTimeOut.tv_usec = 0;

	// 文件描述符集合处理
	FD_ZERO(&tOldFdSet);
	FD_ZERO(&tReadFdSet); 
	FD_SET(nListenFd, &tReadFdSet);
	tOldFdSet = tReadFdSet;
	nMaxFd = nListenFd + 1;

	while (1)
	{
		nRet = select(nMaxFd, &tReadFdSet, NULL, NULL, &tTimeOut);
		if (nRet > 0) // 有文件可读
		{
			if (FD_ISSET(nListenFd, &tReadFdSet))
			{
				new_connect_handle();
			}

			if ()
			{
				recv_handle();
			}
		}
		else if (nRet == -1) // 出错
		{
			printf("select failed, error:%s\n", strerror(errno));
			continue;
		}
		else //超时
		{
			printf("select timeout\n");
			continue;
		}
	}

	return 0;
}