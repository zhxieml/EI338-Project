/**
 * Example client program that uses thread pool.
 */

#include <stdio.h>
#include <unistd.h>
#include "threadpool.h"

struct data {
    int a;
    int b;
};

void subtract(void *param) {
    struct data *temp;
    temp = (struct data*)param;

    printf("I subtract %d from %d result = %d\n",temp->a, temp->b, temp->b - temp->a);
}

void add(void *param) {
    struct data *temp;
    temp = (struct data*)param;

    printf("I add two values %d and %d result = %d\n",temp->a, temp->b, temp->a + temp->b);
}

int main(void) {
    int sleep_time;

    // create some work to do
    struct data work;
    work.a = 5;
    work.b = 10;

    // initialize the thread pool
    pool_init();

    // submit the works to the queue
    for (int i = 0; i < 10; ++i) {
        pool_submit(&add, &work);
        pool_submit(&subtract, &work);
    }

    // may be helpful 
    sleep_time = 1;
    printf("Sleep time: %d\n", sleep_time);
    sleep(sleep_time);

    pool_shutdown();

    return 0;
}
