/*
 * @Author: machun Michael 
 * @Date: 2021-06-11 03:00:11 
 * @Last Modified by: machun Michael
 * @Last Modified time: 2021-06-11 03:48:28
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>


int main()
{
    int nRet = 0;
    int fd[2] = {0};
    
    nRet = pipe(fd);
    if (0 != nRet)
    {
        printf("pipe failed, ret:%d, error:%s\n", nRet, strerror(errno));
        return -1;
    }
    else
    {
        printf("pipe succ. fd[0]:%d, fd[1]:%d\n", fd[0], fd[1]);
    }

    return 0;
}