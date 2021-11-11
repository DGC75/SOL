#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <limits.h>
#include <errno.h>
#include <string.h>
#include <wait.h>


#define N _POSIX_PIPE_BUF

int main(void){
    int pfd[2], pid, l = 0;
    char msg[N];


    while(1){
        if(pipe(pfd) == -1){ perror("error pipe"); exit(EXIT_FAILURE);}
        if((pid = fork()) == -1){ perror("fork err"); exit(EXIT_FAILURE);}

        if(pid){//PARENT
            
            fgets(msg, N, stdin);
            l = write(pfd[1], msg, N);
            if(!strncmp("quit\n", msg, 5)){close(pfd[0]); puts("received quit parent"); close(pfd[1]); break;}
        }
        else{//CHILD
            close(pfd[1]);
            l = read(pfd[0],msg, N);
            if(!strncmp("quit\n", msg, 5)){
                puts("received quit child");
                close(pfd[0]);
                break;
            }
            else if(l){
                //printf("Received:%s", msg);
                if ( dup2(pfd[1],1)== -1) {/* errore */}
                break;
            }
            close(pfd[0]);
        }

    }
    if(pid == 0){puts("Il figlio ritorna");}
    if(pid != 0){puts("Il padre ritorna");}
    return 0;
}