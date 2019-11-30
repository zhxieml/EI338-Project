#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <stdlib.h>
#include "buffer.h"

// the buffer
// manipulated as a circular queue
buffer_item buffer[BUFFER_SIZE];
int front, rear;

// the mutex and the semaphore
pthread_mutex_t mutex;
sem_t full;
sem_t empty;

// insert item into buffer
int insert_item(buffer_item item) {
    sem_wait(&empty);
    pthread_mutex_lock(&mutex);

    if ((rear + 1) % BUFFER_SIZE == front) {        
        pthread_mutex_unlock(&mutex);

        // return -1 indicating an error condition 
        return -1;
    }

    buffer[rear] = item;
    rear = (rear + 1) % BUFFER_SIZE;

    pthread_mutex_unlock(&mutex);
    sem_post(&full);

    // return 0 if successful, otherwise
    return 0;
}

// remove an object from buffer
int remove_item(buffer_item *item) {
    sem_wait(&full);
    pthread_mutex_lock(&mutex);

    if (front == rear) {
        pthread_mutex_unlock(&mutex);

        // return -1 indicating an error condition
        return -1;
    }

    // place it in item
    *item = buffer[front];

    front = (front + 1) % BUFFER_SIZE;
    
    pthread_mutex_unlock(&mutex);
    sem_post(&empty);

    // return 0 if successful, otherwise
    return 0;
}

void buffer_init() {
    front = rear = 0;

    // initialize mutual-exclusion locks and semaphores
    sem_init(&full, 0, 0);
    sem_init(&empty, 0, BUFFER_SIZE);

    pthread_mutex_init(&mutex, NULL);

    printf("buffer initialized\n");
}

void *producer(void *param) {
    buffer_item item;

    while (TRUE) {
        // sleep for a random period of time
        sleep(rand() % MAX_SLEEP_TIME);

        // generate a random number
        item = rand();

        if (insert_item(item))
            fprintf(stderr, "producer produced error");
        else
            printf("producer produced %d\n",item);
    }
}

void *consumer(void *param) {
    buffer_item item;

    while (TRUE) {
        // sleep for a random period of time
        sleep(rand() % MAX_SLEEP_TIME);

        if (remove_item(&item))
            fprintf(stderr, "consumer consumed error");
        else
            printf("consumer consumed %d\n",item);
    }
}