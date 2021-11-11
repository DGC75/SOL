#define _POSIX_C_SOURCE  200112L
#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/select.h>
#include <sys/time.h>
#include <sys/types.h>

#include <util.h>

static volatile sig_atomic_t sigintcounter = -1;
static volatile sig_atomic_t sigstpcounter = -1;
static volatile sig_atomic_t sigstpflag    =  0;

static void sighandler(int sig) {
    
    switch(sig) {
    case SIGINT: {
	if (sigintcounter == -1) sigintcounter = 0;
	++sigintcounter;
    } break;
    case SIGTSTP: {
	sigstpflag = 1;
	if (sigstpcounter == -1) sigstpcounter = 0;
	++sigstpcounter;
    } break;
    case SIGALRM: {
	return;
    } break;
    default:{	
	abort();
    }
    }
}


int main() {

    sigset_t mask, oldmask;
    sigemptyset(&mask);   // resetto tutti i bits
    sigaddset(&mask, SIGINT);  // aggiunto SIGINT alla machera
    sigaddset(&mask, SIGTSTP); // aggiunto SIGTSTP alla machera

    // blocco i segnali SIGINT e SIGTSTP finche' non ho finito
    // l'installazione degli handler -- mi conservo la vecchia maschera
    if (sigprocmask(SIG_BLOCK, &mask, &oldmask) == -1) {
	perror("sigprocmask");
	return EXIT_FAILURE;
    }
   
    // installo un unico signal handler per tutti i segnali che mi interessano
    struct sigaction sa;
    // resetto la struttura
    memset (&sa, 0, sizeof(sa));   
    sa.sa_handler = sighandler;
    sigset_t handlermask;
    sigemptyset(&handlermask);
    sigaddset(&handlermask, SIGINT);
    sigaddset(&handlermask, SIGTSTP);
    sigaddset(&handlermask, SIGALRM);
    sa.sa_mask = handlermask;  // l'handler eseguirà con SIGINT e SIGTSTP mascherati

    if (sigaction(SIGINT,  &sa, NULL) == -1) 
	perror("sigaction SIGINT");
    if (sigaction(SIGTSTP, &sa, NULL) == -1) 
	perror("sigaction SIGSTOP");
    if (sigaction(SIGALRM, &sa, NULL) == -1)
	perror("sigaction SIGALRM");

    if (system("clear")<0) {
      perror("system");
    }
    while(1) {
	// atomicamente setta la maschera e si sospende
        // come soluzione alternativa avrei potuto usare sigwait
        // senza installare il signal handler
	if (sigsuspend(&oldmask) == -1 && errno != EINTR) { 
	    perror("sigsuspend");
	    return (EXIT_FAILURE);
	}
	// qui i segnali sono nuovamente bloccati

	if (sigstpflag) {
	    printf("Ricevuti %d SIGINT\n", (sigintcounter<0)?0:sigintcounter);
	    sigstpflag = 0;
	    sigintcounter = 0;
	} 
	if (sigstpcounter == 3) {
	    sigstpcounter = 0;
	    //printf("\033[2J"); // Clear screen
	    printf("Per continuare premere invio, altrimenti verrai terminato entro 10 secondi\n" );
	    fflush(stdout);
	    alarm(10);
	    fd_set set, tmpset;
	    FD_ZERO(&set);
	    FD_SET(0, &set);
	    int cnt=10;
	    char c='\0';
	    do {
	      // questa parte non era prevista nell'esercizio......
	      // si usa una select per implementare il countdown
	      // nella stampa
	      
		struct timeval tm={1,0};
		tmpset = set;
		int r=select(0+1, &tmpset, NULL, NULL, &tm);
		switch(r) {
		case -1: {
		    if (errno == EINTR) {
			//printf("\033[2J"); // Clear screen (man console_code)
			printf("Terminato!\n");
			return 0;
		    }
		    perror("select");
		    exit(EXIT_FAILURE);
		} break;
		case 0: // timeout
		    --cnt;
		    printf("\033[1A");  // Move cursor up 1 row  (man console_code)
		    printf("Per continuare premere invio, altrimenti verrai terminato entro %d secondi \n",cnt);
		    fflush(stdout);
		    continue;
		default:;
		}

		if (read(0, &c, sizeof(char)) == -1 && errno != EINTR) {
		    perror("read");
		    return EXIT_FAILURE;
		}
		alarm(0); // resetto l'allarme..... potrei non farcela
		if (system("clear")<0) {
		  perror("system");
		}
	    } while(c=='\0' && cnt>0);
	}
    }
    return 0;
}
