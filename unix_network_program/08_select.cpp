//#include <stdio.h>
//#include <stdlib.h>
//#include <string.h>
//#include <sys/time.h>
//#include <sys/types.h>
//#include <unistd.h>
//#include "common.h"
//
//
//int main()
//{
//	s32 i = 0, j = 0;
//	s32 nRet = 0;
//	s32 nListenFd = 0;
//	s32 nAcceptFd = 0;
//	s32 nMaxFd = 0;
//	u32 dwSelectFdCnt = 0;
//	s32 anSelectFdSet[FD_SETSIZE] = { 0 }; // ����������ӵ����������ļ����������ϣ�����1024
//	s32 nOpt = 0;
//	socklen_t nAddrLen = 0;
//	fd_set tReadSet;
//	fd_set tOldSet;
//	s8  achAddr[64] = { 0 };
//	s8  achRecvBuf[1500+1] = { 0 };
//	struct sockaddr_in tServerAddr;
//	struct sockaddr_in tClientAddr;
//
//	memset(&tReadSet,  0, sizeof(tReadSet));
//	memset(&tOldSet,   0, sizeof(tOldSet));
//	memset(&tServerAddr, 0, sizeof(tServerAddr));
//	memset(&tClientAddr, 0, sizeof(tClientAddr));
//
//	nListenFd = socket(AF_INET, SOCK_STREAM, 0);
//	if (-1 == nListenFd)
//	{
//		printf("create ipv4 tcp socket failed, error:%s\n", strerror(errno));
//		return -1;
//	}
//
//	// �˿ڸ���
//	nOpt = 1;
//	setsockopt(nListenFd, SOL_SOCKET, SO_REUSEADDR, &nOpt, sizeof(nOpt));
//
//	tServerAddr.sin_family = AF_INET;
//	tServerAddr.sin_port = htons(TCP_SERVER_PORT);
//	tServerAddr.sin_addr.s_addr = 0;
//	nRet = bind(nListenFd, (struct sockaddr*)&tServerAddr, sizeof(tServerAddr));
//	if (-1 == nRet)
//	{
//		printf("bind 0.0.0.0:%d failed, error:%s\n", TCP_SERVER_PORT, strerror(errno));
//		return -1;
//	}
//
//	nRet = listen(nListenFd, TCP_LISTEN_QUEUE_LEN);
//	if (-1 == nRet)
//	{
//		printf("listen failed, error:%s\n", strerror(errno));
//		return -1;
//	}
//
//	// �ļ����������ϴ���
//	FD_ZERO(&tOldSet);
//	FD_ZERO(&tReadSet); 
//	FD_SET(nListenFd, &tOldSet);
//	nMaxFd = nListenFd;
//
//	anSelectFdSet[dwSelectFdCnt] = nListenFd;
//	dwSelectFdCnt++;
//
//	while (1)
//	{
//		tReadSet = tOldSet;
//
//		nRet = select(nMaxFd + 1, &tReadSet, NULL, NULL, NULL);
//		if (nRet > 0) // ���ļ��ɶ�
//		{
//			// ����ķ���ֵ��ʵû�ã���Ϊֻ֪���м����ļ����������Ա仯����֪���������ĸ���
//			// ����Ҫͨ�������ķ�ʽ�ҵ����Ա仯���ļ�������������м��кܶ��ļ�������û�ã���ʵЧ�ʺܵ͡�
//			// epoll��Ƚ϶��ԣ��и��������Խ��ļ�����������ȥ�������Ͳ���Ҫȥ�������������Ч��
//			// ������Ҫ���������ļ�������
//			for (i = 0; i < dwSelectFdCnt; i++)
//			{
//				if (FD_ISSET(anSelectFdSet[i], &tReadSet))
//				{
//					if (anSelectFdSet[i] == nListenFd) // ���µ�����
//					{
//						nAddrLen = sizeof(tClientAddr);
//						nAcceptFd = accept(nListenFd, (struct sockaddr*)&tClientAddr, &nAddrLen);
//						if (-1 == nAcceptFd)
//						{
//							printf("accept failed, error:%s\n", strerror(errno));
//						}
//						else
//						{
//							inet_ntop(AF_INET, &tClientAddr.sin_addr, achAddr, sizeof(achAddr));
//							printf("new client comes. peer addr %s:%d\n", achAddr, ntohs(tClientAddr.sin_port));
//
//							// ��ӵ������ļ�����������
//							FD_SET(nAcceptFd, &tOldSet);
//							if (nAcceptFd > nMaxFd)
//							{
//								nMaxFd = nAcceptFd;
//							}
//							anSelectFdSet[dwSelectFdCnt] = nAcceptFd;
//							dwSelectFdCnt++;
//						}
//					}
//					else // �����ݿɶ� 
//					{
//						memset(achRecvBuf, 0, sizeof(achRecvBuf));
//						nRet = recv(anSelectFdSet[i], achRecvBuf, (size_t)sizeof(achRecvBuf), 0);
//						if (0 == nRet || -1 == nRet) // �Է��رջ����
//						{
//							if (0 == nRet)
//							{
//								printf("client closed\n");
//							}
//							else
//							{
//								printf("recv failed, error:%s\n", strerror(errno));
//							}
//
//							// �Ӽ������ļ���������������������رո�socket
//							FD_CLR(anSelectFdSet[i], &tOldSet);
//							close(anSelectFdSet[i]);
//
//							// ������fd�����Ӧ��fd��0
//							// ���Խ����鳤�ȼ�1���������һֱ++��dwSelectFdCnt���ܳ���1024
//							for (j = 0; j < dwSelectFdCnt; j++)
//							{
//								if (anSelectFdSet[i] == anSelectFdSet[dwSelectFdCnt])
//								{
//									anSelectFdSet[dwSelectFdCnt] = 0;
//								}
//							}
//
//							continue;
//						}
//						else // ���յ�����
//						{
//							printf("recv %s\n", achRecvBuf);
//						}
//					}					
//				}
//			}
//		}
//		else if (nRet == -1) // ����
//		{
//			printf("select failed, error:%s\n", strerror(errno));
//			continue;
//		}
//		else //��ʱ
//		{
//			printf("select timeout\n");
//			continue;
//		}
//	}
//
//	return 0;
//}