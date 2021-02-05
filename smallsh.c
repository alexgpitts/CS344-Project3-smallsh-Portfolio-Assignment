#include <sys/wait.h> // for waitpid
#include <stdio.h>    // for printf and perror
#include <stdlib.h>   // for exit
#include <unistd.h>   // for execv, getpid, fork
#include <unistd.h>
#include <string.h>

/*
The following program forks a child process. The child process then replaces the program using execv to run "/bin/ls". The parent process waits for the child process to terminate.
*/
int main(){
  int childStatus;
  int temp, result;
  int words = 1;
  char line[1028];
  
  //loop for shell
  while(1){
    char **array;//pointer to array
    
    while(result < 1){
      printf(": ");
      result = scanf("%1028[^\n]",line);
      getchar();
      if(result > 0){

        for(int i = 0; line[i]!='\0'; i++){
            if(line[i]==' ' || line[i]=='\n' || line[i]=='\t'){
                    words++;
            }
        }

        array = malloc(words * sizeof(char*));

        array[0] = strtok (line," ");
        for(int w = 1; w < words; w++){
            array[w] = strtok (NULL," ");
        }
      }
    }
    result = 0; 

    // Fork a new process
    pid_t spawnPid = fork();

    switch(spawnPid){
      case -1:
        perror("fork()\n");
        exit(1);
        break;
      case 0:
        // In the child process
        //printf("CHILD(%d) running ls command\n", getpid());
        // Replace the current program with user entered command line
        execvp(array[0], array);
        // exec only returns if there is an error
        perror("execve");
        exit(2);
        break;
      default:
        // In the parent process
        // Wait for child's termination
        spawnPid = waitpid(spawnPid, &childStatus, 0);
        //printf("PARENT(%d): child(%d) terminated. Exiting\n", getpid(), spawnPid);
        
        break;
    }
    
  } 
  exit(0);
}