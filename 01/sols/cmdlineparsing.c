// -----------------------------
// Esercizio 3 Assegnamento 1
//
// -----------------------------
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

// il seguente prototipo della funzione e' necessario solo se si compila
// con -std=c99/c11 perche' strdup non e' nello standard ISO
char *strdup(const char *s);

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

void print_usage(const char *programname) {
    printf("usage: %s -n <num> -m <num> -s <string> -h\n", programname);
}

int main(int argc, char *argv[]) {
    char *programname = argv[0];

    if (argc == 1) {
	printf("nessun argomento passato al programma\n");
	return 0;
    }

    char c;
    char foundn = 0, foundm=0, founds=0;
    long narg = -1, marg = -1;
    char *sarg = NULL;
    while(--argc > 0) 
	if ((*++argv)[0] == '-') {
	    while((c = *++argv[0]) == '-');  //alternativamente: while((c = argv[0][0]) == '-') ++argv[0];
	    switch(c) {
	    case 'n': {
		foundn = 1;
		if (argv[0][1] == '\0') {
		    ++argv, --argc;
		    if (argv[0] == NULL || (isNumber(argv[0], &narg))!=0) {
			foundn = 0;
			printf("argomento n non valido\n");
		    }
		} else 
		  if (isNumber(&argv[0][1], &narg)!=0) {
			foundn = 0;
			printf("argomento n non valido\n");
		    } 
	    } break;
	    case 'm': {
		foundm = 1;
		if (argv[0][1] == '\0') {
		    ++argv, --argc;
		    if (argv[0] == NULL || (isNumber(argv[0], &marg))!=0) {
			foundm = 0;
			printf("argomento m non valido\n");
		    }
		} else
		  if (isNumber(&argv[0][1], &marg)!=0) {
			foundm = 0;
			printf("argomento m non valido\n");
		    } 
	    } break;
	    case 's': {
		founds = 1;
		if (argv[0][1] == '\0') {
		    ++argv, --argc;
		    if (argv[0]) sarg = strdup(argv[0]);
		} else sarg = strdup(&argv[0][1]);
		if (sarg == NULL) {
		    founds = 0;
		    printf("argomento s non valido\n");
		}
	    } break;
	    case 'h': {
		print_usage(programname);
		return 0;
	    } break;
	    default : 
		printf("argomento %c non riconosciuto\n", c);	    
	    }
	}

    if (foundn) 
	printf("-n: %ld\n", narg);
    if (foundm) 
	printf("-m: %ld\n", marg);
    if (founds) 
	printf("-s: %s\n", sarg);

    free(sarg);
    
    return 0;

}
