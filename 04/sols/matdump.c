// ---------------------------
// Esercizio 2 Assegnamento 4
//
// ---------------------------

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <utils.h>

#define DIM_LIMIT 512


// tipo della funzione che uso per fare il confronto
typedef int (*F_t)(const void*, const void*, size_t);

int confronta(F_t cmp, const void *s1, const void *s2, size_t n) {
    return cmp(s1,s2,n);
}

int main(int argc, char *argv[]) {

    if (argc != 3) {
	fprintf(stderr, "use: %s <dim> crea|check\n", argv[0]);
	return EXIT_FAILURE;
    }
    long dim;
    if (isNumber(argv[1], &dim) != 0) {
      fprintf(stderr, "'%s' non e' un numero valido",argv[1]);
      return EXIT_FAILURE;
    }
    if (dim > DIM_LIMIT) {
	fprintf(stderr,"dim (%ld) è troppo grande, deve essere <=%d\n", dim, DIM_LIMIT);
	return EXIT_FAILURE;
    }
    int crea = (strncmp("crea", argv[2], strlen("crea")) == 0);

    if (crea)  {  // creiamo la matrice e la scriviamo su file
    
	float *mat = NULL;
	// alloco la matrice come un singolo vettore
	CHECK_EQ_EXIT(mat = (float*)malloc(dim*dim*sizeof(float)), NULL, "malloc");
	
	// riempio la matrice
	for(long i=0;i<dim;++i)
	    for(long j=0;j<dim;++j)
		mat[i*dim + j] = (i+j)/2.0;
	
	FILE *fp1;
	CHECK_EQ_EXIT(fp1 = fopen("./mat_dump.dat", "w"), NULL, "fopen");
	
#if 1 // write singola
	CHECK_NEQ_EXIT(fwrite(&mat[0], sizeof(float), dim*dim, fp1), (dim*dim), "fwrite");
#else // alternativamente 
	for(long i=0;i<dim;++i)
	    for(long j=0;j<dim;++j) 
		CHECK_NEQ_EXIT(fwrite(&mat[i*dim+j], sizeof(float), 1, fp1), 1, "fwrite");    
#endif
	fclose(fp1);
	
	// dump testuale
	CHECK_EQ_EXIT(fp1 = fopen("./mat_dump.txt", "w"), NULL, "fopen");
	for(long i=0;i<dim;++i)
	    for(long j=0;j<dim;++j) {
		if (fprintf(fp1, "%f\n", mat[i*dim+j])<0) {
		    perror("fprintf");
		    return EXIT_FAILURE;
		}
	    }
	fclose(fp1);
	if (mat) free(mat);
    } else {  // controllo

	float *mat = NULL;
	CHECK_EQ_EXIT(mat = (float*)malloc(dim*dim*sizeof(float)), NULL, "malloc");
	
	float *mat2 = NULL;
	CHECK_EQ_EXIT(mat2 = (float*)malloc(dim*dim*sizeof(float)), NULL, "malloc");
	FILE *fp1;
	CHECK_EQ_EXIT(fp1=fopen("./mat_dump.dat", "r"), NULL, "fopen mat_dump.dat");
	FILE *fp2;
	CHECK_EQ_EXIT(fp2=fopen("./mat_dump.txt", "r"), NULL, "fopen max_dump.txt");

#define MAX_NUM_LEN 128	
	char buf[MAX_NUM_LEN];
	for(long i=0;i<dim;++i)
	    for(long j=0;j<dim;++j) {
		if (fgets(buf, MAX_NUM_LEN, fp2) == NULL) {
		    perror("fgets");
		    exit(EXIT_FAILURE);
		}
		buf[strlen(buf)-1]='\0'; // sostituisco '\n' con '\0'
		mat[i*dim+j] = strtof(buf, NULL);
	    }
	fclose(fp2);
	
#if 1 // read singola
	CHECK_NEQ_EXIT(fread(&mat2[0], sizeof(float), dim*dim, fp1),(dim*dim), "fread");
#else // alternativamente
	for(long i=0;i<dim;++i)
	    for(long j=0;j<dim;++j)
		CHECK_NEQ_EXIT(fread(&mat2[i*dim+j], sizeof(float), 1, fp1), 1, "fread");
#endif
	fclose(fp1);

	
	if (confronta(memcmp, mat, mat2, dim*dim*sizeof(float)) != 0) {
	    fprintf(stderr, "Le due matrici non corrispondono\n");
	    return EXIT_FAILURE;
	}
	fprintf(stdout, "Le due matrici sono uguali\n");
	free(mat);
	free(mat2);
    }
    return EXIT_SUCCESS;
}




