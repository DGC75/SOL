#include <unistd.h>
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <errno.h>

#include <utils.h>

int main(int argc, char *argv[]) {
    
    pid_t pid1 = -1, pid2 = -1;
    int fromP1[2], toP1[2];
    int r;
    //PID1 E' QUELLO CHE RICEVE X E SCRIVE
    //PID2 LEGGE
    

    if(pipe(fromP1) == -1){ perror("error fromP1"); exit(EXIT_FAILURE);}
    if(pipe(toP1) == -1){ perror("error toP1"); exit(EXIT_FAILURE);}



    if((pid2 = fork()) == -1){ perror("fork pid2 err"); exit(EXIT_FAILURE);}

    if((pid2 != 0)){
        if((pid1 = fork()) == -1){ perror("fork pid1 err"); exit(EXIT_FAILURE);}  

        

    }
    if((pid2 != 0) && (pid1 != 0)){close(fromP1[0]);
                                   close(fromP1[1]); 
                                   close(toP1[0]);
                                   close(toP1[1]);  
                                    
                                    }

    if(pid2 == 0){
        
        printf("pid2 = %d\n", getpid());fflush(stdout);

        dup2(fromP1[0], 0);dup2(toP1[1], 1);
        
        close(fromP1[1]);close(toP1[0]);

        execl("./dec", "dec", (char *)NULL);
        
        perror("execl pid2");
        int myErr = errno;
        exit(myErr);
    }
    else if(pid1 == 0){
        
        printf("pid1 = %d\n", getpid());fflush(stdout);

        dup2(toP1[0], 0);dup2(fromP1[1], 1);

        close(fromP1[0]);close(toP1[1]);

        execl("./dec", "dec", argv[1], (char *)NULL);
        
        perror("execl pid1");
        int myErr = errno;
        exit(myErr);
    }


    (atoi(argv[1])%2==1)?(waitpid(pid1, NULL, 0)):(waitpid(pid2, NULL, 0));

    return 0;
}