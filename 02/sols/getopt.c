// ---------------------------
// Esercizio 1 Assegnamento 2
//
// ---------------------------
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <errno.h>

// numero delle opzioni che voglio gestire
#define NOPTIONS       4
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

int arg_h(const char* programname) {
    printf("usage: %s -n <num> -m <num> -o <string> -h\n", programname);
    return -1;
}
int arg_n(const char* n) {
  long tmp;
  if (isNumber(n, &tmp) != 0) {
    printf("l'argomento di '-n' non e' valido\n");
    return -1;
  }
  printf("-n : %ld\n", tmp);
  return 0;
}
int arg_m(const char* m) {
  long tmp;
  if (isNumber(m, &tmp) != 0) {
    printf("l'argomento di '-m' non e' valido\n");
    return -1;
  }
  printf("-m : %ld\n", tmp);
  return 0;
}
int arg_o(const char* o) {
  if (strlen(o) > MY_ARGV_MAX) {
    printf("la string di '-o' e' troppo lunga\n");
    return -1;
  }
  printf("-o : \"%s\"\n", o);
  return 0;
}

int main(int argc, char *argv[]) {
  if (argc==1) {
    printf("almeno una opzione deve essere passata\n");
    arg_h(argv[0]);
    return -1;
  }
  
  int opt;
  // se il primo carattere della optstring e' ':' allora getopt ritorna
  // ':' qualora non ci sia l'argomento per le opzioni che lo richiedono
  // se incontra una opzione (cioe' un argomento che inizia con '-') e tale
  // opzione non e' in optstring, allora getopt ritorna '?'
  while ((opt = getopt(argc,argv, ":n:m:o:h")) != -1) {
    switch(opt) {
    case 'n': arg_n(optarg);  break;
    case 'm': arg_m(optarg);  break;
    case 'o': arg_o(optarg);  break;
    case 'h': arg_h(argv[0]); break;
    case ':': {
      printf("l'opzione '-%c' richiede un argomento\n", optopt);
    } break;
    case '?': {  // restituito se getopt trova una opzione non riconosciuta
      printf("l'opzione '-%c' non e' gestita\n", optopt);
    } break;
    default:;
    }
  }
  return 0;
}
