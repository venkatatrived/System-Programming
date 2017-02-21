#include <iostream>
#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>

//User defined header file
#include "structures.h"

// States of a philosopher
#define THINKING 0
#define HUNGRY 1
#define EATING 2

//LEFT and RIGHT philosophers with philosopher id i
#define LEFT(id,N) (id+N-1)%N
#define RIGHT(id,N) (id+1)%N 

using namespace std;

void thinking(int id);
void eating(int id);
void hungry(int id);
void take_fork(int id, int N, philstates* philstates_map, semaphore_t* shared, semaphore_t* phil_semap)   ;
void test(int id, int N, philstates* philstates_map, semaphore_t* phil_semap);
void put_fork(int id, int N, philstates* philstates_map, semaphore_t* philleft_semmap,semaphore_t* philright_semmap, semaphore_t* shared);
void still_thinking(int id);

int main(int argc, char *argv[])
{
	if(argc != 3)
    {
        cout << "Some of the arguments are missing\n";
        return -1;
    } 
    
    // N is No of philosophers, M is no of iterations
    int N = atoi(argv[1]);
    int M = atoi(argv[2]);
    int waitstatus;
    semaphore_t *philphr_semap[N];
    semaphore_t *barrier_semap, *phil_semap, *mutualex_semap;
    philstates *philstates_map;

	// Barrier Semaphore
    char barrier[20] =  "barrier_semaphore";
    barrier_semap = semaphore_create(barrier, N);
    if(barrier_semap == NULL)
		exit(1);
    semaphore_close(barrier_semap);

    //Philosopher States Semaphore
    char state[20] = "philstates_map";
    philstates_map = philstate_create(state, N);
    if(philstates_map == NULL)
		exit(1);
    philstate_close(philstates_map, N);

	//shared Global Semaphore
    char shared[20] = "mutualex_semaphore";
    mutualex_semap = semaphore_create(shared,1);
    if(mutualex_semap == NULL)
		exit(1);
   //cout << "shared semmap count" << mutualex_semap->count <<endl;
    semaphore_close(mutualex_semap);

    //Semaphore for each philosopher
	for(int i=0; i<N; i++)
	{
		string phil_str = "phil_semaphore"+to_string(i);
        char * philphr = new char[phil_str.size() + 1];
        copy(phil_str.begin(), phil_str.end(), philphr);
        philphr[phil_str.size()] = '\0';
		philphr_semap[i] = semaphore_create(philphr, 0);
		if(philphr_semap[i] == NULL)
			exit(1);
		semaphore_close(philphr_semap[i]);
	}

    //Creating N philosopher processes
	for(int i=0; i<N; i++)
	{
		pid_t pid = fork();
		if(pid == 0)
		{
            int philid = i;
            string philstr = "phil_semaphore"+to_string(philid);
            char * phil = new char[philstr.size() + 1];
            copy(philstr.begin(), philstr.end(), phil);
            phil[philstr.size()] = '\0';
            
            barrier_semap = semaphore_open(barrier);

            if (barrier_semap == NULL)
            {
                cout<< "Not able to Open barrier_semap" << endl;
                exit(1);
            }  
            bar_semaphore_wait(barrier_semap);

            philstates_map = philstate_open(state,N);
            if (barrier_semap == NULL)
            {
                cout<< "Not able to Open philstates_map" << endl;
                exit(1);
            } 

            mutualex_semap = semaphore_open(shared);
            if (mutualex_semap == NULL)
            {
                cout<< "Not able to Open mutualex_semap" << endl;
                exit(1);
            } 

            semaphore_t *phil_semap = semaphore_open(phil);
            if (phil_semap == NULL)
            {
                cout<< "Not able to Open phil_semap" << endl;
                exit(1);
            }

            string leftphil_str = "phil_semaphore"+to_string(LEFT(philid,N));
            char * leftphil = new char[leftphil_str.size() + 1];
            copy(leftphil_str.begin(), leftphil_str.end(), leftphil);
            leftphil[leftphil_str.size()] = '\0';

            semaphore_t *philleft_semap = semaphore_open(leftphil);
            if (philleft_semap == NULL)
            {
                cout<< "Not able to Open philleft_semap" << endl;
                exit(1);
            }

            string rightphil_str = "phil_semaphore"+to_string(RIGHT(philid,N));
            char * rightphil = new char[rightphil_str.size() + 1];
            copy(rightphil_str.begin(), rightphil_str.end(), rightphil);
            rightphil[rightphil_str.size()] = '\0';

            semaphore_t *philright_semap = semaphore_open(rightphil);
            if (philright_semap == NULL)
            {
                cout<< "Not able to Open philright_semap" << endl;
                exit(1);
            }

            for(int i=0; i<M; i++)  
            {
                //Before Take fork  Philospher is in thinking State,.
                thinking(philid);
                take_fork(philid, N, philstates_map, mutualex_semap, phil_semap);
                eating(philid);
                put_fork(philid, N, philstates_map, philleft_semap, philright_semap, mutualex_semap);
                //Think function is called inside put fork function
            }

            thinking(philid);
            semaphore_close(barrier_semap);
            philstate_close(philstates_map, N);
            semaphore_close(mutualex_semap);
            semaphore_close(phil_semap);
            semaphore_close(philleft_semap);
            semaphore_close(philright_semap);
            cout<<"Philosopher " << philid <<" is exiting\n";
			return 0;
		}
        else if(pid < 0)
        {
            cerr<< "Philosopher " << i << " process Creation failed" <<endl;
            exit(-1);
        }
	}

	for(int i=0; i<N; i++)
		wait(&waitstatus);

    cout << "Host is Exiting" << endl;
	return 0;
}


void thinking(int id)
{
    cout << "Philosopher "<< id<< " is in thinking state" << endl;
    //sleep(1);
}

void eating(int id)
{
    cout << "Philosopher "<< id<< " is in eating state" << endl;
    //sleep(1);
}

void hungry(int id)
{
    cout << "Philosopher "<< id<< " is in hungry state" << endl;
    //sleep(1);
}

void take_fork(int id, int N, philstates* philstates_map, semaphore_t* mutualex_semap, semaphore_t* phil_semap)   
{
    semaphore_wait(mutualex_semap);
    philstates_map->state[id] = HUNGRY;
    hungry(id);
    test(id, N, philstates_map, phil_semap);
    semaphore_post(mutualex_semap);
    semaphore_wait(phil_semap);
}

void test(int id, int N, philstates* philstates_map, semaphore_t* phil_semap)
{
    int left = philstates_map->state[LEFT(id,N)];
    int current = philstates_map->state[id];
    int right = philstates_map->state[RIGHT(id,N)];

    if((current ==  HUNGRY) && (left != EATING) && (right != EATING))
    {
        philstates_map->state[id] = EATING;
        semaphore_post(phil_semap);
    }
}

void put_fork(int id, int N, philstates* philstates_map, semaphore_t* philleft_semmap,semaphore_t* philright_semmap, semaphore_t* mutualex_semap)
{
    semaphore_wait(mutualex_semap);
    philstates_map->state[id] = THINKING;
    //thinking(id);
    test(LEFT(id,N), N, philstates_map, philleft_semmap);
    test(RIGHT(id,N), N, philstates_map, philright_semmap);
    semaphore_post(mutualex_semap);
    
}
