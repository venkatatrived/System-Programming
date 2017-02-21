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

//Trying to Change the device node from mode1 to mode2 
void *thread1(void *arg)
{
	printf("Executing thread1 \n");
	int thread1_fd = open(DEVICE, O_RDWR);
	if(thread1_fd == -1) 
	{
		printf("File %s either does not exist or has been locked by another process\n", DEVICE);
		exit(-1);
	}
	//printf("Opened device in thread1 \n");
	sleep(10);
	printf("Calling Ioctl from thread1 to change to mode2, which results in a deadlock\n");

	//Changing device mode to MODE1
	if (ioctl(thread1_fd, E2_IOCMODE2, 0))
	{ 
		perror("\n***error in ioctl***\n");
		exit (-1); 
	}

	close(thread1_fd);
	printf("Exiting from  thread1 \n");
	pthread_exit(NULL);
}

//Just Open the device node
void *thread2(void *arg)
{
	sleep(4);
	printf("Excuting thread2 \n");
	int thread2_fd = open(DEVICE, O_RDWR);
	if(thread2_fd == -1) 
	{
		printf("File %s either does not exist or has been locked by another process\n", DEVICE);
		exit(-1);
	}

	//printf("Opened device in thread2 \n");
	//sleep(2);
	close(thread2_fd);
	printf("Exiting from  thread2 \n");
	pthread_exit(NULL);
}

int main(int argc, char *argv[]) 
{

	int result = 0;
	pthread_t th1;
	pthread_t th2;
	int i=0;
	//printf("Creating thread1 \n");
    result = pthread_create(&th1, NULL, thread1, &i);
    if (result)
    {
        printf("Error:unable to create thread1 %d", result);
        exit(-1);
    }

    //printf("Creating thread2 \n");
    result = pthread_create(&th2, NULL, thread2, &i);
    if (result)
    {
        printf("Error:unable to create thread2 %d", result);
        exit(-1);
    }

    pthread_join(th1, NULL);
    pthread_join(th2, NULL);
    pthread_exit(NULL);
	return 0;
}