/**
 * Implementation of thread pool.
 */

#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <semaphore.h>
#include <fcntl.h>
#include "threadpool.h"

#define QUEUE_SIZE 50
#define NUMBER_OF_THREADS 3

#define TRUE 1

// this represents work that has to be 
// completed by a thread in the pool
typedef struct {
    void (*function)(void *p);
    void *data;
} task;

// the work queue
task worktodos[QUEUE_SIZE];
int total;

// the worker bee
pthread_t bees[NUMBER_OF_THREADS];

// the mutex and the semaphore
pthread_mutex_t mutex;
sem_t *sem;

// insert a task into the queue
// returns 0 if successful or 1 otherwise, 
int enqueue(task t) {
    pthread_mutex_lock(&mutex);

    if (total == QUEUE_SIZE) {
        pthread_mutex_unlock(&mutex);
        return 1;
    }
    
    worktodos[total++] = t;

    pthread_mutex_unlock(&mutex);

    return 0; 
}

// remove a task from the queue
task *dequeue() {
    pthread_mutex_lock(&mutex);

    task *task_tmp;

    if (total == 0) {
        pthread_mutex_unlock(&mutex);
        return NULL;
    }

    task_tmp = &worktodos[--total];

    pthread_mutex_unlock(&mutex);
    return task_tmp;
}

// the worker thread in the thread pool
void *worker(void *param) {
    task *todo;

    // execute the task
    while(TRUE) {
        sem_wait(sem);

        todo = dequeue();

        execute(todo->function, todo->data);
    }
}

// execute the task provided to the thread pool
void execute(void (*somefunction)(void *p), void *p) {
    (*somefunction)(p);
}

// submit work to the pool
int pool_submit(void (*somefunction)(void *p), void *p) {
    task todo;
    int flag;

    todo.function = somefunction;
    todo.data = p;
    
    // add the task to the queue
    flag = enqueue(todo);

    if (flag) 
        return flag;

    sem_post(sem);
    return 0;
}

// initialize the thread pool
void pool_init(void) {
    pthread_attr_t attrs[NUMBER_OF_THREADS];

    // create the threads at startup
    for (int i = 0; i < NUMBER_OF_THREADS; ++i) {
        pthread_create(&bees[i], &attrs[i], worker, NULL);
        printf("Thread [%d] created\n", i);
    }
    
    // initialize mutual-exclusion locks and semaphores
    sem_init(sem, 0, 0);
    pthread_mutex_init(&mutex, NULL);
}

// shutdown the thread pool
void pool_shutdown(void) {
    // cancel each worker thread and wait for each thread to terminate
    for (int i = 0; i < NUMBER_OF_THREADS; ++i) {
        pthread_cancel(bees[i]);
        pthread_join(bees[i], NULL);
    }
}
