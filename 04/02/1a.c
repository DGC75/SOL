#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define OFFSET(I,J,DIM) DIM*I+J
#define ELEM(MATR, DIM,I,J) *(MATR + OFFSET(I,J,DIM))


int matr_cmp(float *mat1, float *mat2, int dim, int (*compar)(const void *, const void *, size_t));

int main(int argc, char* argv[]){
    
    if(argc != 2 && argc != 4){
        printf("Usage:.%s N OR ./%s file1_txt file2_dat N\n", argv[0], argv[0]);
        exit(EXIT_FAILURE);
    }
    
    if(argc == 2){
        /*ALLOCA E INIT MATR*/
        int dim = atoi(argv[1]);
        float * matr = calloc(dim*dim, sizeof(float));
        if(matr == NULL){
            perror("malloc");
            exit(EXIT_FAILURE);
        }
        int i, j;
        for(i = 0; i < dim; i++){
            for(j = 0; j < dim; j++){
                ELEM(matr,dim,i,j) = ((float)(i + j))/2.0;
                printf("%.2f\t", ELEM(matr,dim,i,j));
            }
            puts("");
        }

        /*SALVA MATR*/

        FILE *f_mat_txt = fopen("./mat_dump.txt", "w");
        FILE *f_mat_dat = fopen("./mat_dump.dat", "w");

        if(f_mat_txt == NULL){
            perror("fopen");
            exit(EXIT_FAILURE);
        }

        for(i = 0; i < dim; i++){
            for(j = 0; j < dim; j++){
                fprintf(f_mat_txt, "%.2f ", ELEM(matr,dim, i , j));
                fwrite(&ELEM(matr,dim, i , j),sizeof(float), 1, f_mat_dat);
            }
            fprintf(f_mat_txt,"\n");
        }

        fclose(f_mat_dat);
        fclose(f_mat_txt);
    }
    else{
        /*CONFRONTA MATRICI*/
        //./%s file1_txt file2_dat N
        int dim = atoi(argv[3]);

        FILE *f_mat1 = fopen("./mat_dump.txt", "r");
        FILE *f_mat2 = fopen("./mat_dump.dat", "r");

        float *matr1 = calloc(dim*dim, sizeof(float));
        float *matr2 = calloc(dim*dim, sizeof(float));

        if(matr1 == NULL || matr2 == NULL){
            perror("malloc");
            exit(EXIT_FAILURE);
        }
        /*LOAD MATRICES INTO STRUCTURES*/
        int i, j;
        for(i = 0; i < dim; i++){
            for(j = 0; j < dim; j++){
                //printf("%.2f\t", ELEM(matr2, dim, i, j));
                //printf("%d\t", OFFSET(i,j,dim));
                fscanf(f_mat1, "%f", &ELEM(matr1,dim, i, j));
                fread(&ELEM(matr2,dim, i, j), sizeof(float),1,f_mat2);
                //printf("%.2f\t", ELEM(matr2,dim, i, j));
            }
            //puts("");
        }

        /*COMPARE THE TWO MATRICES, WITH A GENERIC CMP FUNCTION*/
        if(matr_cmp(matr1,matr2, dim, memcmp) != 0)
            puts("Matrici diverse");
        else
            puts("Matrici uguali");
    }

    return 0;
}

int matr_cmp(float *mat1, float *mat2, int dim, int (*compar)(const void *, const void *, size_t size)){
    int i, j, ret;
    for(i=0; i < dim; i++){
        for(j = 0; j < dim; j++){
            if(compar(&ELEM(mat1,dim,i,j), &ELEM(mat2,dim,i,j), sizeof(float))!= 0)
                return 1;
        }
    }

    return 0;
}