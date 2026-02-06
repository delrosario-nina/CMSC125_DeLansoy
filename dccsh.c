#include "dccsh.h"

int main(void) {
    char input[MAX_INPUT];

    while (true) {
        printf("dccsh> "); // dubai chewy cookie shell :> 
        fflush(stdout);

        if (fgets(input, sizeof(input), stdin) == NULL) {
            printf("\n");
            break;
        }
        
        input[strcspn(input, "\n")] = '\0';

        if (strcmp(input, "exit") == 0) {
            break;
        }

        if (strlen(input) == 0) {
            continue;
        }

        char **args = parse_input(input);
        execute_command(args);
    }

    return 0;
}