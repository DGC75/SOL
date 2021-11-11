#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>
#include <stdlib.h>
#include <errno.h>

#define N_PHILS 5
#define TIMES 100

#define NANOSEC_MIN 0
#define NANOSEC_MAX 1000

#define SEC_MIN 0
#define SEC_MAX 1

int forks_arr[N_PHILS] = {0};
//0 LIBERE, 1 PRESE


pthread_cond_t cond_var_forks;
pthread_mutex_t mtx_forks = PTHREAD_MUTEX_INITIALIZER;

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


static void * philosopher(void *arg){


    int id = *(int*)arg;
    struct timespec think_time, eat_time;
    unsigned int seed = (unsigned int) rand();
    think_time.tv_nsec = 0;
    think_time.tv_sec = (time_t) rand_r(&seed)%(SEC_MAX-SEC_MIN) + SEC_MIN; 
    eat_time.tv_nsec = 0;
    eat_time.tv_sec = (time_t) rand_r(&seed)%(SEC_MAX-SEC_MIN) + SEC_MIN; 
    printf("I'm philosopher %d\n", id);

    int i;
    for(i = 0; i < TIMES; i++){
    
        //THINK
        nanosleep(&think_time, NULL);

        //ACQUIRE
        mutex_lock(&mtx_forks);
        //ALL' N-ESIMO PHIL SONO ASSOCIATE N E N+1 FORKS
        //0->0,1
        //...
        //5->0,1
        while((forks_arr[(i)%N_PHILS] != 0) || (forks_arr[(i+1)%N_PHILS] != 0)){
            pthread_cond_wait(&cond_var_forks, &mtx_forks);
        }

        forks_arr[(i)%N_PHILS] = 1;
        forks_arr[(i+1)%N_PHILS] = 1;

        //EAT
        printf("%d eats\n", id);
        nanosleep(&eat_time, NULL);



        //RELEASE
        forks_arr[(i)%N_PHILS] = 0;
        forks_arr[(i+1)%N_PHILS] = 0;
        printf("%d release\n", id);
        mutex_unlock(&mtx_forks);   

    think_time.tv_nsec = 0;
    think_time.tv_sec = (time_t) rand_r(&seed)%(SEC_MAX-SEC_MIN) + SEC_MIN; 
    eat_time.tv_nsec = 0;
    eat_time.tv_sec = (time_t) rand_r(&seed)%(SEC_MAX-SEC_MIN) + SEC_MIN; 
    }


    return NULL;
}

void create_threads(pthread_t *pid_arr, int dim){
    int i;
    int err;
    for(i = 0; i < dim; i++){
        if((err = pthread_create(pid_arr + i, NULL, philosopher, (void *)&i)) != 0)
            perror("pthread create");
    }


}

void join_threads(pthread_t* pid_arr, int dim){
    int i;
    int err;
    for(i = 0; i < dim; i++){
        if((err = pthread_join(pid_arr[i], NULL)) != 0)
            perror("pthread join");
    }

    puts("Threads terminati con successo");
}


int main(){
    srand(time(NULL));
    pthread_t pid_arr[N_PHILS];
    create_threads(pid_arr, N_PHILS);
    join_threads(pid_arr, N_PHILS);

    return 0;
}