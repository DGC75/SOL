/* 
 *  utilizza dec.c
 *
 */
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <errno.h>

// utility macro
#define SYSCALL(r,c,e) \
    if((r=c)==-1) { perror(e);exit(errno); }

// procedure eseguite dai processi 1 e 2 rispettivamente
void processo1(int in, int out, const char arg[]);
void processo2(int in, int out);

int main(int argc, char *argv[]) {    
    if (argc < 2) {
	fprintf(stderr, "usa: %s n\n", argv[0]);
	return EXIT_FAILURE;
    }

    // controllo che n sia positivo
    int n = atoi(argv[1]);
    if (n<0) {
	fprintf(stderr, "n deve essere maggiore di 0!\n");
	return EXIT_FAILURE;
    }
		
    int canale12[2];  // canale 1-->2
    int canale21[2];  // canale 2-->1

    int r;
    SYSCALL(r, pipe(canale12), "pipe1");
    SYSCALL(r, pipe(canale21), "pipe2");

    if (fork() == 0) {   // primo figlio 
	close(canale21[1]); 
	close(canale12[0]);
	processo1(canale21[0],canale12[1], argv[1]);
    } else {
	close(canale12[1]); 
	close(canale21[0]);
	if (fork() == 0)  // secondo figlio
	    processo2(canale12[0],canale21[1]);
	else {
	    // Il padre non usa le pipe quindi 
	    // chiudo tutti i descrittori che non uso
	    close(canale12[0]);	
	    close(canale12[1]);
	    close(canale21[0]);	
	    close(canale21[1]);
	}
    }       

    // attendo la terminazione del processo figlio
    //int status;
    SYSCALL(r, waitpid(-1, NULL, 0), "wait");
    //fprintf(stderr, "processo %d terminato, status=%d\n", r, WEXITSTATUS(status));
    SYSCALL(r, waitpid(-1, NULL, 0), "wait");
    //fprintf(stderr, "processo %d terminato, status=%d\n", r, WEXITSTATUS(status));
    return 0;
}


void processo1(int in, int out, const char arg[]) {
    fprintf(stderr, "Processo1 %d started\n", getpid());

    dup2(in,0);  // equivalente ad eseguire in modo atomico close(0) e dup(in);    
    dup2(out,1); // equivalente ad eseguire in modo atomico close(1) e dup(out);
    
    // chiudo tutti i descrittori che non uso prima di chiamare la exec
    close(in);
    close(out);
    
    execl("./dec", "dec", arg, NULL);    // solo ad uno dei due passo l'argomento
    
    perror("execl");
    exit(errno);
}

void processo2(int in, int out) {
    fprintf(stderr, "Processo2 %d started\n", getpid());

    dup2(in,0);  //close(0); dup(in);    
    dup2(out,1); //close(1); dup(out);
    
    // chiudo tutti i descrittori che non uso prima di chiamare la exec
    close(in);
    close(out);
    
    execl("./dec", "dec", NULL);
    
    perror("execl");
    exit(errno);
}
