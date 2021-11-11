// ---------------------------
// Esercizio 3 Assegnamento 2
//
// ---------------------------

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define MY_ARGV_MAX  512

// Il prototipo e' necessario perche' le funzioni che seguono non rientrano
// nello standard ISO c99 c11 
// in alternativa avrei potuto compilare definedo: -D_POSIX_C_SOURCE=200112L
char *strndup(const char *, size_t);
char *strtok_r(char *str, const char *delim, char **saveptr);

int main(int argc, char *argv[]) {
    if (argc != 3) {
	printf("usa: %s stringa1 stringa2\n", argv[0]);
	return -1;
    }
    // faccio un controllo sulla lunghezza massima delle stringhe in input
    if ((strlen(argv[1]) > MY_ARGV_MAX) ||
	(strlen(argv[2]) > MY_ARGV_MAX)) {
      printf("ERRORE: stringhe troppo lunghe\n");
      return -1;
    }
   
#if 0  // versione originale non corretta
    
    char* token1 = strtok(argv[1], " ");
    
    while (token1) {
	printf("%s\n", token1);
	char* token2 = strtok(argv[2], " ");
	while(token2) {
	    printf("%s\n", token2);
	    token2 = strtok(NULL, " ");
	}
	token1 = strtok(NULL, " ");
    }
    
#else  // versione rientrante 

    // non e' strettamente necessario in questo esercizio, ma copio comunque
    // argv[1] perche' strtok_r modifica la stringa in ingresso
    //
    char *str1    = strndup(argv[1], MY_ARGV_MAX);
                              
    char* tmpstr1 = NULL; // stato della prima strtok_r
    char* tmpstr2 = NULL; // stato della seconda strtok_r
    char* token1  = strtok_r(str1, " ", &tmpstr1);
    while (token1) {
	printf("%s\n", token1);

	char *str2    = strndup(argv[2], MY_ARGV_MAX); // copio la stringa perche' strtok_r la modifica
	char* token2  = strtok_r(str2, " ", &tmpstr2); // <--- qui avrei potuto usare anche strtok

	while(token2) {
	    printf("%s\n", token2);
	    token2 = strtok_r(NULL, " ", &tmpstr2);    
	}
	free(str2);  // libero memoria
	token1 = strtok_r(NULL, " ", &tmpstr1); // continuo con la prima stringa
    }
    free(str1);
#endif
    return 0;
}
