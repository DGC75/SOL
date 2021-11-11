#include <unistd.h>
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <stdlib.h>
#include <pthread.h>
#include <ctype.h>

#include <util.h>
#include <conn.h>

// versione 1 thread per connessione (thread lanciati in modalita' detached)


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

// converte tutti i carattere minuscoli in maiuscoli
void toup(char *str) {
    char *p = str;
    while(*p != '\0') { 
        *p = (islower(*p)?toupper(*p):*p); 
	++p;
    }        
}

void *threadF(void *arg) {
    assert(arg);
    long connfd = (long)arg;
    do {
	msg_t str;
	int n;
	SYSCALL_EXIT("readn", n, readn(connfd, &str.len, sizeof(int)), "read1", "");
	if (n==0) break;
	str.str = calloc((str.len), sizeof(char));
	if (!str.str) {
	    perror("calloc");
	    fprintf(stderr, "Memoria esaurita....\n");
	    break;
	}		    
	SYSCALL_EXIT("readn", n, readn(connfd, str.str, str.len * sizeof(char)), "read2", "");

	toup(str.str);

	SYSCALL_EXIT("writen", n, writen(connfd, &str.len, sizeof(int)), "write", "");
	SYSCALL_EXIT("writen", n, writen(connfd, str.str, str.len*sizeof(char)), "write", "");
	free(str.str);
    } while(1);
    close(connfd);	    
    return NULL;
}

void spawn_thread(long connfd) {
    pthread_attr_t thattr;
    pthread_t thid;

    if (pthread_attr_init(&thattr) != 0) {
	fprintf(stderr, "pthread_attr_init FALLITA\n");
	close(connfd);
	return;
    }
    // settiamo il thread in modalità detached
    if (pthread_attr_setdetachstate(&thattr,PTHREAD_CREATE_DETACHED) != 0) {
	fprintf(stderr, "pthread_attr_setdetachstate FALLITA\n");
	pthread_attr_destroy(&thattr);
	close(connfd);
	return;
    }
    if (pthread_create(&thid, &thattr, threadF, (void*)connfd) != 0) {
	fprintf(stderr, "pthread_create FALLITA");
	pthread_attr_destroy(&thattr);
	close(connfd);
	return;
    }
}


int main(int argc, char *argv[]) {
    cleanup();    
    atexit(cleanup);    

    int listenfd;
    SYSCALL_EXIT("socket", listenfd, socket(AF_UNIX, SOCK_STREAM, 0), "socket", "");

    struct sockaddr_un serv_addr;
    memset(&serv_addr, '0', sizeof(serv_addr));
    serv_addr.sun_family = AF_UNIX;    
    strncpy(serv_addr.sun_path, SOCKNAME, strlen(SOCKNAME)+1);

    int notused;
    SYSCALL_EXIT("bind", notused, bind(listenfd, (struct sockaddr*)&serv_addr,sizeof(serv_addr)), "bind", "");
    SYSCALL_EXIT("listen", notused, listen(listenfd, MAXBACKLOG), "listen", "");

    while(1) {      
	long connfd;
	SYSCALL_EXIT("accept", connfd, accept(listenfd, (struct sockaddr*)NULL ,NULL), "accept", "");
	//printf("connection accepted\n");    	
	spawn_thread(connfd);
    }
    return 0;
}
