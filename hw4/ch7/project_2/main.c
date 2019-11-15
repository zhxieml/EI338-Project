#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include "producer_consumer.h"
#include "buffer.h"


int main(int argc, char *argv[]) {
    int sleep_time, pro_num, con_num;

    // get command line arguments
    if (argc != 4) {
        printf("Invalid input! An valid example is 'sleep_time pro_num con_num'\n");
        return 0;
    }

    sleep_time = atoi(argv[1]);
    pro_num = atoi(argv[2]);
    con_num = atoi(argv[3]);

    printf("sleep_time [%d] pro_num [%d] con_num [%d]\n", sleep_time, pro_num, con_num);

    // initialize the producer_consumer
    buffer_init();

    // create producer threads
    pthread_t pids[pro_num];

    for (int i = 0; i < pro_num; ++i) {
        pthread_create(&pids[i], NULL, &producer, NULL);
    }

    // create consumer threads
    pthread_t cids[con_num];

    for (int i = 0; i < con_num; ++i) {
        pthread_create(&cids[i], NULL, &consumer, NULL);
    }

    // sleep for a period of time
    sleep(sleep_time);

    // cancel and join threads
    for(int i = 0; i < pro_num; ++i) {
        pthread_cancel(pids[i]);
        pthread_join(pids[i], NULL);
    }

    for(int i = 0; i < con_num; ++i) {
        pthread_cancel(cids[i]);
        pthread_join(cids[i], NULL);
    }

    return 0;
}