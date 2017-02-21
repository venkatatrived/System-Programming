#include <linux/ioctl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <time.h>
#include <string.h>

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>

#define DEVICE "/dev/mycdrv"

#define CDRV_IOC_MAGIC 'Z'
#define ASP_CHGACCDIR _IOW(CDRV_IOC_MAGIC, 1, int)


int main(int argc, char *argv[]) 
{
	if (argc < 2) {
		fprintf(stderr, "Device number not specified\n");
		return 1;
	}
	int dev_no = atoi(argv[1]);
	char dev_path[20];
	int i,fd;
	int rc=0;
	char ch1;
	char ch, write_buf[1000], read_buf[1000];
	int offset=0, origin=0, bufsize=0;
	int dir;
	sprintf(dev_path, "%s%d", DEVICE, dev_no);
	fd = open(dev_path, O_RDWR);
	if(fd == -1) 
	{
		printf("File %s either does not exist or has been locked by another "
				"process\n", DEVICE);
		exit(-1);
	}

	printf("\n Select one of the below choices");
	printf("\n r = read from device after seeking to desired offset"
			"\n w = write to device after seeking to desired offset");
	printf("\n c = change direction of data access");
	printf("\n e = to exit the program");
	printf("\n Note: maximum write/read data an userapplication can send/receive to/from device is set to 1000 bytes.");
	//printf("\n if you need more than 1000 bytes of data, please change the buffer size in the program and recompile it before ");
	printf("\n\n enter command: ");

	scanf("%c", &ch);
	while(ch != 'e')
	{
		switch(ch) 
		{
			case 'w':
				printf("\n Origin of Write \n 0 = beginning <Sets f_pos to offset>"
					"\n 1 = current <Will add offset to current f_pos>" 
					"\n 2 = end <Sets f_pos to end of ramdisk and add offset>");
				printf("\n\n enter origin: ");
				scanf("%d", &origin);
				
				printf("\n Enter offset value as 0 if no offset is needed");
				printf(" \n offset from Origin: ");
				scanf("%d", &offset);
				printf("%d\n", offset);

				rc = lseek(fd, offset, origin);
				if(rc == -2)
				{
					printf("\n Seek postion Computed from Origin and Offset is out of bound");
					return -1;
				}
				else if(rc < 0)
				{
					printf("\n Not able to lseek. Possible reasons are invalid origin or offset value");
					return -1;
				}

				ch1 = getchar(); // to remove the next line entered while choosing the mode
				printf("\n Enter Data to write: ");
				i=0;
				memset(write_buf,0,1000);

				while( (ch1 = getchar()) != '\n')
				{
					//printf("Reached\n");
					write_buf[i] = ch1;
					i++;
				}
				if(i < 1)
				{
					printf("No data to write");
					break;
				}
				else if(i > 1000)
				{
					printf("More than 1000 bytes of data is given as input. Triming down to 1000 bytes\n");
					i=1000; 
				}

				printf("\n Writing %d bytes of data to device %d", i, dev_no);
				rc = write(fd, write_buf, i);
				if(rc > 0)
				{	
					printf("\n Write is successful");
				}
				else
				{
					fprintf(stderr, "Writing failed\n");
				}
				break;

			case 'r':
				printf(" \n Please enter Read buffer size: ");
				scanf("%d", &bufsize);
				if(bufsize < 1 )
				{
					printf("Buffer size is less than one\n");
					break;
				}
				else if(bufsize > 1000)
				{
					printf("Buffer size is more than 1000. Setting buffer size to 1000 bytes\n");
					bufsize = 1000;
				}

				printf("\n Origin of Read \n 0 = beginning <Sets f_pos to offset>"
					"\n 1 = current <Will add offset to current f_pos>" 
					"\n 2 = end <Sets f_pos to end of ramdisk and add offset>");
				printf("\n\n enter origin: ");
				scanf("%d", &origin);

				printf("\n Enter offset value as 0 if no offset is needed");
				printf("\n offset from Origin: ");
				scanf("%d", &offset);
				rc = lseek(fd, offset, origin);
				if( rc == -2)
				{
					printf("\n Seek postion Computed from Origin and Offset is out of bound");
					return -1;
				}
				else if(rc < 0)
				{
					printf("\n Not able to lseek. Possible reasons are invalid origin or offset value");
					return -1;
				}

				memset(read_buf,0,1000);
				printf("\n Reading %d bytes of data from device %d", bufsize, dev_no);
				rc = read(fd, read_buf, bufsize);
				if ( rc > 0) 
				{
					printf("\n\n Read data from device%d: %s", dev_no, read_buf);
				} 
				else
				{
					fprintf(stderr, "Reading failed\n");
				}
				ch1 = getchar();
				break;

			case 'c':
				printf("\n 0 = regular \n 1 = reverse");
				printf("\n\n enter direction: ");
				scanf("%d", &dir);
				rc = ioctl(fd, ASP_CHGACCDIR, dir);
				if (rc == -1)
				{ 
					perror("\n***error in ioctl***\n");
					return -1;
				}
				ch1 = getchar();
				break;

			default:
				printf("\n Command not recognized");
				break;

		}
	printf("\n\n r = read from device after seeking to desired offset"
			"\n w = write to device after seeking to desired offset");
	printf("\n c = change direction of data access");
	printf("\n e = to exit the program");
	//printf("\n Please note that maximum write/read data an userapplication can send/receive to/from device is 1000 bytes.");
	printf("\n\n enter command: ");

		scanf("%c", &ch);
	}
	close(fd);
	printf("\n Exiting the user program for Device %d", dev_no);
	return 0;
}
