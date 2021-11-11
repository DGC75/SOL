// ---------------------------
// Esercizio 3 Assegnamento 4
//
// ---------------------------

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <getopt.h>

#include <utils.h>

// lunghezza massima per una singola lettura di linea con fgets
#define DEFAULT_MAX_LINE 2048

// la variabile optind e' definita nella libc ed e' usata dalla 
// funzione getopt
// NOTA: non e' strettamente necessario dichiarare la variabile
//      'extern' se si include getopt.h
extern int optind; 

// ritorna il numero di parole contenute nella stringa buffer
int wcount(const char buf[]) {
    static int flag=0;  // preservo lo stato tra due invocazioni
    char *p = (char*)&buf[0];
    int count = 0;

    // avevo iniziato a contare una parola nella chiamata precedente
    // pero' ero arrivato alla fine del buffer senza incontrare uno spazio
    // oppure '\n'
    if (flag && isspace(*p)) { flag=0; count = 1;}
    
    // togliamo tutti gli spazi ed i caratteri di controllo
    // iniziali in modo da iniziare con un carattere
    while(*p != '\0' && (iscntrl(*p) || isspace(*p))) ++p;

    while(*p != '\0') { 
	if (isspace(*p)) { // se ho uno spazio e se il carattere precedente 
	                   // non e' ne uno spazio 
	  if ( !isspace(*(p-1)) ) {  
	    ++count;
	    flag = 0;
	  }
	} else flag=1;  // ho iniziato una nuova parola
	++p;
    }
    return count; 
}

int main(int argc, char *argv[]) {
    if (argc == 1) {
	fprintf(stderr, "usa: %s [-l -w -m <num> ] <nome-file> [<nome-file> ...]\n", argv[0]);
	fprintf(stderr, " -l conta il numero di linee\n");
	fprintf(stderr, " -w conta il numero di parole\n");
	fprintf(stderr, " -m <num> setta la lunghezza massima di una linea a <num>, il default e': %d\n", DEFAULT_MAX_LINE);
	exit(EXIT_FAILURE);
    }
    int countline = 0, countword = 0;
    char opt;
    long max_line = DEFAULT_MAX_LINE;
    // parsing della command line
    while((opt = getopt(argc, argv, ":lwm:")) != -1) {
	switch(opt) {
	case 'l': {
	    countline = 1;
	} break;
	case 'w': {
	    countword = 1;
	} break;
	case 'm': {
	  if (isNumber(optarg, &max_line) != 0) {
	    fprintf(stderr, "WARNING: l'argomento di '-m' non e' un numero valido, uso il valore di default\n");
	  }
	  if (max_line <= 1) {
	    fprintf(stderr, "ERROR: il buffer deve essere lungo almeno 2\n");
	    exit(EXIT_FAILURE);
	  }
	} break;
	case '?': 
	  fprintf(stderr, "WARNING: l'opzione '-%c' non e' riconosciuta\n", optopt);
	  break;
	case ':':
	  fprintf(stderr, "l'opzione '-m' deve avere un argomento numerico, uso il valore di default\n");
	  break;
	default: ;
	}
    }
    
    // se non ci sono opzioni considero sia -l che -w come abilitate
    if (countword + countline == 0) { countline=1; countword=1; }
    
    char *line = malloc(max_line*sizeof(char)); // buffer
    if (!line) {
	perror("malloc");
	return EXIT_FAILURE;
    }
    // per ogni file 
    while(argv[optind] != NULL) {
	FILE *fp;             // file pointer
	long numline,numword; // contatori
	size_t len;
	char *filename = argv[optind];
	if ((fp = fopen(filename,"r")) == NULL) {
	    perror("fopen");
	    return EXIT_FAILURE;
	}

	numline=numword=0;
	while(fgets(line, max_line*sizeof(char), fp) != NULL) {	   
	    if ((len = strlen(line)) && (line[len-1] == '\n'))
		++numline;
	    if (countword) numword += wcount(line);
	}
	fclose(fp);
	if (countline && !countword) printf("%5ld %s\n", numline, argv[optind]);
	if (!countline && countword) printf("%5ld %s\n", numword, argv[optind]);
	if (countline && countword)  printf("%5ld %5ld %s\n", numline, numword, argv[optind]);
	++optind;
    }
    if (line) free(line);

    return EXIT_SUCCESS; // va bene anche return 0;
}
