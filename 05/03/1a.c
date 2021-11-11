#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

#include <dirent.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include <pwd.h>
#include <grp.h>


#define BUF_SIZE 512

void find_and_print_files(char *path);
char* printattr(char *pathname, char *filename);
void processdir(char * filename);
void searchdir(char *path, char *file);


int main (int argc, char *argv[]){
    int i;
 
    for(i = 0; i < argc; i++){
        find_and_print_files(argv[1]);    
    }
    return 0;
}

void find_and_print_files(char *path){
    searchdir("/home/g", path);
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

        if(strcmp(filename, path) == 0){
            puts("");
            puts("found!");
            printf("name:%s\n", s);
            printf("inode number:%ld\n", (long)info.st_ino);
            //TYPE
            printf("File type:");
            if (S_ISREG(info.st_mode)) printf("regular");
            else if (S_ISDIR(info.st_mode)) printf("directory"); 
            else if (S_ISLNK(info.st_mode)) printf("link simb");
            else if (S_ISCHR(info.st_mode)) printf("character special file");
            else if (S_ISBLK(info.st_mode)) printf("block special file");
            else if (S_ISFIFO(info.st_mode)) printf("pipe");
            else if (S_ISSOCK(info.st_mode)) printf("socket");
            else printf("non riconosciuto");
            puts("");
            //BIT DI PROTEZIONE
            if (S_IRUSR & info.st_mode) putchar('r'); else putchar('-');
            if (S_IWUSR & info.st_mode) putchar('w'); else putchar('-');
            if (S_IXUSR & info.st_mode) putchar('x'); else putchar('-');/* group */
            if (S_IRGRP & info.st_mode) putchar('r'); else putchar('-');
            if (S_IWGRP & info.st_mode) putchar('w'); else putchar('-');
            if (S_IXGRP & info.st_mode) putchar('x');else putchar('-');
            /* others */
            if (S_IROTH & info.st_mode) putchar('r');else putchar('-');
            if (S_IWOTH & info.st_mode) putchar('w');else putchar('-');
            if (S_IXOTH & info.st_mode) putchar('x');else putchar('-');
            //USER E GROUP IDENTIFIER
            puts("");
            struct passwd * userid = getpwuid(info.st_uid);
            if(userid == NULL)
                perror("getpwuid failed");

            printf("username:%s\n", userid->pw_name);    
            struct group * groupname = getgrgid(info.st_gid);
            printf("groupname:%s\n", groupname->gr_name);


        }
        free(s);
        if(S_ISDIR(info.st_mode))
            searchdir(path, filename);

    }
}