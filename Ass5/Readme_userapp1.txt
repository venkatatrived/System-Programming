In this scenario, two threads are accessing the device node . 
Thread1 Opens the device node in default mode1 and writes wbuf data into device node by calling device node write entry function.
Thread1 closes the device node
Thread2 waits until thread1 is closed, then it opens the device node in default mode1 and reads data from device node into rbuf using read entry function

Timing Diagram

Thread1                     Thread2
open()                                        Thread 2 Opened the device
write()                                       Writes Data
close()	                                      closed the device
exit()                                        Thread2 Exited Successfully

                            open()            Thread 2 Opened the device            
                            read()            Reads Data
                            close()           Closed the device
                            exit()            Thread3 Exited Successful