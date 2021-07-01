/*
 * @Author: machun Michael 
 * @Date: 2021-06-10 04:53:01 
 * @Last Modified by: machun Michael
 * @Last Modified time: 2021-06-10 04:57:48
 */

/*
    description: 
    0. getsockopt获取TCP socket发送接收缓冲区大小
    1. getsockopt获取TCP发送缓冲区低水位标志、TCP接收缓冲区低水位标志
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>

int main()
{
    int nRet = 0;
    int nTcpSockFd = 0;
    int nRcvBufLen = 0;
    int nSndBufLen = 0;
    int nRcvLowFlag = 0;
    int nSndLowFlag = 0;
    socklen_t dwLen = 0;
    
    // 0. 获取TCP socket发送缓冲区大小
    nTcpSockFd = socket(AF_INET, SOCK_STREAM, 0);
    if (nTcpSockFd < 0)
    {
        printf("create tcp socket failed, ret:%d, error:%s\n", nTcpSockFd, strerror(errno));
        return -1;
    }
    
    dwLen = sizeof(nRcvBufLen);
    nRet = getsockopt(nTcpSockFd, SOL_SOCKET, SO_RCVBUF, &nRcvBufLen, &dwLen);
    if (0 != nRet)
    {
        printf("getsockopt tcp socket SO_RCVBUF failed, ret:%d, error:%s\n", nRet, strerror(errno));
        return -1;
    }
    else
    {
        printf("getsockopt tcp socket SO_RCVBUF succ, nRcvBufLen:%d\n", nRcvBufLen);
    }

    dwLen = sizeof(nSndBufLen);
    nRet = getsockopt(nTcpSockFd, SOL_SOCKET, SO_SNDBUF, &nSndBufLen, &dwLen);
    if (0 != nRet)
    {
        printf("getsockopt tcp socket SO_SNDBUF failed, ret:%d, error:%s\n", nRet, strerror(errno));
        return -1;
    }
    else
    {
        printf("getsockopt tcp socket SO_SNDBUF succ, nSndBufLen:%d\n", nSndBufLen);
    }

    // 1. 获取发送、接收缓冲区低水位标志
    dwLen = sizeof(nRcvLowFlag);
    nRet = getsockopt(nTcpSockFd, SOL_SOCKET, SO_RCVLOWAT, &nRcvLowFlag, &dwLen);
    if (0 != nRet)
    {
        printf("getsockopt tcp socket SO_RCVLOWAT failed, ret:%d, error:%s\n", nRet, strerror(errno));
        return -1;
    }
    else
    {
        printf("getsockopt tcp socket SO_RCVLOWAT succ, nRcvLowFlag:%d\n", nRcvLowFlag);
    }

    dwLen = sizeof(nSndLowFlag);
    nRet = getsockopt(nTcpSockFd, SOL_SOCKET, SO_SNDLOWAT, &nSndLowFlag, &dwLen);
    if (0 != nRet)
    {
        printf("getsockopt tcp socket SO_SNDLOWAT failed, ret:%d, error:%s\n", nRet, strerror(errno));
        return -1;
    }
    else
    {
        printf("getsockopt tcp socket SO_SNDLOWAT succ, nSndLowFlag:%d\n", nSndLowFlag);
    }

    return 0;
}