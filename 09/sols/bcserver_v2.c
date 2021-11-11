/*
 * Seconda versione.
 * In questo caso il processo che esegue bc e' messo 
 * direttamente in comunicazione con il client.
 * Il server non risponde al client.
 */

#include <unistd.h>
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/uio.h>

#include <util.h>
#include <conn.h>

void cleanup() {
    unlink(SOCKNAME);
}

int cmd(int connfd) {
    int notused;

    if (fork() == 0) {

      // redirigo input output ed error sul descrittore associato
      // alla connessione con il client
      SYSCALL_EXIT("dup2", notused, dup2(connfd, 0), "dup2 child (1)", "");
      SYSCALL_EXIT("dup2", notused, dup2(connfd, 1), "dup2 child (2)", "");
      SYSCALL_EXIT("dup2", notused, dup2(connfd, 2), "dup2 child (2)", "");
      
      execl("/usr/bin/bc", "bc", "-lq", NULL);
      return -1;
    }
    SYSCALL_EXIT("wait", notused, wait(NULL), "wait", "");
    printf("child exited\n");
    
    return 0;
}

int main(int argc, char *argv[]) {
    // cancello il socket file se esiste
    cleanup();
    // se qualcosa va storto ....
    atexit(cleanup);

    int listenfd;
    // creo il socket
    SYSCALL_EXIT("socket", listenfd, socket(AF_UNIX, SOCK_STREAM, 0), "socket", "");

    // setto l'indirizzo 
    struct sockaddr_un serv_addr;
    memset(&serv_addr, '0', sizeof(serv_addr));
    serv_addr.sun_family = AF_UNIX;    
    strncpy(serv_addr.sun_path, SOCKNAME, strlen(SOCKNAME)+1);

    int notused;
    // assegno l'indirizzo al socket 
    SYSCALL_EXIT("bind", notused, bind(listenfd, (struct sockaddr*)&serv_addr,sizeof(serv_addr)), "bind", "");
  
    // setto il socket in modalita' passiva e definisco un n. massimo di connessioni pendenti
    SYSCALL_EXIT("listen", notused, listen(listenfd, 1), "listen", "");

    int connfd;
    do {
      SYSCALL_EXIT("accept", connfd, accept(listenfd, (struct sockaddr*)NULL ,NULL), "accept","");
      if (cmd(connfd) < 0) { 
	fprintf(stderr, "Errore nell'esecuzione del comando\n");
	break;
      }
      close(connfd);   
      printf("connection done\n");
    } while(1);
    close(listenfd);
    return 0;
}
