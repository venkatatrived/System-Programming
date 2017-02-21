In this scenario, we are opening the device node twice continously in the same process, without execution of close device node. 
This will result in a dead lock, since the device node is in mode1 by default, first Open acquires sem2 lock and proceed further  When second open gets executed, it also opens in mode1 and waits for sem2. Since sem2 is not release by sem1.

Therefore current system is in a deadlock, since first open didn't release sem2.

Timing Diagram 
open()
open() <----------------- deadlock