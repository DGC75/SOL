/*
 * Semplice test per la struttura dati BQueue_t
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>
#include <pthread.h>
#include <errno.h>
#include <assert.h>
#include <boundedqueue.h>


// End-Of-Stream (EOS): valore speciale per la terminazione
#define EOS (void*)0x1   

typedef struct threadArgs {
    int      thid;
    BQueue_t *q;
    int      start;
    int      stop;
} threadArgs_t;

// thread produttore
void *Producer(void *arg) {
    BQueue_t *q = ((threadArgs_t*)arg)->q;
    int   myid  = ((threadArgs_t*)arg)->thid;
    int   start = ((threadArgs_t*)arg)->start;
    int   stop  = ((threadArgs_t*)arg)->stop;
    for(int i=start;i<stop; ++i) {
	int *data = malloc(sizeof(int));
	if (data == NULL) {
	    perror("Producer malloc");
	    pthread_exit(NULL);
	}
	*data = i;
	if (push(q, data) == -1) {
	    fprintf(stderr, "Errore: push\n");
	    pthread_exit(NULL);
	}
	printf("Producer%d pushed %d\n", myid, i);
    }
    printf("Producer%d exits\n", myid);
    pthread_exit(NULL);
}

// thread consumatore
void *Consumer(void *arg) {
    BQueue_t *q = ((threadArgs_t*)arg)->q;
    int   myid  = ((threadArgs_t*)arg)->thid;

    while(1) {
	int *data;
	data = pop(q);
	if (data == EOS) break;
	printf("Consumer%d: estratto %d\n", myid, *data);
	free(data);
    }

    printf("Consumer%d exits\n",myid);
    pthread_exit(NULL);
}


void usage(char *pname) {
    fprintf(stderr, "\nusa: %s -p <num-producers> -c <num-consumers> -n <num-messages>\n\n", pname);
    exit(EXIT_FAILURE);
}
	    

int main(int argc, char *argv[]) { 
    extern char *optarg;
    int p=0,c=0, n=0, opt;    
    while((opt = getopt(argc, argv, "p:c:n:")) != -1) {
	switch(opt) {
	case 'p':
	    p=atoi(optarg);
	    break;
	case 'c':
	    c=atoi(optarg);
	    break;
	case 'n':
	    n=atoi(optarg);
	    break;
	default:
	    usage(argv[0]);
	    break;
	}
    }
    if (p==0 || c==0 || n==0) 	usage(argv[0]);
    printf("num producers =%d, num consumers =%d\n", p, c);
    
    pthread_t    *th;
    threadArgs_t *thARGS;

    th     = malloc((p+c)*sizeof(pthread_t));
    thARGS = malloc((p+c)*sizeof(threadArgs_t));
    if (!th || !thARGS) {
	fprintf(stderr, "malloc fallita\n");
	exit(EXIT_FAILURE);
    }
    
    BQueue_t *q = initBQueue(10);
    if (!q) {
	fprintf(stderr, "initBQueue fallita\n");
	exit(errno);
    }
    
    int chunk = n/p, r= n%p;
    int start = 0;
    for(int i=0;i<p; ++i) {	
	thARGS[i].thid = i;
	thARGS[i].q    = q;
	thARGS[i].start= start;
	thARGS[i].stop = start+chunk + ((i<r)?1:0);
	start = thARGS[i].stop;	
    }
    for(int i=p;i<(p+c); ++i) {	
	thARGS[i].thid = i-p;
	thARGS[i].q    = q;
	thARGS[i].start= 0;
	thARGS[i].stop = 0;
    }    
    for(int i=0;i<c; ++i)
	if (pthread_create(&th[p+i], NULL, Consumer, &thARGS[p+i]) != 0) {
	    fprintf(stderr, "pthread_create failed (Consumer)\n");
	    exit(EXIT_FAILURE);
	}
    for(int i=0;i<p; ++i)
	if (pthread_create(&th[i], NULL, Producer, &thARGS[i]) != 0) {
	    fprintf(stderr, "pthread_create failed (Producer)\n");
	    exit(EXIT_FAILURE);
	}

    // aspetto prima tutti i produttori
    for(int i=0;i<p; ++i)
	pthread_join(th[i], NULL);
    // produco tanti EOS quanti sono i consumatori
    for(int i=0;i<c; ++i) {
	push(q, EOS);
    }
    // aspetto la terminazione di tutti i consumatori
    for(int i=0;i<c; ++i)
	pthread_join(th[p+i], NULL);

    // libero memoria
    deleteBQueue(q, NULL);
    free(th);
    free(thARGS);
    return 0;   
}
