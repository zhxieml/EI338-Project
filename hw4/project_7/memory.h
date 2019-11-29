#define TRUE 1
#define FALSE 0
#define MAX_CMD 20
#define INT_MAX 2147483647
#define INT_MIN -2147483648

typedef struct node {
    int low;
    int high;
    char process[5];

    struct node *next;
} Node;

void report_stat(void);
void compact(void);
int request_memory(char *process, int space, char *strategy);
int release_memory(char *process);
int parse_cmd(char *cmd, char *process, int *space, char *strategy);