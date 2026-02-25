#include "dccsh.h"
#include <string.h>
#include <stdlib.h>

void run_script(const char *filename) {
    FILE *f = fopen(filename, "r");
    if (!f) {
        perror("fopen");
        return;
    }

    char input[MAX_INPUT];

    while (fgets(input, sizeof(input), f)) {
        input[strcspn(input, "\n")] = '\0';

        // skip blank lines and comments
        if (input[0] == '\0' || input[0] == '#') continue;

        // stop if exit
        if (strcmp(input, "exit") == 0) break;

        printf("dccsh> %s\n", input);

        char *input_copy = malloc(strlen(input) + 1);
        if (input_copy == NULL) {
            fprintf(stderr, "Memory allocation failed\n");
            continue;
        }
        strcpy(input_copy, input);

        Command *cmd = parse_input(input_copy);
        if (cmd == NULL) {
            free(input_copy);
            continue;
        }

        execute_command(cmd);
        free_command(cmd);
        free(input_copy);

        if (get_bg_job_count() > 0) {
            cleanup_bg_jobs();
        }
    }

    fclose(f);

    // cleanup_bg_jobs();
}

int main(int argc, char *argv[]) {
    if (argc == 2) {
        run_script(argv[1]);
        return 0;
    }

    char input[MAX_INPUT];

    while (true) {
        printf("dccsh> ");
        fflush(stdout);

        if (fgets(input, sizeof(input), stdin) == NULL) {
            printf("\n");
            break;
        }
        
        input[strcspn(input, "\n")] = '\0';

        char *input_copy = malloc(strlen(input) + 1);
        if (input_copy == NULL) {
            fprintf(stderr, "Memory allocation failed\n");
            continue;
        }
        strcpy(input_copy, input);

        Command *cmd = parse_input(input_copy);
        if (cmd == NULL) {
            free(input_copy);
            continue;
        }

        execute_command(cmd);
        free_command(cmd);
        free(input_copy);
        
        if (get_bg_job_count() > 0) {
            cleanup_bg_jobs();
        }
    }

    return 0;
}