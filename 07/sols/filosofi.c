#define _POSIX_C_SOURCE 200112L
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>
#include <pthread.h>
#include <stdlib.h>
#include <errno.h>
#include <assert.h>
#include <stdatomic.h>

#include <util.h>

typedef pthread_mutex_t forchetta_t;
typedef struct threadArgs {
    int          thid;
    int          N;
    forchetta_t *forks;
} threadArgs_t;


static long ITER = 2000;  // durata della cena

// utility function
static inline long atomicDec(long *v) {  
  long r;
#if 0
  static pthread_mutex_t iter_lock = PTHREAD_MUTEX_INITIALIZER;  
  LOCK(&iter_lock);
  r = --(*v);
  UNLOCK(&iter_lock);
#else
  r = atomic_fetch_sub(v, 1);
#endif
  return r;
}

void Mangia(unsigned int *seed) {
    long r = rand_r(seed) % 800000;
    // per perdere un po' di tempo si puo'
    // usare anche il seguente ciclo for
    // for(volatile long i=0;i<r; ++i);
    struct timespec t={0, r};
    nanosleep(&t,NULL);
}
void Pensa(unsigned int *seed) {
    long r = rand_r(seed) % 1000000;
    //for(volatile long i=0;i<r; ++i);
    struct timespec t={0, r};
    nanosleep(&t,NULL);
}


// thread filosofo
void *Filosofo(void *arg) {
    int   myid  = ((threadArgs_t*)arg)->thid;
    int   N     = ((threadArgs_t*)arg)->N;
    int left    = myid % N;
    int right   = myid-1; 
    forchetta_t* destra   = &((threadArgs_t*)arg)->forks[right];
    forchetta_t* sinistra = &((threadArgs_t*)arg)->forks[left];
    unsigned int seed = myid * time(NULL);// un seed diverso per ogni filosofo
    int mangia = 0;
    int pensa  = 0;

    for( ;; ) {
	if (atomicDec(&ITER) <= 0) break;

	Pensa(&seed); 
	++pensa;

	// due versioni:
#if 0
	// fisso un ordinamento totale tra i filosofi per l'acquisizione delle forchette
	if (myid % 2) { // il filosofo di indice dispari prende prima la forchetta di destra
	    LOCK(destra);
	    LOCK(sinistra);
	    ++mangia;
	    Mangia(&seed);
	    UNLOCK(sinistra);
	    UNLOCK(destra);
	    
	} else {  // il filosofo di indice pari prende prima la forchetta di sinistra
	    LOCK(sinistra);
	    LOCK(destra);
	    ++mangia;
	    Mangia(&seed);
	    UNLOCK(destra);
	    UNLOCK(sinistra);
	}
#else
	// fisso un ordinamento totale tra i filosofi per l'acquisizione delle forchette
	// in questo caso l'ordinamento e' sull'indice delle forchette
	if (left < right) {
	    LOCK(destra);
	    LOCK(sinistra);
	    ++mangia;
	    Mangia(&seed);
	    UNLOCK(sinistra);
	    UNLOCK(destra);
	    
	} else {
	    LOCK(sinistra);
	    LOCK(destra);
	    ++mangia;
	    Mangia(&seed);
	    UNLOCK(destra);
	    UNLOCK(sinistra);
	}
#endif	
    }
    fprintf(stdout, "Filosofo %d:  ho mangiato %d volte e pensato %d volte\n", myid, mangia, pensa);
    fflush(stdout);
    return NULL;
}

int main(int argc, char *argv[]) { 
    int N = 5;
    if (argc > 1) {
	N = atoi(argv[1]);
	if (N > 100) {
	    fprintf(stderr, "N ridotto a 100\n");
	    N=100;
	}
    }
    pthread_t    *th;
    threadArgs_t *thARGS;
    forchetta_t  *forks;
    th     = malloc(N*sizeof(pthread_t));
    thARGS = malloc(N*sizeof(threadArgs_t));    
    // array di mutex una per ogni forchetta
    forks  = malloc(N*sizeof(forchetta_t));
    if (!th || !thARGS || !forks) {
	fprintf(stderr, "malloc fallita\n");
	exit(EXIT_FAILURE);
    }   
    for(int i=0;i<N; ++i) { 
      if (pthread_mutex_init(&forks[i], NULL) != 0) {
	fprintf(stderr, "pthread_mutex_init fallita\n");
	exit(EXIT_FAILURE);
      }
    }    
    for(int i=0;i<N; ++i) {	
	thARGS[i].thid  = (i+1);
	thARGS[i].N     = N;
	thARGS[i].forks = forks;
    }
    for(int i=0; i<N; ++i)
	if (pthread_create(&th[i], NULL, Filosofo, &thARGS[i]) != 0) {
	    fprintf(stderr, "pthread_create failed\n");
	    exit(EXIT_FAILURE);
	}

    for(int i=0;i<N; ++i)
	if (pthread_join(th[i], NULL) == -1) {
	    fprintf(stderr, "pthread_join failed\n");
	}
    
    free(th);
    free(thARGS);
    free(forks);
    return 0;   
}
