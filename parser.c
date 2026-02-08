#include "dccsh.h"

Command *parse_input(char *input) {
    Command *cmd = malloc(sizeof(Command));
    if (!cmd) {
        perror("malloc");
        return NULL;
    }

    // initialize
    cmd->command = NULL;
    cmd->input_file = NULL;
    cmd->output_file = NULL;
    cmd->append = false;
    cmd->background = false;

    for (int i = 0; i < MAX_ARGS; i++) {
        cmd->args[i] = NULL;
    }

    int argc = 0;
    char *token = strtok(input, " ");

    while (token != NULL) {
        if (strcmp(token, "<") == 0) {
            token = strtok(NULL, " ");
            if (token == NULL) {
                fprintf(stderr, "dccsh: syntax error <\n"); // placeholder danay
                free_command(cmd);
                return NULL;
            }
            cmd->input_file = token;

        } else if (strcmp(token, ">") == 0) {
            token = strtok(NULL, " ");
            if (token == NULL) {
                fprintf(stderr, "dccsh: syntax error >\n");
                free_command(cmd);
                return NULL;
            }
            cmd->output_file = token;
            cmd->append = false;

        } else if (strcmp(token, ">>") == 0) {
            token = strtok(NULL, " ");
            if (token == NULL) {
                fprintf(stderr, "dccsh: syntax error near >>\n");
                free_command(cmd);
                return NULL;
            }
            cmd->output_file = token;
            cmd->append = true;

        } else if (strcmp(token, "&") == 0) {
            cmd->background = true;

        } else {
            cmd->args[argc++] = token;
        }

        token = strtok(NULL, " ");
    }

    if (argc == 0) {
        free_command(cmd);
        return NULL;
    }

    cmd->args[argc] = NULL;
    cmd->command = cmd->args[0];

    return cmd;
}

void free_command(Command *cmd) {
    if (cmd) {
        free(cmd);
    }
}


// parse_input(input) function
