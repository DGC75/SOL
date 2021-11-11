#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <errno.h> 
#include <limits.h>

#define SOCKNAME "./mysock"
#define MAX_MSG_LEN _POSIX_PIPE_BUF

void run_cl(struct sockaddr * sa){
    int fd_skt;
    char msg[MAX_MSG_LEN] = {'\0'};
    
    fd_skt=socket(AF_UNIX,SOCK_STREAM,0);
    while (connect(fd_skt,(struct sockaddr*)sa,sizeof(*sa)) == -1 ){
    if ( errno == ENOENT ) sleep(1); /* sock non esiste */
    else{
        perror("client connect");
        exit(EXIT_FAILURE);
        } 
    }
    
    while(1){
        
        memset(msg, '\0', MAX_MSG_LEN);
		//RACCOGLI MESSAGGIO UTENTE
		fgets(msg, MAX_MSG_LEN, stdin);
        //SE QUIT, ESCI
		if(strncmp(msg, "quit\n", sizeof("quit\n")) == 0){
			puts("client: received quit");
			puts("client:closing connection...");
			close(fd_skt);
			puts("client: connection closed. Exiting.");
			break;
		}
		//MANDALO AL SERVER
		 write(fd_skt, msg, MAX_MSG_LEN);
		//ASPETTA RISULTATO 
		memset(msg, '\0', MAX_MSG_LEN);
	
		read(fd_skt, msg, MAX_MSG_LEN);
		printf("result:%s\n", msg);
    }
}

int main (void){
    int i;
    struct sockaddr_un sa; /* ind AF_UNIX */
    strcpy(sa.sun_path, SOCKNAME);
    sa.sun_family=AF_UNIX;
   
    run_cl((struct sockaddr *)&sa);/* attiv server */
            
    return 0;
}
