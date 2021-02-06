#include <sys/wait.h> // for waitpid
#include <stdio.h>    // for printf and perror
#include <stdlib.h>   // for exit
#include <unistd.h>   // for execv, getpid, fork
#include <unistd.h>
#include <string.h>
#include <fcntl.h>


/*
The following program forks a child process. The child process then replaces the program using execv to run "/bin/ls". The parent process waits for the child process to terminate.
*/
int main(){
  int words = 1;
  char line[1028];
  int flag, flag2, flag3, newOut, temp, result, childStatus;  

  //loop for shell
  while(1){
    char **array;//pointer to array
    char *output;//pointer to path for redirecting output
    char *input; //pointer to path for redirecting input
    
    //this while loop was inspired by 
    while(result < 1){
      flag = 0;
      flag2 = 0;
      flag3 = 0;
      printf(": ");
      result = scanf("%1028[^\n]",line);
      getchar();
      
      if(result > 0){
        //for loop to walk to end of line and counting words
        for(int i = 0; line[i]!='\0'; i++){
          if(line[i]=='>' && line[i-1]==' ' && line[i+1]==' '){
            flag = i;
            
          }
          if(line[i]=='<' && line[i-1]==' ' && line[i+1]==' '){
            flag2 = i;
            
          }
          if(line[i]=='&'){
            flag3 = i;
            
          }

          else if(line[i]==' ' || line[i]=='\n' || line[i]=='\t'){
            words++;
          }
        }
        //printf("\n%d, %d, %d", flag, flag2, flag3);
        //allocate array to store correct number of words
        //printf("\n%d, %d, %d\n", flag, flag2, flag3);
        if(flag == 0 && flag2 == 0 && flag3 == 0){
          array = malloc(words * sizeof(char*));
          //printf("hit");
            array[0] = strtok (line," ");
            for(int w = 1; w < words; w++){
              array[w] = strtok (NULL," ");
            }
        }

        else if(flag !=0 && flag2==0 && flag3==0){
          printf("%d", words); 
          array = malloc((words - 2) * sizeof(char*));
          array[0] = strtok (line," ");
          for(int w = 1; w < 2; w++){
            array[w] = strtok (NULL," >");
          }
          // array[2] = "NULL";
          output = strtok (NULL," >\0");
          //printf("%s", output);
          fflush(stdout);
        }

        else if(flag ==0 && flag2!=0 && flag3==0){
          array = malloc((words - 2) * sizeof(char*));
          array[0] = strtok (line," ");
          for(int w = 1; w < 2; w++){
            array[w] = strtok (NULL," >");
          }
          // array[2] = "NULL";
          output = strtok (NULL," >\0");
          //printf("%s", output);
          fflush(stdout);
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
        
        if(flag !=0 && flag2==0 && flag3==0){ 
          newOut = open (output, O_WRONLY | O_CREAT | O_TRUNC, 0664);
          if(newOut == -1){
            perror("newOUt fopen()");
            exit(1);
          }
          result = dup2(newOut, 1);
          if(result == -1){
            perror("newOUt dub2()");
            exit(2);
          }
        
        }

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