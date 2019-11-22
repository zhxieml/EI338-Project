#define NUMBER_OF_CUSTOMER 5
#define NUMBER_OF_RESOURCE 4
#define TRUE 1
#define FALSE 0
#define MAX_CMD 20

// grant a request if it satisfies the safety algorithm
// deny a request if it does not leave the system in a saft state
int request_resource(int customer_id, int update[]);
int release_resource(int customer_id, int update[]);
int is_safe();