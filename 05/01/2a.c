#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>


#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#define BUF_SIZE 256

long isNumber(const char* s) {
   char* e = NULL;
   long val = strtol(s, &e, 0);
   if (e != NULL && *e == (char)0) return val; 
   return -1;
}

int main (int argc, char *argv[]){

    if(argc != 3 && argc != 4){
        printf("Usage: %s filein fileout [buffersize]\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    //CHECK IF file_in is a file
    //         file_out

    //Check if there is a buffersize, and substitute
    long buf_size = BUF_SIZE;
        if(argc == 4){
            long size = isNumber(argv[3]);
            buf_size = (size == -1) ? BUF_SIZE:size;
        }

    char buffer[BUF_SIZE];
    //OPEN IN READING THE PATH  filein

    FILE *fIn = fopen(argv[1], "r");
    if(fIn == NULL){
        perror("fin == NULL");
        exit(EXIT_FAILURE);
    }


    FILE* fOut = fopen(argv[2], "w");
    if(fOut == NULL){
        perror("fOut == NULL");
        exit(EXIT_FAILURE);
    }        
    int x = fread(buffer, sizeof(char), BUF_SIZE, fIn);
    while(!feof(fIn)){
        fwrite(buffer,sizeof(char),x,fOut);
        x = fread(buffer, sizeof(char), BUF_SIZE, fIn);
    }
    fwrite(buffer,sizeof(char),x,fOut);
    
    fclose(fIn);
    fclose(fOut);
    return 0;
}
