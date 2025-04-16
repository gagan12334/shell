# Authors
gc787 - Gagan Charagondla
as4314 - Arnav Saxena

# MyShell
The shell consists of 8 methods: main, tokenizer, parser, processAndPipe, nextLine, freeTokenArray, readTokens, match

On a high level, the program works by reading input in the form of lines(both in interactive and batch mode), tokenizing the input, then processing it by reading through the tokens and running any necessary commands.

Running the program:
Run "make mysh" to compile the program. Then, for interactive mode, do "./mysh". For batch mode do either "./mysh script.sh" or "cat script.sh | ./mysh", and replace the script with any script of your liking.

Methods: 

main:
The main method handles the shell's startup and mode selection. Detects interactive/batch mode using isatty(), printing prompts and welcome messages for interactive mode, while processing files line-by-line in batch mode. Manages the core-shell loop that reads input, executes commands via the parser, and cleans up resources. Also handles file descriptor management for batch mode operations.

tokenizer: 
This method basically breaks down the commands into tokens and stores them in a list of tokens. It processes raw input strings into executable tokens. Splits commands by whitespace while handling things like truncating comments after # characters and expanding wildcard * patterns by matching files in the current directory (skipping hidden files). Returns a dynamically allocated TokenArray structure containing an array of tokens and a the number of tokens.

parser: 
This method is where we have a bulk of our logic that parses through the tokens and has code for the built-in commands and the other commands that the shell should be able to handle. This method is basically the command execution engine. First, it processes built-in commands (cd, pwd, exit, etc.), then handles redirections (< >) and pipes (|). It also implements conditional logic using last_command_status to control the and/or flow. For external commands, it forks child processes, sets up I/O streams, and searches for executables in /usr/local/bin, /usr/bin, and /bin. It maintains rigorous error checking throughout.

processAndPipe: 
This is our specialized function that connects two commands via pipes. It creates pipe file descriptors and manages forked processes, carefully redirecting their stdin/stdout to establish communication. Handles all pipe-related cleanup including closing unused file descriptors and waiting for child processes to complete. This fucntion is also called in the parser.

nextLine:
This is a line reader that handles both interactive input and file streams. It uses buffered reading for better performance with large inputs. The method also preserves partial lines at EOF for proper batch mode operation and maintains the current read position across multiple calls.

freeTokenArray()
Memory management helper that safely deallocates all resources in a TokenArray. It also frees both the token array and individual token strings while handling NULL pointers. It is called after a inputted line is executed, before moving on to the next line.

readTokens()
Used for debugging as it prints the tokenized representation of commands. Displays each token with its position in the array, primarily used during development to verify proper command parsing and wildcard expansion.

match()
Wildcard pattern matcher that checks if an inputted file name matches with an inputted wildcard pattern. Checks by checking if segments prior to wildcard and after wildcard match. Note, the matching is case-sensitive.

# Test Plan
For the testing, we had multiple script files that tested different things. We tested the conditionals, redirection, common commands(bare names), in-built commands, wildcard expansion, comments, and piping. We started off first with batch mode and then started testing the interactive mode as well, depending on how we ran the program. Along with our script.sh, files; we also had multiple input files and text files that work with the script files to further make sure our program could handle more nuanced tasks. 

script.sh:
This is our primary testing file. It tests for in-built commands like pwd, which, cd, and die. Additonally, it tests for comments, both if a comment is at the end of a line and if an entire line is a comment. Line 4: ./helloworld test, tests running a user made executable with arg input. Line 5: cat < input.txt, tests running a linux command(bare name) with input redirection. Line 6: ls -l  .. > output1.txt, tests running a bare name command with output redirection to a file output1.txt. Line 7: cat foo*baz.txt | grep txt, tests wildcard expansion using the pattern foo*baz.txt and piping the output to another program. Line 8: cat < input.txt | grep txt > output2.txt tests combining input redirection, piping, and output redirection.

script2.sh: 
This file contains a series of tests ensuring that our code executes conditionals correctly. The first test tests first, if an and correctly evaluates when the previous statment works but the current statement involving the and doesn't. And after that, it checks if the or executes only when the previous statement fails. The second test checks if a chain of or statements keeps going until stopping at the first or statement that works. Finally, the third test checks once again if an or statement works correctly if the previous statement has failed, and it also tests to make sure exit works.


As mentioned, along with our testscripts, we also have a variety of input files that go along with them. Here is a brief description of them:
- input.txt: basic input file with somewhat random text, used in script.sh line 5
- foo12345baz.txt, foo54321a3baz.txt, foocopy123baz.txt: input files that match the foo*baz.txt pattern, tested in script.sh line 7
- helloworld.c, ./helloworld: simple user-made executable that takes in argument input, tested in script.sh line 4


Overall, we tried many different things along with these test cases, such as testing commands that combine pipes and redirections, ensuring that in commands involving pipes or redirections that input and output flows correctly, verifying wildcard expansion in various directory contexts, and checking various edge cases. We made sure to test all built-in commands (cd, pwd, which, exit, die) as well as conditional command execution (and/or) to ensure proper status tracking between commands. We also verified that our shell correctly handles interactive mode as opposed to batch mode, by running the commands outlined in script.sh, in interactive mode, and making sure that everything the same. These tests comprehensively cover the project's requirements regarding command parsing, process spawning, I/O redirection, and pipeline functionality while ensuring robust error handling throughout.
