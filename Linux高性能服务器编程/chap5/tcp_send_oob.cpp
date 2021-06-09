#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <unistd.h>

#define TCP_BUF_LEN     256

int main(int argc, char *argv[])
{
    int nRet = 0;
    int nSockFd = 0;
    char achNomalBuf[TCP_BUF_LEN] = "normal message";
    char achOobBuf[TCP_BUF_LEN] = "oob message";
    struct sockaddr_in tSrvAddr;

    memset(&tSrvAddr, 0, sizeof(tSrvAddr));

    if (argc < 3)
    {
        printf("input param error. usage:%s ip port\n", argv[0]);
        return -1;
    }

    nSockFd = socket(AF_INET, SOCK_STREAM, 0);
    if (nSockFd < 0)
    {
        printf("create tcp socket failed, ret:%d, error:%s\n", nSockFd, strerror(errno));
        return -1;
    }

    tSrvAddr.sin_family = AF_INET;
    tSrvAddr.sin_port = htons(atoi(argv[2]));
    inet_pton(AF_INET, argv[1], &tSrvAddr.sin_addr.s_addr);
    nRet = connect(nSockFd, (struct sockaddr*)&tSrvAddr, sizeof(tSrvAddr));
    if (nRet != 0)
    {
        printf("connet %s:%d failed, ret:%d, error:%s\n", argv[1], argv[2], nRet, strerror(errno));
        return -1;
    }

    nRet = send(nSockFd, achNomalBuf, strlen(achNomalBuf), 0);
    if (nRet < 0)
    {
        printf("send normal message 1 failed, ret:%d, error:%s\n", nRet, strerror(errno));
        return -1;
    }

    nRet = send(nSockFd, achOobBuf, strlen(achOobBuf), MSG_OOB);
    if (nRet < 0)
    {
        printf("send oob message failed, ret:%d, error:%s\n", nRet, strerror(errno));
        return -1;
    }

    nRet = send(nSockFd, achNomalBuf, strlen(achNomalBuf), 0);
    if (nRet < 0)
    {
        printf("send normal message 2 failed, ret:%d, error:%s\n", nRet, strerror(errno));
        return -1;
    }

    close(nSockFd);
    return 0;
}