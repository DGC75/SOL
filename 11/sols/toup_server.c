#define _POSIX_C_SOURCE 2001112L
#include <unistd.h>
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>
#include <ctype.h>
#include <signal.h>
#include <sys/select.h>

#include <threadpool.h>
#include <conn.h>
#include <util.h>
#include <threadF.h>


/**
 *  @struct sigHandlerArgs_t
 *  @brief struttura contenente le informazioni da passare al signal handler thread
 *
 */
typedef struct {
    sigset_t     *set;           /// set dei segnali da gestire (mascherati)
    int           signal_pipe;   /// descrittore di scrittura di una pipe senza nome
} sigHandler_t;


// funzione eseguita dal signal handler thread
static void *sigHandler(void *arg) {
    sigset_t *set = ((sigHandler_t*)arg)->set;
    int fd_pipe   = ((sigHandler_t*)arg)->signal_pipe;

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
	    //printf("ricevuto segnale %s, esco\n", (sig==SIGINT) ? "SIGINT": ((sig==SIGTERM)?"SIGTERM":"SIGQUIT") );
	    close(fd_pipe);  // notifico il listener thread della ricezione del segnale
	    return NULL;
	default:  ; 
	}
    }
    return NULL;	   
}

static void usage(const char*argv0) {
    fprintf(stderr, "use: %s threads-in-the-pool\n", argv0);
}
static void checkargs(int argc, char* argv[]) {
    if (argc != 2) {
	usage(argv[0]);
	_exit(EXIT_FAILURE);
    }
    if ((int)strtol(argv[1], NULL, 10)<=0) {
	fprintf(stderr, "threads-in-the-pool must be greater than zero\n\n");
	usage(argv[0]);
	_exit(EXIT_FAILURE);
    }
}
int main(int argc, char *argv[]) {
    checkargs(argc, argv);	    
    int threadsInPool      = (int)strtol(argv[1], NULL, 10);

    sigset_t mask;
    sigemptyset(&mask);
    sigaddset(&mask, SIGINT); 
    sigaddset(&mask, SIGQUIT);
    sigaddset(&mask, SIGTERM);
    
    if (pthread_sigmask(SIG_BLOCK, &mask,NULL) != 0) {
	fprintf(stderr, "FATAL ERROR\n");
	goto _exit;
    }

    // ignoro SIGPIPE per evitare di essere terminato da una scrittura su un socket
    struct sigaction s;
    memset(&s,0,sizeof(s));    
    s.sa_handler=SIG_IGN;
    if ( (sigaction(SIGPIPE,&s,NULL) ) == -1 ) {   
	perror("sigaction");
	goto _exit;
    } 

    /*
     * La pipe viene utilizzata come canale di comunicazione tra il signal handler thread ed il 
     * thread lisener per notificare la terminazione. 
     * Una alternativa è quella di utilizzare la chiamata di sistema 
     * 'signalfd' ma non e' POSIX.
     *
     */
    int signal_pipe[2];
    if (pipe(signal_pipe)==-1) {
	perror("pipe");
	goto _exit;
    }
    
    pthread_t sighandler_thread;
    sigHandler_t handlerArgs = { &mask, signal_pipe[1] };
   
    if (pthread_create(&sighandler_thread, NULL, sigHandler, &handlerArgs) != 0) {
	fprintf(stderr, "errore nella creazione del signal handler thread\n");
	goto _exit;
    }
    
    int listenfd;
    if ((listenfd= socket(AF_UNIX, SOCK_STREAM, 0)) == -1) {
	perror("socket");
	goto _exit;
    }

    struct sockaddr_un serv_addr;
    memset(&serv_addr, '0', sizeof(serv_addr));
    serv_addr.sun_family = AF_UNIX;    
    strncpy(serv_addr.sun_path, SOCKNAME, strlen(SOCKNAME)+1);

    if (bind(listenfd, (struct sockaddr*)&serv_addr,sizeof(serv_addr)) == -1) {
	perror("bind");
	goto _exit;
    }
    if (listen(listenfd, MAXBACKLOG) == -1) {
	perror("listen");
	goto _exit;
    }

    threadpool_t *pool = NULL;

    pool   = createThreadPool(threadsInPool, threadsInPool); 
    if (!pool) {
	fprintf(stderr, "ERRORE FATALE NELLA CREAZIONE DEL THREAD POOL\n");
	goto _exit;
    }
    
    fd_set set, tmpset;
    FD_ZERO(&set);
    FD_ZERO(&tmpset);

    FD_SET(listenfd, &set);        // aggiungo il listener fd al master set
    FD_SET(signal_pipe[0], &set);  // aggiungo il descrittore di lettura della signal_pipe
    
    // tengo traccia del file descriptor con id piu' grande
    int fdmax = (listenfd > signal_pipe[0]) ? listenfd : signal_pipe[0];

    volatile long termina=0;
    while(!termina) {
	// copio il set nella variabile temporanea per la select
	tmpset = set;
	if (select(fdmax+1, &tmpset, NULL, NULL, NULL) == -1) {
	    perror("select");
	    goto _exit;
	}
	// cerchiamo di capire da quale fd abbiamo ricevuto una richiesta
	for(int i=0; i <= fdmax; i++) {
	    if (FD_ISSET(i, &tmpset)) {
		long connfd;
		if (i == listenfd) { // e' una nuova richiesta di connessione 
		    if ((connfd = accept(listenfd, (struct sockaddr*)NULL ,NULL)) == -1) {
			perror("accept");
			goto _exit;
		    }

		    long* args = malloc(2*sizeof(long));
		    if (!args) {
		      perror("FATAL ERROR 'malloc'");
		      goto _exit;
		    }
		    args[0] = connfd;
		    args[1] = (long)&termina;
		    
		    int r =addToThreadPool(pool, threadF, (void*)args);
		    if (r==0) continue; // aggiunto con successo
		    if (r<0) { // errore interno
			fprintf(stderr, "FATAL ERROR, adding to the thread pool\n");
		    } else { // coda dei pendenti piena
			fprintf(stderr, "SERVER TOO BUSY\n");
		    }
		    free(args);
		    close(connfd);
		    continue;
		}
		if (i == signal_pipe[0]) {
		    // ricevuto un segnale, esco ed inizio il protocollo di terminazione
		    termina = 1;
		    break;
		}
	    }
	}
    }
    
    destroyThreadPool(pool, 0);  // notifico che i thread dovranno uscire

    // aspetto la terminazione de signal handler thread
    pthread_join(sighandler_thread, NULL);

    unlink(SOCKNAME);    
    return 0;    
 _exit:
    unlink(SOCKNAME);
    return -1;
}

