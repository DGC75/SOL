// -----------------------------
// Esercizio 2 Assegnamento 1
//
// versione dove RIALLOCA è un macro
// -----------------------------
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>


#define REALLOC_INC 16

#define RIALLOCA(buf, newsize)			\
  buf = realloc(buf, newsize);			\
  if (!buf) {					\
    printf("realloc: MEMORIA ESAURITA, esco\n");\
    exit(-1); /* uscita brutale!! */		\
  }

char *mystrcat(char *buf, size_t sz, char *first, ...) {
    va_list ap;
    va_start(ap, first);
    
    if (sz < (strlen(first)+1)) {
	RIALLOCA(buf, sz + strlen(first)+1 + REALLOC_INC);
	sz += strlen(first)+1 + REALLOC_INC;
    }
    strncat(buf, first, strlen(first));
    char *s = NULL;
    while ((s = va_arg(ap, char*))!=NULL) {
	if (sz < strlen(buf) + (strlen(s)+1)) {
	    RIALLOCA(buf, strlen(buf)+strlen(s)+1 + REALLOC_INC);
	    sz=strlen(buf) + strlen(s)+1+REALLOC_INC;
	}
	strncat(buf, s, strlen(s));
    }
    va_end(ap);
    return buf;
}

int main(int argc, char *argv[]) {
    if (argc != 7) {
	printf("troppi pochi argomenti, esco\n");
	return -1;
    }
    char *buffer=NULL;
    RIALLOCA(buffer, REALLOC_INC);
    buffer[0]='\0';
    buffer = mystrcat(buffer, REALLOC_INC, argv[1], argv[2], argv[3], argv[4], argv[5], argv[6],NULL);
    printf("%s\n", buffer);
    free(buffer);
    return 0;

}
