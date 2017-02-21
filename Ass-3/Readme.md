
Implementation of Semaphore-based and deadlock free Dining philosopher problem with N philosophers, where each philosopher executes M no of times using mmap/munmap system calls and PTHREAD_PROCESS_SHARED feature for condition and mutex variables.

Each of the child process which are philosophers in our case, co-ordinate using the process sharable semaphores created by the parent and implement a dinning philosopher problem solution discussed in class lecture.

Input Arguments to host_and_philosopher.cc - N: No of philosophers, M: No of iterations 

How to run program
---------------------------------------------------------------------
Extract ASP_Ass3 zip file and go inside the folder. Execute the following commands

make clean
make 
./host No_of_philosophers No_of_iterations 

Example: ./host 4 4




