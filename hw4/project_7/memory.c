/* 
 * A program responding to four different requests:
 * 1. Request for a contiguous block of memory
 * 2. Release of a contiguous block of memory
 * 3. Compact unused holes of memory into one single block
 * 4. Report the regions of free and allocated memory 
 */

#include <stdio.h>
#include "memory.h"

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Invalid input!\n");
        return 1;
    }

    char cmd[MAX_CMD];

    while (TRUE) {
        printf("Enter the command (input 'exit' to exit)> ");
        fflush(stdout);

        // read user input
        fgets(cmd, MAX_CMD, stdin);
        cmd[strlen(cmd) - 1] = '\0';


    }

    return 0;
}