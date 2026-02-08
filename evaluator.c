#include "dccsh.h"

void execute_command(Command *cmd) {
    if (cmd == NULL)
        return;

    if (is_builtin(cmd)) {
        run_builtin(cmd);
        return;
    }

    // placeholder for external commands
    printf("external command: %s\n", cmd->command);
}

// checks if user typed built in func
bool is_builtin(Command *cmd) {
    if (cmd == NULL || cmd->command == NULL)
        return false;

    return strcmp(cmd->command, "exit") == 0 ||
           strcmp(cmd->command, "cd") == 0   ||
           strcmp(cmd->command, "pwd") == 0;
}

void run_builtin(Command *cmd) {
    // if exit, shell exits lol 
    if (strcmp(cmd->command, "exit") == 0) {
        exit(0);
    }

    // if pwd, gets the path of working directory using getcwd
    else if (strcmp(cmd->command, "pwd") == 0) {
        char cwd[MAX_INPUT];
        if (getcwd(cwd, sizeof(cwd)) != NULL) {
            printf("%s\n", cwd);
        } else {
            perror("pwd");
        }
    }

    else if (strcmp(cmd->command, "cd") == 0) {
        char *path = cmd->args[1];
        if (path == NULL) {
            path = getenv("HOME");
            if (path == NULL) {
                fprintf(stderr, "dccsh: HOME not set\n"); 
                return;
            }
        }
        
        if (chdir(path) != 0) {
            perror("cd");
        }
    }
}
