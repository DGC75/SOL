#define _POSIX_C_SOURCE  200112L
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

// esempio di tokenizzazione di stringhe con strtok (non rientrante!)

void tokenizer(char *stringa) {
  char* token = strtok(stringa, " ");
  while (token) {
    printf("%s\n", token);
    token = strtok(NULL, " ");
  }
}

int main(int argc, char *argv[]) {
    for(int i=1;i<argc;++i) 
	tokenizer(argv[i]);
    return 0;
}
