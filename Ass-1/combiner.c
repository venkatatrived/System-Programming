#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>

int main(int argc, char *argv[])
{
    
    //check for number input arguments
    if(argc < 2)
    {
        //perror("Invalid number of Arguments");
        printf("Input fileName argument is missing\n");
        return -1;
    }

    //check for pipe conditions 
    int filedes[2];
    pipe(filedes);
    pid_t mapper = fork();

    //check for creation is successful or not
    if(mapper == 0)
    {
        close(filedes[0]);
        dup2(filedes[1], 1);
        execl("mapper", "mapper", argv[1],0); 
    }
    else if(mapper > 0)
    {
        //waitpid(mapper,NULL,0);
        pid_t reducer = fork();
        if(reducer == 0)
        {
            //Write end is closed
            close(filedes[1]);
            dup2(filedes[0], 0);
            execl("reducer","reducer",0);
        }
        else if(reducer > 0)
        {
            close(filedes[0]);
            close(filedes[1]);
            waitpid(reducer,NULL,0);
        }
        else if(reducer < 0)
        {
            perror("Fork Failed\n");
            return -1;
        }
        //reducer
        //pipe system call
    }
    else if(mapper < 0)
    {
        perror("Fork Failed\n");
        return -1;
    }
        
    //printf("Hi  %d  \n", mapper);
    return 0;    
}