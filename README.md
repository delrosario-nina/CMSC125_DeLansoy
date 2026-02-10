
**Members**
Del Rosario, Nina Claudia
Lansoy, Sam

#Overview

dccsh is a simple UNIX-like command-line shell written in C.
It demonstrates core operating system concepts such as process creation, command execution, I/O redirection, built-in commands, and background job handling.

###Features implemented so far:

**Interactive Shell Loop**

Displays a continuous prompt:
dccsh>

Reads full lines of user input
Handles end-of-file (Ctrl+D) 
Continues running after each command execution unless user types exit

**Command Parsing**

Converts raw user input into a structured Command object
Tokenizes input based on spaces

Identifies:
+ command name and arguments
+ input redirection (<)
+ output redirection (> and >>)
+ background execution (&)
+ Detects basic syntax errors (missing filenames after redirection, real error handling is still WIP)
+ Ignores empty commands

**Built-in Commands**

Built-in commands are executed directly by the shell (without fork()):

+ cd [path]
  + Changes the current working directory
  + Defaults to $HOME if no path is given

+ pwd
  + Prints the current working directory

+ exit
  + Terminates the shell

**External Command Execution**

Executes non–built-in commands using:

+ fork() to create a child process
+ execvp() to run the program

Parent process waits for foreground jobs using waitpid()

Examples:
```
ls -l
cat file.txt
grep main dccsh.c 
```

**I/O Redirection**

Supports standard input and output redirection:
+ < file : read input from a file
+ > file : write output to a file (overwrite)
+ >> file : write output to a file (append)

Implemented using:
+ open()
+ dup2()
+ proper file descriptor management

Example:
```
ls > files.txt
cat < input.txt
echo hello >> log.txt
```

**Background Jobs**

+ Commands ending with & run in the background
+ Shell immediately returns to the prompt
+ Background process IDs are tracked internally
+ Finished background jobs are cleaned up using:
  + waitpid(pid, &status, WNOHANG)
+ Prevents zombie processes and keeps the shell responsive

Example:
```
sleep 10 &
```

**Error Handling**

+ The shell checks return values of system calls and handles errors safely without crashing:
  + fork() and execvp() failures
+ Invalid redirection syntax
+ File open errors
+ Built-in command errors (invalid cd, missing HOME, etc.)
+ Prints helpful error messages using perror() or fprintf()

# Estimated Implementation Timeline:

**Week 1:**
 - Set up project structure and design notes
 - Implement basic REPL loop (prompt, read, print)
 - Distribute tasks evenly
 - Create readme file

**Week 2:**
 - Implement tokenization/parsing
 - Implement fork/exec/wait pattern
 - Add built-in commands (cd, exit, pwd)
 - Add error handling and edge cases
 - (Stretch) Background processes (&)
 - Update readme file

**Week 3:**
 - Code review and refactoring
 - Implement I/O redirection (>, <)
 - Comprehensive testing (unit tests, integration tests)
 - Implement Error handling and fix bugs from testing
 - Complete documentation (README, comments)

