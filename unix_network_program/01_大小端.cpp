#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef union
{
	unsigned short int num;
	char buf[2];
}UData;

int main(void)
{
	UData tmp;
	memset(&tmp, 0, sizeof(tmp));

	tmp.num = 0x0102;
	
	if (0x01 == tmp.buf[0] && 0x02 == tmp.buf[1])
	{
		printf("Big end.\n");
	}
	else if (0x02 == tmp.buf[0] && 0x01 == tmp.buf[1])
	{
		printf("Small end.\n");
	}
	else
	{
		printf("unknown end.\n");
	}

	return 0;
}