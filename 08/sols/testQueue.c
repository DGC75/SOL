#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>
#include <pthread.h>
#include <errno.h>
#include <assert.h>

#include <queue.h>    // definisce il tipo Queue_t

// tipo di dato usato per passare gli argomenti al thread
typedef struct threadArgs {
    int      thid;
    Queue_t *q;
    int      start;
    int      stop;
} threadArgs_t;

// funzione eseguita dal thread produttore
void *Producer(void *arg) {
    Queue_t *q  = ((threadArgs_t*)arg)->q;
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
	printf("Producer %d pushed <%d>\n", myid, i);
    }
    printf("Producer%d exits\n", myid);
    return NULL;
}

// funzione eseguita dal thread consumatore
void *Consumer(void *arg) {
    Queue_t *q  = ((threadArgs_t*)arg)->q;
    int   myid  = ((threadArgs_t*)arg)->thid;

    size_t consumed=0;
    while(1) {
	int *data;
	data = pop(q);
	assert(data);
	if (*data == -1) { free(data); break;}
	++consumed;
	printf("Consumer %d: estratto <%d>\n", myid, *data);
	free(data);
    }

    printf("Consumer %d, consumed <%ld> messages, now it exits\n",myid,consumed);
    return NULL;
}


void usage(char *pname) {
    fprintf(stderr, "\nusa: %s -p <num-producers> -c <num-consumers> -n <num-messages>\n\n", pname);
    exit(EXIT_FAILURE);
}
	    

void print_queue(Queue_t *q){
    Node_t *node = q->head;
    puts("printing queue:");
    while(node != NULL){
        if(node->data != NULL)
            printf("%s\n", (char*)node->data);
        else{puts("NULL");}

        node = node->next;
    }
    puts("end queue");
}

int main(int argc, char *argv[]) { 
    extern char *optarg;
    int p=0,c=0, n=0, opt;
    
    // parsing degli argomenti
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
    
    Queue_t *q = initQueue();
    if (!q) {
	fprintf(stderr, "initQueue fallita\n");
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

    /* possibile protocollo di terminazione:
     * si aspettano prima tutti i produttori
     * quindi si inviano 'c' valori speciali (-1)
     * quindi si aspettano i consumatori
     */
    // aspetto prima tutti i produttori
    for(int i=0;i<p; ++i)
	pthread_join(th[i], NULL);
    // quindi termino tutti i consumatori
    for(int i=0;i<c; ++i) {
	int *eos = malloc(sizeof(int));
	*eos = -1;
	push(q, eos);
    }
    // aspetto la terminazione di tutti i consumatori
    for(int i=0;i<c; ++i)
	pthread_join(th[p+i], NULL);

    // libero memoria

    print_queue(q);

    deleteQueue(q);
    free(th);
    free(thARGS);
    return 0;   
}
