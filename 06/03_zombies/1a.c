#include <unistd.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>

int main(int argc, char *argv[]) {

	int status;
	pid_t mainPid = getpid();
	pid_t childPid = 0;
	printf("The main is %d\n", mainPid);
	int i;
	int n = 10;
	int *arr = calloc(n, sizeof(int));
	//CHIAMA n PROCESSI FIGLI E SALVA I PID IN UN ARRAY
	//DOPODICHE' ASPETTA k SECONDI
	//CHIAMA LA WAITPID PER OGNI Pid NELL'ARRAY
	for(i = 0; i < n; i++){
		if(getpid() == mainPid){
			arr[i]  = fork();
			if(arr[i] == 0)
				exit(17);
		}
		else
			break;
	}



	if(getpid() == mainPid){
		printf("Aspetto per %d secondi...\n", n);
		sleep(n);
		for (i = 0; i < n; i++)
		{
			waitpid(arr[i], &status, 0);

			if(WIFEXITED(status))
				printf("proc %d e' uscito con codice %d\n", arr[i], WEXITSTATUS(status));	
		}
		
	}

	free(arr);
	return 0;
}