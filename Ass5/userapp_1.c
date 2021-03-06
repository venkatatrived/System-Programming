#include <linux/ioctl.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#define DEVICE "/dev/a5"

#define CDRV_IOC_MAGIC 'Z'
#define E2_IOCMODE1 _IOWR(CDRV_IOC_MAGIC, 1, int)
#define E2_IOCMODE2 _IOWR(CDRV_IOC_MAGIC, 2, int)

int rc;

void *write_thread(void *arg)
{
	printf("Opening Device for write \n");
	char wbuf[30];
	int thread1_fd = open(DEVICE, O_RDWR);
	if(thread1_fd == -1) 
	{
		printf("File %s either does not exist or has been locked by another process\n", DEVICE);
		exit(-1);
	}

	memset(wbuf, 0, 22);
	sprintf(wbuf, "This is a general test");
	if (!(write(thread1_fd, wbuf, 22)))
	{ 
		perror("Write Unsuccessful \n");
		exit (-1); 
	}
	printf("Written Data: %s\n", wbuf);
	printf("Closing write thread \n");
	close(thread1_fd);
	pthread_exit(NULL);
}

void *read_thread(void *arg)
{
	printf("Opening Device for read \n");
	char rbuf[30];
	int thread2_fd = open(DEVICE, O_RDWR);
	if(thread2_fd == -1) 
	{
		printf("File %s either does not exist or has been locked by another process\n", DEVICE);
		exit(-1);
	}

	//sleep(4);

	memset(rbuf, 0, 30);
	if (!(read(thread2_fd, rbuf, 22)))
	{ 
		perror("Read Unsuccessful \n");
		exit (-1); 
	}
	printf("Read Data: %s\n", rbuf);
	printf("Closing read thread \n");
	close(thread2_fd);
	pthread_exit(NULL);
}

int main(int argc, char *argv[]) 
{

	int result = 0;
	pthread_t th1;
	pthread_t th2;
	int i=0;
	//printf("Creating thread1 \n");
    result = pthread_create(&th1, NULL, write_thread, &i);
    if (result)
    {
        printf("Error:unable to create thread1 %d", result);
        exit(-1);
    }

    pthread_join(th1, NULL);

    //printf("Creating thread2 \n");
    //sleep(2);

    result = pthread_create(&th2, NULL, read_thread, &i);
    if (result)
    {
        printf("Error:unable to create thread2 %d", result);
        exit(-1);
    }

    pthread_exit(NULL);
	return 0;
}