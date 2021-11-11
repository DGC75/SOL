#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

//ESERCIZIO COMPLETATO IN TEORIA
//MANCANO DEI DETTAGLI PER RENDERLO FUNZIONANTE,
//MA SI TRATTA DI COPIA E INCOLLA.

//(it's the new 'dimostrazione lasciata al lettore')

long isNumber(const char* s) {
   char* e = NULL;
   long val = strtol(s, &e, 0);
   if (e != NULL && *e == (char)0) return val; 
   return -1;
}

int arg_n(const char* arg_str);
int arg_m(const char* arg_str);
int arg_o(const char* arg_str);
int arg_h(const char* arg_str);

 
int main(int argc, char* argv[]) {
 
  // controllo di argc ed inizializzazione del vettore V con i puntatori a funzione
    if(argc != 5){
        puts("quantità args scorretta");
        exit(EXIT_FAILURE);
    }

  //DICHIARAZIONE E INIT VETTORE FUNZIONI ARGOMENTI
  int (*V[4])(const char*);  

  V[0] = &(arg_n);
    
  int opt;
  while ((opt = getopt(argc,argv, "n:m:o:h")) != -1) {
    switch(opt) {
    case '?': { 
       puts("non ho capito");
    } break;
    default:
     // invocazione della funzione di gestione passando come parametro l'argomento restituito da getopt
     if (V[opt%4]( (optarg==NULL ? argv[0] : optarg) ) == -1) {
       puts("Sig. Utonto, gli argomenti non sono stati inseriti nel formato corretto");
       exit(EXIT_FAILURE);
     }
    }
  }
  return 0; 
} 


int arg_n(const char* arg_str){
  
  long val =  isNumber(arg_str);

  if(val != -1){
    printf("%li\n", val);
    return 0;
  }

  return -1;
}

int arg_m(const char* arg_str);
int arg_o(const char* arg_str){

  //CONTROLLA CON STRNLEN, LA LUNGHEZZA
    //SE == 0, RITORNA -1

  printf("'%s'\n");
  return 0;
}
int arg_h(const char* arg_str);
  //-h FUNZIONA A PRESCINDERE, INDIPENDENTEMENTE DALL'ARGOMENTO

