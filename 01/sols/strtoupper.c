// -----------------------------
// Esercizio 1 Assegnamento 1
// -----------------------------
#include <string.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>

void strtoupper(const char* in, size_t len, char* out) {
  for(int i=0;i<len;++i) // converto carattere per carattere
    out[i]=toupper(in[i]);
  out[len]='\0';
}

int main(int argc, char* argv[]) {

  for(int i=1; i<argc; ++i) {
    size_t len = strlen(argv[i]);
    char* str;
    if ((str = malloc(len+1)) != NULL) {
      strtoupper(argv[i], len, str);
      printf("%s --> %s\n", argv[i], str);
      free(str);
    } else {
      printf("FATAL ERROR, memory exhausted\n");
      return -1;
    }
  }

  return 0;
}
