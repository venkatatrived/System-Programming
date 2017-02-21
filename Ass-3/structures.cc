#include <pthread.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <iostream>
#include <fcntl.h>
#include <fstream>
#include <string>

//User defined header file
#include "structures.h"

using namespace std;

//philosopher, barrier, shared semaphores functionalites
semaphore_t *semaphore_create(char *semaphore_name, int N)
{
    ifstream filepresent(semaphore_name);
    if(filepresent)
    {
        remove(semaphore_name);
        filepresent.close();
    }
    int fd, err;
    semaphore_t *semap;
    pthread_mutexattr_t psharedm;
    pthread_condattr_t psharedc;

    fd = open(semaphore_name, O_RDWR | O_CREAT | O_EXCL, 0666);
    if (fd < 0)
        return NULL;

    ftruncate(fd, sizeof(semaphore_t));
    pthread_mutexattr_init(&psharedm);
    err = pthread_mutexattr_setpshared(&psharedm, PTHREAD_PROCESS_SHARED);
    if(err)
    {
      cout<<"error in setting mutex attribute" << err <<endl;
      exit(-1); 
    }

    pthread_condattr_init(&psharedc);
    err = pthread_condattr_setpshared(&psharedc, PTHREAD_PROCESS_SHARED);
    if(err)
    {
      cout<<"error in setting conditional attribute" << err <<endl;
      exit(-1); 
    }

    semap = (semaphore_t *) mmap(NULL, sizeof(semaphore_t),PROT_READ | PROT_WRITE, MAP_SHARED,fd, 0);
    close (fd);

    err = pthread_mutex_init(&semap->lock, &psharedm);
    if(err)
    {
      cout<<"error in Mutex Initialization " << err <<endl;
      exit(-1); 
    }

    err=pthread_cond_init(&semap->nonzero, &psharedc);
    if(err)
    {
      cout<<"error in Condition Initialization" << err <<endl;
      exit(-1); 
    }

    semap->count = N;
    return semap;   
}

semaphore_t *semaphore_open(char *semaphore_name)
{
    semaphore_t* semap;
    int fd = open(semaphore_name, O_RDWR);
    if(fd < 0)  
        return NULL;

    semap = (semaphore_t*) mmap(NULL, sizeof(semaphore_t), PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);
    close(fd);
    return semap;
}

void semaphore_post(semaphore_t *semap)
{
    pthread_mutex_lock(&semap->lock);
    if(semap->count == 0)
    {
    	pthread_cond_signal(&semap->nonzero);
    }
        
    semap->count++;  
    pthread_mutex_unlock(&semap->lock);
}

void semaphore_wait(semaphore_t *semap)
{
    pthread_mutex_lock(&semap->lock);
    if(semap->count == 0)
    {       
        pthread_cond_wait(&semap->nonzero, &semap->lock);
    }

    semap->count--;
    pthread_mutex_unlock(&semap->lock);

}

void bar_semaphore_wait(semaphore_t *semap)
{
  pthread_mutex_lock(&semap->lock);
  semap->count--;
  if(semap->count != 0)
  { 
    pthread_cond_wait(&semap->nonzero, &semap->lock); 
  }
  pthread_cond_broadcast(&semap->nonzero);
  pthread_mutex_unlock(&semap->lock);
}

void semaphore_close(semaphore_t *semap)
{
    munmap((void*)semap, sizeof(semaphore_t));
}
//end of philosopher, barrier, shared semaphores functionalites

// Philsopherstate  functions
philstates* philstate_create(char *state_filename, int N)
{
  ifstream filepresent(state_filename);
    if(filepresent)
    {
        remove(state_filename);
        filepresent.close();
    }

  int fd;
  philstates* philstate_map;

  fd = open(state_filename, O_RDWR | O_CREAT | O_EXCL, 0666);
  if (fd < 0)
      return NULL;

  ftruncate(fd, (sizeof(philstates)+N*sizeof(int)));
  philstate_map = (philstates*)mmap(NULL, (sizeof(philstates)+N*sizeof(int)), PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);
  close (fd);

  // initialization of all philosopher states to thinking.
  for(int i=0; i<N; i++)
      philstate_map->state[i] = 0;

  return philstate_map;	
	
}

philstates* philstate_open(char *state_filename, int N)
{
	philstates* philstate_map;
	int fd = open(state_filename, O_RDWR);
	if(fd < 0)	
		return NULL;
	
	philstate_map = (philstates*) mmap(NULL, (sizeof(philstates)+N*sizeof(int)), PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);
	close(fd);

	return philstate_map;
}

void philstate_close(philstates *philstate_map, int N)
{
    munmap((void*)philstate_map, (sizeof(philstates)+N*sizeof(int)));
}
