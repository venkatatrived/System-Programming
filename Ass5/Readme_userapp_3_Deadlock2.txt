In this scenario, two threads are accessing the device node . 
Thread1 Opens the device node in default mode1 and sleeps for 10 time units. Note that at this point count1 = 1 and thread1 acquires sem2.
In this time gap, thread2 will be scheduled.
Thread2 intially sleeps for 4 units of time (to align thread2 open after thread1 open).
Now it opens the device node, and now count2=2. It waits for sem2 at line 49 of the driver code, 
After that thread1 executes ioctl to change from mode1 to mode2. Since count1>1, even thread1 will wait in the queue1. 
The system current state is a deadlock, since there are no other thread/process to wakeup thread from queue.1

Timing Diagram

Thread1                     Thread2
open()                      sleep(4)             count1=1 and thread1 acquired sem2
                            open()              count1=2 and thread is waiting for sem2, which is holded by thread1
sleep(10)										
ioctl(to mode2)                                 since count1 >1 thread1 goes into queueu1 at line 155 <---- deadlock						
