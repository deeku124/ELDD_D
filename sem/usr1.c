#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>

int main()
{
    int fd;
    char Ubuff[20]="abc";
    fd=open("/dev/cdevice",O_RDWR,0777);
    write(fd,Ubuff,sizeof(Ubuff));
    close(fd);
}