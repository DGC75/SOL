/* ---------------------------
// Esercizio 5 Assegnamento 3
//
// ---------------------------
*/
/* prototipi delle funzioni utilizzate nel programma
 per tali funzioni non e' necessario utilizzare il modificatore extern
 che per le funzioni e' implicito.
*/
int printf(const char *format, ...);
int getopt(int argc, char * const argv[],const char *optstring);

/* dichiarazione delle variabili utilizzate nel programma e definite
 nella libreria standard (sono usate dalla funzione getopt)
*/
extern char *optarg;  /* perche' si puo' omettere il modificatore extern ? */
extern int   optopt;


int main(int argc, char *argv[]) {
  int opt;
  while ((opt = getopt(argc,argv, ":n:m:h")) != -1) {
    switch(opt) {
    case 'n': case 'm':printf("l'argomento di '-%c' e' '%s'\n",opt,optarg); break;
    case 'h':printf("'-h' non ha argomenti\n"); break;
    case ':':printf("'-%c' richiede un argomento\n", optopt); break;
    case '?':printf("'-%c' non e' riconosciuta\n", optopt);
    default:;
    }
  }  
  return 0;
}
