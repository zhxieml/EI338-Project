#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "schedulers.h"
#include "list.h"
#include "task.h"
#include "cpu.h"

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

    while (t_num > 0) {
        tmp = head;

        // find the first-coming task
        while (tmp->next != NULL) {
            tmp = tmp->next;
        }

        run(tmp->task, tmp->task->burst);
        delete(&head, tmp->task);
        --t_num;
    }
}