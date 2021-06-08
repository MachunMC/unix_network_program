#include <iostream>
#include <sys/socket.h>
#include <linux/if_ether.h>
#include "common.h"

int main()
{
    s32 nSockFd = 0;

    nSockFd = socket(PF_PACKET, SOCK_RAW, htons(ETH_P_ALL));
    if (-1 == nSockFd)
    {
        printf("create raw socket failed, ret:%d, error:%s", nSockFd, strerror(errno));
        return -1;
    }

    


    return 0;
}