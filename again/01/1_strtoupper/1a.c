#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

#include <ctype.h>
#include <stdarg.h>

void strtoupper(const char* in, size_t len, char* out){
    int i;
    for(i = 0; i < len; i++){
        out[i] = toupper(in[i]);
    }
}

int main (int argc, char *argv[]){
    va_list va;

    if(argc > 1)
        va_start(va, argv[0]);
    else{
        puts("Nessun argomento");
        exit(EXIT_SUCCESS);
    }

    int i;
    for(i = 1; i < argc; i++){
        printf("%s\n", va_arg(va, char*));
    }




    va_end(va);
    return 0;
}