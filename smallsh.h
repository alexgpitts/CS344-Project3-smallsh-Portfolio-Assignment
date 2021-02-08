
#include <math.h>

struct Command{
    char **array; //to hold what user entered in prompt seperated by prompt
    int flag; //flag for redirecting stdout (detect a >)
    int flag2; //flag for redirection stdin (detect a <)
    int background; //flag for if background (detect an '&' at the end of line)
    int comment; //flag for if its a comment (detect #at biggining)
    int newOut, newIn; //file descritors for new out and in files
    int words; //word counter for how many arguments user entered
 
    char *output;//pointer to path for redirecting output
    char *input; //pointer to path for redirecting input
    
};
//make struct object based on information entered in prompt by user
struct Command get_command(char line[2048]){

    //make struct object and initialize default values
    struct Command command;
    command.background = 0;
    command.flag = 0;
    command.flag2 = 0;
    command.comment = 0;
    command.words = 1;
    
    //walk through the user entered command line and detect special caracters and apply proper flags
    //also keep track of number of words
    for(int i = 1; line[i]!='\0'; i++){
      if(line[i]=='>'){
        command.flag = i;
        
      }
      if(line[i]=='<'){
        command.flag2 = i;
        
      }
      if(line[i] == '&' && line[i+1] == '\0'){
        command.background = 1;
        command.words--;//subtract 1 from words to just remove the '&' at end
      }

      else if(line[i]==' ' || line[i]=='\n' || line[i]=='\t'){
        command.words++;
      }
    }


    ///////////////allocate array to store correct number of words///////////////////

    //if not redirecting anything
    if(command.flag == 0 && command.flag2 == 0){
      //allocate size based on flags
      command.array = malloc((command.words +1) * sizeof(char*));
  
      //fill array
      command.array[0] = strtok (line," ");
      for(int w = 1; w < command.words; w++){
        command.array[w] = strtok (NULL," ");
      }
      command.array[command.words] = NULL; //set last array index to NULL for execvp()
    }

    //if only redirecting stdout
    else if(command.flag !=0 && command.flag2==0){
      //allocate size based on flags
      command.array = malloc((command.words - 1) * sizeof(char*));

      //fill array
      command.array[0] = strtok (line," ");
      for(int w = 1; w < command.words - 2; w++){
        command.array[w] = strtok (NULL," ");
      }
      command.array[command.words-2] = NULL;//set last element to NULL for execv()
      command.output = strtok (NULL," >"); //store output file name in output attribute of command object
    }


    //if only redirecting stdin
    else if(command.flag ==0 && command.flag2!=0){
      //allocate size based on flags
      command.array = malloc((command.words - 1) * sizeof(char*));

      //fill array
      command.array[0] = strtok (line," ");
      for(int w = 1; w < command.words - 2; w++){
        command.array[w] = strtok (NULL," ");
      }
      command.array[command.words-2] = NULL;//set last element to NULL for execv()
      command.input = strtok (NULL," <");//store input file name in input attribute of command object
    }

    //if redirecting both stdin and stdout
    else if(command.flag !=0 && command.flag2!=0){
      //allocate size based on flags
      command.array = malloc((command.words - (command.words - 2)) * sizeof(char*));

      //fill array
      command.array[0] = strtok (line," ");
      for(int w = 1; w < (command.words - (command.words - 1)); w++){
        command.array[w] = strtok (NULL," ");
      }
      command.array[command.words-(command.words - 1)] = NULL;//set last element to NULL for execv()

      //detect if < comes before > or vice versa
      if(command.flag < command.flag2){
        command.output = strtok (NULL," <>");//store output file name in output attribute of command object
        command.input = strtok (NULL," <>");//store input file name in input attribute of command object

      }
      else{
        command.input = strtok (NULL," ><");///store input file name in input attribute of command object
        command.output = strtok (NULL," <>");//store output file name in output attribute of command object
      }
    }
    return command; //return the command object
}



////////////////////////////parnet logic ^^^^^^^^^^^^//////////////////


/////////////////////////////child function vvvvv//////////////////////



//child logic decides how we redirect things using the C api depending on flags
void child_logic(struct Command command){
  int newOut, newIn, result, dev_null; //file desciptors
  
  //if its a comment just exit (detects if user entered # at the biginning)
  //I guess this is kind of redundant because im not useing my "comment" flag in the struct.
  if(command.array[0][0]=='#'){
    exit(0);
  }

  //if its a non specifide io background task redirect stdin and stdout to /dev/null
  else if(command.flag == 0 && command.flag2 == 0 && command.background ==1){
    //create file discriptor for /dev/null for stdin (read only)
    dev_null = open("/dev/null", O_RDONLY);

    //if couldnt open, error and exit
    if(dev_null == -1){
      perror("dev_null fopen()");
      exit(1);
    }

    //set stdin in to /dev/null
    result = dup2(dev_null, 0);

    //if cant redirect std in, error and exit
    if(result == -1){
      perror("/dev/null dub2()");
      exit(2);
    }

    //create file discriptor for /dev/null for stdout (write only)
    dev_null = open("/dev/null", O_WRONLY);

    //if couldnt open, error and exit
    if(dev_null == -1){
      perror("dev_null fopen()");
      exit(1);
    }

    //set stdout in to /dev/null
    result = dup2(dev_null, 1);

    //if cant redirect std out, error and exit
    if(result == -1){
      perror("/dev/null dub2()");
      exit(2);
    }

  }

  //if only redirecting stdout
  else if(command.flag !=0 && command.flag2==0){ 
    //open file for new out for write only, creat if dont exits, and truncate if already exists. 
    newOut = open (command.output, O_WRONLY | O_CREAT | O_TRUNC, 0777);
    if(newOut == -1){
      perror("newOUt fopen()");
      exit(1);
    }
    
    //redirect stdout to new output 
    result = dup2(newOut, 1);
    if(result == -1){
      perror("newOUt dub2()");
      exit(2);
    }

    //if background command with unspecifid stdin, redirect stdin to /dev/null
    if(command.background == 1){
      dev_null = open("/dev/null", O_RDONLY);
      if(dev_null == -1){
        perror("dev_null fopen()");
        exit(1);
      }
      //set stdin to /dev/null
      result = dup2(dev_null, 0);
      if(result == -1){
        perror("< /dev/null dub2()");
        exit(2);
      }
    }
  }

  //if only redirecting stdin
  else if(command.flag ==0 && command.flag2!=0){
    //open file for read only for new stdin
    newIn = open(command.input, O_RDONLY);
    if(newIn == -1){
      perror("newIn open()");
      exit(1);
    }

    //redirect stdin to new file
    result = dup2(newIn, 0);
    if(result == -1){
      perror("newIn dub2()");
      exit(2);
    }

    //if background command with unspecifid stdout, set stdout to /dev/null
    if(command.background == 1){
      dev_null = open("/dev/null", O_WRONLY);
      if(dev_null == -1){
        perror("dev_null fopen()");
        exit(1);
      }
      //set stdout to /dev/null
      result = dup2(dev_null, 1);
      if(result == -1){
        perror("> /dev/null dub2()");
        exit(2);
      }
    }
      

  }

  //if redirecting both stdin and stdout
  else if(command.flag !=0 && command.flag2!=0){
    //open new file for new std out. Open for read only, create if dont exist, and truncate if already exists.
    newOut = open (command.output, O_WRONLY | O_CREAT | O_TRUNC, 0777);
    if(newOut == -1){
      perror("newOUt fopen()");
      exit(1);
    }

    //redirect std out to new file discriptor.
    result = dup2(newOut, 1);
    if(result == -1){
      perror("newOUt dub2()");
      exit(2);
    }

    //open file for read only for new stdin
    newIn = open(command.input, O_RDONLY);
      if(newIn == -1){
      perror("newIn open()");
      exit(1);
    }

    //redirecting stdin to new file discriptor.
    result = dup2(newIn, 0);
      if(result == -1){
      perror("newIn dub2()");
      exit(2);
    }
        
  }
}