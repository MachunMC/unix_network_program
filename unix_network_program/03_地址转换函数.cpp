#include "common.h"

int main(void)
{
	s32 nRet = 0;
	u32 dwIp = 0;
	s8 achBuf[16] = "192.168.17.136";
	u8 *p = NULL;

	printf("inet_pton 地址转换函数示例\n");

	nRet = inet_pton(AF_INET, achBuf, &dwIp);
	if (1 == nRet)
	{
		p = (unsigned char*)&dwIp;
		printf("inet_pton succ. ip %u %d.%d.%d.%d\n", dwIp, *(p + 0), *(p+1), *(p+2), *(p+3));
	}
	else
	{
		printf("inet_pton failed, ret:%d\n", nRet);
	}

	memset(achBuf, 0, sizeof(achBuf));
	printf("inet_ntop 地址转换函数示例\n");

	if (NULL == inet_ntop(AF_INET, &dwIp, achBuf, sizeof(achBuf)))
	{
		printf("inet_ntop failed, errno:%d, %s\n", errno, strerror(errno));
	}
	else
	{
		printf("inet_ntop succ, ip:%s\n", achBuf);
	}

	return 0;
}