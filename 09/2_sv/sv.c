#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <errno.h> 
#include <pthread.h>
#include <ctype.h>
#include <limits.h>

#define SOCKNAME "./mysock"
#define MAX_MSG_LEN _POSIX_PIPE_BUF

static void * th_func(void *arg){
    int fd_c = *(int*) arg;
    char msg[MAX_MSG_LEN] = {'\0'};

    while(1){
        /*SE IL SOCK E' CHIUSO, ALLORA LA READ RITORNA 0*/
        if(read(fd_c, msg, MAX_MSG_LEN) == 0) break; 
        /*MODIFICHIAMO LA STRINGA*/
        int i;
        for(i = 0; i < strlen(msg); i++){
            msg[i] = toupper(msg[i]);
        }
        /*RESTITUIAMO LA STRINGA AL CLIENT*/
        write(fd_c, msg, MAX_MSG_LEN);

    }

    return NULL;
}

void run_sv(struct sockaddr * sa){

    int fd_skt, fd_c;
    unlink(SOCKNAME);
    //SOCKET,BIND,LISTEN
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
    pthread_t new_th;
    while(1){
        puts("server: accetto connessioni in entrata...");
		//ACCETTA CONNESSIONE
        if(-1 == (fd_c=accept(fd_skt,NULL,0))){
            perror("accept err");
            exit(EXIT_FAILURE);
        }
        printf("server:connessione accettata:%d\n", fd_c);
        //PER OGNI NUOVA CONNESSIONE, SPAWNA THREAD DETACHED
        if(pthread_create(&new_th, NULL, th_func, (void *) &fd_c) != 0){
            perror("pthread_create");
            exit(EXIT_FAILURE);
        }
        pthread_detach(new_th);
        //PTHREAD CREATE + PTHREAD DETACH
        //IL THREAD TERMINA QUANDO IL CLIENT CHIUDE LA CONNESSIONE
        //MI SERVE A QUALCOSA TENERE TRACCIA DEL THREAD?
    }

}

int main (void){
    int i;
    struct sockaddr_un sa; /* ind AF_UNIX */
    strcpy(sa.sun_path, SOCKNAME);
    sa.sun_family=AF_UNIX;
   
    run_sv((struct sockaddr *)&sa);/* attiv server */
            
    return 0;
}

