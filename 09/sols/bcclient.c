/* 
 * Client per la versione 1 del server (bcserver.c)
 * Il server risponde con un messaggio che contiene
 * lunghezza della stringa e la stringa di risposta.
 */
#include <unistd.h>
#include <stdio.h>
#include <string.h>
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

int main(int argc, char *argv[]) {
    struct sockaddr_un serv_addr;
    int sockfd;
    SYSCALL_EXIT("socket", sockfd, socket(AF_UNIX, SOCK_STREAM, 0), "socket","");
    memset(&serv_addr, '0', sizeof(serv_addr));

    serv_addr.sun_family = AF_UNIX;    
    strncpy(serv_addr.sun_path,SOCKNAME, strlen(SOCKNAME)+1);

    int notused;
    SYSCALL_EXIT("connect", notused, connect(sockfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)), "connect","");

    msg_t str;
    CHECKNULL(str.str,malloc(BUFSIZE), "malloc");
    do {
	char buffer[BUFSIZE]; 
	memset(str.str, '\0', BUFSIZE);
	if (fgets(str.str, BUFSIZE-1, stdin) == NULL) break;
	if (strncmp(str.str, "quit", 4)== 0) break;
	str.len = strlen(str.str);
	
	SYSCALL_EXIT("writen 1", notused, writen(sockfd, &str.len, sizeof(int)), "writen", "");
	SYSCALL_EXIT("writen 2", notused, writen(sockfd, str.str, str.len), "writen", ""); 
	int n, notused;

#if 1
	SYSCALL_EXIT("readn 1", notused, readn(sockfd, &n, sizeof(int)), "read", "");
	SYSCALL_EXIT("readn 2", notused, readn(sockfd, buffer, n), "read", "");  
#else
	// versione che fa una sola chiamata di sistema (man 2 readv)
	struct iovec iov[2];
	iov[0].iov_base = &n; iov[0].iov_len = sizeof(int);
	iov[1].iov_base = buffer; iov[1].iov_len =  BUFSIZE;

	// NOTA: qui andrebbe implementata una readvn sempre per evitare
	//       le "short reads". L'implementazione è un po' piu'
	//       complessa della readn.
	SYSCALL_EXIT("readv", notused, readv(sockfd,  iov, 2), "readv", "");

#endif
	buffer[n] = '\0';

	printf("result: %s\n", buffer);
	
    } while(1);
    
    close(sockfd);
    return 0;
}
