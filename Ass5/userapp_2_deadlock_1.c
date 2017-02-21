#include <linux/ioctl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <time.h>
#include <string.h>

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>

#define DEVICE "/dev/a5"

#define CDRV_IOC_MAGIC 'Z'
#define E2_IOCMODE1 _IOWR(CDRV_IOC_MAGIC, 1, int)
#define E2_IOCMODE2 _IOWR(CDRV_IOC_MAGIC, 2, int)

int main(int argc, char *argv[]) 
{
	int fd,fd1;
	printf("Opening the Device for first time\n");
	fd1 = open(DEVICE, O_RDWR);
	if(fd1 == -1) 
	{
		printf("File %s either does not exist or has been locked by another process\n", DEVICE);
		exit(-1);
	}
	
	printf("First Open executed Successfully\n");
	
	printf("Trying to Open the device Again, which will result in a deadlock\n");
	fd1 = open(DEVICE, O_RDWR);
	if(fd1 == -1) 
	{
		printf("File %s either does not exist or has been locked by another process\n", DEVICE);
		exit(-1);
	}

}