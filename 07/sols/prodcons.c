#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

#include <util.h>

static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t  cond  = PTHREAD_COND_INITIALIZER;
static int buffer;       // risorsa condivisa (buffer di una sola posizione)
static char bufempty=1;  // flag che indica se il buffer e' vuoto

// produttore
void *Producer(void *notused) {
  for(int i=0;i<1000;++i) {
    LOCK(&mutex);
    while(!bufempty)
      WAIT(&cond,&mutex);
    
    buffer = i;
    bufempty = 0;
    
    SIGNAL(&cond);
    UNLOCK(&mutex);
  }
  printf("Producer exits\n");
  
  // produco un valore speciale per provocare la terminazione
  // del consumatore
  LOCK(&mutex);
  while(!bufempty)
    WAIT(&cond,&mutex);
  
  buffer = -1;    // suppongo che non ci possano essere numeri negativi 
  bufempty = 0;
  
  SIGNAL(&cond);
  UNLOCK(&mutex);
  
  return NULL;
}

// consumatore 
void *Consumer(void *notused) {
  int val=0;
  while(val>=0) {   // suppongo che i valori siano tutti positivi o nulli
    LOCK(&mutex);
    while(bufempty) 
      WAIT(&cond,&mutex);
    
    val = buffer;
    bufempty = 1;
    SIGNAL(&cond);
    UNLOCK(&mutex);
    
    printf("Consumer:   %d\n", val);
  }
  printf("Consumer exits\n");
  
  return NULL;
}

int main() {
  pthread_t thconsumer, thproducer;
  
  if (pthread_create(&thconsumer, NULL, Consumer, NULL) != 0) {
    fprintf(stderr, "pthread_create failed (Consumer)\n");
    return (EXIT_FAILURE);
  }
  if (pthread_create(&thproducer, NULL, Producer, NULL) != 0) {
    fprintf(stderr, "pthread_create failed (Producer)\n");
    return (EXIT_FAILURE);
  }    
  if (pthread_join(thproducer, NULL) != 0) {
    fprintf(stderr, "pthread_join failed (Producer)\n");
    return (EXIT_FAILURE);
  }
  if (pthread_join(thconsumer, NULL) != 0) {
    fprintf(stderr, "pthread_join failed (Consumer)\n");
    return (EXIT_FAILURE);
  }
  return 0;
}
