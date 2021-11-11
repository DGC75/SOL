#ifndef QUEUE_H_
#define QUEUE_H_

#include <pthread.h>

/** Elemento della coda.
 *
 */
typedef struct Node {
    void        * data;
    struct Node * next;
} Node_t;

/** Struttura dati coda.
 *
 */
typedef struct Queue {
    Node_t        *head;    // elemento di testa
    Node_t        *tail;    // elemento di coda 
    unsigned long  qlen;    // lunghezza 
    pthread_mutex_t qlock;
    pthread_cond_t  qcond;
} Queue_t;


/** Alloca ed inizializza una coda. Deve essere chiamata da un solo 
 *  thread (tipicamente il thread main).
 *
 *   \retval NULL se si sono verificati problemi nell'allocazione (errno settato)
 *   \retval puntatore alla coda allocata
 */
Queue_t *initQueue();

/** Cancella una coda allocata con initQueue. Deve essere chiamata da
 *  da un solo thread (tipicamente il thread main).
 *  
 *   \param q puntatore alla coda da cancellare
 */
void deleteQueue(Queue_t *q);

/** Inserisce un dato nella coda.
 *   \param data puntatore al dato da inserire
 *  
 *   \retval 0 se successo
 *   \retval -1 se errore (errno settato opportunamente)
 */
int    push(Queue_t *q, void *data);

/** Estrae un dato dalla coda.
 *
 *  \retval data puntatore al dato estratto.
 */
void  *pop(Queue_t *q);

/** Ritorna la lunghezza attuale della coda passata come parametro.
 */
unsigned long length(Queue_t *q);

#endif /* QUEUE_H_ */
