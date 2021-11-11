#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <errno.h> 

#define SOCKNAME "./mysock"
#define N 100
#define N_CLIENTS 4



int aggiorna(fd_set *set, int old_fd_num){
    int i;
    int max= old_fd_num;
    for(i = 0; i <= old_fd_num; i++){
        if(FD_ISSET(i, set) && i > max) max = i;
    }

    return max;
}

static void run_client(struct sockaddr * psa){
    if (fork()==0){ 
        /* figlio, client */
        int fd_skt; 
        char buf[N];
        fd_skt=socket(AF_UNIX,SOCK_STREAM,0);
        while (connect(fd_skt,(struct sockaddr*)psa,sizeof(*psa)) == -1){
            if ( errno == ENOENT ) sleep(1); 
            else exit(EXIT_FAILURE); 
        }
        write(fd_skt,"Hallo!",7);
        read(fd_skt,buf,N);
        printf("Client got: %s\n",buf);
        close(fd_skt); 

        exit(EXIT_SUCCESS);
    }
} /* figlio terminato */

static void run_server(struct sockaddr * psa){
    int fd_sk;      /* socket di connessione */
    int fd_c;       /* socket di I/O con un client */
    int fd_num=0;   /* max fd attivo */ 
    int fd;         /* indice per verificare risultati select */
    char buf[N]; 
    fd_set set, rdset; 
    int nread;
    fd_sk=socket(AF_UNIX,SOCK_STREAM,0);
    bind(fd_sk,(struct sockaddr *)psa,sizeof(*psa));
    listen(fd_sk,SOMAXCONN);
    if (fd_sk > fd_num) 
        fd_num = fd_sk;
    
    FD_ZERO(&set);
    FD_SET(fd_sk,&set); 
    while(1){
        rdset=set; /* preparo maschera per select */
        if (select(fd_num+1,&rdset,NULL,NULL,NULL)==-1) {
            /* gest errore */
        }
        else{ 
        /* select OK */
            for (fd = 0; fd<=fd_num;fd++){
                if (FD_ISSET(fd,&rdset)){ 
                    if (fd== fd_sk){
                        /* sock connect pronto */
                        fd_c=accept(fd_sk,NULL,0);
                        FD_SET(fd_c, &set);
                        if (fd_c > fd_num) 
                            fd_num = fd_c; 
                        } 
                        else{
                            /* sock I/0 pronto */
                            nread=read(fd,buf,N);
                            if (nread==0){
                                /* EOF client finito */
                                FD_CLR(fd,&set);
                                fd_num=aggiorna(&set, fd_num);
                                close(fd); 
                            }
                            else{ 
                            /* nread !=0 */
                                printf("Server got: %s\n",buf);
                                write(fd,"Bye!",5);
                            }
                        } 
                } 
            }
        }  
    } /* chiude while(1) */
} /* chiude run_server */


int main (void){
    int i;
    struct sockaddr_un sa; /* ind AF_UNIX */
    strcpy(sa.sun_path, SOCKNAME);
    sa.sun_family=AF_UNIX;



    for(i=0; i< N_CLIENTS; i++) 
        run_client((struct sockaddr *)&sa);/* attiv client*/ 
    
    
    run_server ((struct sockaddr *)&sa);/* attiv server */
            
    return 0;
}