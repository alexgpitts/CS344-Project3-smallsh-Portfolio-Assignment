#include <sys/wait.h> // for waitpid
#include <stdio.h>    // for printf and perror
#include <stdlib.h>   // for exit
#include <unistd.h>   // for execv, getpid, fork
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include "smallsh.h"


/*
The following program forks a child process. The child process then replaces the program using execv to run "/bin/ls". The parent process waits for the child process to terminate.
*/
int main(){
  char new_dir[256];
  int words = 1;
  char line[2048];
  int flag, flag2, flag3, newOut, newIn, temp, result;
  int childStatus = 0;  
  //struct Command command;
  //struct Command *commandptr; 
  //loop for shell
  while(1){
    setbuf(stdout, NULL); //recommended by someone from class. needed to clear my buffer to clean up my output
    struct Command command;
    while(result < 1){
      printf(": ");
      result = scanf("%2047[^\n]",line);
      getchar();
      
      if(result > 0){
        command = get_command(line);
      }
  
    }
    result = 0; 

    // if user enters exit, exit shell
    if(strcmp(command.array[0], "exit") == 0){
      exit(0);
    }
    else if(strcmp(command.array[0], "cd")==0){
      if(command.array[1] == NULL){
        chdir(getenv("HOME"));
      }
      else{
        chdir(command.array[1]);
      }
      
    }
  
    else if(strcmp(command.array[0], "status") == 0){
    
      if(childStatus == 0){
        printf("%d\n", 0);
      }
      else{
        printf("%d\n", WIFEXITED(childStatus));
      }
     
      
    }
    
    // Fork a new process
    else{
      pid_t spawnPid = fork();

      switch(spawnPid){
        case -1:
          perror("fork()\n");
          exit(1);
          break;
        case 0:
          // In the child process
          //printf("CHILD(%d) running ls command\n", getpid());

          //controls all the logic for a given command depending on special characters entered by user
          child_logic(command); 

          // Replace the current program with user entered command line
          execvp(command.array[0], command.array);
          
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
    
  } 
  
  exit(0);
}