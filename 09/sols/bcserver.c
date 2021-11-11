/*
 * Il server fa lo spawn di un processo che esegue bc.
 * La comunicazione con il processo bc avviene attraverso 2 pipe senza nome.
 * Il server risponde al client con la risposta ricevuta da bc.
 * 
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


#define CHECKNULL(r,c,e) CHECK_EQ_EXIT(e, (r=c), NULL,e,"") 

/** 
 * tipo del messaggio
 */
typedef struct msg {
    int len;
    char *str;
} msg_t;


void cleanup() {
  unlink(SOCKNAME);
}
int cmd(const char str[], char *buf) {
  int tobc[2];
  int frombc[2];
  
  int notused;
  SYSCALL_EXIT("pipe", notused, pipe(tobc), "pipe1", "");
  SYSCALL_EXIT("pipe", notused, pipe(frombc), "pipe2","");
  
  if (fork() == 0) {
    // chiudo i descrittori che non uso
    SYSCALL_EXIT("close", notused, close(tobc[1]), "close", "");  
    SYSCALL_EXIT("close", notused, close(frombc[0]), "close", "");
    
    SYSCALL_EXIT("dup2", notused, dup2(tobc[0],0),   "dup2 child (1)", "");  // stdin 
    SYSCALL_EXIT("dup2", notused, dup2(frombc[1],1), "dup2 child (2)", "");  // stdout
    SYSCALL_EXIT("dup2", notused, dup2(frombc[1],2), "dup2 child (3)", "");  // stderr
    
    execl("/usr/bin/bc", "bc", "-l", NULL);
    return -1;
  }
  // chiudo i descrittori che non uso
  SYSCALL_EXIT("close", notused, close(tobc[0]), "close","");
  SYSCALL_EXIT("close", notused, close(frombc[1]), "close","");
  int n;
  SYSCALL_EXIT("write", n, write(tobc[1], (char*)str,strlen(str)), "writen","");
  SYSCALL_EXIT("read",  n, read(frombc[0], buf, BUFSIZE), "readn","");  // leggo il risultato o l'errore
  SYSCALL_EXIT("close", notused, close(tobc[1]), "close","");  // si chiude lo standard input di bc cosi' da farlo terminare
  SYSCALL_EXIT("wait", notused, wait(NULL), "wait","");
  return n;
}

int main(int argc, char *argv[]) {
    // cancello il socket file se esiste
    cleanup();
    // se qualcosa va storto ....
    atexit(cleanup);

    int listenfd;
    // creo il socket
    SYSCALL_EXIT("socket", listenfd, socket(AF_UNIX, SOCK_STREAM, 0), "socket","");

    // setto l'indirizzo 
    struct sockaddr_un serv_addr;
    memset(&serv_addr, '0', sizeof(serv_addr));
    serv_addr.sun_family = AF_UNIX;    
    strncpy(serv_addr.sun_path, SOCKNAME, strlen(SOCKNAME)+1);

    int notused;
    // assegno l'indirizzo al socket 
    SYSCALL_EXIT("bind", notused, bind(listenfd, (struct sockaddr*)&serv_addr,sizeof(serv_addr)), "bind", "");
  
    // setto il socket in modalita' passiva e definisco un n. massimo di connessioni pendenti
    SYSCALL_EXIT("listen", notused, listen(listenfd, 1), "listen","");

    int connfd, n;
    do {
      SYSCALL_EXIT("accept", connfd, accept(listenfd, (struct sockaddr*)NULL ,NULL), "accept","");

      msg_t str;
      CHECKNULL(str.str,malloc(BUFSIZE), "malloc");
      while(1) {      
	char buffer[BUFSIZE];
	
	memset(str.str, '\0', BUFSIZE);
	// leggo la size della stringa
	SYSCALL_EXIT("readn", n, readn(connfd, &str.len, sizeof(int)), "readn1", ""); 
	// leggo la stringa
	SYSCALL_EXIT("readn", n, readn(connfd, str.str, str.len), "readn2", "");
	if (n==0) break;
	memset(buffer, '\0', BUFSIZE);
	if ((n = cmd(str.str, buffer)) < 0) { 
	  fprintf(stderr, "Errore nell'esecuzione del comando\n");
	  break;
	}
	buffer[n] = '\0';
	
	// invio la risposta
#if 1
	SYSCALL_EXIT("writen", notused, writen(connfd, &n, sizeof(int)), "writen1", "");
	SYSCALL_EXIT("writen", notused, writen(connfd, buffer, n), "writen2", "");
#else
	// qui si puo' utilizzare anche writev (man 2 writev) invece che 2 write distinte
	// NOTA: andrebbe implementata una writevn per evitare le "scritture parziali",
	//       l'implementazione e' un po' piu' complessa della writen
	struct iovec iov[2] = { {&n, sizeof(int)}, {buffer, n} };
	SYSCALL_EXIT("writev", notused, writev(connfd, iov, 2), "writev", "");
#endif	    
      }
      close(connfd);   
      printf("connection done\n");
      if (str.str) free(str.str);
    } while(1);
    return 0;
}
