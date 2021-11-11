#define _POSIX_C_SOURCE  200112L
#include <unistd.h>
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <stdlib.h>
#include <pthread.h>
#include <ctype.h>
#include <signal.h>

#include <util.h>
#include <conn.h>

/** 
 * tipo del messaggio
 */
typedef struct msg {
    int len;
    char *str;
} msg_t;


static volatile sig_atomic_t termina=0;
static void sighandler(int useless) {
    termina=1;
}

// converte tutti i carattere minuscoli in maiuscoli
void toup(char *str) {
    char *p = str;
    while(*p != '\0') { 
        *p = (islower(*p)?toupper(*p):*p); 
	++p;
    }        
}
// thread che gestisce tutta la connessione verso il client
void *threadF(void *arg) {
  assert(arg);
  long connfd = (long)arg;
  do {
    msg_t str;
    int n;
    if ((n=readn(connfd, &str.len, sizeof(int))) == -1) {
      perror("read1");
      break;
    }
    
    if (n==0) break;
    str.str = calloc((str.len), sizeof(char));
    if (!str.str) {
      perror("calloc");
      fprintf(stderr, "Memoria esaurita....\n");
      break;
    }		    
    if ((n=readn(connfd, str.str, str.len * sizeof(char))) == -1) {
      perror("read2");
      free(str.str);
      break;
    }
    
    toup(str.str);
    
    if ((n=writen(connfd, &str.len, sizeof(int))) == -1) {
      perror("write1");
      free(str.str);
      break;
    }
    if ((n=writen(connfd, str.str, str.len*sizeof(char))) == -1) {
      perror("write2");
      free(str.str);
      break;
    }
    free(str.str);
  } while(1);
  close(connfd);	    
  return NULL;
}

int spawn_thread(long connfd) {
  pthread_attr_t thattr;
  pthread_t thid;
  
  sigset_t mask,oldmask;
  sigemptyset(&mask);
  sigaddset(&mask, SIGINT); 
  sigaddset(&mask, SIGQUIT);
  sigaddset(&mask, SIGTERM);
  sigaddset(&mask, SIGHUP);
  
  if (pthread_sigmask(SIG_BLOCK, &mask,&oldmask) != 0) {
    fprintf(stderr, "FATAL ERROR, pthread_sigmask\n");
    close(connfd);
    return -1;
  }
  
  
  if (pthread_attr_init(&thattr) != 0) {
    fprintf(stderr, "pthread_attr_init FALLITA\n");
    close(connfd);
    return -1;
  }
  // settiamo il thread in modalità detached
  if (pthread_attr_setdetachstate(&thattr,PTHREAD_CREATE_DETACHED) != 0) {
    fprintf(stderr, "pthread_attr_setdetachstate FALLITA\n");
    pthread_attr_destroy(&thattr);
    close(connfd);
    return -1;
  }
  if (pthread_create(&thid, &thattr, threadF, (void*)connfd) != 0) {
    fprintf(stderr, "pthread_create FALLITA");
    pthread_attr_destroy(&thattr);
    close(connfd);
    return -1;
  }
  
  if (pthread_sigmask(SIG_SETMASK, &oldmask, NULL) != 0) {
    fprintf(stderr, "FATAL ERROR\n");
    close(connfd);
    return -1;
  }
  return 0;
}


int main(int argc, char *argv[]) {
  sigset_t mask, oldmask;
  sigemptyset(&mask);   
  sigaddset(&mask, SIGINT); 
  sigaddset(&mask, SIGQUIT);
  sigaddset(&mask, SIGTERM);
  sigaddset(&mask, SIGHUP);

  if (pthread_sigmask(SIG_BLOCK, &mask,&oldmask) != 0) {
    fprintf(stderr, "FATAL ERROR, pthread_sigmask\n");
    return EXIT_FAILURE;
  }

  // installo il signal handler per tutti i segnali che mi interessano
  struct sigaction sa;
  // resetto la struttura
  memset (&sa, 0, sizeof(sa));   
  sa.sa_handler = sighandler;
  int notused;
  SYSCALL_EXIT("sigaction", notused,sigaction(SIGINT,   &sa, NULL), "sigaction", "");
  SYSCALL_EXIT("sigaction", notused,sigaction(SIGQUIT,  &sa, NULL), "sigaction", "");
  SYSCALL_EXIT("sigaction", notused,sigaction(SIGTERM,  &sa, NULL), "sigaction", "");
  SYSCALL_EXIT("sigaction", notused,sigaction(SIGHUP,   &sa, NULL), "sigaction", "");
  
  int listenfd;
  SYSCALL_EXIT("socket", listenfd, socket(AF_UNIX, SOCK_STREAM, 0), "socket", "");
  
  struct sockaddr_un serv_addr;
  memset(&serv_addr, '0', sizeof(serv_addr));
  serv_addr.sun_family = AF_UNIX;    
  strncpy(serv_addr.sun_path, SOCKNAME, strlen(SOCKNAME)+1);
  
  SYSCALL_EXIT("bind", notused, bind(listenfd, (struct sockaddr*)&serv_addr,sizeof(serv_addr)), "bind", "");
  SYSCALL_EXIT("listen", notused, listen(listenfd, MAXBACKLOG), "listen", "");

  if (pthread_sigmask(SIG_SETMASK, &oldmask, NULL) != 0) {
    fprintf(stderr, "FATAL ERROR\n");
    return EXIT_FAILURE;
  }

  int r=0;
  while(!termina) {      
    long connfd=-1;	
    if (!termina && (connfd= accept(listenfd, (struct sockaddr*)NULL ,NULL)) == -1) {
      if (errno==EINTR) {
	if (termina) break;
      } else {
	perror("accept");
	r=EXIT_FAILURE;
      }
    }
    //printf("connection accepted\n");    	
    if (spawn_thread(connfd)<0) {
      r=EXIT_FAILURE;
      break;
    }
  }
  // clean-up
  unlink(SOCKNAME);
  return r;
}
