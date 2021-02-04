#include <sys/wait.h> // for waitpid
#include <stdio.h>    // for printf and perror
#include <stdlib.h>   // for exit
#include <unistd.h>   // for execv, getpid, fork
#include <unistd.h>
#include <string.h>

/*
The following program forks a child process. The child process then replaces the program using execv to run "/bin/ls". The parent process waits for the child process to terminate.
*/
int main (void){

    char line[1000];
    char newargv[512][512];

    while (1){
                   
        printf(":");
        fgets(line, sizeof line, stdin);
        strcpy(newargv[0], strtok(line, " "));
        strcpy(newargv[1], strtok(NULL, " "));    
        
        // printf("%s", line);

        for(int i = 0; i<2; i++){
            printf("%s\n", newargv[i]);
        }
    }

    return 0;
}