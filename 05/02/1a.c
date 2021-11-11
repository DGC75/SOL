#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/types.h> 
#include <dirent.h>      
#include <sys/stat.h>

#include <time.h>
#include <errno.h>
#include <unistd.h>

#define BUF_SIZE 512

char* printattr(char *pathname, char *filename);
void processdir(char * filename);
void searchdir(char *path, char *file);


int main(int argc, char *argv[]){

    searchdir(argv[1], argv[2]);
    
    return 0;
}


void searchdir(char *path, char *file){
    char buf[BUF_SIZE] = ""; 
    if(getcwd(buf, BUF_SIZE) == NULL){
        perror("getcwd");
        exit(EXIT_FAILURE);
    }   
    //printf("directory %s\n", path);
    
    if(chdir(path) == -1){
        perror("chdir");
        exit(EXIT_FAILURE);
    }

    processdir(file);

    
    if(chdir(buf) == -1){
        perror("err returning to main dir");
        exit(EXIT_FAILURE);
    }
}

void processdir(char *filename){

    DIR *d;
    struct dirent* file;
    if ((d = opendir(".")) == NULL){
        perror("opening cwd");  
        exit(EXIT_FAILURE); 
        }

    
    while ( (errno = 0, file = readdir(d))!=NULL) {
            
            if(*( file->d_name) == '.')
                continue;

            
            char *s = printattr(file->d_name, filename);
            if (errno != 0) { /* trattamento errore */ }
            else { /* trattamento caso OK */ }
            }
            /* chiusura directory */
            if (( closedir(d) == -1) ){ 
                perror("closing cwd");
                exit(EXIT_FAILURE);
                } 


}

char* printattr(char * path, char*filename){
    //printf("path:%s\n", path);
    struct stat info;
    if ( stat(path,&info)== -1){
        /* gestione errore */
    } else {
        char *s = realpath(path, NULL);

        if(strcmp(filename, path) == 0)
            printf("realpath:%s\t\tlast_mod:%s", s, ctime(&info.st_mtime));

        if (S_ISDIR(info.st_mode)){
            
            //printf("%s is a dir\n", s);
            searchdir(path, filename);
            
        }
    free(s);
    }
}