#define _POSIX_C_SOURCE 200112L
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <errno.h>
#include <string.h>

#include <boundedqueue.h>
#include <icl_hash.h>


#define MAX_LINE_SIZE 1024
/* valore speciale usato per indicare End-Of-Stream (EOS) */
#define EOS           (void*)0x1
/* n. di buckets nella tabella hash */
#define NBUCKETS      256

// prototipo della funzione di libreria
char *strdup(const char *s);

// 
// Code che implementano i due canali
// Stage1 -- Q1 --> Stage2 -- Q2 --> Stage3
//
// NOTA: sarebbe stato piu' "pulito" passare le code
//       ai thread che implementano gli stadi, invece
//       che dichiararli globali!
BQueue_t *Q1, *Q2;


// primo stadio della pipeline
static void* Stage1(void *path) {
    FILE *in = NULL;
    char *line = NULL;
    
    if ((in = fopen((const char*)path, "r")) == NULL) {
	perror("fopen");
	push(Q1,EOS);
	return NULL;
    }
    line = malloc(MAX_LINE_SIZE*sizeof(char));
    // finche' ho linee da leggere vado avanti
    while (line && fgets(line, MAX_LINE_SIZE, in) != NULL ) {
	line[strlen(line)-1]='\0';
	if (strlen(line)==0) continue;
	push(Q1, line);
	line = malloc(MAX_LINE_SIZE * sizeof(char));
    }
    if (line) free(line);
    // inserisco l'EOS nella coda di uscita
    push(Q1, EOS);
    fclose(in);
    return NULL;
}

// secondo stadio della pipeline
static void* Stage2() {
    while(1) {
	char *line = NULL;
	line = pop(Q1);
	if (line == EOS) {
	    // ricevuto l'EOS lo devo forwardare
	    break;
	}

	char *tmpstr;
	char *token = strtok_r(line, " ", &tmpstr);
	while(token) {
	    char *tout = strdup(token);
	    push(Q2, tout);
	    token = strtok_r(NULL, " ", &tmpstr);
	}
	free(line);
    }
    // inserisco l'EOS nella coda di uscita
    push(Q2, EOS);
    return NULL;
}

// terzo stadio della pipeline
static void* Stage3() {
    // utilizzo una hash table per trovare le parole uniche
    icl_hash_t *hash_table = NULL;
    hash_table = icl_hash_create(NBUCKETS, NULL, NULL);
    if (!hash_table) {
	fprintf(stderr, "Errore nella creazione della tabella hash\n");
	// vado comunque avanti ....
    }
    char at_least_one_insert=0;
    while(1) {
	char *token = NULL;
	token = pop(Q2);
	if (token == EOS) break;

	// NOTA: i token andrebbero normalizzati: togliendo spazi, tab, resi tutti minuscoli/maiuscoli, parentesi, etc  ...
	
	if (hash_table && !icl_hash_find(hash_table, token)) {
	    icl_hash_insert(hash_table, token, (void*)token);
	    at_least_one_insert=1;
	} else 	
	    free(token);
    }

    int k;
    icl_entry_t *entry;
    char *key, *value;

    if (hash_table && at_least_one_insert) {
	printf("Le parole individuate sono:\n");
	printf("-----------------------------------------------\n");
	icl_hash_foreach(hash_table, k, entry, key, value)
	    printf("%s ", value);
	printf("\n-----------------------------------------------\n");
	icl_hash_destroy(hash_table, NULL, free);
    }   
    return NULL;
}


int main(int argc, char *argv[]) {
    
    if ( argc < 2 || argc > 3) {
	fprintf(stderr, "Usage: %s filename\n", argv[0]);
	return -1;
    }
    
    // inizializzo la tabella hash

    Q1 = initBQueue(100);
    Q2 = initBQueue(1000);
    
    pthread_t th1, th2, th3;
    
    if (pthread_create(&th1, NULL, Stage1, argv[1]) != 0) {
	fprintf(stderr, "pthread_create failed (Stage1)\n");
	exit(EXIT_FAILURE);
    }
    if (pthread_create(&th2, NULL, Stage2, NULL) != 0) {
	fprintf(stderr, "pthread_create failed (Stage2)\n");
	exit(EXIT_FAILURE);
    }    
    if (pthread_create(&th3, NULL, Stage3, NULL) != 0) {
	fprintf(stderr, "pthread_create failed (Stage3)\n");
	exit(EXIT_FAILURE);
    }

    pthread_join(th1,NULL);
    pthread_join(th2,NULL);
    pthread_join(th3,NULL);

    deleteBQueue(Q1,NULL);
    deleteBQueue(Q2,NULL);
    return 0;
}










