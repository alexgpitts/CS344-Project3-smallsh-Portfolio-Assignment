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

//Author: Alex Pitts
//Date: 2/8/2021

//description: A simple shell that shares some of the basic functionality of bash


int mode = 0; //for counting modes for signal handles
int killed =0; // to see if chiled was ctrl+c ended


void catchSIGTSTP(int signo){
  //if we are in forground only mode when signal is caught, then print message and exit forground only mode with "mode" flag
  if(mode == 1){
    char* message = "\nExiting foreground-only mode\n";
    write(STDOUT_FILENO, message, 30);
    mode = 0;
  }

  //if we are not in forground only mode when signal is caught, then print message and enter forground only mode with "mode" flag
  else if(mode == 0){
    char* message = "\nEntering foreground-only mode (& is now ignored)\n";
    write(STDOUT_FILENO, message, 50);
    mode = 1;
  }
  //reprint prompt because when I caught signals it wasnt redisplaying it.
  char* message3 = ":";
  write(STDOUT_FILENO, message3, 1);
  
}



/*
The following program forks a child process. The child process then replaces the program using execv to run "/bin/ls". The parent process waits for the child process to terminate.
*/
int main(){
  struct sigaction SIGINT_action = {0}, SIGTSTP_action = {0}; //signal handler structs for ctr+c and ctr+z 
  SIGINT_action.sa_handler = SIG_IGN; //set ctr+c to ignore
  SIGINT_action.sa_flags = SA_RESTART; //set flag to restart after catch (dont know if I need this but I think lecture said it was good practice)
  sigfillset(&SIGINT_action.sa_mask); 
  sigaction(SIGINT, &SIGINT_action, NULL); //set action on SIGINT to behave like SIGINT action

  SIGTSTP_action.sa_handler = catchSIGTSTP; //set ctr+z to signal handler at top of this file
  SIGTSTP_action.sa_flags = SA_RESTART; //set flag to restart after each catch
  sigfillset(&SIGTSTP_action.sa_mask);
  sigaction(SIGTSTP, &SIGTSTP_action, NULL); //set sigaction for ctr+z to behave like SIGTSTP_action


  // char new_dir[256]; 
  int words = 1; //counter for number of words in my command line minus any redirections 
  char line1[2048] =""; //array for inital read in value
  char line[3000] = ""; //array that gets everything from "line1", but handles variabel expansion
  // int flag, flag2, flag3, newOut, newIn, temp, result; //initalizing all my ints 
  int result; //to hold return value from waitpid() in the parent part of my switch statement
  int childStatus = 0; //to hold child status. Set to zero initally for if no child gets ran and we call "status"

  //calculating length of PID and storing it in char ID[].
  int length = snprintf( NULL, 0, "%d", getpid()); 
  char* ID = malloc( length + 1 );
  snprintf( ID, length + 1, "%d", getpid());


  //counters
  int k = 0;
  int j =0; 
  int i =0;
  int z= 0;

  //initialize an array of background PIDs. 
  int processes[100];
  for(i = 0; i<100; i++){
    processes[i] = 0; 
  }
  i = 0; 
  
  //loop for shell
  while(1){
  
    setbuf(stdout, NULL); //recommended by someone from class. needed to clear my buffer to clean up my output
    setbuf(stdin, NULL);

    //create a command object from header file to hold all the information about the users command
    struct Command command; 
      
    //print prompt and get input from user  
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
    line[k] = '\0'; //set null terminator at end of our string
    k=0;
    i=0;

    //if the user entered something then do all the logic for storing information about command line in our command object
    if(result > 0){
      command = get_command(line);
    }

    /////////////////built in commands/////////////
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

    /////////////////end of built in commands/////////////
    
    // if we are not running a built in command, fork a new process
    else{
      pid_t spawnPid = fork(); //fork

      switch(spawnPid){
        case -1:
          //if fork failed for some reason
          perror("fork()\n");
          exit(1);
          break;

        case 0:
          // In the child process

          //if we are in forground only mode, ignore the &
          if(mode == 1){
            command.background = 0;
          }

          //if we are in forground, then stop ignoring ctr+c
          if(command.background == 0){
            //changing signal handler back to default for this child
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

          //if background and not in forground only mode, dont wait and add it to array of background PIDs
          if(command.background == 1 && mode == 0){

            //find empty spot in array
            while(processes[i] != 0){
              i++;
            }
            printf("background pid is %d\n", spawnPid); //print background pid to console
            processes[i]=spawnPid; //add PID to the found empty spot. Could add verification for if array is full, but I dont think we will have over 100 on OS1
            i=0;
          }
          //if forground proccess, wait for child's termination
          else{
            result = waitpid(spawnPid, &childStatus, 0);

            //if it was interupted by a signal, print that signal to user
            if(!WIFEXITED(childStatus)){
              printf("\nterminated by signal %d\n", WTERMSIG(childStatus));
            }
          }
            
          //every time we run loop, we want to check all the background PIDs and see if any are done
          //if they are done, we need to wait on them and print they are done to the user before next prompt  
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