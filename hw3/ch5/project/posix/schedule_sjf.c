#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "schedulers.h"
#include "list.h"
#include "task.h"
#include "cpu.h"

#define INT_MAX 2147483647

static struct node *head = NULL;
static int t_num = 0;

// add a task to the list 
void add(char *name, int priority, int burst) {
    Task *t = malloc(sizeof(Task));
    t->name = malloc(strlen(name) + 1);

    strcpy(t->name, name);
    t->priority = priority;
    t->burst = burst;
    t->tid = t_num;
    ++t_num;

    insert(&head, t);
}

// invoke the scheduler
void schedule() {
    struct node *tmp;
    Task *shortest_burst_t;
    int shortest_burst;

    while (t_num > 0) {
        tmp = head;
        shortest_burst = INT_MAX;

        // find the task with shortest burst
        while (tmp != NULL) {
            if (tmp->task->burst < shortest_burst) {
                shortest_burst = tmp->task->burst;
                shortest_burst_t = tmp->task;
            }
            
            tmp = tmp->next;
        }

        run(shortest_burst_t, shortest_burst);
        delete(&head, shortest_burst_t);
        --t_num;
    }
}