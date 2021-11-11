#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>

#define SYSCALL(r,c,e) if((r=c)==-1) { perror(e);exit(errno); }
int main(int argc, char *argv[]) {
    int  x,r;
    if (argc>1) {
	x = atoi(argv[1]);
		if (x<0) goto fine;
		SYSCALL(r, write(1, &x,sizeof(x)),"write1");
    }
    do {
	SYSCALL(r,read(0, &x,sizeof(x)),"read");
	if (r==0) { 
	    fprintf(stderr, "Processo %d, esco perche' lo standard input e' chiuso!\n", getpid());
	    return 0;
	}
	fprintf(stderr, "Processo %d, ricevuto %d\n", getpid(),x);
	--x;
	if (x<0)  break;
	SYSCALL(r, write(1, &x,sizeof(x)), "write2");
    } while(1);
 fine:
    fprintf(stderr, "Processo %d, esco perche' raggiunto valore negativo\n", getpid());
    return 0;
}