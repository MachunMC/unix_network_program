#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "common.h"
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/epoll.h>

int main()
{
	s32 i = 0;
	s32 nRet = 0;
	s32 nListenFd = 0;
	s32 nAcceptFd = 0;
	s32 nOpt = 0;
	s32 nEpollFd = 0;
	s32 nEpollCnt = 0;
	socklen_t nAddrLen = 0;
	struct sockaddr_in tServerAddr;
	struct sockaddr_in tClientAddr;
	struct epoll_event tTmpEvent;
	struct epoll_event atEpollEvent[16];
	s8 achAddr[64] = { 0 };
	s8 achRecvBuf[1500 + 1] = { 0 };

	memset(&tServerAddr, 0, sizeof(tServerAddr));
	memset(&tClientAddr, 0, sizeof(tClientAddr));
	memset(&tTmpEvent, 0, sizeof(tTmpEvent));
	memset(&atEpollEvent, 0, sizeof(atEpollEvent));

	nListenFd = socket(AF_INET, SOCK_STREAM, 0);
	if(-1 == nListenFd)
	{
		printf("create tcp socket failed, error:%s\n", strerror(errno));
		return -1;
	}

	nOpt = 1;
	setsockopt(nListenFd, SOL_SOCKET, SO_REUSEADDR, &nOpt, sizeof(nOpt));

	tServerAddr.sin_family = AF_INET;
	tServerAddr.sin_port = htons(5432);
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

	tTmpEvent.events = EPOLLIN;
	tTmpEvent.data.fd = nListenFd;
	nRet = epoll_ctl(nEpollFd, EPOLL_CTL_ADD, nListenFd, &tTmpEvent);
	if (-1 == nRet)
	{
		printf("epoll_ctl add listen fd failed, error:%s\n", strerror(errno));
		return -1;
	}

	while (1)
	{
		nEpollCnt = epoll_wait(nEpollFd, atEpollEvent, 16, 500);
		if (0 == nEpollCnt) // timeout，没有文件描述符属性变化
		{
			continue;
		}
		else if (-1 == nEpollCnt) // 出错
		{
			printf("epoll_wait failed, error:%s\n", strerror(errno));
			continue;
		}
		else // 有文件描述符变化
		{
			for (i = 0; i < nEpollCnt; i++)
			{
				if (atEpollEvent[i].events & EPOLLIN) // 读事件
				{
					if (atEpollEvent[i].data.fd == nListenFd) // 有新的连接
					{
						nAddrLen = sizeof(tClientAddr);
						nAcceptFd = accept(nListenFd, (struct sockaddr*)&tClientAddr, &nAddrLen);
						if (-1 == nAcceptFd)
						{
							printf("accept failed, error:%s\n", strerror(errno));
							continue;
						}
						else
						{
							inet_ntop(AF_INET, &tClientAddr.sin_addr.s_addr, achAddr, sizeof(achAddr));
							printf("new connection comes. %s:%d\n", achAddr, ntohs(tClientAddr.sin_port));

							tTmpEvent.events = EPOLLIN;
							tTmpEvent.data.fd = nAcceptFd;
							nRet = epoll_ctl(nEpollFd, EPOLL_CTL_ADD, nAcceptFd, &tTmpEvent);
							if (-1 == nRet)
							{
								printf("epoll_ctl add failed, error:%s\n", strerror(errno));
								continue;
							}
						}
					}
					else // 有数据接收
					{
						memset(achRecvBuf, 0, sizeof(achRecvBuf));
						nRet = recv(atEpollEvent[i].data.fd, achRecvBuf, sizeof(achRecvBuf), 0);
						if (0 == nRet || -1 == nRet) // 对方关闭或者出错
						{
							if (0 == nRet)
							{
								printf("peer closed\n");
							}
							else
							{
								printf("recv failed, error:%s\n", strerror(errno));
							}

							memset(&tTmpEvent, 0, sizeof(tTmpEvent));
							tTmpEvent.events = EPOLLIN;
							tTmpEvent.data.fd = atEpollEvent[i].data.fd;
							nRet = epoll_ctl(nEpollFd, EPOLL_CTL_DEL, atEpollEvent[i].data.fd, &tTmpEvent);
							if (-1 == nRet)
							{
								printf("epoll_ctl del failed, error:%s\n", strerror(errno));
							}

							close(atEpollEvent[i].data.fd);
							continue;
						}
						else // 收到数据
						{
							printf("recv:%s\n", achRecvBuf);
						}
					}
				}
			}
		}
	}

	close(nListenFd);
	close(nEpollFd);

	return 0;
}