#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <limits.h>
#include <dirent.h>
#include <unistd.h>

#include <sys/stat.h>
#include <sys/types.h>

#define CWD_STRING_SIZE 512

#define PRINT_PERMISSSIONS(modeDir) \
            if (S_IRUSR & modeDir) putchar('r'); else putchar('-');\
            if (S_IWUSR & modeDir) putchar('w'); else putchar('-');\
            if (S_IXUSR & modeDir) putchar('x'); else putchar('-');\
            if (S_IRGRP & modeDir) putchar('r'); else putchar('-');\
            if (S_IWGRP & modeDir) putchar('w'); else putchar('-');\
            if (S_IXGRP & modeDir) putchar('x');else putchar('-');\
            if (S_IROTH & modeDir) putchar('r');else putchar('-');\
            if (S_IWOTH & modeDir) putchar('w');else putchar('-');\
            if (S_IXOTH & modeDir) putchar('x');else putchar('-');


void printTreeDir(char *absPathRoot);

void printTreeDir(char *absPathRoot);
void processDir();
void processFile(char *relPathFile);

int main (int argc, char *argv[]){
    //controllo input
    //Usage: /.1a path

    printTreeDir(argv[1]);
    return 0;
}

void printTreeDir(char *absPathRoot){
    char nameCwd[CWD_STRING_SIZE] = "";

    if(getcwd(nameCwd, CWD_STRING_SIZE) == NULL){
        perror("error in getting the cwd");
        exit(EXIT_FAILURE);
    }

    if(chdir(absPathRoot)== -1){
        perror("couldn't change into absPathRoot");
        exit(EXIT_FAILURE);
    }
    printf("Directory: %s\n", absPathRoot);
    processDir();

    if(chdir(nameCwd) == -1){
        perror("couldn't return into the previous cwd");
        exit(EXIT_FAILURE);
    }
} 

void processDir(){
    
    DIR *d;
    struct dirent *file;
    if((d = opendir("."))==NULL){
        perror("coulnd't opendir the cwd");
        exit(EXIT_FAILURE);
    }
   
    while((errno = 0, file = readdir(d))!= NULL){
        if('.' == *(file->d_name))
            continue;
        processFile(file->d_name);
    }
    if(errno != 0){
        perror("couldn't open some file into directory");
        exit(EXIT_FAILURE);
    }


    if(closedir(d) == -1){
        perror("error into closedir the cwd");
        exit(EXIT_FAILURE);
    }
}

void processFile(char *relPathFile){
    struct stat info;
    if(stat(relPathFile, &info) == -1){
        perror("error in stat opening file");
        exit(EXIT_FAILURE);
    }

    if(S_ISDIR(info.st_mode)){
        puts("------------------");
        char *realPathDir = realpath(relPathFile,NULL);
        printTreeDir(realPathDir);
        free(realPathDir);
    }

    printf("%s\t%ld\t",relPathFile, info.st_size);
    PRINT_PERMISSSIONS(info.st_mode);
    puts("");
}




