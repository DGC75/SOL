// ---------------------------
// Esercizio 3 Assegnamento 3
//
// ---------------------------

#include <stdio.h>
#include <stdlib.h>


#define dimRighe   16
#define dimColonne  8 

#define CHECK_PTR_EXIT(X, str) 	\
    if ((X)==NULL) {		\
	perror(#str);		\
	exit(EXIT_FAILURE);	\
    }

#define ELEM(Mat,i,j)	 Mat[(i*dimColonne)+j]
#define PRINTMAT(MAT)				 \
    do {					 \
     printf("Stampo la matrice %s:\n",#MAT);     \
     for(size_t i=0;i<dimRighe;++i) {		 \
	 for(size_t j=0;j<dimColonne;++j)	 \
	     printf("%4ld ", ELEM(MAT,i,j));	 \
	 printf("\n");				 \
     }						 \
    }while(0)
	    
int main() {
#if 0
  long* M=malloc(dimRighe*dimColonne*sizeof(long));
  CHECK_PTR_EXIT(M, malloc);
#else
  long* M;
  CHECK_PTR_EXIT( M = malloc(dimRighe*dimColonne*sizeof(long)), "malloc");
#endif
  
  for(size_t i=0;i<dimRighe;++i)
    for(size_t j=0;j<dimColonne;++j)			
      ELEM(M,i,j) = i+j;
  
  PRINTMAT(M);
  free(M);
  return 0;
}
