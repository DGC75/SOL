#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <assert.h>

void strtoupper(const char* in, int len, char* out);

int main(int argc, char *argv[]){
    if(argc==1){
        puts("Too few arguments.");
        puts("Usage: ./strtoupper string_1 ... string_n");
        exit(EXIT_FAILURE);
    }
    int i;
    char *buffer = NULL;
    for(i = 1; i<argc; i++){
        int len = strlen(argv[i]);
        buffer = calloc(len+1, sizeof(char));
        assert(buffer);

        strtoupper(argv[i], len, buffer);
        printf("%s\n", buffer);
        
        free(buffer);
        buffer = NULL;
    }
    return 0;
}

void strtoupper(const char* in, int len, char* out){
    if((strlen(in)) != len){
        puts("Invalid string length.");
        exit(EXIT_FAILURE);
    }

    int i;
    for(i = 0; i < len; i++){
        char tmp = in[i];
        if(isalpha(tmp))
            out[i] = toupper(tmp);        
        else
            out[i] = tmp;
    }
        
    return;
}