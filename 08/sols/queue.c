#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <errno.h>
#include <pthread.h>

#include <util.h>
#include <queue.h>

/**
 * @file queue.c
 * @brief File di implementazione dell'interfaccia per la coda
 */



/* ------------------- funzioni di utilita' -------------------- */

// qui assumiamo (per semplicita') che le mutex non siano mai di
// tipo 'robust mutex' (pthread_mutex_lock(3)) per cui possono
// di fatto ritornare solo EINVAL se la mutex non e' stata inizializzata.

static inline Node_t  *allocNode()                  { return malloc(sizeof(Node_t));  }
static inline Queue_t *allocQueue()                 { return malloc(sizeof(Queue_t)); }
static inline void freeNode(Node_t *node)           { free((void*)node); }
static inline void LockQueue(Queue_t *q)            { LOCK(&q->qlock);   }
static inline void UnlockQueue(Queue_t *q)          { UNLOCK(&q->qlock); }
static inline void UnlockQueueAndWait(Queue_t *q)   { WAIT(&q->qcond, &q->qlock); }
static inline void UnlockQueueAndSignal(Queue_t *q) { SIGNAL(&q->qcond); UNLOCK(&q->qlock); }

/* ------------------- interfaccia della coda ------------------ */

Queue_t *initQueue() {
    Queue_t *q = allocQueue();
    if (!q) return NULL;
    q->head = allocNode();
    if (!q->head) return NULL;
    q->head->data = NULL; 
    q->head->next = NULL;
    q->tail = q->head;    
    q->qlen = 0;
    if (pthread_mutex_init(&q->qlock, NULL) != 0) {
	perror("mutex init");
	return NULL;
    }
    if (pthread_cond_init(&q->qcond, NULL) != 0) {
	perror("mutex cond");
	if (&q->qlock) pthread_mutex_destroy(&q->qlock);
	return NULL;
    }    
    return q;
}

void deleteQueue(Queue_t *q) {
    while(q->head != q->tail) {
	Node_t *p = (Node_t*)q->head;
	q->head = q->head->next;
	freeNode(p);
    }
    if (q->head) freeNode((void*)q->head);
    if (&q->qlock)  pthread_mutex_destroy(&q->qlock);
    if (&q->qcond)  pthread_cond_destroy(&q->qcond);
    free(q);
}

int push(Queue_t *q, void *data) {
    if ((q == NULL) || (data == NULL)) { errno= EINVAL; return -1;}
    Node_t *n = allocNode();
    if (!n) return -1;
    n->data = data; 
    n->next = NULL;

    LockQueue(q);
    q->tail->next = n;
    q->tail       = n;
    q->qlen      += 1;
    UnlockQueueAndSignal(q);
    return 0;
}

void *pop(Queue_t *q) {        
    if (q == NULL) { errno= EINVAL; return NULL;}
    LockQueue(q);
    while(q->head == q->tail) {
	UnlockQueueAndWait(q);
    }
    // locked
    assert(q->head->next);
    Node_t *n  = (Node_t *)q->head;
    void *data = (q->head->next)->data;
    q->head    = q->head->next;
    q->qlen   -= 1;
    assert(q->qlen>=0);
    UnlockQueue(q);
    freeNode(n);
    return data;
} 
// NOTA: in questa funzione si puo' accedere a q->qlen NON in mutua esclusione
//       ovviamente il rischio e' quello di leggere un valore non aggiornato, ma nel
//       caso di piu' produttori e consumatori la lunghezza della coda per un thread
//       e' comunque un valore "non affidabile" se non all'interno di una transazione
//       in cui le varie operazioni sono tutte eseguite in mutua esclusione. 

unsigned long length(Queue_t *q) {
    LockQueue(q);
    unsigned long len = q->qlen;
    UnlockQueue(q);
    return len;
}

