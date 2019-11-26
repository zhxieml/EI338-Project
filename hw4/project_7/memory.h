#define TRUE 1
#define FALSE 0
#define MAX_CMD 20
// #define INT_MAX 2147483647
// #define INT_MIN -2147483648

typedef struct node {
    int low;
    int high;
    char process[5];

    struct node *next;
} Node;