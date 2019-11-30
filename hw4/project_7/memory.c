/* 
 * A program responding to four different requests:
 * 1. Request for a contiguous block of memory
 * 2. Release of a contiguous block of memory
 * 3. Compact unused holes of memory into one single block
 * 4. Report the regions of free and allocated memory 
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "memory.h"

Node *head;

void list_list(int size) {
    Node *next;

    head = malloc(sizeof(Node));
    next = malloc(sizeof(Node));

    next->low = 0;
    next->high = size;
    strcpy(next->process, "NULL");
    next->next = NULL;

    head->next = next;
}

void list_destroy(void) {
    Node *tmp = head;
    Node *tmp_next = tmp->next;

    free(tmp);

    while (tmp_next != NULL) {
        tmp = tmp_next;
        tmp_next = tmp_next->next;

        free(tmp);
    }
}

void report_stat(void) {
    Node *tmp = head->next;
    
    while (tmp != NULL) {
        if (!strcmp(tmp->process, "NULL")) 
            printf("Addresses [%d:%d] Unused\n", tmp->low, tmp->high);
        
        else 
            printf("Addresses [%d:%d] Process %s\n", tmp->low, tmp->high, tmp->process);

        tmp = tmp->next;
    }
}

void compact(void) {
    Node *tmp_prev = head;
    Node *to_delete;
    int compact_space = 0;

    while (tmp_prev->next != NULL) {
        if (!strcmp(tmp_prev->next->process, "NULL")) {
            compact_space += tmp_prev->next->high - tmp_prev->next->low;

            // adjust
            to_delete = tmp_prev->next;
            tmp_prev->next = tmp_prev->next->next;

            if (tmp_prev->next == NULL)
                break;

            free(to_delete);
        }

        if (tmp_prev != NULL) {
            tmp_prev->next->high -= tmp_prev->next->low - tmp_prev->high;
            tmp_prev->next->low = tmp_prev->high;
        }

        else {
            tmp_prev->next->high -= tmp_prev->next->low;
            tmp_prev->next->low = 0;
        }

        tmp_prev = tmp_prev->next;
    }

    // move toward one end
    Node *new = malloc(sizeof(Node));

    new->low = tmp_prev->high;
    new->high = new->low + compact_space;
    strcpy(new->process, "NULL");
    new->next = NULL;

    tmp_prev->next = new;
}

int request_memory(char *process, int space, char *strategy) {
    Node *hole_prev = NULL;

    // find the hole
    // first fit
    if (!strcmp(strategy, "F")) {
        hole_prev = head;

        while (hole_prev->next != NULL) {
            if (!strcmp(hole_prev->next->process, "NULL") && hole_prev->next->high - hole_prev->next->low >= space)
                break; 

            hole_prev = hole_prev->next;
        }
    }

    else if (!strcmp(strategy, "B")) {
        Node *tmp = head;
        int best = INT_MAX;

        while (tmp->next != NULL) {
            if (!strcmp(tmp->next->process, "NULL") && tmp->next->high - tmp->next->low >= space && tmp->next->high - tmp->next->low < best) {
                hole_prev = tmp;
                best = tmp->next->high - tmp->next->low;
            }

            tmp = tmp->next;
        }
    }

    else if (!strcmp(strategy, "W")) {
        Node *tmp = head;
        int worst = INT_MIN;

        while (tmp->next != NULL) {
            if (!strcmp(tmp->next->process, "NULL") && tmp->next->high - tmp->next->low >= space && tmp->next->high - tmp->next->low > worst) {
                hole_prev = tmp;
                worst = tmp->next->high - tmp->next->low;
            }

            tmp = tmp->next;
        }
    }

    else 
        return -1;

    // can not find unused memory
    if (hole_prev->next == NULL)
        return -1;

    // allocate memory
    else {
        Node *allocated = malloc(sizeof(Node));
        Node *remain = malloc(sizeof(Node));
        
        allocated->low = hole_prev->high;
        allocated->high = allocated->low + space;
        strcpy(allocated->process, process);

        remain->low = allocated->high;
        remain->high = hole_prev->next->high;
        strcpy(remain->process, "NULL");

        allocated->next = remain;
        remain->next = hole_prev->next->next;

        free(hole_prev->next);
        hole_prev->next = allocated;

        return 0;
    }
}

int release_memory(char *process) {
    Node *partition = head->next;

    // find the allocated partition and then release
    while (partition != NULL) {
        if (!strcmp(partition->process, process))
            strcpy(partition->process, "NULL");

        partition = partition->next;
    }

    // merge unused memory
    Node *tmp_prev = head;
    
    while (tmp_prev->next != NULL) {
        if (!strcmp(tmp_prev->next->process, "NULL")) {
            Node *to_merge_tmp = tmp_prev->next;
            Node *to_delete;
            Node *merge_next = to_merge_tmp->next;

            while (merge_next != NULL && !strcmp(merge_next->process, "NULL")) {
                to_merge_tmp->high += merge_next->high - merge_next->low;
                to_delete = merge_next;
                merge_next = merge_next->next;

                free(to_delete);
            }

            to_merge_tmp->next = merge_next;
        }

        tmp_prev = tmp_prev->next;
    }

    // always success
    return 0;
}

int parse_cmd(char *cmd, char *process, int *space, char *strategy) {
    char *token;
	int i = 0;
    int flag;

    // take the first token
    token = strtok(cmd, " "); 

    if (!token) 
        return 1;

    while (token) {
        if ((i >= 4 && flag == 2) || (i >= 2 && flag == 3))
            return 1;

        if (i == 0) {
            if (!strcmp(token, "RQ")) 
                flag = 2;
            else if (!strcmp(token, "RL"))
                flag = 3;
            else 
                return 1;
        }

        else if (i == 1) 
            strcpy(process, token);

        else if (i == 2)
            *space = atoi(token);

        else 
            strcpy(strategy, token);

        token = strtok(NULL, " ");
        ++i;
    }

    return flag;
}   

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Invalid input!\n");
        return 1;
    }

    // initialize the linked list
    list_list(atoi(argv[1]));

    // have the user enter conmands
    char cmd[MAX_CMD];

    while (TRUE) {
        printf("allocator>");
        fflush(stdout);

        // read user input
        fgets(cmd, MAX_CMD, stdin);
        cmd[strlen(cmd) - 1] = '\0';

        // special cases
        if (!strcmp(cmd, "STAT"))
            report_stat();

        else if (!strcmp(cmd, "X")) {
            list_destroy();

            return 0;
        }

        else if (!strcmp(cmd, "C"))
            compact();

        // parse the command
        else {
            int flag;
            char process[5];
            char strategy[5];
            int space;

            flag = parse_cmd(cmd, process, &space, strategy);

            switch (flag) {
                case 1:
                    printf("Invalid command!\n");
                    break;
                
                case 2:
                    if (request_memory(process, space, strategy) == -1)
                        printf("Failed in request!\n");
                    else 
                        printf("Request accepted!\n");

                    break;

                case 3:
                    release_memory(process);
                    break;
                
                default:
                    printf("Unknown error occurs\n");
            }
        }

    }

    list_destroy();
    return 0;
}