default: combiner mapper reducer

combiner.o: combiner.c 
	gcc -c combiner.c -o combiner.o

combiner: combiner.o
	gcc combiner.o -o combiner

mapper.o: mapper.c 
	gcc -c mapper.c -o mapper.o

mapper: mapper.o
	gcc mapper.o -o mapper

reducer.o: reducer.c 
	gcc -c reducer.c -o reducer.o

reducer: reducer.o
	gcc reducer.o -o reducer

clean:
	-rm -f combiner.o mapper.o reducer.o
	-rm -f combiner mapper reducer