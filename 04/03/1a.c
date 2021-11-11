#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>

//BUF_SIZE = #BYTE
//ESM BUF_SIZE = 512 SIGNIFICA CHE AVRAI UN BUFFER DA 512 bytes
#define BUF_SIZE 512

extern char *optarg;
extern int optind, opterr, optopt;

int getopt(int argc, char * const argv[], const char *optstring);

void  print_count(FILE *filename, int l, int w);

int main(int argc, char *argv[]){

    opterr = 0;
    int opt, l = 0, w = 0;
    char *filename = NULL;
    int arg_count  = argc - 1; //NUMERO DI ARGOMENTI RIMASTI DA VEDERE
    int cur_optind = optind; //OGNI VOLTA CHE OPTIND CAMBIA, FARE --arg_count;
    while((opt = getopt(argc, argv,"lw")) != -1){
        switch (opt){
        case 'l':
            l = 1;
            break;
        case 'w':
            w = 1;
            break; 
        case '?':
            printf("Option %c not recognized.\n", optopt);
            exit(EXIT_FAILURE);
            break;                   
        }
    }
    if(optind == argc){
        printf("Missing file(s). Usage:...\n");
        exit(EXIT_FAILURE);
    }

    if(w == 0 && l == 0){
        w=1;
        l=1;
    }

    int i;
    FILE *file = NULL;
    for(i = optind; i < argc; i++){
        filename = argv[i];
        file = fopen(filename, "r");
        print_count(file, l, w);
        fclose(file);
    }  

    return 0;
}


void  print_count(FILE *filename, int l, int w){
    //SE VOGLIO MANTENERE LE PRESTAZIONI DECENTI, DEVO USARE UN BUFFER PER PARSARE
    //OPTO PER UN BUFFER DA 512bytes, VISTE LE TABELLE NEL LIBRO(Rochkind)
    //E VISTO ANCHE CHE E' LA DIM DEL SETTORE CHE I DISCHI MAGNETICI LEGGONO :)
    char *buf = calloc(BUF_SIZE, sizeof(char));
    if(buf == NULL){
        fclose(filename);
        perror("malloc");
        exit(EXIT_FAILURE);
    }

    int words = 0, lines = 0;
    int i = 0;
    char c;
    int start_word = 0;
    while(fgets(buf, BUF_SIZE, filename) != NULL){
        i = 0;
        c = buf[i];
        while(c != '\0' && c != EOF && i < BUF_SIZE){
            if((!isspace(c)) && start_word == 0){
                start_word = 1;
                ++words;
            }
            else if(isspace(c)){
                if(c == '\n')
                    ++lines;
                if(start_word == 1)
                    start_word = 0;
            }

            c = buf[++i];

        }    
    }
    if( w==l==1)
        printf(" %d %d\n", lines, words);

    if(w==0)
        printf("%d\n", lines);  
    if(l==0)
        printf("%d\n", words);        

    free(buf);
}
