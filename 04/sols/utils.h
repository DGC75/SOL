#if !defined(_UTILS_H)
#define _UTILS_H

#include <errno.h>
#include <stdlib.h>


// macro di utilità per controllare errori
#define CHECK_EQ_EXIT(X, val, str)	\
  if ((X)==val) {			\
    perror(#str);			\
    exit(EXIT_FAILURE);			\
  }
#define CHECK_NEQ_EXIT(X, val, str)	\
  if ((X)!=val) {			\
    perror(#str);			\
    exit(EXIT_FAILURE);			\
  }

/** 
 * \brief Controlla se la stringa passata come primo argomento e' un numero.
 * \return  0 ok  1 non e' un numbero   2 overflow/underflow
 */
static int isNumber(const char* s, long* n) {
  if (s==NULL) return 1;
  if (strlen(s)==0) return 1;
  char* e = NULL;
  errno=0;
  long val = strtol(s, &e, 10);
  if (errno == ERANGE) return 2;    // overflow/underflow
  if (e != NULL && *e == (char)0) {
    *n = val;
    return 0;   // successo 
  }
  return 1;   // non e' un numero
}

#endif 
