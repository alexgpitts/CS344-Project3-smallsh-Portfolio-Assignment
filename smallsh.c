#include <sys/wait.h> // for waitpid
#include <stdio.h>    // for printf and perror
#include <stdlib.h>   // for exit
#include <unistd.h>   // for execv, getpid, fork
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <math.h>
#include "smallsh.h"



/*
The following program forks a child process. The child process then replaces the program using execv to run "/bin/ls". The parent process waits for the child process to terminate.
*/
int main(){
  char new_dir[256];
  int words = 1;
  char line1[2048] ="";
  char line[3000] = "";
  int flag, flag2, flag3, newOut, newIn, temp, result;
  int childStatus = 0;  
  int length = snprintf( NULL, 0, "%d", getpid());
  char* ID = malloc( length + 1 );
  snprintf( ID, length + 1, "%d", getpid());
  int k = 0;
  int j =0; 
  int i =0;
  

  //struct Command command;
  //struct Command *commandptr; 
  //loop for shell
  while(1){
    setbuf(stdout, NULL); //recommended by someone from class. needed to clear my buffer to clean up my output
    setbuf(stdin, NULL);
    struct Command command;

    
      
    printf(":");
    result = scanf("%2047[^\n]",line1);
    getchar();
    while(line1[i]!='\0'){
      //printf("%c\n", line1[i]);
      if(line1[i]=='$' && line1[i+1]== '$'){
        
        while(ID[j]!='\0'){
          line[k] = ID[j];
          //printf("%c", ID[j]);
          j++;
          k++;
        }
        i+=3;
        j =0;
      }

      else{   
        line[k] = line1[i];
        i++;
        k++;  
        
      }

    }
    line[k] = '\0';
    k=0;
    i=0;
      
    
  
    // while(line[i]!='\0'){
    //   printf("%c", line[i]);
    //   i++;
    // }

    if(result > 0){
      command = get_command(line);
    }
  
  

    // if user enters exit, exit shell
    if(strcmp(command.array[0], "exit") == 0){
      _exit(0);
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
        printf("exit code: %d\n", 0);
      }
      else{
        printf("exit code: %d\n", WIFEXITED(childStatus));
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
          if(command.background == 1){
            
          }
          else{
            spawnPid = waitpid(spawnPid, &childStatus, 0);
          }
         
          
          
          //printf("PARENT(%d): child(%d) terminated. Exiting\n", getpid(), spawnPid);
          break;
      }
    }
    
  } 
  
  exit(0);
}