# Map-reduce Program
Map-Reduce program to generate (key,value) pairs for the words in the input file name provided during execution using fork(), exec() and pipe() system calls.

Map-Reduce program has three executable files combiner, mapper and reducer. It is impemented such that, the combiner takes file name as command line argument. Combiner forks two child process, one executes mapper and other executes reducer. The output of mapper is piped as input to reducer using system call. The beauty of the program is that mapper and reducer doesn't know that they are reading/writing from/to pipe. 

Input - Text File Name containing the words. 

Output : (Key,Value) pairs for all the words in the given text file.
