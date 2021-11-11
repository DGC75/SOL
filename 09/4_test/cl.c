#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <errno.h> 

#define SOCKNAME "./cs_sock"
#define N 100


static void run_client(struct sockaddr * psa){
    
        /* figlio, client */
        int fd_skt; 
        char buf[N];
        fd_skt=socket(AF_UNIX,SOCK_STREAM,0);
        while (connect(fd_skt,(struct sockaddr*)psa,sizeof(*psa)) == -1){
            if ( errno == ENOENT ) sleep(1); 
            else {
                perror("connect err");
                exit(EXIT_FAILURE); 
            }
        }
        
        while(1){
        fgets(buf, N, stdin);
        write(fd_skt, buf, N);
        
        if(read(fd_skt,buf,N) == 0) break;
        printf("Client got: %s\n",buf);
        }
        exit(EXIT_SUCCESS);
} /* figlio terminato */

int main (void){
    int i;
    struct sockaddr_un sa; /* ind AF_UNIX */
    strcpy(sa.sun_path, SOCKNAME);
    sa.sun_family=AF_UNIX;
   
    run_client((struct sockaddr *)&sa);/* attiv server */
            
    return 0;
}
