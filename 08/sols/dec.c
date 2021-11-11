#include <stdlib.h>
#include <stdio.h>
#include <util.h>

#define SYSCALL(name, r, sc, str, ...)	SYSCALL_EXIT(name,r,sc,str,__VA_ARGS__)

int main(int argc, char *argv[]) {
    int  x;
    int  r;

    if (argc>1) {
	x = atoi(argv[1]);
	if (x<0) goto fine;
	SYSCALL(write, r, write(1, &x,sizeof(x)), "Processo %d scrivendo: errno=%d\n", getpid(), errno);
    }

    do {
	SYSCALL(read,r,read(0, &x,sizeof(x)),"Processo %d leggendo: errno=%d\n", getpid(), errno);
	if (r==0) { 
	    fprintf(stderr, "Processo %d, esco perche' lo standard input e' chiuso!\n", getpid());
	    return 0;
	}
	fprintf(stderr, "Processo %d, ricevuto %d\n", getpid(),x);
	--x;
	if (x<0)  break;

	SYSCALL(write, r, write(1, &x,sizeof(x)), "Processo %d scrivendo: errno=%d\n", getpid(), errno);
    } while(1);
    
 fine:
    fprintf(stderr, "Processo %d, esco perche' raggiunto valore negativo\n", getpid());
    return 0;
}
