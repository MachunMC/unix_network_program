#include <stdio.h>
#include <arpa/inet.h>

int main(void)
{
	unsigned int ip = 0x01020304;
	unsigned short int port = 0x0102;

	printf("原始数据\n");
	printf("ip:   0x%08x\n", ip);
	printf("port: 0x%04x\n", port);

	printf("主机字节序转网络字节序\n");
	printf("htonl ip:   0x%08x\n", htonl(ip));
	printf("htons port: 0x%04x\n", htons(port));

	printf("网络字节序转主机字节序\n");
	printf("ntohl ip:   0x%08x\n", ntohl(htonl(ip)));
	printf("ntohs port: 0x%04x\n", ntohs(htons(port)));

	return 0;
}