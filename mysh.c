#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <errno.h>
#include <ctype.h>
#include <sys/stat.h>
#include <stdbool.h>

#define MAX_ARGS 1000
// All the tokens we care about: [|,>,<,and, or, {any file/command name}]
typedef struct {
  char *argv[MAX_ARGS]; // argument list
  char *input_file;
  char *output_file;
  bool is_builtin;
  bool has_pipe;
  struct Command *pipe_to;
} Command;

typedef struct {
  char** tokens;
  int tokenCount;
} TokenArray;

TokenArray tokenizer(char* line);
void parser();
void processAndPipe();

int main(int argc, char* argv){
  int isInter = isatty(STDIN_FILENO);
  //Check between interactive mode and batch mode using isatty()
  if(isInter){
    printf("Welcome to mysh\n");
    while(1){
      printf("mysh> ");
      char* line = NULL;
      if(fgets(line, 1024, stdin) == NULL){
        perror("Error reading Line");
        exit(1);
      }
      TokenArray tokens = tokenizer(line);
      parser(tokens.tokens, tokens.tokenCount);

    }
    printf("Bye\n");
    return 0;
  }
  else{
    printf("Batch mode\n");
    //We open a file and then assuming that each line is a command we would run a tokenizer on it

    int fd;
    if((fd = open(argv[1], O_RDONLY) == -1)){
      perror("eorror");
    }

  }
  
}


TokenArray tokenizer(char* line) {
  // read through line
  // break it into indivudal tokens
  // store them in a list
  
  char* currentToken;
  TokenArray tokens;
  
  
  return tokens;
}

void parser(char** tokens, int tokenCount) {
  // check for built in commands
  // check for redirection
  // check for pipes
  // check for background processes
  // check for foreground processes
  // check for other commands
  // execute the command
  /*Parse the command to detect the | token.
Split the command into two parts: the command before the pipe (ls) and the command after the pipe (grep myfile).*/
  char cwd[1024];
  for (int i = 0; i < tokenCount-1; i++){
    if(strcmp(tokens[i],"exit") == 0){
      exit(1);
    }
    if(strcmp(tokens[i],"cd") == 0){
      if(chdir(tokens[i+1]) == -1){
        perror("Error changing the directory");
        exit(1);
      }
    }
    if(strcmp(tokens[i],"pwd") == 0){
      if(getcwd(cwd, sizeof(cwd)) == NULL){
        perror("Error getting path of current directory");
        exit(1);
      }
    }
  }

  //check for the last token



  
}
void processAndPipe(char** cmd1, char** cmd2) {
  /*Use the pipe() system call to create a pipe.
  Use fork() to create two child processes:
  The first process writes its output to the pipe.
  The second process reads its input from the pipe.
  Use dup2() to redirect the standard output of the first process to the write end of the pipe and the standard input of the second process to the read end of the pipe.
  Close the pipe file descriptors in the parent process after setting up the redirection.*/

}

