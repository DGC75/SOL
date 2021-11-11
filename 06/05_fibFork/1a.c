#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

/* 
 * Calcola ricorsivamente il numero di Fibonacci dell'argomento 'n'.
 * La soluzione deve effettuare fork di processi con il vincolo che 
 * ogni processo esegua 'doFib' al più una volta.  
 * Se l'argomento doPrint e' 1 allora la funzione stampa il numero calcolato 
 * prima di passarlo al processo padre. 
 */
static void doFib(int n, int doPrint){
    //C'E' LA SOLUZIONE BANALE CON 2 FORK, MA NEANCHE STO A SCRIVERLA
    //VEDIAMO SE RIESCO A FARLA CON UNA SOLA FORK E QUALCHE TRUCCO DI MAGIA...

    //SUPPONGO NON SIA PROPRIO POSSIBILE FARLA CON UNA FORK E BASTA,
    //A MENO DI NON MODIFICARE LA SIGNATURE DELLA FUNZIONE
    //(SEPPUR IN CODA, SFRUTTA DUE PARAMETRI)

    //SE POI AVESSI VOLUTO FARLA CON UNA SOLA FORK PER PROCESSO
    //AVREI POTUTO USARE UN WHILE CONTINUE ALL'INTERNO DELLA FORK
    //O QUALCHE ALTRA PORCATA SIMILE PER OTTENERE UN'ITERATIVA MASCHERATA DA RICORSIVA(TRA PROCESSI!)

    //SCRIVO LA SOL BANALE E POI GUARDO LA SOLUZIONE DEL PROF.
    
    //LA SOLUZIONE DEL PROF E' QUELLA BANALE... ''-_-
    int r;
    int s;

    if(n == 1 || n == 2)
    {
        if(doPrint)
            printf("m:%d\n", 1);

        exit(1);
    }

    if(n > 2)
    {
        pid_t pid_left, pid_right;

        if((pid_left = fork()) != 0){
            int status_left;
            pid_left = waitpid(pid_left, &status_left, 0);

            r= WEXITSTATUS(status_left);
        }
        else{
            doFib(n-1, 0);
        }

        if((pid_right = fork()) != 0){
            int status_right;
            pid_right = waitpid(pid_right, &status_right, 0);

            s= WEXITSTATUS(status_right);
        }
        else{
            doFib(n-2, 0);
        }

        int m = r + s;
        if(doPrint)
            printf("m:%d\n", m);

        exit(m);

    }

}

int main(int argc, char *argv[]) {
    // questo programma puo' calcolare i numeri di Fibonacci solo fino a 13.  
    const int NMAX=13;
    int arg;
    
    if(argc != 2){
	fprintf(stderr, "Usage: %s <num>\n", argv[0]);
	return EXIT_FAILURE;
    }
    arg = atoi(argv[1]);
    if(arg <= 0 || arg > NMAX){
	fprintf(stderr, "num deve essere compreso tra 1 e 13\n");
	return EXIT_FAILURE;
    }   
    doFib(arg, 1);
    return 0;
}