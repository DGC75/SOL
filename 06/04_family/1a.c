        #include <stdio.h>
        #include <stdlib.h>
        #include <errno.h>

       #include <sys/types.h>
       #include <unistd.h>
       #include <sys/types.h>
       #include <sys/wait.h>

       #include <time.h>


void rec_function(int n);

//NOTA: CI SAREBBERO DA AGGIUNGERE LE FFLUSH AD OGNI SCRITTURA

void print_dash(char c, int n){
    while(n-- > 0)
        printf("%c", c);
}

int main (int argc, char *argv[]){

    if(argc != 2){
        puts("Usage ./family N");
        exit(EXIT_FAILURE);
    }
    
    rec_function(atoi(argv[1]));

    return 0;
}

void rec_function(int n){
    srand(time(NULL));
    pid_t pid;
    int child_status;
    if(n <= 0){
        if((pid = fork()) == 0){
            printf("%d: sono l'ultimo discendente\n", getpid());
            exit(17);
        }
        else{
            pid = waitpid(pid, &child_status, 0);
            if(WIFEXITED(child_status))
                printf("%d: terminato con successo (%d)\n", pid, WEXITSTATUS(child_status));
        }
    }
    else{
        if((pid = fork()) == 0){
            print_dash('-', n);
            printf(" %d: creo un processo figlio\n", getpid());
            rec_function(n-1);
            exit( rand()%5);
        }
        else{
            pid = waitpid(pid, &child_status, 0);
            if(WIFEXITED(child_status))
                print_dash('-', n);
                printf("%d: terminato con successo (%d)\n", pid, WEXITSTATUS(child_status));
        }
    }
}