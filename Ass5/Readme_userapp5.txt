In this scenario, two threads are accessing the device node . 
Thread1 Opens the device node in mode1, changes the mode from MODE1 to MODE2 and writes wbuf data into device node using write function
Thread1 closes the device node
Thread2 waits until thread1 is closed, it opens the device node in current MODE which is MODE2 and reads data from device node into rbuf using read entry function

Timing Diagram

Thread1         Thread2           
open()                                     Thread1 opened the device and changed the mode from
ioctl()                                    MODE1 to MODE2
write()                                    Writes Data
close()                                    closed the device
exit()									   Thread1 Exited Successfully

				open()                     Thread 2 Opened the device
				read()                     Reads Data
				close()                    closed the device
				exit()                     Thread2 Exited Successfully