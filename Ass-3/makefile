default: host_and_philosopher

host_and_philosopher: host_and_philosopher.cc
	g++ -std=c++0x -o host host_and_philosopher.cc structures.cc -pthread

clean: 
	rm -rf host
	rm -rf phil_semaphore* barrier_semaphore mutualex_semaphore philstates_map