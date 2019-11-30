#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "schedulers.h"
#include "list.h"
#include "task.h"
#include "cpu.h"

#define QUANTUM 10
#define max(x,y) (x > y ? x : y)
#define min(x,y) (x < y ? x : y)

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
    struct node *tmp = head;
    struct node *nextTmp;
    int run_time;
    
    // go to front
    while (tmp->next != NULL) {
        tmp = tmp->next;
    }

    while (t_num > 0) {
        // run
        run_time = min(QUANTUM, tmp->task->burst);
        run(tmp->task, run_time);

        // find the next task to run
        nextTmp = head;

        if (tmp == head) {
            while (nextTmp->next != NULL) {
                nextTmp = nextTmp->next;
            }
        }
        else {
            while (nextTmp->next != tmp)
            {
                nextTmp = nextTmp->next;
            }            
        }

        // check if the task needs to be deleted
        if (tmp->task->burst == run_time) {           
            delete(&head, tmp->task);
            --t_num;
        }
        else {
            tmp->task->burst -= run_time;
        }

        // assign next task
        tmp = nextTmp;
    }
}