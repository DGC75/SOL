    #include <stdio.h>
    #include <stdlib.h>
    #include <errno.h>
    #include <sys/types.h>
    #include <sys/wait.h>
    #include <limits.h>
    #include <unistd.h>
    #include <pwd.h>
    #include <string.h>

    #define BUF_SIZE 256    /*Max num of chars in a command line*/         
    #define MAXARG 32


    #define FALSE 0
    #define TRUE !(FALSE)

    #define ANSI_COLOR_GREEN   "\x1b[32m"
    #define ANSI_COLOR_MAGENTA "\x1b[35m"
    #define ANSI_COLOR_RESET   "\x1b[0m"


#define SECURE_ALLOC(charPtr,dim) \
        charPtr = calloc(dim, sizeof(char));\
        if(charPtr == NULL){\
            perror("calloc couldn't allocate");\
            exit(EXIT_FAILURE);\
        }

#define SECURE_FREE(charPtr) \
        free(charPtr);\
        charPtr = NULL; 



void init_shell();
void prompt_default_line();
int  read_cmd_line(int *argc_ptr, char** argv, int max_arg);
int  cmdexit(int argc, char** argv);
void execute(int argc, char** argv);

//LA STATIC SERVE A RENDERE 'PRIVATA' LA FUNZIONE:
//E' VISIBILE SOLO NEI FILE DOVE E' PRESENTE LA DICHIARAZIONE DELLA FUNZIONE?
//A LIVELLO DI COMPILAZIONE, LA FUNZIONE NON VIENE MESSA NELLA TABELLA DEI SIMBOLI ESPORTATI
static void print_status(pid_t pid,int status);


int main (){
    
    char *argv[MAXARG];
    int argc;

    init_shell();

    while (TRUE){
        prompt_default_line();
        if(read_cmd_line(&argc, argv, MAXARG)!= -1){
            if(cmdexit(argc, argv))
                exit(EXIT_SUCCESS);
            
            execute(argc, argv);
        }   
        else{
            fprintf(stderr, "invalid command!\n");
        }
    }
    return 0;
}

void init_shell(){
    //NOTHING TO INIT
}

void prompt_default_line(){
    //"{hostname}@{user_owner_of_the_process}:{Absolute Path of the process} $"

    //get process owner name
    uid_t id = geteuid();
    struct passwd *pwd = getpwuid(id);
    //get hostname
    char hostname[HOST_NAME_MAX + 1];
    gethostname(hostname, HOST_NAME_MAX + 1);
    //get path relative to ~
    //Ottieni il realpath
        //Se la stringa inizia con /home/g/
            //Sostituisci '/home/{user_owner_of_the_process}' con '~' 

    char *path = realpath(".", NULL);

    if(path == NULL){
        perror("cannot get realpath:");
    }


    int str_size = strlen(pwd->pw_name)+5;    
    char *tilde = calloc( str_size + 1, sizeof(char));

    if(tilde == NULL){
        perror("calloc didn't allocate");
        exit(EXIT_FAILURE);
    }

    strcpy(tilde, "/home/");
    strncat(tilde, pwd->pw_name, strlen(pwd->pw_name));
    if(strncmp(tilde, path, strlen(tilde)-1) == 0){
        memmove(path + 2, ( path + strlen(tilde) + 1), strlen(path) - strlen(tilde));
        memcpy(path, "~/", 2);
    }

    printf(ANSI_COLOR_GREEN "%s@%s" ANSI_COLOR_RESET, pwd->pw_name ,hostname, path);
    printf(":");
    printf(ANSI_COLOR_MAGENTA "%s" ANSI_COLOR_RESET, path);
    printf("$ ");
    free(path);

}

int  read_cmd_line(int *argc_ptr, char** argv, int max_arg){
    //PARSARE TUTTO QUELLO CHE E' STATO PASSATO ALLO STDIN
    (*argc_ptr) = 0;
    int relocation_times = 1;
    char *buf;
    int buf_size = BUF_SIZE;
    SECURE_ALLOC(buf, buf_size);

    char c;
    int buf_pos = 0;
    while(TRUE){
        c = getchar();


        if(c== EOF || c == '\n'){
            buf[buf_pos] = '\0';
            //PARSE STRING, THEN RETURN
            int i = 0;
            char * token = strtok(buf, " ");
            while(token != NULL){
                argv[i++] = token;
                token = strtok(NULL, " ");
                if(i >= max_arg){
                    printf("too many args\n");
                    return -1;
                }
            }
            *argc_ptr = i;
            return 1;
        }else{
            buf[buf_pos] = c;
        }

        buf_pos++;

        if(buf_pos >= buf_size){
            buf_size = (++relocation_times)*BUF_SIZE;
            if(realloc(buf, buf_size) == NULL){
                perror("realloc err:");
                exit(EXIT_FAILURE);
            }
            
        }
    }


    SECURE_FREE(buf);
}

int  cmdexit(int argc, char** argv){
    if(argc == 1 && (strcmp(argv[0], "exit")== 0))
        return 1;
    
    return 0;
}

void execute(int argc, char** argv){
    //PER MOTIVI TECNICI, LA cd DEV'ESSERE IMPLEMENTATA COME BUILTIN
    argv[argc] = NULL;

    pid_t pid;
    int status;
    switch(pid = fork()){
        case -1:
        perror("cannot fork:");
        break;
        case 0:
        execvp(argv[0], argv);
        perror("cannot execvp:");
        exit(EXIT_FAILURE);
        break;
        default:
        if(waitpid(pid, &status, 0) == -1){
            perror("waitpid_err:");
            exit(EXIT_FAILURE);
        }
        print_status(pid, status);
    }    
}

static void print_status(pid_t pid,int status){
    if(pid != 0) printf("Process %d\n", pid);

    if(WIFEXITED(status)) printf("Exit value:%d\n", WEXITSTATUS(status));

    if(WIFSIGNALED(status)) printf("Killed by signal:%d\n", WTERMSIG(status));

    if(__WCOREDUMP(status)) puts("-- core dumped\n");

    if(WIFSTOPPED(status)) puts("stopped");

}