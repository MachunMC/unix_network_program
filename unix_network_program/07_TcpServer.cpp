#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include "common.h"


int main()
{
	s32 nRet = 0;
	s32 nSockFd = 0;
	s32 nAcceptFd = 0;
	socklen_t nAddrLen = 0;
	s8 achAddrBuf[64] = { 0 };
	s8 achReadBuf[256] = { 0 };
	struct sockaddr_in tServerAddr;
	struct sockaddr_in tClientAddr;

	memset(&tServerAddr, 0, sizeof(tServerAddr));
	memset(&tClientAddr, 0, sizeof(tClientAddr));


	// ����socket
	nSockFd = socket(AF_INET, SOCK_STREAM, 0);
	if (-1 == nSockFd)
	{
		printf("socket failed, ret:%d error:%s\n", nSockFd, strerror(errno));
		return -1;
	}

	// ���ö˿ڸ���
	s32 nPortReuse = 1;
	setsockopt(nSockFd, SOL_SOCKET, SO_REUSEADDR, &nPortReuse, sizeof(nPortReuse));

	// ��socket
	tServerAddr.sin_family = AF_INET;
	tServerAddr.sin_port = htons(12345);
	tServerAddr.sin_addr.s_addr = 0;	// 0��ʾ�󶨱������е�ַ��������һ��ʹ�������ַ
	nRet = bind(nSockFd, (struct sockaddr*)&tServerAddr, sizeof(tServerAddr));
	if (0 != nRet)
	{
		printf("%s %d bind failed, ret:%d error:%s\n", __FUNCTION__, __LINE__, nRet, strerror(errno));
		return -1;
	}

	// ���� listen
	nRet = listen(nSockFd, TCP_LISTEN_QUEUE_LEN);
	if (0 != nRet)
	{
		printf("listen failed, ret:%d error:%s\n", nRet, strerror(errno));
		return -1;
	}

	// ��ȡ accept
	nAddrLen = sizeof(tClientAddr);
	nAcceptFd = accept(nSockFd, (struct sockaddr*)&tClientAddr, &nAddrLen);
	if (-1 == nAcceptFd)
	{
		printf("accept failed, ret:%d error:%s\n", nRet, strerror(errno));
		return -1;
	}

	inet_ntop(AF_INET, &tClientAddr.sin_addr, achAddrBuf, sizeof(achAddrBuf));
	printf("client addr info: ip:%s, port:%d\n", achAddrBuf, ntohs(tClientAddr.sin_port));

	while (1)
	{
		// ��������
		memset(achReadBuf, 0, sizeof(achReadBuf));
		nRet = recv(nAcceptFd, achReadBuf, sizeof(achReadBuf), 0);
		if (0 == nRet) // �Զ˹ر�
		{
			printf("client close, break\n");
			break;
		}
		else
		{
			printf("recv:%d %s\n", nRet, achReadBuf);
		}

		// �ظ�����
		nRet = send(nAcceptFd, achReadBuf, strlen(achReadBuf), 0);
		if (nRet != strlen(achReadBuf))
		{
			printf("send %d bytes, expect %d bytes\n", nRet, strlen(achReadBuf));
		}
	}

	close(nSockFd);
	close(nAcceptFd);

	return 0;
}