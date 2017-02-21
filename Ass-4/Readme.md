Assignment-4
-------------
Venkata Trived Menta (UFID: 87371010)

Problem Statement
-----------------
Implementation of character device driver by extending the simple driver that is discussed in class.

Implementation
--------------
The directory contains program "char_driver.c" has the following functionalities implemented.

1. mycdrv_init_module
	This function gets executed when we run insmod command from terminal, this function is executed. It creates a ASP_mycdrv structure for every device followed by initialization of some of the data fields of the structure.
	It calls function mycdrv_setup_dev for each device structure,which will initialize and add the cdev field of the structure and create a node for each device. 

2.  mycdrv_cleanup_module
	This function gets executed when we run rmmod command from terminal, or if there are any failures in mycdrv_init_module execution. 
	It frees up all the resources that are allocated to the device nodes of char_driver module and destroys the device nodes and the module. 

3.	mycdrv_open
	This functions gets inode and file pointers as input. It makes sure that the file pointer points to the inode representing file.

4. mycdrv_read
	This function gets file_pointer, buffer, buffersize and file_data_pointer as input. Based on the Access direction of device, from the position pointed by file_data_pointer, it reads, buffersize bytes of data from the device pointed by file_pointer and copy it to input buffer.
	mycdrv_ead function can only access data up to address pointed by end of device data location. . 
	Here, If position of file_data_pointer + buffersize is greater than the address pointed by end of device data location, it only reads upto address pointed by end of device data and copy it to the input buffer. 

5. mycdrv_write
	This function gets file_pointer, buffer, buffersize and file_data_pointer as input. Based on the Access direction of device, from the position pointed by file_data_pointer, it copy buffersize bytes of data from input buffer and write it to the device pointed by the file_pointer.
	mycdrv_ead function can only access data up to address pointed by end of device data location. . 
	Here, If position of file_data_pointer + buffersize is greater than the address pointed by end of device data location, it only writes upto address pointed by end of device.

6. mycdrv_ioctl
	This function gets file_pointer, magic number and direction as input arguments. Magic number is like an identity to type of character device. It is used to check whether the correct device lseek function call is made or not. 
	After these checks, it will set the direction of data access  of device pointed by file_pointer based on the input direction value. 

7. mycdrv_llseek
	This function gets file_pointer, offset and origin as input. It sets the data access position of the device pointed by the file_pointer based on origin and offset values.

The directory also contains a program "userapp.c" which is used to test the above char_driver program.

Files
--------------
Driver Program: char_driver.c
User Application: userapp.c

Input Argument to userapp.c - <device_number>

How to run program
---------------------------------------------------------------------
Extract ASP_Ass4 zip file and go inside the folder. Execute the following commands

1) Compile driver module : $ make

2) Load module : $ sudo insmod char_driver.ko NUM_DEVICES=<num_devices>

3) Test driver :
	1) Compile userapp : $ make app
	2) Run userapp : $ sudo ./userapp <device_number>			
		where device_number identifies the id number of the device to be tested.   

	Note : userapp has to be executed with sudo privilege as the device files
		   in /dev/ are created  in the driver with root privileges.
		   
4) Unload module : $ sudo rmmod char_driver

Sample_output for Regular Read and Write:
-----------------------------------------------------------------------------
mvtrived@mvtrived-VirtualBox:~/Documents/asp$ sudo ./userapp 0

 Select one of the below choices
 r = read from device after seeking to desired offset
 w = write to device after seeking to desired offset
 c = change direction of data access
 e = to exit the program
 Note: maximum write/read data an userapplication can send/receive to/from device is set to 1000 bytes.

 enter command: w

 Origin of Write 
 0 = beginning <Sets f_pos to offset>
 1 = current <Will add offset to current f_pos>
 2 = end <Sets f_pos to end of ramdisk and add offset>

 enter origin: 0

 Enter offset value as 0 if no offset is needed 
 offset from Origin: 0

 Enter Data to write: hi this is a test program

 Writing 25 bytes of data to device 0
 Write is successful

 r = read from device after seeking to desired offset
 w = write to device after seeking to desired offset
 c = change direction of data access
 e = to exit the program

 enter command: r
 
 Please enter Read buffer size: 25

 Origin of Read 
 0 = beginning <Sets f_pos to offset>
 1 = current <Will add offset to current f_pos>
 2 = end <Sets f_pos to end of ramdisk and add offset>

 enter origin: 1

 Enter offset value as 0 if no offset is needed
 offset from Origin: -25

 Reading 25 bytes of data from device 0

 Read data from device0: hi this is a test program

 r = read from device after seeking to desired offset
 w = write to device after seeking to desired offset
 c = change direction of data access
 e = to exit the program

 enter command: e
mvtrived@mvtrived-VirtualBox:~/Documents/asp$

Sample_output for Reverse Read and Write:
-----------------------------------------------------------------------------
mvtrived@mvtrived-VirtualBox:~/Documents/asp$ sudo ./userapp 1

 Select one of the below choices
 r = read from device after seeking to desired offset
 w = write to device after seeking to desired offset
 c = change direction of data access
 e = to exit the program
 Note: maximum write/read data an userapplication can send/receive to/from device is set to 1000 bytes.

 enter command: c

 0 = regular 
 1 = reverse

 enter direction: 1


 r = read from device after seeking to desired offset
 w = write to device after seeking to desired offset
 c = change direction of data access
 e = to exit the program

 enter command: w

 Origin of Write 
 0 = beginning <Sets f_pos to offset>
 1 = current <Will add offset to current f_pos>
 2 = end <Sets f_pos to end of ramdisk and add offset>

 enter origin: 0

 Enter offset value as 0 if no offset is needed 
 offset from Origin: 100

 Enter Data to write: This is a test program for reverse read and write

 Writing 49 bytes of data to device 1
 Write is successful

 r = read from device after seeking to desired offset
 w = write to device after seeking to desired offset
 c = change direction of data access
 e = to exit the program

 enter command: r
 
 Please enter Read buffer size: 49

 Origin of Read 
 0 = beginning <Sets f_pos to offset>
 1 = current <Will add offset to current f_pos>
 2 = end <Sets f_pos to end of ramdisk and add offset>

 enter origin: 1

 Enter offset value as 0 if no offset is needed
 offset from Origin: 49

 Reading 49 bytes of data from device 1

 Read data from device1: This is a test program for reverse read and write

 r = read from device after seeking to desired offset
 w = write to device after seeking to desired offset
 c = change direction of data access
 e = to exit the program

 enter command: e



