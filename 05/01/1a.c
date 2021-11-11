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

    int fdinput = open(argv[1], O_RDONLY);
    if(fdinput == -1){
        perror("Errore file input in apertura");
        exit(EXIT_FAILURE);
    }
    
    //OPEN IN WRITE fileout
    int fdout = open(argv[2], O_WRONLY | O_CREAT| O_TRUNC, S_IRWXU | S_IRWXG);
    if(fdout == -1){
        perror("Errore file output in apertura");
        exit(EXIT_FAILURE);
    }
    
    
    int x = read(fdinput, buffer, BUF_SIZE);
    
    while(x != 0 && x != -1){
        write(fdout, buffer, x);
        x = read(fdinput, buffer, BUF_SIZE);
    }

    if(x == -1){
        perror("errore read");
        exit(EXIT_FAILURE);
    }
    printf("%d\n", x);
    close(fdout);
    close(fdinput);

    umask(0002);

    
    return 0;
}
