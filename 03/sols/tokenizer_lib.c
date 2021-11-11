#define _POSIX_C_SOURCE  200112L
#include <stdio.h>
#include <string.h>

#include <tokenizer.h>

// versione non rientrante
void tokenizer(char *stringa, FILE *out) {
    char* token = strtok(stringa, " ");
    while (token) {
	fprintf(out, "%s\n", token);
	token = strtok(NULL, " ");
    }
}

// versione rientrante
void tokenizer_r(char *stringa, FILE *out) {
    char *tmpstr;
    char *token = strtok_r(stringa, " ", &tmpstr);
    while (token) {
	fprintf(out, "%s\n", token);
	token = strtok_r(NULL, " ", &tmpstr);
    }
}
