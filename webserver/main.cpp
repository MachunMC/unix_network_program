#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <arpa/inet.h>
#include "define.h"

int http_req_handle(s8* pchData)
{
	printf("%s\n", pchData);

	return 0;
}

int main()
{
	s32 i = 0;
	s32 nRet = 0;
	s32 nListenFd = 0;
	s32 nAcceptFd = 0;
	s32 nEpollFd = 0;
	s32 nEpollCnt = 0;
	socklen_t nSockLen = 0;
	struct sockaddr_in tServerAddr;
	struct sockaddr_in tClientAddr;
	struct epoll_event tTmpEvent;
	struct epoll_event atEpollEvent[1024];
	s8 achAddr[64] = { 0 };
	s8 achRecvBuf[1500 + 1] = { 0 };

	memset(&tServerAddr,  0, sizeof(tServerAddr));
	memset(&tClientAddr,  0, sizeof(tClientAddr));
	memset(&tTmpEvent,    0, sizeof(tTmpEvent));
	memset(&atEpollEvent, 0, sizeof(atEpollEvent));
	
	nListenFd = socket(AF_INET, SOCK_STREAM, 0);
	if (-1 == nListenFd)
	{
		printf("create tcp socket failed, error:%s\n", strerror(errno));
		return -1;
	}

	s32 nOpt = 1;
	setsockopt(nListenFd, SOL_SOCKET, SO_REUSEADDR, &nOpt, sizeof(nOpt));

	tServerAddr.sin_family = AF_INET;
	tServerAddr.sin_port = htons(8888);
	tServerAddr.sin_addr.s_addr = 0;
	nRet = bind(nListenFd, (struct sockaddr*)&tServerAddr, sizeof(tServerAddr));
	if (-1 == nRet)
	{
		printf("bind failed, error:%s\n", strerror(errno));
		return -1;
	}

	nRet = listen(nListenFd, 128);
	if (-1 == nRet)
	{
		printf("listen failed, error:%s\n", strerror(errno));
		return -1;
	}

	nEpollFd = epoll_create(1);
	if (-1 == nEpollFd)
	{
		printf("epoll_create failed, error:%s\n", strerror(errno));
		return -1;
	}

	tTmpEvent.events = EPOLLIN; // 还要设置边沿触发模式
	tTmpEvent.data.fd = nListenFd;
	nRet = epoll_ctl(nEpollFd, EPOLL_CTL_ADD, nListenFd, &tTmpEvent);
	if (-1 == nRet)
	{
		printf("epoll_ctl add listen fd failed, error:%s\n", strerror(errno));
		return -1;
	}

	while (1)
	{
		// 阻塞
		nEpollCnt = epoll_wait(nEpollFd, atEpollEvent, 1024, 200);
		if (nEpollCnt == -1) // 出错
		{
			printf("epoll_wait failed, error:%s\n", strerror(errno));
			continue;
		}
		else if (nEpollCnt == 0) // 超时
		{
			continue;
		}
		else // 有事件触发
		{
			for (i = 0; i < nEpollCnt; i++)
			{
				if (atEpollEvent[i].events & EPOLLIN) // 读事件
				{
					if (atEpollEvent[i].data.fd = nListenFd) // 有新的连接
					{
						nSockLen = sizeof(tClientAddr);
						nAcceptFd = accept(nListenFd, (struct sockaddr*)&tClientAddr, &nSockLen);
						if (-1 == nAcceptFd)
						{
							printf("accept failed, error:%s\n", strerror(errno));
							continue;
						}
						else // 添加新的fd到epoll
						{
							memset(achAddr, 0, sizeof(achAddr));
							inet_ntop(AF_INET, &tClientAddr.sin_addr.s_addr, achAddr, sizeof(achAddr));
							printf("new client comes, %s:%d\n", achAddr, htons(tClientAddr.sin_port));

							memset(&tTmpEvent, 0, sizeof(tTmpEvent));
							tTmpEvent.events = EPOLLIN;
							tTmpEvent.data.fd = nAcceptFd;
							nRet = epoll_ctl(nEpollFd, EPOLL_CTL_ADD, nAcceptFd, &tTmpEvent);
							if (-1 == nRet)
							{
								printf("epoll_ctl add accept fd failed, error:%s\n", strerror(errno));
								continue;
							}
						}
					}
					else // 有数据可读
					{
						printf("recv data\n");

						memset(achRecvBuf, 0, sizeof(achRecvBuf));
						nRet = recv(atEpollEvent[i].data.fd, achRecvBuf, sizeof(achRecvBuf), 0);
						if (nRet == 0 || -1 == nRet) // 对方关闭或出错
						{
							if (nRet == 0)
							{
								printf("peer closed\n");
							}
							else
							{
								printf("recv failed, error:%s\n", strerror(errno));
							}

							nRet = epoll_ctl(nEpollFd, EPOLL_CTL_DEL, atEpollEvent[i].data.fd, &atEpollEvent[i]);
							if (-1 == nRet)
							{
								printf("epoll_ctl add accept fd failed, error:%s\n", strerror(errno));
							}
							
							continue;
						}
						else // 收到数据
						{
							http_req_handle(achRecvBuf);
						}
					}
				}
			}
		}
	}
	








	return 0;
}