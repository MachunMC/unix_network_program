#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include "common.h"


int main()
{
	s32 i = 0;
	s32 nRet = 0;
	s32 nListenFd = 0;
	s32 nAcceptFd = 0;
	s32 nMaxFd = 0;
	s32 nOpt = 0;
	socklen_t nAddrLen = 0;
	fd_set tReadSet;
	fd_set tOldSet;
	s8  achAddr[64] = { 0 };
	s8  achRecvBuf[1500+1] = { 0 };
	struct sockaddr_in tServerAddr;
	struct sockaddr_in tClientAddr;

	memset(&tReadSet,  0, sizeof(tReadSet));
	memset(&tOldSet,   0, sizeof(tOldSet));
	memset(&tServerAddr, 0, sizeof(tServerAddr));
	memset(&tClientAddr, 0, sizeof(tClientAddr));

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
	tServerAddr.sin_port = htons(TCP_SERVER_PORT);
	tServerAddr.sin_addr.s_addr = 0;
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

	// 文件描述符集合处理
	FD_ZERO(&tOldSet);
	FD_ZERO(&tReadSet); 
	FD_SET(nListenFd, &tOldSet);
	nMaxFd = nListenFd;

	while (1)
	{
		tReadSet = tOldSet;

		nRet = select(nMaxFd + 1, &tReadSet, NULL, NULL, NULL);
		if (nRet > 0) // 有文件可读
		{
			printf("%d fd change", nRet);

			// 这里的返回值其实没用，因为只知道有几个文件描述符属性变化，不知道具体是哪个，
			// 还需要通过遍历的方式找到属性变化的文件描述符，如果中间有很多文件描述符没用，其实效率很低。
			// epoll相比较而言，有个参数可以将文件描述符传进去，这样就不需要去遍历，可以提高效率
			// 这里需要遍历所有文件描述符
			for (i = 1; i < nMaxFd; i++)
			{
				// 可以自定义一个数组，将文件描述符保存到这个数组里！！！
				// 不需要遍历所有文件描述符，只需要遍历数组的有效元素即可
				if (FD_ISSET(i, &tReadSet))
				{
					if (i == nListenFd) // 有新的连接
					{
						nAddrLen = sizeof(tClientAddr);
						nAcceptFd = accept(nListenFd, (struct sockaddr*)&tClientAddr, &nAddrLen);
						if (-1 == nAcceptFd)
						{
							printf("accept failed, error:%s\n", strerror(errno));
						}
						else
						{
							inet_ntop(AF_INET, &tClientAddr.sin_addr, achAddr, sizeof(achAddr));
							printf("new client comes. peer addr %s:%d\n", achAddr, ntohs(tClientAddr.sin_port));

							// 添加到监听文件描述符集合
							FD_SET(nAcceptFd, &tOldSet);
							if (nAcceptFd > nMaxFd)
							{
								nMaxFd = nAcceptFd;
							}
						}
					}
					else // 有数据可读 
					{
						nRet = recv(i, achRecvBuf, (size_t)sizeof(achRecvBuf), 0);
						if (nRet == 0) // 对方关闭
						{
							printf("client closed\n");

							// 从监听的文件描述符集合中清除，并关闭该socket
							FD_CLR(i, &tOldSet);
							close(i);
							continue;
						}
						else if (nRet < 0) // 出错
						{
							printf("recv failed, error:%s\n", strerror(errno));

							FD_CLR(i, &tOldSet);
							close(i);
							continue;
						}
						else // 接收到数据
						{
							printf("recv %s\n", achRecvBuf);
						}
					}					
				}
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