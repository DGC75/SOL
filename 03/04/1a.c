#include <stdio.h>
#include <stdlib.h>

#ifndef INIT_VALUE
#define INIT_VALUE 100
#endif

int somma(int x){
    static int total = INIT_VALUE;
    total += x;
    return total;
    /*NON E' RIENTRANTE PERCHE' SE 
      INTERROTTA DOPO RIGA 10
      E RICHIAMATA DACCAPO PRODUCE UN 
      OUTPUT DIVERSO DALL'ESECUZIONE
      ININTERROTTA.*/
}

int somma_r(int x, int *s){

    *s+=x;
    return *s;
}

int main(int argc, char* argv[]){
    if(argc != 2){
        printf("Usage ./%s cardinalità_somma\n", argv[0]);
        exit(EXIT_SUCCESS);
    }

    int count = atoi(argv[1]);

    int i, input, total;

    for(i = 0; i < count; i++){
        scanf("%d", &input);
        scanf("%*[^\n]");
        scanf("%*c");

        printf("total= %d\n", somma(input));
    }   

    return 0;
}

int somma(int x);