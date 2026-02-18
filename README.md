
**Members**
Del Rosario, Nina Claudia
Lansoy, Sam

# Overview

dccsh (dubai chewy cookie shell) is a simple UNIX-like command-line shell written in C.
It demonstrates core operating system concepts such as process creation, command execution, I/O redirection, built-in commands, and background job handling.

## Implemented Features

**Interactive Shell Loop**

+ Displays a continuous prompt: `dccsh>`
+ Reads full lines of user input
+ Handles end-of-file (Ctrl+D) gracefully
+ Continues running after each command execution unless user types `exit`

**Command Parsing**

+ Converts raw user input into a structured Command object
+ Tokenizes input based on spaces
+ Identifies:
  - Command name and arguments
  - Input redirection (`<`)
  - Output redirection (`>` and `>>`)
  - Background execution (`&`)
+ Detects syntax errors (missing filenames after redirection operators)
+ Ignores empty commands

**Built-in Commands**

Built-in commands are executed directly by the shell without forking:

+ `cd [path]` - Changes the current working directory
  - Defaults to `$HOME` if no path is given
  - Prints error if path is invalid

+ `pwd` - Prints the current working directory

+ `exit` - Terminates the shell

**External Command Execution**

+ Executes non-built-in commands using:
  - `fork()` to create a child process
  - `execvp()` to run the program
+ Parent process waits for foreground jobs using `waitpid()`

Examples:
```
ls -l
cat file.txt
grep main dccsh.c
echo hello world
```

**I/O Redirection**

Supports standard input and output redirection:
+ `< file` - Read input from a file
+ `> file` - Write output to a file (overwrite)
+ `>> file` - Write output to a file (append)

Implemented using `open()`, `dup2()`, and proper file descriptor management:

Examples:
```
ls > files.txt
cat < input.txt
echo hello >> log.txt
```

**Background Jobs**

+ Commands ending with `&` run in the background
+ Shell immediately returns to the prompt
+ Background process IDs are tracked internally
+ Finished background jobs are cleaned up using `waitpid(pid, &status, WNOHANG)` 
+ Prints job completion status when detecting finished processes
+ Prevents zombie processes and keeps the shell responsive

Example:
```
sleep 10 &
```

**Error Handling**

+ Safely handles failures from system calls:
  - `fork()` and `execvp()` failures
  - File I/O errors
  - Built-in command errors (invalid `cd` path, missing `HOME`, etc.)
+ Prints helpful error messages using `perror()` and `fprintf()`
+ Shell continues running after errors instead of crashing

## How to Build and Run

**Build:**
```
make
```

**Run:**
```
./dccsh
```

## Project Timeline

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

