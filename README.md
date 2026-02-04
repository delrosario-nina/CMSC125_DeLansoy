# Members:
Del Rosario, Nina Claudia

Lansoy, Sam


# Problem Analysis:
Main Problem: Creating a shell (command line interpreter) to better understand how a user interacts with an operating system.

### Subproblems:
**a. User Input Loop**

What it needs to do:
- display a continuous prompt to show the shell is ready (like "dcc> ")
- read complete lines of user input (could be short like "ls" or long with many arguments)
- detect end-of-file
- keep running after each command finishes


**b. Command Parsing and Tokenization:**

What it needs to do:
- transform raw user input (string) into structured data that can be executed
- split the input into command name and arguments
- identify and extract special symbols (>, <, >>, &)
- handle edge cases like multiple spaces, tabs, quotes
- detect syntax errors (like "ls > > file.txt" or "> < file")
- build a data structure that contains all the info needed for execution


**c. Process creation and execution:** execute external commands in separate processes while keeping the shell running.

What it needs to do:
- execute external commands (ls, cat, grep, etc.) in separate processes
- keep the shell itself running while commands execute
- understand the difference between built-in commands (run by shell) and external commands (run as new process)
- use fork() to create a child process
- use exec() to replace child with the actual program
- use wait() to know when the command finishes


**d. I/O Redirection**

What it needs to do:
- redirect standard output to a file when user types > or >>
- redirect standard input from a file when user types 
- handle output redirection modes:
  - `>` : overwrite file (truncate)
  - `>>` : append to file
- manipulate file descriptors (dup2) to connect stdin/stdout to files
- handle errors (file doesn't exist, no write permission, etc.)


**e. Built-in Command Implementation:** commands that modify the shells' own state
What it needs to do:
- implement commands that MUST run in the shell's own process:
  - `cd` : change the shell's current directory
  - `exit` : terminate the shell
  - `pwd` : print the shell's current working directory
  - (optional) `echo`, `export`, `jobs`
- use system calls like chdir(), getcwd(), exit()
- handle errors (directory doesn't exist, permission denied)
- NOT fork when running these (they run in the shell itself)


**f. Background Jobs:** allow commands to run in the background while the shell remains interactive.

What it needs to do:
- detect & symbol at the end of commands
- run the command without waiting for it to finish
- immediately return to the prompt so user can type more commands


**g. Error handling:** handle various error conditions without crashing the shell.

What it needs to do:
- check return values of all system calls (fork, exec, open, chdir, etc.)
- validate user input before executing (syntax errors, empty commands)
- handle all error types:
  - **Parsing errors**: invalid syntax like "> >", "< <", unmatched quotes
  - **Command not found**: exec() fails because program doesn't exist
  - **Permission errors**: can't execute file, can't read/write file
  - **File errors**: can't open file for redirection, file doesn't exist
  - **Built-in errors**: cd to non-existent directory, invalid arguments
- print clear, helpful error messages


# Solution Architecture:

**a. User Input Loop**
Pseudocode:
```
while (true) {
    display_prompt()              // Show "dcc> "
    input = read_input()          // Read line from user
    if (input is EOF or "exit")   // Check for exit conditions
         → break                     // Exit shell 
    if (input is empty/whitespace)
         → continue                  // Skip empty lines
    tokens = parse_input(input)   // Convert to Command struct
    if (parse_error)              // Check if parsing succeeded
         → display_error()
         → continue                  // Don't execute bad commands
    execute_command(tokens)       // Run the command
}
```

**b. Command Parsing and Tokenization:** Convert raw user input string into a structured command that can be executed. This is where we make sense of what the user typed.

Pseudocode:

    - split input string into command and arguments
    - detect syntax errors in input and handle multiple cases (special cases, multiple spaces and tabs, long arguments)
    - create data structures and functions for the tokens
    
Data structure:
```
typedef struct {
    char *command;        // Command name
    char *args[256];      // Arguments (NULL-terminated)
    char *input_file;     // For < redirection (NULL if none)
    char *output_file;    // For > or >> redirection (NULL if none)
    bool append;          // true for >>, false for >
    bool background;      // true if & present
} Command;
```

**c. Process creation and execution:** Handles command execution using fork/exec/wait

after parsing the input string, it will be differentiated based on whether it is a built-in command or external command before being executed.

Pseudocode:
```
execute_command(cmd):
    if is_builtin(cmd->command):
        execute_builtin(cmd)
    else:
        execute_external(cmd)
```

**d. I/O Redirection:** Manages file redirection (>, <, >>) to make commands read from/write to files.

types of redirection:
  * output redirection (>) - Save output to a file (replaces file contents)
  * append redirection (>>) - Add output to the end of a file
  * input redirection (<) - Read from a file instead of keyboard

Pseudocode:
```
for output redirection (> or >>)
  1. open the file (create if it doesn't exist)
  2. use dup2() to make stdout point to the file
  3. now when the command prints, it goes to the file
  4. close the file when done

for input redirection (<)
  1. open the file for reading
  2. use dup2() to make stdin read from the file
  3. now when the command reads input, it comes from the file
  4. close the file when done
```

**e. Built-in Command Implementation:**

after checking that the command is built-in, it will then run the appropriate built-in per each command (exit, cd, pwd)

for cd:
- get the directory path from arguments
- call chdir(path) to change directory
- if it fails, print error message

for pwd:
- call getcwd() to get current directory
- print the result
- if it fails, print error message

for exit:
- optionally get exit code from arguments
- print goodbye message
- call exit() to terminate the shell


**f. Background Jobs**

use waitpid()
        - waiting for specific child processes
        - non-blocking checks using WNOHANG (prevents parent from hanging if child has not yet terminated or changed state
/// wait blocks (nagffreeze ang shell) so waitpid is better 

therefore, the shell tracks background process IDs and calls waitpid() with the WNOHANG option at the start of each command loop iteration. This allows the shell to clean up finished background processes without blocking. Basically, it makes the shell responsive while handling background processes (avoids zombie processes) 
```
ex. 
waitpid(id, &status, WNOHANG)
```

**g. Error Handling**
- implemented thru checking the return values of the system calls and validating the parsed user input before it will be executed. If error, then it will print what error message 

Error handling includes:

- Parsing error (invalid syntax and shi)
- process creation error
- built-in command errors
- invalid arguments
- etc. 


# Data Flow
```
User Input: "ls -l > output.txt"
    ↓
read_input()  → "ls -l > output.txt"
    ↓
parse_input(input) → Command {
                    command: "ls"
                    args: ["ls", "-l", NULL]
			   Input_file: NULL
                    output_file: "output.txt"
                    append_mode: 0
                    background: 0
                }
    ↓
execute_command() → determine if “ls” is built-in? No
    ↓
execute_externalcommand → fork()
    ↓
Child: setup_redirections() → open("output.txt"), dup2()
       execvp("ls", ["ls", "-l", NULL])
    ↓
Parent: wait()
    ↓
Command complete → Back to main loop
```

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

