// ---------------------------
// Esercizio 2 Assegnamento 2
//
// ---------------------------
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <errno.h>

// numero delle opzioni che voglio gestire
#define NOPTIONS 4
// limite sulla lunghezza della stringa passata come argomento
#define MY_ARGV_MAX  512

// prototipo necessario solo se compiliamo con c99 o c11
// altrimenti il compilatore da un warning
char *strndup(const char *s, size_t n);

// ritorna
//   0: ok
//   1: non e' un numbero
//   2: overflow/underflow
//
int isNumber(const char* s, long* n) {
  if (s==NULL) return 1;
  if (strlen(s)==0) return 1;
  char* e = NULL;
  errno=0;
  long val = strtol(s, &e, 10);
  if (errno == ERANGE) return 2;    // overflow
  if (e != NULL && *e == (char)0) {
    *n = val;
    return 0;   // successo 
  }
  return 1;   // non e' un numero
}

int arg_h(const char* programname, void** a) {
    printf("usage: %s -n <num> -m <num> -o <string> -h\n", programname);
    return -1;
}
int arg_n(const char* n, void** a) {
  long tmp;
  if (isNumber(n, &tmp) != 0) {
    printf("l'argomento %s non e' valido\n", n);
    return -1;
  }
  *a = malloc(sizeof(long));
  if (!*a) {
    perror("malloc");
    return -1;
  }
  *(long*)*a = tmp;
  return 0;
}
int arg_m(const char* m, void** a) {
  return arg_n(m,a);
}
int arg_o(const char* o, void** a) {
  if ((*a = strndup(o, MY_ARGV_MAX)) == NULL) {
    perror("strndup");
    return -1;
  }
  return 0;
}

// tipo puntatore a funzione: F_t e' di int (*)(const char*, void**)
typedef int (*F_t)(const char*, void** arg);

typedef struct V_entry {
  F_t   F;   // puntatore a funzione
  void* a;   // valore dell'argomento gestito dalla funzione
} V_entry;

int main(int argc, char *argv[]) {
  if (argc==1) {
    printf("almeno una opzione deve essere passata\n");
    arg_h(argv[0], NULL);
    return -1;
  }

  // V conterra' un puntatore a funzione per ogni opzione da gestire
  V_entry V[NOPTIONS] = {
			 {NULL,NULL},
			 {NULL,NULL},
			 {NULL,NULL},
			 {NULL,NULL}
  };
  
  V[0].F = arg_h;   // 'h' corrisponde all'intero 104 e 104 % 4 --> 0
  V[1].F = arg_n;   // 'm' --> 109 % 4 --> 1
  V[2].F = arg_m;   // 'n' --> 110 % 4 --> 2
  V[3].F = arg_o;   // 'o' --> 111 % 4 --> 3

  int opt;
  int r = EXIT_SUCCESS;
  while ((opt = getopt(argc,argv, ":n:m:o:h")) != -1) {
    switch(opt) {
    case ':': {
      printf("l'opzione '-%c' richiede un argomento\n", optopt);
      r = EXIT_FAILURE;
    } break;
    case '?': {  // restituito se getopt trova una opzione non riconosciuta
      printf("l'opzione '-%c' non e' gestita\n", optopt);
      r = EXIT_FAILURE;
    } break;
    default:
      if (V[opt % NOPTIONS].F((optarg==NULL?argv[0]:optarg), &V[opt % NOPTIONS].a) == -1)
	r = EXIT_FAILURE;
    }
  }

  // stampe dei valori
  if (V[1].a)  printf("m = %ld\n",     *(long*)(V[1].a));
  if (V[2].a)  printf("n = %ld\n",     *(long*)(V[2].a));
  if (V[3].a)  printf("o = \"%s\"\n",   (char*)(V[3].a));

  free(V[1].a);
  free(V[2].a);
  free(V[3].a);
  return r;
}
