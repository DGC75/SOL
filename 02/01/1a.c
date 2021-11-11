#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

long isNumber(const char* s) {
   char* e = NULL;
   long val = strtol(s, &e, 0);
   if (e != NULL && *e == (char)0) return val; 
   return -1;
}

int main(int argc, char* argv[]){


    char opt;
    int val;
    /*AGGIUNGERE CHECK ROUTINE PER -h*/
    while((opt = getopt(argc, argv, "n:m:o:h")) != -1){
        switch (opt){
    
        case ('h'):
            puts("nome-programma -n <num. intero> -m <num. intero> -o <stringa> -h");
            exit(EXIT_SUCCESS);
            break;
        
        case ('n'):
        val = isNumber(optarg);
        if(val != -1)
            printf("n:%d\n", val);
        else
            printf("n:invalid argument\n");
        break;


        case ('m'):
        val = isNumber(optarg);
        if(val != -1)
            printf("m:%d\n", val);
        else
            printf("m:invalid argument\n");
        break;

        case ('o'):
            printf("o:%s\n", optarg);
        break;
        }
    }
    return 0;
}