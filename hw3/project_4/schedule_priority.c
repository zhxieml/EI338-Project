#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "schedulers.h"
#include "list.h"
#include "task.h"
#include "cpu.h"

#define INT_MIN -2147483648

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
    Task *highest_t;
    int highest;

    while (t_num > 0) {
        tmp = head;
        highest = INT_MIN;

        // find the task with highest priority
        while (tmp != NULL) {
            if (tmp->task->priority > highest) {
                highest = tmp->task->priority;
                highest_t = tmp->task;
            }
            
            tmp = tmp->next;
        }

        run(highest_t, highest);
        delete(&head, highest_t);
        --t_num;
    }
}