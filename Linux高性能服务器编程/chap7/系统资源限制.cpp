#include <stdio.h>
#include <string.h>
#include <sys/resource.h>
#include <sys/time.h>

int main()
{
    int nRet = 0;
    struct rlimit tRLimit;

    // 获取进程虚拟内存总量限制，单位：字节
    memset(&tRLimit, 0, sizeof(tRLimit));
    nRet = getrlimit(RLIMIT_AS, &tRLimit);
    if (0 == nRet)
    {
        printf("getrlimit succ, rlim_cur:%ld, rlim_max:%ld\n", tRLimit.rlim_cur, tRLimit.rlim_max);
    }
    else
    {
        printf("getrlimit failed\n");
    }

    // 获取core dump文件大小限制，单位：字节
    memset(&tRLimit, 0, sizeof(tRLimit));
    nRet = getrlimit(RLIMIT_CORE, &tRLimit);
    if (0 == nRet)
    {
        printf("getrlimit succ, rlim_cur:%ld, rlim_max:%ld\n", tRLimit.rlim_cur, tRLimit.rlim_max);
    }
    else
    {
        printf("getrlimit failed\n");
    }

    // 获取进程文件描述符数量限制
    memset(&tRLimit, 0, sizeof(tRLimit));
    nRet = getrlimit(RLIMIT_NOFILE, &tRLimit);
    if (0 == nRet)
    {
        printf("getrlimit succ, rlim_cur:%ld, rlim_max:%ld\n", tRLimit.rlim_cur, tRLimit.rlim_max);
    }
    else
    {
        printf("getrlimit failed\n");
    }

    // 获取进程CPU时间片限制
    memset(&tRLimit, 0, sizeof(tRLimit));
    nRet = getrlimit(RLIMIT_CPU, &tRLimit);
    if (0 == nRet)
    {
        printf("getrlimit succ, rlim_cur:%ld, rlim_max:%ld\n", tRLimit.rlim_cur, tRLimit.rlim_max);
    }
    else
    {
        printf("getrlimit failed\n");
    }

    return 0;
}