
#include <unistd.h>
#include <sys/types.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/wait.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <errno.h> 
#include <limits.h>

#include <assert.h>       /* ind AF_UNIX */
#define UNIX_PATH_MAX 108   /* man 7 unix */
#define SOCKNAME "./mysock"
#define MAX_MSG_LEN _POSIX_PIPE_BUF



void run_server(struct sockaddr *sa){
    
	int fd_skt, fd_c;
	char msg[MAX_MSG_LEN] = {'\0'};
    unlink(SOCKNAME);

    if(-1 == (fd_skt=socket(AF_UNIX,SOCK_STREAM,0))){
        perror("socket err");
        exit(EXIT_FAILURE);
    }
    printf("fd_skt:%d\n", fd_skt);
    
    if(-1 == (bind(fd_skt,sa,sizeof(*sa)))){
        perror("bind err");
        exit(EXIT_FAILURE);
    }

    if(-1 == (listen(fd_skt,SOMAXCONN))){
        perror("listen err");
        exit(EXIT_FAILURE);
    }

    while(1){
        puts("server: accetto connessioni in entrata...");
		//ACCETTA CONNESSIONE
        if(-1 == (fd_c=accept(fd_skt,NULL,0))){
            perror("accept err");
            exit(EXIT_FAILURE);
        }
        printf("server:connessione accettata:%d\n", fd_c);
        while(1){
            //FORKA CALC E COLLEGALA CON PIPE
            if(read(fd_c, msg, MAX_MSG_LEN) == 0) break;
            int pidc;
                int frombc[2];
                int tobc[2];
                if(pipe(tobc) == -1){ perror("error pipe_tobc"); exit(EXIT_FAILURE);}
                if(pipe(frombc) == -1){ perror("error pipe_frombc"); exit(EXIT_FAILURE);}
                if((pidc = fork()) == -1){ perror("fork err"); exit(EXIT_FAILURE);}
                if(pidc == 0){
                        close(fd_skt);
                        close(fd_c);
                        close(frombc[0]);
                        close(tobc[1]);
                        dup2(tobc[0], 0);
                        dup2(frombc[1], 1);
                        dup2(frombc[1], 2);
                        
                        execlp("bc","bc", "-lq", (char*)NULL);
                        puts("you shouldn't be here");
                        exit(EXIT_FAILURE);
                }
                close(frombc[1]);
                close(tobc[0]);

            //MANDA MEX
            write(tobc[1], msg, strlen(msg));
			memset(msg, '\0', MAX_MSG_LEN);
			read(frombc[0], msg, MAX_MSG_LEN);
            close(frombc[0]);
            close(tobc[1]);
            waitpid(pidc, NULL, 0);
			write(fd_c, msg, MAX_MSG_LEN);
			//printf("server: ricevuto dalla calc:%s", msg);
			memset(msg, '\0', MAX_MSG_LEN);
        }
		close(fd_c);
		//CHIUDI CONNESSIONE 
    }        
}

int main (int argc, char *argv[]){
    
    

    struct sockaddr_un sa;      
    strncpy(sa.sun_path, SOCKNAME,UNIX_PATH_MAX);
    sa.sun_family=AF_UNIX;

    run_server((struct sockaddr *)&sa);

    return 0;
}