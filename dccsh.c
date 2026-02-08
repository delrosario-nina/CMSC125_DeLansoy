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

        Command *cmd = parse_input(input);
        if (cmd == NULL) {
            continue;
        }
       
        execute_command(cmd);
        free_command(cmd);

    }

    return 0;
}
