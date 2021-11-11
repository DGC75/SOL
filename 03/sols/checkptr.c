#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define CHECK_PTR_EXIT(X, str)	\
    if ((X)==NULL) {		\
	perror(#str);		\
	exit(EXIT_FAILURE);	\
    }

int main() {
    FILE *fp;

    CHECK_PTR_EXIT(fp=fopen("fileprova.txt", "r"), opening fileprova.txt);
    CHECK_PTR_EXIT(fp=fopen("fileprova2.txt", "r"), "opening fileprova2.txt");
    

    return 0;
}
