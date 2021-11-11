/* 
 * Client per la versione 2 del server (bcserver_v2.c)
 * Il tipo del messaggio scambiato tra client e server
 * e' diverso rispetto all'altra versione.
 */
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <util.h>
#include <conn.h>

int main(int argc, char *argv[]) {
    struct sockaddr_un serv_addr;
    int sockfd;
    SYSCALL_EXIT("socket", sockfd, socket(AF_UNIX, SOCK_STREAM, 0), "socket","");
    memset(&serv_addr, '0', sizeof(serv_addr));

    serv_addr.sun_family = AF_UNIX;    
    strncpy(serv_addr.sun_path,SOCKNAME, strlen(SOCKNAME)+1);

    int notused, n;
    SYSCALL_EXIT("connect", notused, connect(sockfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)), "connect","");
    
    char buf[BUFSIZE];     
    do {
	memset(buf, '\0', BUFSIZE);
	if (fgets(buf, BUFSIZE-1, stdin) == NULL) break;
	if (strncmp(buf, "quit", 4)== 0) break;
	
	SYSCALL_EXIT("writen 1", notused, writen(sockfd, buf, strlen(buf)), "writen", "");
	SYSCALL_EXIT("read", n, read(sockfd, buf, BUFSIZE), "readn", "");
	buf[n] = '\0';

	printf("result: %s\n", buf);	
    } while(1);    
    close(sockfd);
    return 0;
}
