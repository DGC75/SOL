#define _POSIX_C_SOURCE 200112L
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

// esempio di tokenizzazione di stringhe con strtok_r

void tokenizer_r(char *stringa) {
    char *tmpstr;
    char *token = strtok_r(stringa, " ", &tmpstr);
    while (token) {
      printf("%s\n", token);
      token = strtok_r(NULL, " ", &tmpstr);
    }
}

int main(int argc, char *argv[]) {
    for(int i=1;i<argc;++i) 
	tokenizer_r(argv[i]);
    return 0;
}
