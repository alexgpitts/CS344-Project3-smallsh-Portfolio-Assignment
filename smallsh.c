#include <sys/wait.h> // for waitpid
#include <stdio.h>    // for printf and perror
#include <stdlib.h>   // for exit
#include <unistd.h>   // for execv, getpid, fork
#include <unistd.h>

/*
The following program forks a child process. The child process then replaces the program using execv to run "/bin/ls". The parent process waits for the child process to terminate.
*/
int main(){
  char line[2048];
  char newargv[512][512];
  char word[1024];
  int childStatus;
  int temp;
  while(1){
    
    printf(":");
    fgets(line, sizeof line, stdin);
    strcpy(newargv[0], strtok(line, " "));
    // for(int i = 1; i<strlen(line); i++){
    //   strcpy(newargv[i], strtok(NULL, " "));
    // }
    printf("\n%s\n", newargv[0]);

    // Fork a new process
    pid_t spawnPid = fork();

    switch(spawnPid){
      case -1:
        perror("fork()\n");
        exit(1);
        break;
      case 0:
        // In the child process
        printf("CHILD(%d) running ls command\n", getpid());
        // Replace the current program with "/bin/ls"
        execvp(newargv[0], newargv);
        // exec only returns if there is an error
        perror("execve");
        exit(2);
        break;
      default:
        // In the parent process
        // Wait for child's termination
        spawnPid = waitpid(spawnPid, &childStatus, 0);
        printf("PARENT(%d): child(%d) terminated. Exiting\n", getpid(), spawnPid);
        
        break;
    }
  } 
  exit(0);
}