#include <unistd.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>

int main(int argc, char *argv[]) {
    if (argc != 2) {
	fprintf(stderr, "use: %s sleeptime\n", argv[0]);
	return -1;
    }
    int pid = fork();
    if (pid == 0) {                 // primo figlio 
		if ((pid = fork())==0) {    // secondo figlio
			execl("/bin/sleep", "/bin/sleep", argv[1], NULL);
			perror("execl");
			exit(EXIT_FAILURE);
		}
		if (waitpid(pid, NULL, 0)<0) {
			fprintf(stderr, "error waiting pid\n");
			return -1;
		}
		// Il pid del padre stampato con la print seguente e' il pid del
		// processo 'init' (nel sistemi recenti non e' init bensi systemd),
		// perche' il main esce immediatamente ed il primo figlio diventa
		// orfano e viene adottato dal processo init (systemd) che
		// periodicamente libera le risorse di sistema occupate dai
		// processi orfani che terminano
		printf("mio = %d, padre = %d\n", getpid(), getppid());
		
		return 0;
    }
    
    return 0;
}
