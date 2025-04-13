#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>

#define MAX_ARGS 1000
#define BUFLEN 64
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

void readTokens(TokenArray* tokens);

char* nextLine();		    
int currentFD = 0;      // variables for reading lines in both shell and batch mode
int currentPos = 0;
int currentLen = 0;
char buffer[BUFLEN];


int main(int argc, char** argv){

  int isInter = isatty(STDIN_FILENO);
  //Check between interactive mode and batch mode using isatty()
  if(isInter && argc == 1){

    printf("--Welcome to mysh--\n");
		
		char* line = NULL;
		currentFD = STDIN_FILENO;

    printf("mysh> ");
    fflush(stdout);
    while((line = nextLine())){

			TokenArray* tokens = tokenizer(line);
      // readTokens(tokens);
			// parser(tokens->tokens, tokens->tokenCount);

			free(line);
			free(tokens->tokens);
			free(tokens);
      
      printf("mysh> ");
      fflush(stdout);
    }
		 
    printf("--Bye--\n");
  }
	else{
    //We open a file and then assuming that each line is a command we would run the tokenizer on it

    currentFD = open(argv[1], O_RDONLY);
    if(currentFD == -1){
      perror("unable to open");
    }

		char* line = NULL;
		while ((line = nextLine())){
      TokenArray* tokens = tokenizer(line);
      // readTokens(tokens);
      // parser(tokens->tokens, tokens->tokenCount);

      free(line);
      free(tokens->tokens);
      free(tokens);
		}

    close(currentFD);
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
  int pipeIndex = 0;
  char* in_file;
  in_file = NULL;
  char* out_file = NULL;
  char* command1 = NULL;
  char* command2 = NULL;

  for(int i =0; i<tokenCount; i++){
    if(strcmp(tokens[i], "<") == 0){
      in_file = tokens[i+1];
      tokens[i] = NULL;
    }
    if(strcmp(tokens[i], "|") == 0){
      command1 = tokens[i-1];
      command2 = tokens[i+1];
      tokens[i] = NULL;
    }
    if(strcmp(tokens[i], ">") == 0){
      out_file = tokens[i+1];
      tokens[i] = NULL;
    }
  }

  for (int i = 0; i < tokenCount; i++){
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
    break;
  }

  // Handle pipes
  if(pipeIndex != 0){
    int cmd1_start, cmd2_start;
    // Find first command (before pipe)
    cmd1_start = 0;
    while(cmd1_start < tokenCount && tokens[cmd1_start] == NULL){
      cmd1_start++;
    }
    
    // Find second command (after pipe)
    cmd2_start = pipeIndex+1;
    while(cmd2_start < tokenCount && tokens[cmd2_start] == NULL){
      cmd2_start++;
    }  
    if(cmd1_start < pipeIndex && cmd2_start < tokenCount){
      processAndPipe(&tokens[cmd1_start], &tokens[cmd2_start]);
    } 
    else {
      fprintf(stderr, "Invalid pipe syntax\n");
    }
    return;
  }

  //Handle regular commands so meaning no pipes:
  int command_start = 0;
  while(command_start < tokenCount && tokens[command_start] == NULL){
    command_start++;
  }

  if(command_start < tokenCount){
    pid_t pid;
    if((pid = fork()) == 0){
      if(in_file != NULL){
        int fd = open(in_file, O_RDONLY);
        if(fd == -1){
          perror("Error opening input file");
          exit(1);
        }
        dup2(fd, STDIN_FILENO);
        close(fd);
      }
      if(out_file != NULL){
        int fd = open(out_file, O_WRONLY | O_CREAT | O_TRUNC, 0644);
        if(fd == -1){
          perror("Error opening output file");
          exit(1);
        }
        dup2(fd, STDOUT_FILENO);
        close(fd);
      }
      char* path = tokens[command_start];
      if(strchr(path, '/')) { // If it contains a slash, then just do it directly
        execv(path, &tokens[command_start]);
      } 
      else 
      { // Search these: /usr/local/bin, /usr/bin, /bin
        char* possiblePaths[] = {"/usr/local/bin/", "/usr/bin/", "/bin/", NULL};
        char full_path[1024];
        
        for(int i=0; possiblePaths[i]; i++){
          snprintf(full_path, sizeof(full_path), "%s%s", possiblePaths[i], path);
          if(access(full_path, X_OK) == 0){
            execv(full_path, &tokens[command_start]);
          }
        }
      }
      perror("execv failed");
      exit(1);
    } 
    else if(pid > 0){
      //meaning that we r in the parent process
      waitpid(pid, NULL, 0);
    } 
    else {
      //something went wrong in the forking process
      perror("fork");
    }

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

  if(pipe(pipefd) == -1){
    perror("Error creating pipe");
    exit(1);
  }

  if((child1 = fork()) == -1){
    perror("Error forking child process");
    exit(1);
  }

  if(child1 == 0){
    //this means we r in the actual child process and not the parent one
    char* args1[] = {cmd1, NULL};
    close(pipefd[0]); //close the unused read end of the pipe we made
    dup2(pipefd[1], STDOUT_FILENO);
    close(pipefd[1]);

    execv(cmd1, args1);
    perror("Error executing command");
    exit(1);
  }

  if((child2 = fork()) == -1){ 
    perror("Error forking child process");
    exit(1);
  }

  if(child2 == 0){
    char* args2[] = {cmd2, NULL};
    close(pipefd[1]); 
    dup2(pipefd[0], STDIN_FILENO);
    close(pipefd[0]);
  
    execv(cmd2, args2);  
    perror("Error executing command");
    exit(1);
  }
  //The parent process now basically closes both ends of the pipe
  close(pipefd[0]);
  close(pipefd[1]);
  //Wait for both child processes to finish
  waitpid(child1, NULL, 0);
  waitpid(child2, NULL, 0);
}

char* nextLine(){
	char* line = NULL;
  int lineLen = 0;
  int segLen, segStart;

  while (1){

    if (currentPos == currentLen){
      int bytes = read(currentFD, buffer, BUFLEN);
      if (bytes <= 0){
        return NULL;
      }
      currentLen = bytes;
      currentPos = 0;
    }

    segStart = currentPos;
    for(;currentPos < currentLen; currentPos++){
      if (buffer[currentPos] == '\n'){
        segLen = currentPos - segStart;
        line = realloc(line, lineLen + segLen + 1);
        memcpy(line + lineLen, buffer + segStart, segLen);
        line[lineLen + segLen] = '\0';
        currentPos++;
        return line;
      }
    }

    segLen = currentPos - segStart;
    line = realloc(line, lineLen + segLen);
    memcpy(line + lineLen, buffer + segStart, segLen);
    lineLen += segLen;
  }
}

void readTokens(TokenArray* tokens){    // for testing
  for (int i = 0; i < tokens->tokenCount; i++){
    printf("%d : %s\n", i, tokens->tokens[i]);
  }
  printf("\n");
}
