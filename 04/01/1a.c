#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LENGTH 100
#define BUFFER MAX_LENGTH+1

int main(int argc, char* argv[]){
    
    FILE *f_passwd      = NULL;
    FILE *f_login_names = NULL;



    f_passwd = fopen("/etc/passwd", "r");
    f_login_names = fopen("./login_names.txt", "w");

    if(f_passwd == NULL || f_login_names == NULL){
        perror("fopen");
        exit(EXIT_FAILURE);
    }

    char *log_name = calloc(BUFFER, sizeof(char));

    if(log_name == NULL){
        perror("malloc");
        exit(EXIT_FAILURE);
    }

    while(fgets(log_name, BUFFER, f_passwd)!=NULL){
        //printf("STRING: %s\n", log_name);
        char *stop = strchr(log_name, ':');
        *stop = '\0';
        fprintf(f_login_names, "%s\n", log_name);    
    }

    fclose(f_passwd);
    fclose(f_login_names);
    
    return 0;
}