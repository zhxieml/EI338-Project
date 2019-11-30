#define QUEUE_SIZE 50
#define NUMBER_OF_THREADS 3
#define TRUE 1

// this represents work that has to be 
// completed by a thread in the pool
typedef struct {
    void (*function)(void *p);
    void *data;
} task;

// function prototypes
void execute(void (*somefunction)(void *p), void *p);
int pool_submit(void (*somefunction)(void *p), void *p);
void *worker(void *param);
void pool_init(void);
void pool_shutdown(void);

int enqueue(task t);
task *dequeue();
