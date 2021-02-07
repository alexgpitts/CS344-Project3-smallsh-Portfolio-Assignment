
#include <math.h>


struct Command{
    char **array;
    int flag;
    int flag2; 
    int background; 
    int comment; 
    int newOut, newIn;
    int words; 
 
    char *output;//pointer to path for redirecting output
    char *input; //pointer to path for redirecting input
    
};

struct Command get_command(char line[2048]){
    struct Command command;
    command.flag = 0;
    command.flag2 = 0;
    command.comment = 0;
    command.words = 1;
    
    for(int i = 1; line[i]!='\0'; i++){
      if(line[i]=='>'){
        command.flag = i;
        
      }
      if(line[i]=='<'){
        command.flag2 = i;
        
      }
      if(line[i] == '&' && line[i+1] == '\0'){
        command.background = 1;
        command.words--;
      }

      else if(line[i]==' ' || line[i]=='\n' || line[i]=='\t'){
        command.words++;
      }
    }

    //printf("\n%d, %d, %d", flag, flag2, flag3);
    //allocate array to store correct number of words
    //printf("\n%d, %d, %d\n", flag, flag2, flag3);
    if(command.flag == 0 && command.flag2 == 0){
      //allocate array to store correct number of words
      command.array = malloc((command.words +1) * sizeof(char*));
      //printf("hit");

      //fill array
        command.array[0] = strtok (line," &");
        for(int w = 1; w < command.words; w++){
          command.array[w] = strtok (NULL," &");
        }
        command.array[command.words] = NULL;
        // for(int i = 0; i<command.words; i++){
        //   printf("%s\n", command.array[i]);
        // }
    }

    //if only redirecting stdout
    else if(command.flag !=0 && command.flag2==0){
      //printf("%d", words); 
      command.array = malloc((command.words - 1) * sizeof(char*));
      command.array[0] = strtok (line," ");
      //printf("token arg %d: %s\n", 0, array[0]);
      for(int w = 1; w < command.words - 2; w++){
        command.array[w] = strtok (NULL," ");
        //printf("token arg %d: %s\n", w, array[w]);
      }
      command.array[command.words-2] = NULL;
      command.output = strtok (NULL," >");
      //printf("output file: %s\n", output);
      //fflush(stdout);
    }


    //if only redirecting stdin
    else if(command.flag ==0 && command.flag2!=0){
      command.array = malloc((command.words - 1) * sizeof(char*));
      command.array[0] = strtok (line," ");
      for(int w = 1; w < command.words - 2; w++){
        command.array[w] = strtok (NULL," ");
      }
      command.array[command.words-2] = NULL;
      command.input = strtok (NULL," <");
      //printf("input: %s\n", input);
      //fflush(stdout);
    }

    //if redirecting both stdin and stdout
    else if(command.flag !=0 && command.flag2!=0){
      command.array = malloc((command.words - (command.words - 2)) * sizeof(char*));
      command.array[0] = strtok (line," ");
      for(int w = 1; w < (command.words - (command.words - 1)); w++){
        command.array[w] = strtok (NULL," ");
      }
      command.array[command.words-(command.words - 1)] = NULL;
      if(command.flag < command.flag2){
        command.output = strtok (NULL," <>");
        command.input = strtok (NULL," <>");

      }
      else{
        command.input = strtok (NULL," ><");
        command.output = strtok (NULL," <>");
      }
      //printf("arg1: %s\targ2: %s\n", array[0], array[1]);
      //printf("input: %s\toutput: %s\n", input, output);
      //fflush(stdout);
    }
    return command;
}

void child_logic(struct Command command){
  int newOut, newIn, result;
  // In the child process
  //printf("CHILD(%d) running ls command\n", getpid());
 
  
  //if its a comment
  if(command.array[0][0]=='#'){
    exit(0);
  }


  //if only redirecting stdout
  else if(command.flag !=0 && command.flag2==0){ 
    newOut = open (command.output, O_WRONLY | O_CREAT | O_TRUNC, 0777);
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

  //if only redirecting stdin
  else if(command.flag ==0 && command.flag2!=0){
    newIn = open(command.input, O_RDONLY);
    if(newIn == -1){
      perror("newIn open()");
      exit(1);
    }
    result = dup2(newIn, 0);
    if(result == -1){
      perror("newIn dub2()");
      exit(2);
    }
      

  }

  //if redirecting both stdin and stdout
  else if(command.flag !=0 && command.flag2!=0){

    //redirecting stdout
    newOut = open (command.output, O_WRONLY | O_CREAT | O_TRUNC, 0777);
    if(newOut == -1){
      perror("newOUt fopen()");
      exit(1);
    }
    result = dup2(newOut, 1);
    if(result == -1){
      perror("newOUt dub2()");
      exit(2);
    }

    //redirecting stdin
    newIn = open(command.input, O_RDONLY);
      if(newIn == -1){
      perror("newIn open()");
      exit(1);
    }
    result = dup2(newIn, 0);
      if(result == -1){
      perror("newIn dub2()");
      exit(2);
    }
        
  }
}