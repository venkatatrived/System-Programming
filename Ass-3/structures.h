//Below Semaphore structure is used for Philosopher semaphore, mutual exclusion semaphore, Barrier semaphore 
struct semaphore 
{
	pthread_mutex_t lock;
        pthread_cond_t nonzero;
        unsigned count;
};
typedef struct semaphore semaphore_t;


semaphore_t *semaphore_create(char *semaphore_name, int count);
semaphore_t *semaphore_open(char *semaphore_name);
void semaphore_post(semaphore_t *semap);
void semaphore_wait(semaphore_t *semap);
void semaphore_close(semaphore_t *semap);
void bar_semaphore_wait(semaphore_t *semap);

//Below structure is used for philosopher state and the array is created and initialized based on no of philosophers in philstate_create function
struct philosopherstate
{
	int state[];
};
typedef struct philosopherstate philstates;

//N is no of Philosophers
philstates* philstate_create(char *state_filename, int N);
philstates* philstate_open(char *state_filename, int N);
void philstate_close(philstates *philstate_map, int N);