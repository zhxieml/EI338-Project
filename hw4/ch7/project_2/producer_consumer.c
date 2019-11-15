#include <stdio.h>
#include <unistd.h>
#include <stdlib.h> /* required for rand() */
#include "buffer.h"
#include "producer_consumer.h"

#define MAX_SLEEP_TIME 2

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
            printf("producer produced %d∖n",item);
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
            printf("consumer consumed %d∖n",item);
    }
}

// void producer_consumer_init() {
//     // initialize the buffer
//     buffer_init();

//     printf("producer_consumer initialied\n");
// }

// void producer_create(int pro_num) {
//     pthread_t pids[pro_num];

//     for (int i = 0; i < pro_num; ++i) {
//         pthread_create(&pids[i], NULL, &producer, NULL);
//     }
// }

// void consumer_create(int con_num) {
//     pthread_t cids[con_num];

//     for (int i = 0; i < con_num; ++i) {
//         pthread_create(&cids[i], NULL, &consumer, NULL);
//     }
// }
