#include <stdio.h>
#include <stdlib.h>

#define dimN 16
#define dimM  8

#define CHECK_PTR_EXIT(long_ptr, str) \
        if( long_ptr == NULL){\
            perror(str);\
            exit(EXIT_FAILURE);\
        }

#define ELEM(long_ptr, int_var_n, int_var_m) \
        *(long_ptr + sizeof(long)*int_var_n + sizeof(long)*int_var_m)


#define PRINTMAT(long_ptr, int_var_n, int_var_m) \
        int a,b;\
        for(a=0; a< int_var_n; a++){\
            for(b=0; b< int_var_m; b++){\
                printf("%li\t", ELEM(long_ptr, a, b) );\
            }\
            puts("");\
        }




int main() {
    long *M = malloc(dimN*dimM*sizeof(long));

    CHECK_PTR_EXIT(M, "malloc"); 
    for(size_t i=0;i<dimN;++i)
	for(size_t j=0;j<dimM;++j)			
	    ELEM(M,i,j) = i + j;    
    
    PRINTMAT(M, dimN, dimM);
    free(M);
    return 0;
}