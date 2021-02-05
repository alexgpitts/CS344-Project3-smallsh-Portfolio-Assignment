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

    int words = 1;
    char buffer[128];
    int result = scanf("%127[^\n]",buffer);

    if(result > 0){
        char **array;

        for(int i = 0; buffer[i]!='\0'; i++){
            if(buffer[i]==' ' || buffer[i]=='\n' || buffer[i]=='\t'){
                    words++;
            }
        }

        array = malloc(words * sizeof(char*));

        // Using RoadRunner suggestion
        array[0] = strtok (buffer," ");
        for(int w = 1; w < words; w++){
            array[w] = strtok (NULL," ");
        }
        // for(int i = 0; i < sizeof(array)-1; i++){
        //     printf("%s\n", array[i]);
        // }
    }

    return 0;
}