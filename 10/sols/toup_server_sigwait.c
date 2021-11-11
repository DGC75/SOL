#define _POSIX_C_SOURCE  200112L
#include <unistd.h>
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <stdlib.h>
#include <pthread.h>
#include <ctype.h>
#include <signal.h>

#include <util.h>
#include <conn.h>


/** 
 * tipo del messaggio
 */
typedef struct msg {
    int len;
    char *str;
} msg_t;


/*  global variables
 *  NOTA: entrambe le variabili potrebbero (dovrebbero!) essere passate al thread
 *        per riferimento nell'argomento. 
 */
int termina = 0;
int listenfd;

// funzione eseguita dal signal handler thread
static void *sigHandler(void *arg) {
    sigset_t *set = (sigset_t*)arg;

    for( ;; ) {
	int sig;
	int r = sigwait(set, &sig);
	if (r != 0) {
	    errno = r;
	    perror("FATAL ERROR 'sigwait'");
	    return NULL;
	}

	switch(sig) {
	case SIGINT:
	case SIGTERM:
	case SIGQUIT:
	    termina = 1;
	    // sblocca l'accept rendendo non piu' valido il listenfd
	    shutdown(listenfd, SHUT_RDWR);
	    // SOLUZIONE ALTERNATIVA CHE NON USA shutdown:
	    // Il thread dispatcher (in questo esercizio il main thread) invece di sospendersi su una accept si
	    // sospende su una select in cui, oltre al listen socket, registra il discrittore di lettura di una
	    // pipe condivisa con il thread sigHandler. Il thead sigHandler quando riceve il segnale chiude il
	    // descrittore di scrittura in modo da sbloccare la select.	    	    
	    return NULL;
	default:  ; 
	}
    }
    return NULL;	   
}

// converte tutti i carattere minuscoli in maiuscoli
void toup(char *str) {
    char *p = str;
    while(*p != '\0') { 
        *p = (islower(*p)?toupper(*p):*p); 
	++p;
    }        
}

void *threadF(void *arg) {
    assert(arg);
    long connfd = (long)arg;
    do {
	msg_t str;
	int n;
	if ((n=readn(connfd, &str.len, sizeof(int))) == -1) {
	    perror("read1");
	    break;
	}

	
	if (n==0) break;
	str.str = calloc((str.len), sizeof(char));
	if (!str.str) {
	    perror("calloc");
	    fprintf(stderr, "Memoria esaurita....\n");
	    break;
	}		    
	if ((n=readn(connfd, str.str, str.len * sizeof(char))) == -1) {
	    perror("read2");
	    free(str.str);
	    break;
	}

	toup(str.str);

	if ((n=writen(connfd, &str.len, sizeof(int))) == -1) {
	    perror("write1");
	    free(str.str);
	    break;
	}
	if ((n=writen(connfd, str.str, str.len*sizeof(char))) == -1) {
	    perror("write2");
	    free(str.str);
	    break;
	}
	free(str.str);
    } while(1);
    close(connfd);	    
    return NULL;
}

void spawn_thread(long connfd) {
    pthread_attr_t thattr;
    pthread_t thid;

    sigset_t mask,oldmask;
    sigemptyset(&mask);
    sigaddset(&mask, SIGINT); 
    sigaddset(&mask, SIGQUIT);

    if (pthread_sigmask(SIG_BLOCK, &mask,&oldmask) != 0) {
	fprintf(stderr, "FATAL ERROR\n");
	close(connfd);
	return;
    }

    
    if (pthread_attr_init(&thattr) != 0) {
	fprintf(stderr, "pthread_attr_init FALLITA\n");
	close(connfd);
	return;
    }
    // settiamo il thread in modalità detached
    if (pthread_attr_setdetachstate(&thattr,PTHREAD_CREATE_DETACHED) != 0) {
	fprintf(stderr, "pthread_attr_setdetachstate FALLITA\n");
	pthread_attr_destroy(&thattr);
	close(connfd);
	return;
    }
    if (pthread_create(&thid, &thattr, threadF, (void*)connfd) != 0) {
	fprintf(stderr, "pthread_create FALLITA");
	pthread_attr_destroy(&thattr);
	close(connfd);
	return;
    }

    if (pthread_sigmask(SIG_SETMASK, &oldmask, NULL) != 0) {
	fprintf(stderr, "FATAL ERROR\n");
	close(connfd);	
    }
}


int main(int argc, char *argv[]) {
    sigset_t     mask;
    sigemptyset(&mask);
    sigaddset(&mask, SIGINT); 
    sigaddset(&mask, SIGQUIT);
    sigaddset(&mask, SIGTERM);    
    if (pthread_sigmask(SIG_BLOCK, &mask,NULL) != 0) {
	fprintf(stderr, "FATAL ERROR\n");
	abort();
    }
    // ignoro SIGPIPE per evitare di essere terminato da una scrittura su un socket
    struct sigaction s;
    memset(&s,0,sizeof(s));    
    s.sa_handler=SIG_IGN;
    if ( (sigaction(SIGPIPE,&s,NULL) ) == -1 ) {   
	perror("sigaction");
	abort();
    } 
    
    pthread_t sighandler_thread;
    if (pthread_create(&sighandler_thread, NULL, sigHandler, &mask) != 0) {
	fprintf(stderr, "errore nella creazione del signal handler thread\n");
	abort();
    }

    SYSCALL_EXIT("socket", listenfd, socket(AF_UNIX, SOCK_STREAM, 0), "socket", "");

    struct sockaddr_un serv_addr;
    memset(&serv_addr, '0', sizeof(serv_addr));
    serv_addr.sun_family = AF_UNIX;    
    strncpy(serv_addr.sun_path, SOCKNAME, strlen(SOCKNAME)+1);

    int notused;
    SYSCALL_EXIT("bind", notused, bind(listenfd, (struct sockaddr*)&serv_addr,sizeof(serv_addr)), "bind", "");
    SYSCALL_EXIT("listen", notused, listen(listenfd, MAXBACKLOG), "listen", "");

    while(!termina) {      
	long connfd;
	if ((connfd= accept(listenfd, (struct sockaddr*)NULL ,NULL)) == -1) {
	    if (errno == EINVAL && termina) break;
	    perror("accept");
	    exit(-1);
	}
	//printf("connection accepted\n");    	
	spawn_thread(connfd);
    }
    // aspetto la terminazione de signal handler thread
    if (pthread_join(sighandler_thread, NULL) != 0) {
      fprintf(stderr, "Error joining sighandler_thread\n");
    }
    // clean-up
    unlink(SOCKNAME);
    return 0;
}
