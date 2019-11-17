typedef int buffer_item;
#define BUFFER_SIZE 5
#define TRUE 1
#define MAX_SLEEP_TIME 2

void *producer(void *param);
void *consumer(void *param);

int insert_item(buffer_item item);
int remove_item(buffer_item *item);
void buffer_init();