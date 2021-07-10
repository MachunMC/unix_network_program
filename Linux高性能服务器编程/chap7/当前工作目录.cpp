#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

int main(void)
{
    int nRet = 0;
    char *ptr = NULL;
    char achCurPath[256] = {0};
    char achDstPath[256] = "/usr/bin";

    // 获取当前工作目录
    ptr = getcwd(achCurPath, sizeof(achCurPath));
    if (ptr == NULL)
    {
        printf("getcwd failed, error:%s\n", strerror(errno));
    }
    else
    {
        printf("getcwd succ. current path:%s\n", ptr);
    }

    nRet = chdir(achDstPath);
    if (0 == nRet)
    {
        printf("change dir succ\n");
    }
    else
    {
        printf("change dir failed\n");
    }

    memset(achCurPath, 0, sizeof(achCurPath));
    ptr = getcwd(achCurPath, sizeof(achCurPath));
    if (ptr == NULL)
    {
        printf("getcwd failed, error:%s\n", strerror(errno));
    }
    else
    {
        printf("getcwd succ. current path:%s\n", ptr);
    }

    return 0;
}