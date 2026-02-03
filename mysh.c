#include <stdio.h>
#include <stdbool.h>
#include <string.h>

int main(void) {
    char input[1024];

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

        printf("%s\n", input);
    }

    return 0;
}
