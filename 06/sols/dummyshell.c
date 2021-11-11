#define _POSIX_C_SOURCE 200112L
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>     
#include <errno.h> 
#include <sys/wait.h>  
#include <sys/types.h> 

#define MAX_INPUT_LINE    1024           /* massima lunghezza della stringa del comando */
#define MAX_PARAM          256           /* numero massimo di parametri consentiti per un comando */
#define MALLOC(ptr, sz)			   \
    if ((ptr=(char*)malloc(sz)) == NULL) { \
	perror("malloc");		   \
	exit(EXIT_FAILURE);		   \
    }


void build_args(char * line, char * argv[]) {    
    int   i = 0;    
    char *tmpstr, *token = strtok_r(line, " ", &tmpstr);
    size_t len = strlen(token)+1;
    MALLOC(argv[i], len);
    strncpy(argv[i], token, len);
    while((token = strtok_r(NULL, " ", &tmpstr)) != NULL) {
	i++;
	len = strlen(token)+1;
	MALLOC(argv[i],len);
	strncpy(argv[i], token, len);
    }
    argv[i+1] = NULL;
}
static void cleanargv(char * argv[]) {
    int i=0;
    while(argv[i]!=NULL) {
	free(argv[i]);
	++i;
    }
}
static void print_prompt() {
    printf("> ");
    fflush(stdout);
}
int main() {
    int           pid;
    int  childforked = 0;
    int           status;
    char          inputline[MAX_INPUT_LINE];
    char         *argv[MAX_PARAM];
    
    while(1) {
	print_prompt();
	memset(inputline, 0, MAX_INPUT_LINE);
	if (read(STDIN_FILENO, inputline, MAX_INPUT_LINE)  == -1) {
	  perror("leggendo dallo standard input:");
	  continue;
	}
	inputline[strlen(inputline)-1] = '\0';

	if (inputline[0] != '\0') build_args(inputline, argv);
	else continue; 
	  
	if (strncmp(inputline, "exit", 4) == 0)  {
	    cleanargv(argv);
	    break;
	}
	
	if ((pid = fork()) < 0) {
	  perror("fork");
	  continue;
	}
	if (pid == 0) {
	    execvp(argv[0], &argv[0]);
	    perror("execvp");
	    exit(errno);
	}
	childforked++;
	if ((pid = waitpid(-1, &status, 0 /*WNOHANG*/)) != -1)
	    if (pid != 0) {
		childforked--;
		if(WIFEXITED(status)) {
		    fprintf(stdout, "Processo %d terminato con ",pid);
		    fprintf(stdout, "exit(%d)\n",WEXITSTATUS(status));
		    fflush(stdout);
		}
	    }
	cleanargv(argv);
    }
    while(childforked>0) {
	if ((pid = waitpid(-1, &status, WNOHANG)) != -1)
	    if (pid != 0) {
		childforked--;
		if(WIFEXITED(status)) {
		    printf("Processo %d terminato con ",pid);
		    printf("exit(%d)\n",WEXITSTATUS(status));
		    fflush(stdout);
		}		
	    }
    }
    printf("shell done ....\n");
    return 0;
}
