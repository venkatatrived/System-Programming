In this scenario, there are three threads accessing the device node, 
Thread1 Opens the device in default mode1 and execute ioctl and changes the device node to mode2 and exists successfully.
Then Thread2 starts and opens the device. So count2=1 Then it sleeps for 4 units of time. 
In this time gap thread3 opens the device. So count2=2. Then thread3 also sleeps for 4 units of time. 
Mean while Thread2 wakes up and executes ioctl to change from MODE2 to MODE1. Since count2 > 1, it goes into wait queue queue2. 
Thread3 wakes up and executes ioctl to change from MODE2 to MODE1. Since count2 > 1, it also goes into wait queue queue2.

Now the system current state is Dead lock, since there are no other thread/process to wake up threads from queue2.

Timing Diagram

Thread1             Thread2             Thread3			
open()                                                      count1 = 1 and acquired sem2
ioctl()                                                     Change from mode1 to mode2. Count1 = 0 count2 = 1
close()                                                     count1 = 0 and count2 = 0
exit()

                    open()              sleep(3)            current mode is mode2, count2=1				
                    sleep(4)            open()              current mode is mode2, count2=2
                    ioctl()             sleep(4)            Thread2 is changing from mode2 to mode1 since count2>1 
                                                            JoinedQueue2 at line 174	
                                        ioctl()             Thread2 is changing from mode2 to mode1 since count2>1 
                                                            Joined Queue2 at line 174 <------ deadlock

