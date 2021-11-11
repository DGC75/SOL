#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

#include <pthread.h>
#include <unistd.h>

#include <stdlib.h>
#include <time.h>


//NON SO USARE LE COND VAR E IL THREAD 1 E 2 NON SI STANNO CAPENDO
//DA SISTEMARE A MODO I METODI PRODUCER E CONSUMER

//SISTEMATO CON UN PAIO DI SIGNAL BEN PIAZZATE :)

#define DIM_QUEUE 5


#define THREAD_CREATE_ERR_CHECK(tid, err, th_fun, arg_ptr) \
        if((err = pthread_create(&tid, NULL, &th_fun, arg_ptr)) != 0) \
            {perror("err thread_create"); \
            exit(EXIT_FAILURE); \
            }

#define RAND_INT(lowest, highest)\
        (rand()%(highest - lowest)) + lowest





pthread_cond_t cond_var;
pthread_mutex_t mtx = PTHREAD_MUTEX_INITIALIZER;

void mutex_lock(pthread_mutex_t *mtx_ptr);
void mutex_unlock(pthread_mutex_t *mtx_ptr);

static void* producer(void* arg);
static void* consumer(void* arg);

int full(int *queue, int dim);
int empty(int *queue, int dim);

void insert(int val, int *q, int dim);
void consume(int *val, int *q, int dim);
 

int main (int argc, char *argv[]){
    


    // RANDOMIZE SEED
    srand(time(NULL));

    //CODA
    int queue[DIM_QUEUE] = {0};
    
    
    pthread_t tid_1, tid_2;
    int err1, err2;
    void *arg_ptr1, *arg_ptr2;

    //CREAZIONE E GESTIONE ERRORI
    THREAD_CREATE_ERR_CHECK(tid_1, err1, producer, queue)
    else{
        puts("th1 creato");
    }
    THREAD_CREATE_ERR_CHECK(tid_2, err2, consumer, queue)
    else{
        puts("th2 creato");
    }
    sleep(10);
    pthread_join(tid_1, NULL);
    pthread_join(tid_2, NULL); 

    return 0;
}

void mutex_lock(pthread_mutex_t *mtx_ptr){
    int err;
    if((err = pthread_mutex_lock(mtx_ptr)) != 0){
        puts("mutex_lock");
        errno = err;
        pthread_exit(&errno);
    }
    else 
        puts("mutex locked");
        
}

void mutex_unlock(pthread_mutex_t *mtx_ptr){
    int err;
    if((err = pthread_mutex_unlock(mtx_ptr)) != 0){
        perror("mutex_unlock");
        errno = err;
        pthread_exit(&errno);
    }
    else 
        puts("mutex unlocked");
        
}

static void* producer(void* arg){
    puts("producer called");
    int*queue = (int*) arg;
    while(1){
        int val = RAND_INT(1, 10);

        mutex_lock(&mtx);
        while(full(queue, DIM_QUEUE))
            pthread_cond_wait(&cond_var, &mtx);
        
        insert(val, queue, DIM_QUEUE);
        pthread_cond_signal(&cond_var);
        mutex_unlock(&mtx);
    }

    pthread_exit(0);
}

static void* consumer(void* arg){
    puts("consumer called");
    int*queue = (int*) arg;
    int val;
    while(1){
        mutex_lock(&mtx);
        while(empty(queue, DIM_QUEUE)){
            pthread_cond_wait(&cond_var, &mtx);
        }

        consume(&val, queue, DIM_QUEUE);
        printf("consumed %d\n", val);
        sleep(0.3);
        pthread_cond_signal(&cond_var);
        mutex_unlock(&mtx);
    }

    pthread_exit(0);
}

int full(int *queue, int dim){

    int i;
    for(i = 0; i < dim; i++){
        if(queue[i] == 0)
            return 0;
    }
    puts("full");
    return 1;
}

int empty(int *queue, int dim){
    int i;
    for(i = 0; i < dim; i++){
        if(queue[i] != 0)
            return 0;
    }
    puts("empty");
    return 1;
}

void insert(int val, int *q, int dim){
    
    int i = -1;
    while((i < dim) && (q[i] != 0)) i+=1;

    if(i == dim){
        printf("queue full while insert.Something went wrong.");
        exit(EXIT_FAILURE);
    }
    
    q[i] = val;
    printf("inserted %d at pos %d\n", val, i);
    sleep(1);
}

void consume(int *val, int *q, int dim){
    int i = dim - 1;
    while((i > 0) && (q[i] == 0)) i-=1;   
    
    if(i == 0 && (q[i] == 0)){
        printf("queue empty while consume.Something went wrong.");
        exit(EXIT_FAILURE);
    }

    *val = q[i];
    q[i] = 0;
}