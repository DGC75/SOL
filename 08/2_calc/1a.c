#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <limits.h>
#include <sys/types.h>
#include <wait.h>

#include <string.h>
#include <assert.h>

#define TRUE 1
#define MAX_STR _POSIX_PIPE_BUF

int main(void){
    
    int pfd[2];
    int pid, l, len;
    char msg[MAX_STR];


    while(TRUE){
        if(pipe(pfd) == -1){ perror("error pipe"); exit(EXIT_FAILURE);}
        memset(msg, 0, MAX_STR);
        fgets(msg, MAX_STR, stdin);

        l = write(pfd[1], msg, MAX_STR);
        write(pfd[1], "quit\n", MAX_STR);

        if((pid = fork()) == -1){ perror("fork err"); exit(EXIT_FAILURE);}

        if(pid){
            //PARENT



            if(!strncmp("quit\n", msg, 5)){

                close(pfd[0]);
                close(pfd[1]);
                break;
            }

            //printf("Operazione:'%s'", msg);
            memset(msg, 0, MAX_STR);
                        sleep(1);
            l = read(pfd[0], msg, MAX_STR);

            printf("Risultato:%s", msg);
            close(pfd[0]);
            close(pfd[1]);
        }
        else{
            //CHILD
 
            dup2(pfd[0], 0);
            dup2(pfd[1], 1);
            dup2(pfd[1], 2);
            execlp("bc","bc", "-lq", (char*)NULL);
            puts("you shouldn't be here");
            exit(EXIT_FAILURE);
        }

    }
    if(pid == 0){puts("Il child ritorna");}
    if(pid != 0){puts("Il parent ritorna");}
    return 0;
}