# Authors
gc787


# MyShell
The shell consists of 8 methods: main, tokenizer, parser, processAndPipe, nextLine, freeTokenArray, readTokens, match

So for the overall structure of the shell we used sturcts to keep track of commands and information about them to make it easier to tokenize for us. 

Running the program:
Run "make mysh" and then do "./mysh" for interactive mode and do "cat script.sh | ./mysh" and replace the script with any script of your liking.

Methods: 

main:
The main method handles the shell's startup and mode selection. Detects interactive/batch mode using isatty() - printing prompts and welcome messages for interactive mode, while processing files line-by-line in batch mode. Manages the core-shell loop that reads input, executes commands via the parser, and cleans up resources. Also handles file descriptor management for batch mode operations.

tokenizer: 
This method basically breaks down the commands into tokens and stores them in a list of tokens. It processes raw input strings into executable tokens. Splits commands by whitespace while handling special cases: truncates comments after # characters, expands wildcard * patterns by matching files in the current directory (skipping hidden files), and preserves quoted strings. Returns a TokenArray structure containing the parsed command components with proper memory allocation.

parser: 
This method is where we have a bulk of our logic that parses through the tokens and has code for the built-in commands and the other commands that the shell should be able to handle. This method is basically the command execution engine. First, it processes built-in commands (cd, pwd, exit, etc.), then handles redirections (< >) and pipes (|). It also implements conditional logic using last_command_status to control the and/or flow. For external commands, it forks child processes, sets up I/O streams, and searches for executables in /usr/local/bin, /usr/bin, and /bin. Maintains rigorous error checking throughout.

processAndPipe: 
This is our specialized function that connects two commands via pipes. Creates pipe file descriptors and manages forked processes, carefully redirecting their stdin/stdout to establish communication. Handles all pipe-related cleanup including closing unused file descriptors and waiting for child processes to complete. This fucntion is also called in the parser.

nextLine:
Efficient line reader that handles both interactive input and file streams. It uses buffered reading for better performance with large inputs. The method also preserves partial lines at EOF for proper batch mode operation and maintains the current read position across multiple calls.

freeTokenArray()
Memory management helper that safely deallocates all resources in a TokenArray. It also frees both the token array and individual token strings while handling NULL pointers gracefully. Essential for preventing memory leaks during command processing.

readTokens()
Good for debugging, and it prints the tokenized representation of commands. Displays each token with its position in the array, primarily used during development to verify proper command parsing and wildcard expansion.

match()
Wildcard pattern matcher supporting simple prefix and suffix formats. Compares directory entries against patterns while excluding hidden files. Handles edge cases like empty segments and maintains case-sensitive matching per Unix conventions.

# Test Plan
For the testing, we had multiple script files that tested different things. We tested the conditionals, redirection, common commands, and piping. We started off first with batch mode and then started testing the interactive mode as well, depending on how we ran the program. Along with our script.sh, files; we also had multiple input files and text files that work with the script files to further make sure our program could handle more nuanced tasks. 

script.sh:
This test case thoroughly exercises the shell's core functionality by combining multiple operations in a single sequence. It begins with a simple echo command containing an inline comment to verify proper comment handling, followed by built-in commands like which and pwd to test their basic functionality. The test then progresses to more complex scenarios, including executing external programs, input redirection with wildcards, and piping commands together. We specifically included the wildcard tests with both direct arguments and redirection to ensure robust pattern matching, while the final die command checks proper error handling and termination. This compact sequence efficiently verifies all major shell features while maintaining clarity and readability.

script2.sh: 
This test sequence verifies the shell's conditional command handling and error recovery. It begins by executing a command that will fail ("cat notafile"), then tests the "and/or" conditional logic by checking whether subsequent commands execute based on the previous command's failure. The "and notacmd" should be skipped since the first command failed, while both "or" commands should execute - first attempting another invalid command ("notacmd2"), then running a valid echo command. We included this to ensure the shell properly handles command failures, respects conditional execution rules, and maintains correct status tracking between commands. The progression from failure to success demonstrates the shell's ability to recover and continue processing subsequent commands appropriately.

script3.sh:
This pipeline test demonstrates the shell's ability to chain multiple I/O operations together. It first redirects the input from a file to the cat command, then pipes the output to grep, and finally redirects the filtered results to an output file. We included this test to verify three critical shell features working in combination: input redirection, piping between processes, and output redirection. The test ensures the shell correctly handles file descriptors when chaining these operations and properly manages the data flow through all stages of the pipeline. This complex operation validates that the shell can coordinate multiple simultaneous I/O operations while maintaining proper process isolation and resource management.

Overall, we tried many different things along with these test cases, such as testing nested commands with multiple pipes and redirections, verifying wildcard expansion in various directory contexts, and checking edge cases with malformed commands. We made sure to test all built-in commands (cd, pwd, which, exit, die) with both valid and invalid arguments, as specified in the requirements. Special attention was given to conditional command execution (and/or) to ensure proper status tracking between commands. We also verified the shell correctly handles batch mode versus interactive mode, including the required behavior of closing standard input for child processes in batch mode. These tests comprehensively cover the project's requirements regarding command parsing, process spawning, I/O redirection, and pipeline functionality while ensuring robust error handling throughout.
