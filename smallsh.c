#include <sys/wait.h> // for waitpid
#include <stdio.h>    // for printf and perror
#include <stdlib.h>   // for exit
#include <unistd.h>   // for execv, getpid, fork
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <math.h>
#include <signal.h>
#include "smallsh.h"

int mode = 0; //for counting modes for signal handles
int killed =0; // to see if chiled was ctrl+c ended


void catchSIGTSTP(int signo){
  //print message
  if(mode == 1){
    char* message = "\nExiting foreground-only mode\n";
    write(STDOUT_FILENO, message, 30);
    mode = 0;
  }
  else if(mode == 0){
    char* message = "\nEntering foreground-only mode (& is now ignored)\n";
    write(STDOUT_FILENO, message, 50);
    mode = 1;
  }
  
  char* message3 = ":";
  write(STDOUT_FILENO, message3, 1);
  
}



/*
The following program forks a child process. The child process then replaces the program using execv to run "/bin/ls". The parent process waits for the child process to terminate.
*/
int main(){
  struct sigaction SIGINT_action = {0}, SIGTSTP_action = {0}; 
  SIGINT_action.sa_handler = SIG_IGN; 
  SIGINT_action.sa_flags = SA_RESTART;
  sigfillset(&SIGINT_action.sa_mask);
  sigaction(SIGINT, &SIGINT_action, NULL);

  SIGTSTP_action.sa_handler = catchSIGTSTP;
  SIGTSTP_action.sa_flags = SA_RESTART;
  sigfillset(&SIGTSTP_action.sa_mask);
  sigaction(SIGTSTP, &SIGTSTP_action, NULL);


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
  int z= 0;

  int processes[100];
  for(i = 0; i<100; i++){
    processes[i] = 0; 
  }
  i = 0; 
  

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

    // while loop to copy all characters to a new array, inserting proccess id wherever there is a "$$"
    while(line1[i]!='\0'){ 
      if(line1[i]=='$' && line1[i+1]== '$'){
        while(ID[j]!='\0'){
          line[k] = ID[j];
          //printf("%c", ID[j]);
          j++;
          k++;
        }
        i+=2;
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

    if(result > 0){
      command = get_command(line);
    }

    // if user enters exit, exit shell
    if(strcmp(command.array[0], "exit") == 0){

      _exit(0);
    }

    //if user is changing directer of shell
    else if(strcmp(command.array[0], "cd")==0){
      if(command.array[1] == NULL){
        chdir(getenv("HOME"));
      }
      else{
        chdir(command.array[1]);
      }
      
    }
  
    //if user is getting status from last child process
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

          //if we are in forground only mode, ignore the &
          if(mode == 1){
            command.background = 0;
          }

          if(command.background == 0){
            SIGINT_action.sa_handler = SIG_DFL; 
            SIGINT_action.sa_flags = SA_RESTART;
            sigfillset(&SIGINT_action.sa_mask);
            sigaction(SIGINT, &SIGINT_action, NULL);
          }
          
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
          
          if(command.background == 1 && mode == 0){//if background and not in forground only mode, dont wait and add it to array
            while(processes[i] != 0){
              i++;
            }
            printf("background pid is %d\n", spawnPid);
            processes[i]=spawnPid;
            i=0;
          }
          else{// Wait for child's termination
            result = waitpid(spawnPid, &childStatus, 0);
            if(!WIFEXITED(childStatus)){
              printf("\nterminated by signal %d\n", WTERMSIG(childStatus));
            }
          }
            
          while(i < 100){
            if(processes[i]!=0){
              result = waitpid(processes[i], &childStatus, WNOHANG);
              // printf("\n%d\n", result); 
              if (result > 0) {
                printf("background pid %d is done: terminated by signal %d\n", processes[i], WTERMSIG(childStatus));
                waitpid(processes[i], &childStatus, 0);
                processes[i] = 0;
              } 
              else if (result < 0) {
                perror("waitpid");
              }
            }
            i++; 
          }
          i=0; 
          
          
          
          break;
      }
    }
    
  } 
  
  exit(0);
}