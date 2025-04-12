#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>

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

TokenArray* tokenizer(char* line);
void parser();
void processAndPipe();
char* nextLine();		// maybe?


int main(int argc, char** argv){

  int isInter = isatty(STDIN_FILENO);
  //Check between interactive mode and batch mode using isatty()
  if(isInter){

    printf("--Welcome to mysh--\n");
		
		char* line = NULL;
			
		int fd = open(STDIN_FILENO, O_RDONLY);
		if(fd == -1){
			perror("unable to open");
		}

    while(line == nextLine()){
      printf("mysh> ");
			TokenArray* tokens = tokenizer(line);
			parser(tokens->tokens, tokens->tokenCount);

			free(line);
			free(tokens->tokens);
			free(tokens);
    }
		 
    printf("--Bye--\n");
  }
	else{
    printf("Batch mode\n");
    //We open a file and then assuming that each line is a command we would run the tokenizer on it

    int fd = open(argv[1], O_RDONLY);
    if(fd == -1){
      perror("unable to open");
    }

		char* line = NULL;
		while ((line == nextLine())){
      TokenArray* tokens = tokenizer(line);
      parser(tokens->tokens, tokens->tokenCount);

      free(line);
      free(tokens->tokens);
      free(tokens);
		}
  }

	return EXIT_SUCCESS;
}


TokenArray* tokenizer(char* line) {
  
  TokenArray* tokens = (TokenArray*)malloc(sizeof(TokenArray));
  char** tokenArr = (char**)malloc(sizeof(char*));

  int count = 0;
  char* currentTok = strtok(line, " ");

  bool foundComment = false;
  while(currentTok != NULL){


    for (int i = 0; i < strlen(currentTok); i++){           // looks for comment within token
        if (currentTok[i] == '#'){
            currentTok[i] = '\0';                           // if comment found, places null terminator at the beginning of the comment to cut off the token
            foundComment = true;
        }
    }

    if (strlen(currentTok) != 0){
        tokenArr[count] = currentTok;                           // add token to array
        count++;
    }

    if (foundComment){                                      // if comment found, break out and stop reading the line
        break;
    }

    tokenArr = (char**)realloc(tokenArr, (count + 1) * sizeof(char*));
  	currentTok = strtok(NULL, " ");
  }
  
  tokens->tokenCount = count;
  tokens->tokens = tokenArr;
  
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
      exit(0);
    }
    if(strcmp(tokens[i],"cd") == 0){
      if(chdir(tokens[i+1]) == -1){
        perror("Error changing the directory");
      }
      return;
    }
    if(strcmp(tokens[i],"pwd") == 0){
      if(getcwd(cwd, sizeof(cwd)) == NULL){
        perror("Error getting path of current directory");
      }
      else{
        printf("%s\n", cwd);
      }
      return;
    }
    char* command, path, token;
    if(strcmp(tokens[i],"which") == 0){
      if(tokenCount == 2){
        command = tokens[i+1];
        path = getenv("PATH");
        token = strtok(path, ":");

        while(token != NULL){
          char fullPath[1024];
          snprintf(fullPath, sizeof(fullPath), "%s/%s", token, command);
          if(access(fullPath, F_OK) == 0){
            printf("%s\n", fullPath);
            break;
          }
          token = strtok(NULL, ":");
        }
      }
      else
      {
        perror("Error: wrong number of arguments");
        exit(1);
      }
    }
    if(strcmp(tokens[i],"die") == 0){
      exit(1);
    }
    //Now we check for specific tokens
    if(strcmp(tokens[i],"|") == 0){
      //now this means we have a pipe so we should split into 2 commands
      char* command1;
      command1 = tokens[i-1];
      char* command2;
      command2 = tokens[i+1];
      //Now we have to check if the command is a built in command
      if(strcmp(command1,"cd") == 0){
        //we have to check if the command is a built in command
        if(chdir(command2) == -1){
          perror("Error changing the directory");
          exit(1);
        }
      }
      else if(strcmp(command1,"pwd") == 0){
        char cwd[1024];
        if(getcwd(cwd, sizeof(cwd)) == NULL){
          perror("Error getting path of current directory");
          exit(1);
        }
        printf("%s\n", cwd);
      }
      else{
        processAndPipe(command1,command2);
      }
    }
    if(strcmp(tokens[i],">") == 0){
      //this means we have to redirect the output of the command
      
    }
    if(strcmp(tokens[i],"<") == 0){
      //this means we have to redirect the input of the command
    }
    

  }

  //check for the last token
  //in the last token we can't have a pipe
  if(strcmp(tokens[tokenCount-1],"cd") == 0){
    perror("Error changing the directory");
    exit(1);
  }
  if(strcmp(tokens[tokenCount-1],"pwd") == 0){
    char cwd[1024];
    if(getcwd(cwd, sizeof(cwd)) == NULL){
      perror("Error getting path of current directory");
      exit(1);
    }
    printf("%s\n", cwd);
  }
  if(strcmp(tokens[tokenCount-1],"exit") == 0){
    exit(1);
  }
  
}
void processAndPipe(char* cmd1, char* cmd2) {
  /*Use the pipe() system call to create a pipe.
  Use fork() to create two child processes:
  The first process writes its output to the pipe.
  The second process reads its input from the pipe.
  Use dup2() to redirect the standard output of the first process to the write end of the pipe and the standard input of the second process to the read end of the pipe.
  Close the pipe file descriptors in the parent process after setting up the redirection.*/
  int pipefd[2];
  int child1, child2;
  // pipefd[0] = child1;
  // pipefd[1] = child2;
  if(pipe(pipefd) == -1){
    perror("Error creating pipe");
    exit(1);
  }
  if((child1 = fork()) == -1){
    perror("Error forking child process");
    exit(1);
  }
  if(child1 == 0){
    int fd1;
    //this means we r in the actual child process and not the parent one
    if(cmd1){
      fd1 = open(cmd1, O_RDONLY);
      if(fd1 == -1){
        perror("Error opening file");
        exit(1);
      }
      dup2(fd1, STDIN_FILENO);
      close(fd1);
      //now it writes the output of this cmd1 to the pipe
      write(pipefd[1], cmd1, strlen(cmd1));
      close(pipefd[1]);
    }
  }
  if(child2 = fork() == -1){
    perror("Error forking child process");
    exit(1);
  }
  if(child2 == 0){
    if(cmd2){
      int fd2;
      fd2 = open(cmd2, O_RDONLY);
      if(fd2 == -1){
        perror("Error opening file");
        exit(1);
      }
      dup2(fd2, STDIN_FILENO);
      close(fd2);
      //now it reads the output of this cmd2 from the pipe
      read(pipefd[0], cmd2, strlen(cmd2));
      close(pipefd[0]);
    }
  }
}

char* nextLine(){
	char* line = NULL;
}
