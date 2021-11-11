#include <stdarg.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <pthread.h>
#include <errno.h>
#include <assert.h>

#include <getopt.h>


#define MAX_MSG_LEN 300
#define EXIT_MSG "@<@!@EXIT@!@>@"

#define LOCK(l)      if (pthread_mutex_lock(l)!=0)        { \
    fprintf(stderr, "ERRORE FATALE lock\n");		    \
    pthread_exit((void*)EXIT_FAILURE);			    \
  }   
#define UNLOCK(l)    if (pthread_mutex_unlock(l)!=0)      { \
  fprintf(stderr, "ERRORE FATALE unlock\n");		    \
  pthread_exit((void*)EXIT_FAILURE);				    \
  }
#define WAIT(c,l)    if (pthread_cond_wait(c,l)!=0)       { \
    fprintf(stderr, "ERRORE FATALE wait\n");		    \
    pthread_exit((void*)EXIT_FAILURE);				    \
}
#define SIGNAL(c)    if (pthread_cond_signal(c)!=0)       {	\
    fprintf(stderr, "ERRORE FATALE signal\n");			\
    pthread_exit((void*)EXIT_FAILURE);					\
  }
#define BCAST(c)     if (pthread_cond_broadcast(c)!=0)    {		\
    fprintf(stderr, "ERRORE FATALE broadcast\n");			\
    pthread_exit((void*)EXIT_FAILURE);						\
  }


typedef struct _node{
    void* data;
    struct _node* next;
}Node;

typedef struct _q{
    Node *head;
    Node *tail;
    pthread_mutex_t lock_q;
    pthread_cond_t cv_q_empty;
    pthread_cond_t cv_q_all_msgs;
    int tot_msgs;
    int msgs_sent;
    int exit_sents;
}Queue;

typedef struct _thArgs{
    int thId;
    Queue *q;
    int start;
    int stop;

}ThArgs;

int n = 0, p = 0, c = 0;

void   push(Queue* q_ptr, void* data);
void*  pop(Queue* q_ptr);
Queue* initQueue(int tot_msgs);
void   deallocQueue(Queue* q_ptr);
int    isEmpty(Queue q);

int tot_msgs;

void wait_all_msgs_have_been_sent(Queue* q_ptr);
void send_exit_msg(Queue *q_ptr);

void* cons_routine(void *arg){
    ThArgs *myArgs = (ThArgs*) arg;
    printf("c%d spawned\n", myArgs->thId);
    fflush(stdout);

    char *msg = NULL;
    while(1){
        msg = pop(myArgs->q);
        if(msg != NULL){
            if(strncmp(EXIT_MSG, msg, strlen(EXIT_MSG)-1)){
                printf("c%d: %s\n", myArgs->thId, msg);
                free(msg);
                msg = NULL;
            }
            else{
                printf("c%d got exit\n", myArgs->thId);
                free(msg);
                msg = NULL;
                break;
            }
        }
    }

    return NULL;
}
void* prod_routine(void *arg){

    ThArgs *myArgs = (ThArgs*) arg;
    //printf("p%d spawned, %d\t%d\n", myArgs->thId, myArgs->start, myArgs->stop);
    fflush(stdout);
    char *msg = NULL;
    int i;
    for(i = myArgs->start; i <= myArgs->stop; i++){
        
        msg = calloc(MAX_MSG_LEN, sizeof(char));
        if(msg == NULL){perror("calloc in prod_routine"); exit(EXIT_FAILURE);}

        snprintf(msg, MAX_MSG_LEN, "msg %d from p%d", i+1, myArgs->thId);
        push(myArgs->q, (void*)msg);
        msg = NULL;
    }

    //printf("p%d has sent all his msgs, %d\n", myArgs->thId, myArgs->q->msgs_sent);
    wait_all_msgs_have_been_sent(myArgs->q);
    //printf("p%d starts to send exits\n", myArgs->thId);


    while(myArgs->q->exit_sents < c)
        send_exit_msg(myArgs->q);

    printf("p%d returning\n", myArgs->thId);
    return NULL;
}


int main(int argc, char *argv[]){


    char opt;

    while((opt = getopt(argc, argv, ":p:n:c:")) != -1){
        
        switch(opt){
            case -1:
                if((n <= 0) || (p <= 0) || (c <= 0)){
                    puts("Usage: -p <num-prod> -c <num-cons> -n <num-msgs> with all positive non-zero values");
                    return -1;          
                }

            break;
            case ':':
                printf("missing option argument for -%c\n", opt);
                puts("Usage: -p <num-prod> -c <num-cons> -n <num-msgs>");
                return -1;
            break;
            case '?':
                printf("unrecognized or unsupported option: -%c\n", optopt);
                puts("Usage: -p <num-prod> -c <num-cons> -n <num-msgs>");
                return -1;
            break;
            case 'n':
                n = atoi(optarg);
            break;
            case 'p':
                p = atoi(optarg);
            break;
            case 'c':
                c = atoi(optarg);
            break;
            default:
                puts("Usage: -p <num-prod> -c <num-cons> -n <num-msgs>");
                return -1;
            break;
        }
    }
    if((n <= 0) || (p <= 0) || (c <= 0)){
        puts("Usage: -p <num-prod> -c <num-cons> -n <num-msgs> with all positive non-zero values");
        return -1;          
    }
    tot_msgs = n;
    Queue *q = initQueue(tot_msgs);
    
    pthread_t cons_arr[c];
    pthread_t prod_arr[p];

    ThArgs cons_args[c];
    ThArgs prod_args[p];

    int i;
    for(i = 0; i< c; i++){
        cons_args[i].start = 0;
        cons_args[i].stop = 0;
        cons_args[i].q = q;
        cons_args[i].thId = i+1;
        
        if((pthread_create(cons_arr+i, NULL, cons_routine,(void*)cons_args + i*sizeof(ThArgs))) != 0){
            perror("pthread_create cons");
            exit(EXIT_FAILURE);
        }
    }

    int d = tot_msgs/p;
    int r = tot_msgs%p;
    int tail;
    int start_count = 1;
    int stop_count = 0;
    for(i = 0; i < p; i++){
        tail = ((i < r)?(1):(0));
        prod_args[i].start = start_count;
        stop_count = start_count + d +  tail-1;

        prod_args[i].stop = stop_count;
        start_count = stop_count + 1;
        prod_args[i].thId = i+1;
        prod_args[i].q = q;
        if((pthread_create(prod_arr+i, NULL, prod_routine,(void*)prod_args + i*sizeof(ThArgs))) != 0){
            perror("pthread_create prod");
            exit(EXIT_FAILURE);
        }
    }

    
    //WAIT FOR PRODS
    for(i = 0; i < p; i++){
        if((errno = pthread_join(prod_arr[i], NULL)) != 0){
            perror("pthread_join prod");
            exit(EXIT_FAILURE);
        }
    }    

    //WAIT FOR CONS
    for(i = 0; i < c; i++){
        if((errno = pthread_join(cons_arr[i], NULL)) != 0){
            perror("pthread_join cons");
            exit(EXIT_FAILURE);
        }
    }    

    deallocQueue(q);

    return 0;
}


void send_exit_msg(Queue *q_ptr){


    char *msg = calloc(strlen(EXIT_MSG)+1, sizeof(char));
    if(msg == NULL){
        perror("calloc send_exit");
        exit(EXIT_FAILURE);
    }

    strncpy(msg, EXIT_MSG, strlen(EXIT_MSG)*sizeof(char));

    //push(q_ptr, msg);
    /*APPROCCIO FUNZIONANTE 1: questo rispetta la semantica della queue*/
    LOCK(&q_ptr->lock_q);
    if(q_ptr->exit_sents < c){
        if(q_ptr == NULL){puts("push: passed NULL queue"); exit(EXIT_FAILURE);}

            Node* newNode = calloc(1, sizeof(Node));
            if(newNode == NULL){
                perror("calloc newnode");
                exit(EXIT_FAILURE);
            }
            newNode->data = msg;
        if((q_ptr->head == NULL) && (q_ptr->tail == NULL)){
            q_ptr->head = newNode;
            q_ptr->tail = newNode;
        }
        else if(q_ptr->head == q_ptr->tail){
            newNode->next = q_ptr->tail;
            q_ptr->head = newNode;
        }
        else{
            newNode->next = q_ptr->head;
            q_ptr->head = newNode;
            
        }
    q_ptr->exit_sents +=1;
    SIGNAL(&q_ptr->cv_q_empty);
    }

    UNLOCK(&q_ptr->lock_q);

    return;
}

void wait_all_msgs_have_been_sent(Queue* q_ptr){
    LOCK(&q_ptr->lock_q);
        while(q_ptr->msgs_sent < q_ptr->tot_msgs){
            WAIT(&q_ptr->cv_q_all_msgs, &q_ptr->lock_q);
        }
    BCAST(&q_ptr->cv_q_all_msgs);
    UNLOCK(&q_ptr->lock_q);
}


void   push(Queue* q_ptr, void* data){

    if(q_ptr == NULL){puts("push: passed NULL queue"); exit(EXIT_FAILURE);}

    Node* newNode = calloc(1, sizeof(Node));
    if(newNode == NULL){
        perror("calloc newnode");
        exit(EXIT_FAILURE);
    }
    newNode->data = data;
    //next already at NULL

    LOCK(&q_ptr->lock_q);
        if((q_ptr->head == NULL) && (q_ptr->tail == NULL)){
            q_ptr->head = newNode;
            q_ptr->tail = newNode;
        }
        else if(q_ptr->head == q_ptr->tail){
            newNode->next = q_ptr->tail;
            q_ptr->head = newNode;
        }
        else{
            newNode->next = q_ptr->head;
            q_ptr->head = newNode;
            
        }
    
    if(strncmp(EXIT_MSG, (char*)newNode->data, strlen(EXIT_MSG)-1)){
        q_ptr->msgs_sent +=1;
    }
    else{
        q_ptr->exit_sents +=1;
    }

    newNode = NULL;
    SIGNAL(&q_ptr->cv_q_empty);
    UNLOCK(&q_ptr->lock_q);

    
    return;    
}
void*  pop(Queue* q_ptr){
    
    if(q_ptr == NULL){puts("pop: passed NULL queue"); exit(EXIT_FAILURE);}


    LOCK(&q_ptr->lock_q);

    while(isEmpty(*q_ptr) && ((q_ptr->msgs_sent < q_ptr->tot_msgs) || (q_ptr->exit_sents < c)) )
        WAIT(&q_ptr->cv_q_empty, &q_ptr->lock_q);

    if(isEmpty(*q_ptr) && (q_ptr->msgs_sent >= q_ptr->tot_msgs) && (q_ptr->exit_sents >= c))
        return NULL;

        void* ret_data = q_ptr->tail->data;
        Node *second_to_last = q_ptr->head;
        
        if(q_ptr->head == q_ptr->tail){
            q_ptr->head = NULL;
            free(q_ptr->tail);
            q_ptr->tail = NULL;
        }
        else{
            while(second_to_last->next != q_ptr->tail)
                second_to_last = second_to_last->next;
            
            free(second_to_last->next);  
            q_ptr->tail = second_to_last;
            q_ptr->tail->next = NULL; 
        }
    UNLOCK(&q_ptr->lock_q);
    return ret_data;
}
int    isEmpty(Queue q){
    return (q.head == NULL)?(1):0;
}
Queue* initQueue(int tot_msgs){
    Queue *newQ = calloc(1, sizeof(Queue));
    if(newQ == NULL){
        perror("initQueue calloc");
        exit(EXIT_FAILURE);
    }

    newQ->head = NULL;
    newQ->tail = NULL;
    newQ->tot_msgs = tot_msgs;
    newQ->msgs_sent = 0;
    newQ->exit_sents = 0;

    int err;
    if((err = pthread_mutex_init(&newQ->lock_q, NULL)) != 0){
        printf("initQueue pthread_mutex_init:");
        strerror(err);
        exit(EXIT_FAILURE);
    }

    if((err = pthread_cond_init(&newQ->cv_q_empty, NULL))!= 0){
        printf("initQueue pthread_cond_init:");
        strerror(err);
        exit(EXIT_FAILURE);        
    }

    if((err = pthread_cond_init(&newQ->cv_q_empty, NULL))!= 0){
        printf("initQueue pthread_cond_init:");
        strerror(err);
        exit(EXIT_FAILURE);        
    }

    return newQ;
}
void   deallocQueue(Queue* q_ptr){
    if((q_ptr->head != NULL) || (q_ptr->tail != NULL)){
        puts("dellocqueue: semantic of queue not respected");
        puts("printing elements remained in queue:");
        puts("head:");
        Node *node = q_ptr->head;
        while(node != NULL){
            printf("%s\n", (node->data != NULL)?((char*)node->data):("Null"));
            node = node->next;
        }
        puts("tail:");
        printf("%s\n", (q_ptr->tail->data != NULL)?((char*)q_ptr->tail->data):("Null"));

        pthread_mutex_destroy(&q_ptr->lock_q);
        pthread_cond_destroy(&q_ptr->cv_q_empty);

        exit(EXIT_FAILURE);
    }
    free(q_ptr);
    q_ptr = NULL;
}
