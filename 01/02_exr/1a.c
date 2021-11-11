#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
 
const int REALLOC_INC=16;
 
void RIALLOCA(char** buf, size_t newsize) {
  *buf = realloc(*buf, newsize);
  if(*buf == NULL){
    puts("A QUANTO PARE NON C'E' SPAZIO SULLO HEAP");
    exit(EXIT_FAILURE);
  }
}

char* mystrcat(char *buf, size_t sz, char *first, ...) {
  /*<implementare il codice che se necessario chiamerà RIALLOCA>*/
  /*PSC
  * FINCHE' LA STRINGA NON E' DIVERSA DA NULL
  *   SE STA NEL BUFFER
  *     INSERISCILA E CARICA LA PROX STR
  *   SE NON STA NEL BUFFER
  *     RIALLOCA IL BUFFER DELLA DIM NECESSARIA
  *     INSERISCILA E CARICA LA PROX STR
  */
    va_list lst_str;
    va_start(lst_str, first);
    char *curr_s = first;
    int size_buf = sz;
    int size_cur_word;
    int size_words_inserted = 0;
    while(curr_s != NULL){
      size_cur_word = strlen(curr_s);
      while(size_cur_word + 1 > size_buf - size_words_inserted){
        size_buf+=REALLOC_INC;
        RIALLOCA(&buf, size_buf);
      }
      size_words_inserted += size_cur_word;   
      strncat(buf,curr_s, size_cur_word);

      curr_s = va_arg(lst_str, char*);
    }


    return buf;
}  
 
int main(int argc, char *argv[]) {
  if (argc != 7) { printf("troppi pochi argomenti\n"); return -1; }
  char *buffer=NULL;
  RIALLOCA(&buffer, REALLOC_INC);  // macro che effettua l'allocazione del 'buffer'
  buffer[0]='\0'; // mi assicuro che il buffer contenga una stringa
  buffer = mystrcat(buffer, REALLOC_INC, argv[1], argv[2], argv[3], argv[4], argv[5], argv[6], NULL);
  printf("%s\n", buffer);     
  free(buffer);
  return 0;
}