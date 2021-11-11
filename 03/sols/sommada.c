// ---------------------------
// Esercizio 4 Assegnamento 3
//
// ---------------------------

#include <stdio.h>
#include <stdlib.h>

#if !defined(INIT_VALUE)

#error("Compilare con il flag -DINIT_VALUE=n");

#endif

// versione non rientrante
int somma(int x) {
    static int s = INIT_VALUE;
    s +=x;
    return s;
}

// versione rientrante
int somma_r(int x, int *s) {
    *s += x;
    return *s;
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
	fprintf(stderr, "usa: %s N\n", argv[0]);
	return -1;
    }
    int N = strtol(argv[1],NULL, 10);
    if (N<=0 || N > 10) {
	fprintf(stderr, "N deve essere maggiore di 0 e minore o uguale a 10\n");
	return -1;
    }
    int s = INIT_VALUE;  // variabile locale per la versione rientrante (somma_r)

    for(int i=0;i<N;++i) {
	int val;
	fprintf(stdout, "Inserisci un numero (%d rimasti): ", N-i);
	if (fscanf(stdin, "%d", &val) != 1) {
	    perror("fscanf");
	    return EXIT_FAILURE;
	}
	
	fprintf(stdout, "somma   %d\n", somma(val));     
	fprintf(stdout, "somma_r %d\n", somma_r(val, &s));
    }

    return 0;       
}
