// ---------------------------
// Esercizio 4 Assegnamento 2
//
// ---------------------------

#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <stdio.h>


// Il prototipo e' necessario perche' rand_r non rientra nello standard
// ISO c99 c11 
// in alternativa avrei potuto compilare definedo: -D_POSIX_C_SOURCE=200112L
int rand_r(unsigned int *seedp);

// alcuni valori di default
#define N 10000
#define K1 0
#define K2 10

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

int main(int argc, char* argv[]) {
  long k1 = K1;
  long k2 = K2;
  long n  = N;
  if (argc>1) {#include <unistd.h>
    if (argc!=4) {
      printf("usa: %s [K1 K2 N]\n", argv[0]);
      return -1;
    }
    if (isNumber(argv[1], &k1) != 0) {
      printf("l'argomento %s non e' valido\n", argv[1]);
      return -1;
    }
    if (isNumber(argv[2], &k2) != 0) {
      printf("l'argomento %s non e' valido\n", argv[2]);
      return -1;
    }
    if (isNumber(argv[3], &n) != 0) {
      printf("l'argomento %s non e' valido\n", argv[3]);
      return -1;
    }
    if (k2<=k1) {
      printf("%ld deve essere maggiore di %ld\n", k2,k1);
      return -1;
    }	
  }
  
  unsigned int seed = time(NULL); // seme iniziale
  const long k= k2-k1;
  // array di lunghezza variabile ....
  long C[k]; 
  
  // inizializzo il vettore
  for(long i=0;i<k;++i) C[i]=0;
  
  for(long i=0;i<n;++i) {
    long r = k1 + rand_r(&seed)%k;
    C[r % k]++;	
  }
  
  printf("Occorrenze di:\n");
  for(long i=0;i<k;++i)
    printf("%ld  : %5.2f%% \n", i+k1, (C[i]/(float)n)*100);
  printf("\n");
  
  return 0;	 
}
