#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <limits.h>
#include <errno.h>

int main(void){
    int pfd[2];
    long int v;
    if(pipe(pfd) == -1){ perror("error pipe"); exit(EXIT_FAILURE);};

    printf("Capacita' minima POSIX: %i\n", _POSIX_PIPE_BUF);
    errno = 0;

    if ((v = fpathconf(pfd[0],_PC_PIPE_BUF))==-1){
        if (errno != 0) {/* errore */}
        else {printf("reale = illimitato");}
    }
    else printf("reale: %li\n", v);

    return 0;
}