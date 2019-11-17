#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "schedulers.h"
#include "list.h"
#include "task.h"
#include "cpu.h"

#define INT_MIN -2147483648
#define INT_MAX 2147483647
#define SIZE 50
#define QUANTUM 10
#define max(x,y) (x > y ? x : y)
#define min(x,y) (x < y ? x : y)

static int t_num = 0;
static int priority_max = INT_MIN;
static struct node *heads[SIZE];

// add a task to the list 
void add(char *name, int priority, int burst) {
    Task *t = malloc(sizeof(Task));
    t->name = malloc(strlen(name) + 1);

    strcpy(t->name, name);
    t->priority = priority;
    t->burst = burst;
    t->tid = t_num;
    ++t_num;

    // update the maximal priority
    priority_max = max(priority, priority_max);

    insert(&heads[priority], t);
}

// invoke the scheduler
void schedule() {
    int current_head = priority_max;
    int run_time;
    struct node *tmp, *nextTmp;

    while (t_num > 0) {
        tmp = heads[current_head];

        // go to front
        while (tmp->next != NULL) {
            tmp = tmp->next;
        }

        while (tmp != NULL) {
            // run
            run_time = min(QUANTUM, tmp->task->burst);
            run(tmp->task, run_time);

            // find the next task to run
            nextTmp = heads[current_head];

            if (tmp == heads[current_head]) {
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
                delete(&heads[current_head], tmp->task);
                --t_num;
            }
            else {
                tmp->task->burst -= run_time;
            }

            // assign next task
            if (heads[current_head] == NULL) {
                tmp = NULL;
            }
            else {
                tmp = nextTmp;
            }
        }

        // go to lower priority
        while (tmp == NULL)
        {
            --current_head;
            tmp = heads[current_head];
        }
    }
}