#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>




int main (int argc, char *argv[]){

    pid_t pid = fork();
    
    if(pid < 0){
        perror("cannot fork");
        exit(EXIT_FAILURE);
    }
    else if(pid == 0){
        //FIGLIO 1

        if((pid = fork()) == 0){
            //figlio 2
            execvp("/bin/sleep", argv);
        }
        waitpid(pid, NULL, 0);
        puts("something");
    } 

    return 0;
}