#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define DIM 3
#define N   0
#define M   1
#define S   2

long isNumber(const char* s) {
   char* e = NULL;
   long val = strtol(s, &e, 0);
   if (e != NULL && *e == (char)0) return val; 
   return -1;
}

int main(int argc, char* argv[]){

    int opt_selected = 0;
    int i,j;
    int checked[DIM] = {0};
    long val;
    char* str = NULL;

    //OPZIONE H
    for(i = 1; i< argc; i++){
        j=0;
        while(argv[i][j] != '\0' && argv[i][j]== '-')
            j++;

        if(argv[i][j] == 'h'){
            printf("%s -n <numero> -s <stringa> -m <numero> -h\n", argv[0]);
            exit(EXIT_SUCCESS);
        }
    }

    for(i=1; i < argc; i++){
        j=0;

        while(argv[i][j] != '\0' && argv[i][j]== '-')
            j++;
        //OPZIONE N
        if(argv[i][j] == 'n' && checked[N] == 0){
            opt_selected = 1;

            if(argv[i][j+1] != '\0'){
                val = isNumber( &(argv[i][j+1]));
                if(val == -1){
                    printf("opzione -n non riconosciuta\n");
                    opt_selected = 0;
                    continue;

                }
                //SE VAL != -1
                printf("n:%li\n", val);
                checked[N] = 1;
                opt_selected = 0;
                continue;
            }
            else{
                if(++i == argc){
                    printf("errore nell'opzione -n\n");
                    exit(EXIT_FAILURE);
                }
                //ELSE
                val = isNumber(argv[i]);
                if(val == -1){
                    printf("opzione -n non riconosciuta\n");
                    opt_selected = 0;
                    --i;
                    continue;
                }
                
                printf("n:%li\n", val);
                checked[N] = 1;
                opt_selected = 0;
                continue;
            }

        }
        //OPZIONE M
        if(argv[i][j] == 'm' && checked[M] == 0){
            opt_selected = 1;
            if(argv[i][j+1] != '\0'){
                val = isNumber( &(argv[i][j+1]));
                if(val == -1){
                    printf("opzione -m non riconosciuta\n");
                    opt_selected = 0;
                    continue;
                }
                //SE VAL != -1
                printf("m:%li\n", val);
                checked[M] = 1;
                opt_selected = 0;
                continue;

            }
            else{
                if(++i == argc){
                    printf("errore nell'opzione -m\n");
                    exit(EXIT_FAILURE);
                }
                //ELSE
                val = isNumber(argv[i]);
                if(val == -1){
                    printf("opzione -m non riconosciuta\n");
                    opt_selected = 0;
                    --i;
                    continue;
                }
                printf("m:%li\n", val);
                checked[M] = 1;
                opt_selected = 0;
                continue;
            }
        }


        if (argv[i][j] == 's' && checked[S] == 0){
                opt_selected = 1;
                if(argv[i][j+1] != '\0'){
                    str = &(argv[i][j+1]);
                    checked[S] = 1;
                    printf("s: %s\n", str);
                    opt_selected = 0;
                    continue;
                }
                else{
                    if(++i == argc){
                        puts("opzione -s non riconosciuta\n");
                        exit(EXIT_FAILURE);
                    }
                    checked[S] = 1;
                    printf("s: %s\n", argv[i]);
                    opt_selected = 0;
                    continue;
                }
            opt_selected = 0;
        }
        

        if(argv[i][j] != '\0' && (opt_selected == 0))
        if( *(&(argv[i][j]) - 1) == '-'){
            printf("opzione %c non riconosciuta\n", argv[i][j]);
        }
            //IGNORA L'ARG SUCCESSIVO
            if(argv[i][j+1] != '\0')
                continue;
            else{
                ++i;
                continue;
            }
    }

    return 0;
}