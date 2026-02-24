#include "dccsh.h"
#include <string.h>
#include <stdlib.h>

int main(void) {
    char input[MAX_INPUT];

    while (true) {
        printf("dccsh> ");
        fflush(stdout);

        if (fgets(input, sizeof(input), stdin) == NULL) {
            printf("\n");
            break;
        }
        
        input[strcspn(input, "\n")] = '\0';

        //creates a copy of the input to prevent memory issues 
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
                
                // cleans up any finished bg jobs
                if (get_bg_job_count() > 0) {
                    cleanup_bg_jobs();
                }
    }

    return 0;
}