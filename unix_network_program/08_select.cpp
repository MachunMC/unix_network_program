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

	// �˿ڸ���
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

	// �ļ����������ϴ���
	FD_ZERO(&tOldSet);
	FD_ZERO(&tReadSet); 
	FD_SET(nListenFd, &tOldSet);
	nMaxFd = nListenFd;

	while (1)
	{
		tReadSet = tOldSet;

		nRet = select(nMaxFd + 1, &tReadSet, NULL, NULL, NULL);
		if (nRet > 0) // ���ļ��ɶ�
		{
			printf("%d fd change", nRet);

			// ����ķ���ֵ��ʵû�ã���Ϊֻ֪���м����ļ����������Ա仯����֪���������ĸ���
			// ����Ҫͨ�������ķ�ʽ�ҵ����Ա仯���ļ�������������м��кܶ��ļ�������û�ã���ʵЧ�ʺܵ͡�
			// epoll��Ƚ϶��ԣ��и��������Խ��ļ�����������ȥ�������Ͳ���Ҫȥ�������������Ч��
			// ������Ҫ���������ļ�������
			for (i = 1; i < nMaxFd; i++)
			{
				// �����Զ���һ�����飬���ļ����������浽������������
				// ����Ҫ���������ļ���������ֻ��Ҫ�����������ЧԪ�ؼ���
				if (FD_ISSET(i, &tReadSet))
				{
					if (i == nListenFd) // ���µ�����
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

							// ��ӵ������ļ�����������
							FD_SET(nAcceptFd, &tOldSet);
							if (nAcceptFd > nMaxFd)
							{
								nMaxFd = nAcceptFd;
							}
						}
					}
					else // �����ݿɶ� 
					{
						nRet = recv(i, achRecvBuf, (size_t)sizeof(achRecvBuf), 0);
						if (nRet == 0) // �Է��ر�
						{
							printf("client closed\n");

							// �Ӽ������ļ���������������������رո�socket
							FD_CLR(i, &tOldSet);
							close(i);
							continue;
						}
						else if (nRet < 0) // ����
						{
							printf("recv failed, error:%s\n", strerror(errno));

							FD_CLR(i, &tOldSet);
							close(i);
							continue;
						}
						else // ���յ�����
						{
							printf("recv %s\n", achRecvBuf);
						}
					}					
				}
			}
		}
		else if (nRet == -1) // ����
		{
			printf("select failed, error:%s\n", strerror(errno));
			continue;
		}
		else //��ʱ
		{
			printf("select timeout\n");
			continue;
		}
	}

	return 0;
}