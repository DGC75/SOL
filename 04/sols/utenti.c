// ---------------------------
// Esercizio 1 Assegnamento 4
//
// ---------------------------

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define PASSWD_FILE "/etc/passwd"

#if !defined(MAX_LEN)
#define MAX_LEN 1024
#endif

int main(int argc, char *argv[]) {
    if (argc != 2) {
	fprintf(stderr,
		"too few arguements\n"
		"use: %s out-filename\n",
		argv[0]);
	return -1;
    }    
    FILE *fd=NULL;
    FILE *fout=NULL;
    char *buffer= NULL;
    if ((fd=fopen(PASSWD_FILE, "r")) == NULL) {
	perror("opening password file");
	goto error; 
    }
    if ((fout=fopen(argv[1], "w")) == NULL) {
	perror("opening output file");
	goto error;
    }    
    if ((buffer=malloc(MAX_LEN*sizeof(char))) == NULL) {
	perror("malloc buffer");
	goto error;       
    }
    while(fgets(buffer, MAX_LEN, fd) != NULL) {
	char *newline;
	// controllo di aver letto tutta una riga
	if ((newline=strchr(buffer, '\n')) == NULL) {
	    fprintf(stderr, "buffer di linea troppo piccolo, aumentare MAX_LEN (%d) ricompilando con '-DMAX_LEN=<n>'\n", MAX_LEN);
	    goto error;
	}
	*newline = '\0';  // tolgo lo '\n', non strettamente necessario
	char *colons ;
	if ((colons=strchr(buffer, ':')) == NULL) {
	    fprintf(stderr, "wrong file format\n");
	    goto error;
	}
	*colons = '\0';	// sostituisco : con \0
	if (fprintf(fout, "%s\n", buffer) != strlen(buffer)+1) {
	    perror("fprintf");
	    goto error;
	}	       
    }
    fclose(fd);
    fclose(fout);
    free(buffer);

    return EXIT_SUCCESS;

 error:
    if (fd) fclose(fd);
    if (fout) fclose(fout);
    if (buffer) free(buffer);
    exit(EXIT_FAILURE);
}
