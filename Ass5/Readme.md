Assignment-5 
-------------
Venkata Trived Menta (UFID: 87371010)

Problem Statement
-----------------
Check and Test a simple char driver to find out deadlocks and Name the critical regions

Implementation Details
----------------------
After Reviewing the code, it is observed that the device can run two modes MODE1 and MODE2. In MODE1, the device can be read/write/IOCTL by only single process/thread.
While executing IOCTL, there should be only one process/thread that is accessing the device and that is the process which wants to execute the IOCTL. If there are more than one process/thread opened/accessing the device, and the IOCTL is changing to a different from current mode, then this can cause a deadlock. 
There are 4 user application programs written to test the program. 
userapp_1.c            - It tests the read and write in MODE1 of the device.
userapp_2_deadlock_1.c - It tries to open the file more than once without closing it and runs into deadlock
userapp_3_deadlock_4.c - Multiple threads opened the device in MODE1 and one of the thread which has sem2 issues ioctl to change the device from MODE1 to MODE2 and runs into a deadlock
userapp_4_deadlock_4.c - Two threads opened the device in mode2 and both of them tries to change the device from MODE2 to MODE1 and runs into a deadlock
userapp_5.c            - It tests the read and write in MODE2 of the device.

How to run program
---------------------------------------------------------------------
Extract ASP_Ass5 zip file and go inside the folder. Execute the following commands
Switch to root user: sudo su
make clean
make 
insmod assignment5.ko
make apps

Run any of the userapp.


