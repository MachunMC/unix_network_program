/*
 * @Author: machun Michael 
 * @Date: 2021-06-10 03:11:45 
 * @Last Modified by: machun Michael
 * @Last Modified time: 2021-06-10 03:12:06
 * description: getsockname和getpeername分别获取socket本地、对端ip和端口
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <unistd.h>

#define TCP_RECV_BUF 256
#define TCP_LISTEN_PORT 12345

int main(int argc, char *argv[])
{
    int nRet = 0;
    int nListenFd = 0;
    int nAcceptFd = 0;
    struct sockaddr_in tSrvAddr;
    char achRecvBuf[TCP_RECV_BUF] = {0};

    memset(&tSrvAddr, 0, sizeof(tSrvAddr));

    // if (argc < 2)
    // {
    //     printf("input param error. usage:%s port\n", argv[0]);
    //     return -1;
    // }

    nListenFd = socket(AF_INET, SOCK_STREAM, 0);
    if (nListenFd < 0)
    {
        printf("create tcp socket failed, ret:%d, error:%s\n", nListenFd, strerror(errno));
        return -1;
    }

    int nPortReuse = 1;
	setsockopt(nListenFd, SOL_SOCKET, SO_REUSEADDR, &nPortReuse, sizeof(nPortReuse));

    tSrvAddr.sin_family = AF_INET;
    // tSrvAddr.sin_port = htons(atoi(argv[1]));
    tSrvAddr.sin_port = htons(TCP_LISTEN_PORT);
    tSrvAddr.sin_addr.s_addr = 0;
    nRet = bind(nListenFd, (struct sockaddr*)&tSrvAddr, sizeof(tSrvAddr));
    if (nRet != 0)
    {
        printf("bind 0.0.0.0:%d failed, ret:%d, error:%s\n", argv[1], nRet, strerror(errno));
        return -1;
    }

    nRet = listen(nListenFd, 128);
    if (nRet != 0)
    {
        printf("listen failed, ret:%d, error:%s\n", nRet, strerror(errno));
        return -1;
    }

    nAcceptFd = accept(nListenFd, NULL, NULL);
    if (nAcceptFd < 0)
    {
        printf("accept failed, ret:%d, error:%s\n", nAcceptFd, strerror(errno));
        return -1;
    }

    struct sockaddr_in tLocalAddr;
    socklen_t nLocalLen = sizeof(tLocalAddr);
    getsockname(nAcceptFd, (struct sockaddr*)&tLocalAddr, &nLocalLen);
    char achLocalAddr[64] = {0};
    inet_ntop(AF_INET, &tLocalAddr.sin_addr.s_addr, achLocalAddr, sizeof(achLocalAddr));
    printf("local addr %s:%d\n", achLocalAddr, htons(tLocalAddr.sin_port));

    struct sockaddr_in tRemoteAddr;
    socklen_t nRemoteLen = sizeof(tRemoteAddr);
    getpeername(nAcceptFd, (struct sockaddr*)&tRemoteAddr, &nRemoteLen);
    char achRemoteAddr[64] = {0};
    inet_ntop(AF_INET, &tRemoteAddr.sin_addr.s_addr, achRemoteAddr, sizeof(achRemoteAddr));
    printf("remote addr %s:%d\n", achRemoteAddr, htons(tRemoteAddr.sin_port));

    nRet = recv(nAcceptFd, achRecvBuf, sizeof(achRecvBuf), 0);
    if (nRet < 0)
    {
        printf("recv failed, ret:%d, error:%s\n", nAcceptFd, strerror(errno));
        return -1;
    }
    else
    {
        printf("recv %s\n", achRecvBuf);
    }

    return 0;
}