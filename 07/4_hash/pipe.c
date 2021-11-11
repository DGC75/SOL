#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <pthread.h>
#include <string.h>
#include <assert.h>

#include "icl_hash.h"

#define NBUCKETS 37


typedef struct strNode{
    char *str;
    struct strNode *next;
}StrNode;

typedef struct _strQueue{
    StrNode *head;
    StrNode *tail;
}StrQueue;

pthread_mutex_t mtx_ql = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mtx_qw = PTHREAD_MUTEX_INITIALIZER;

void mtx_lock(pthread_mutex_t *mtx_ptr){
    int err;
    if((err = pthread_mutex_lock(mtx_ptr)) != 0){
        puts("mutex_lock");
        errno = err;
        pthread_exit(&errno);
    }
    else{
        //puts("mutex locked");
    }
}

void mtx_unlock(pthread_mutex_t *mtx_ptr){
    int err;
    if((err = pthread_mutex_unlock(mtx_ptr)) != 0){
        perror("mutex_unlock");
        errno = err;
        pthread_exit(&errno);
    }
    else{
        //puts("mutex unlocked");
    }
}



void initStrQueue(StrQueue *q_ptr){
    q_ptr->head = NULL;
    q_ptr->tail = NULL;
}

int isEmpty(StrQueue q){

    if((q.head == NULL))
        return 1;

    return 0;
}

void enqueue(StrQueue *q_ptr, char *str){

    StrNode* newNode = calloc(1, sizeof(StrNode));
    if(newNode == NULL){
        perror("calloc");
        exit(EXIT_FAILURE);
    }

    newNode->str = str;
    //METTE IN CIMA
    if(q_ptr == NULL){
        puts("enqueue: passed non allocated queue");
        exit(EXIT_FAILURE);
    }

    if(q_ptr->head == NULL){
        q_ptr->tail = newNode;
        newNode->next = NULL;
    }
    else{
        newNode->next = q_ptr->head;
    }

    q_ptr->head = newNode;

}

char* dequeue(StrQueue *q_ptr){
    
    char *retVal;
    StrNode *tmpNode = q_ptr->head;
    if(q_ptr->head == NULL){
        retVal = NULL;
    }
    else if( q_ptr->head == q_ptr->tail){

        q_ptr->head = NULL; 
        q_ptr->tail = NULL;
        retVal = tmpNode->str;
        free(tmpNode);
    }
    else{
        
        while(tmpNode->next != q_ptr->tail)
            tmpNode = tmpNode->next;
        
        q_ptr->tail = tmpNode;
        tmpNode = tmpNode->next;
        q_ptr->tail->next = NULL;

        retVal = tmpNode->str;

        free(tmpNode);

    }

    return retVal;
}

void printQueue(StrQueue q){
    StrNode *tmp = q.head;

    while(tmp != NULL){
        puts(tmp->str);
        tmp = tmp->next;
    }
}
//VENGONO INIZIALIZZATE ALL'INIZIO DEL MAIN
StrQueue lineQueue, wordQueue;


int doc_started     = 0;
int doc_finished    = 0;

int lines_started   = 0;
int lines_finished  = 0;

static void free_key(void *arg){
    int *key_ptr = (int*)arg;
    free(key_ptr);
    arg = NULL;
    key_ptr = NULL;
    return;
}

static void free_data(void *arg){
    char *str = (char*)arg;
    free(str);
    str = NULL;
    arg = NULL;
    return;    
}


static void*parseDoc(void *arg){

    

    char *line = NULL;
    size_t len = 0;
    ssize_t nread;
    char *fileName = (char*)arg;

    FILE *file = fopen(fileName, "r");
    if(file == NULL){
        perror("fopen");
        exit(EXIT_FAILURE);
    }

    
    while ((nread = getline(&line, &len, file)) != -1) {
        doc_started = 1;
        mtx_lock(&mtx_ql);
        enqueue(&lineQueue, line);
        mtx_unlock(&mtx_ql);
        len = 0;
    }
    doc_finished = 1;

    fclose(file);
    return NULL;


    return NULL;
}

static void*parseLine(void *arg){
    
    char* line;
    char *state;


    while((!doc_started) && (isEmpty(lineQueue)))
        continue;
    



    while((!doc_finished) || (!isEmpty(lineQueue) )){
        
        lines_started = 1;
        mtx_lock(&mtx_ql);
        line = dequeue(&lineQueue);
        //printf("line:%s\n", line);
        fflush(stdout);
        mtx_unlock(&mtx_ql);
        
        char *word = strtok_r(line, " ", &state);
        while(word != NULL){

            mtx_lock(&mtx_qw);
            enqueue(&wordQueue, word);
            //printf("enqueued: %s, ", wordQueue.head->str);
            mtx_unlock(&mtx_qw);

            word = strtok_r(NULL, " ", &state);
        }


        
    }
    lines_finished = 1;

    return NULL;
}

static void*printWords(void *arg){
    //KEY: LA STRINGA
    //VALUE NULL(0), 1(E' PRESENTE)
    char *word;
    int inserted = 1;
    void * check;
    while((!lines_started) && (isEmpty(wordQueue)))
        continue;
    
    icl_hash_t *hashTable = icl_hash_create(NBUCKETS, &hash_pjw, &string_compare);

    while((!lines_finished) || (!isEmpty(wordQueue))){
        mtx_lock(&mtx_qw);
        word = dequeue(&wordQueue);
        if(word != NULL){
            if(!(icl_hash_find(hashTable, word))){
                if((check = icl_hash_insert(hashTable, word, &(inserted))) == NULL)
                    printf("%s not inserted\n God knows why\n", word);
            }
        }
        fflush(stdout);
        mtx_unlock(&mtx_qw);


    }


    //PRINT_HASH_TABLE

    icl_hash_dump(stdout, hashTable);

    fflush(stdout);
    //DESTROY HT
    //Non la distruggo perche' altrimenti avresti grossi problemi di memoria
    //icl_hash_destroy(hashTable, &free_key, &free_data);
    return NULL;
}


int main (int argc, char *argv[]){
    
    pthread_t parseDocTh, parseLineTh, printWordsTh;
    //CREA THREAD
    initStrQueue(&lineQueue);
    initStrQueue(&wordQueue);
    int errpD, errpL, errpW;

    if((errpD = pthread_create(&parseDocTh, NULL, parseDoc, (void *)(argv[1]))) != 0)
        perror("pthread create");
    
    
    if((errpL = pthread_create(&parseLineTh, NULL, parseLine, NULL)) != 0)
        perror("pthread create");

    
    if((errpW = pthread_create(&printWordsTh, NULL, printWords, NULL)) != 0)
        perror("pthread create");
    

    pthread_join(parseDocTh, NULL);
    pthread_join(parseLineTh, NULL);
    pthread_join(printWordsTh, NULL);

    return 0;
}