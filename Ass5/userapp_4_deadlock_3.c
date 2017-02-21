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

void *thread1(void *arg)
{
	printf("Opening Device from thread1 \n");
	int thread1_fd = open(DEVICE, O_RDWR);
	if(thread1_fd == -1) 
	{
		printf("File %s either does not exist or has been locked by another process\n", DEVICE);
		exit(-1);
	}
	printf("Calling Ioctl from thread1 \n");

	//Changing device mode to MODE2
	if (ioctl(thread1_fd, E2_IOCMODE2, 0))
	{ 
		perror("\n***error in ioctl***\n");
		exit (-1); 
	}

	close(thread1_fd);
	printf("Thread1 exited successfully by changing device from mode1 to mode2 \n");
	pthread_exit(NULL);
}
 
void *thread2(void *arg)
{
	printf("Opening Device from thread2 \n");
	int thread2_fd = open(DEVICE, O_RDWR);
	if(thread2_fd == -1) 
	{
		printf("File %s either does not exist or has been locked by another process\n", DEVICE);
		exit(-1);
	}

	sleep(4);
	printf("Calling Ioctl from thread2 \n");

	//Changing device mode to MODE1
	if (ioctl(thread2_fd, E2_IOCMODE1, 0))
	{ 
		perror("\n***error in ioctl***\n");
		exit (-1); 
	}
	printf("Closing device thread2");
	close(thread2_fd);
	//printf("Calling Close in thread2 \n");
	pthread_exit(NULL);
}

//Changes the device node to mode1
void *thread3(void *arg)
{
	sleep(3);
	printf("Opening Device in thread3 \n");
	int thread3_fd = open(DEVICE, O_RDWR);
	if(thread3_fd == -1) 
	{
		printf("File %s either does not exist or has been locked by another process\n", DEVICE);
		exit(-1);
	}

	sleep(4);
	printf("Calling Ioctl from thread3  \n");

	//Changing device mode to MODE1
	if (ioctl(thread3_fd, E2_IOCMODE1, 0))
	{ 
		perror("\n***error in ioctl***\n");
		exit (-1); 
	}
	printf("Closing device thread3");
	close(thread3_fd);
	//printf("Calling Close in thread3 \n");
	pthread_exit(NULL);
}

int main(int argc, char *argv[]) 
{

	int result = 0;
	pthread_t th1;
	pthread_t th2;
	pthread_t th3;
	int i=0;
	//printf("Creating thread1 \n");
    result = pthread_create(&th1, NULL, thread1, &i);
    if (result)
    {
        printf("Error:unable to create thread1 %d", result);
        exit(-1);
    }

    pthread_join(th1, NULL);

  	//printf("Creating thread2 \n");
    result = pthread_create(&th2, NULL, thread2, &i);
    if (result)
    {
        printf("Error:unable to create  thread2  %d", result);
        exit(-1);
    }


    //printf("Creating thread3 \n");
    result = pthread_create(&th3, NULL, thread3, &i);
    if (result)
    {
        printf("Error:unable to create thread3 %d", result);
        exit(-1);
    }

    pthread_exit(NULL);
    printf("Main Program Exiting successfully \n");
	return 0;
}