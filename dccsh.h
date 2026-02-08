#ifndef DCCSH_H
#define DCCSH_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <sys/wait.h>

#define MAX_ARGS 64
#define MAX_INPUT 1024

typedef struct {
    char *command;        // Command name
    char *args[MAX_ARGS];      // Arguments (NULL-terminated)
    char *input_file;     // For < redirection (NULL if none)
    char *output_file;    // For > or >> redirection (NULL if none)
    bool append;          // true for >>, false for >
    bool background;      // true if & present
} Command;

// insert parser functions here
Command *parse_input(char *input);
void free_command(Command *cmd);
void execute_command(Command *cmd);

// insert evaluator functions here
bool is_builtin(Command *cmd);
void run_builtin(Command *cmd);

#endif
