#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <limits.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/wait.h>

#define MAX_STR _POSIX_PIPE_BUF


int main(void){
    int pfd[2];
    pid_t pid1,pid2;


    if(pipe(pfd) == -1){ perror("error pipe"); exit(EXIT_FAILURE);};

    switch (pid1 = fork())
    {
    case -1:{perror("fork error -1");}
        break;
    case 0:
        if ( dup2(pfd[1],1)== -1) {/* errore */}
        else { 
            close(pfd[0]); 
            close(pfd[1]); 
            execlp("who", "who", (char*)NULL);
        }
        break;
    }

switch (pid2 = fork())
    {
    case -1:{perror("fork error -1");}
        break;
    case 0:
        if ( dup2(pfd[0],0)== -1) {/* errore */}
        else { 
            close(pfd[0]); 
            close(pfd[1]); 
            execlp("wc", "wc", (char*)NULL);
        }
        break;
    }
    close(pfd[0]);  
    close(pfd[1]); 
    waitpid(pid1,NULL,0); 
    waitpid(pid2,NULL,0);

    return 0;
}