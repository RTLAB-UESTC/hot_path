#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
void main()
{
	int fd;
	int ret;
	char buf[100];
	fd=open("/data/work/libtest.so",2);
	ret=read(fd,buf,100);
	printf("%d\n",ret);
}
