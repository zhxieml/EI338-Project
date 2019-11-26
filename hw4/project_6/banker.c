#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "banker.h"

// the available amount of each resource
int available_res[NUMBER_OF_RESOURCE];

// the maximum demand of each customer
int max_demand[NUMBER_OF_CUSTOMER][NUMBER_OF_RESOURCE];

// the amount currently allocated to each customer
int allocated_res[NUMBER_OF_CUSTOMER][NUMBER_OF_RESOURCE];

// the remaining need of each customer
int remain_demand[NUMBER_OF_CUSTOMER][NUMBER_OF_RESOURCE];

void display_info(void) {
    printf("---------------------------------------------------\n");
            
    // output available
    printf("[Available]\n");
    
    for (int i = 0; i < NUMBER_OF_RESOURCE; ++i) 
        printf("\tRES %d", i);
    
    printf("\n");

    for (int i = 0; i < NUMBER_OF_RESOURCE; ++i)
        printf("\t  %d", available_res[i]);

    printf("\n");

    // output maximum
    printf("[Maximum]\n");

    for (int i = 0; i < NUMBER_OF_RESOURCE; ++i) 
        printf("\tRES %d", i);

    for (int i = 0; i < NUMBER_OF_CUSTOMER; ++i) {
        printf("\nCUS %d", i);

        for (int j = 0; j < NUMBER_OF_RESOURCE; ++j)
            printf("\t  %d", max_demand[i][j]);
    }

    printf("\n");

    // output allocation
    printf("[Allocation]\n");

    for (int i = 0; i < NUMBER_OF_RESOURCE; ++i) 
        printf("\tRES %d", i);

    for (int i = 0; i < NUMBER_OF_CUSTOMER; ++i) {
        printf("\nCUS %d", i);

        for (int j = 0; j < NUMBER_OF_RESOURCE; ++j)
            printf("\t  %d", allocated_res[i][j]);
    }

    printf("\n");

    // output need
    printf("[Need]\n");

    for (int i = 0; i < NUMBER_OF_RESOURCE; ++i) 
        printf("\tRES %d", i);

    for (int i = 0; i < NUMBER_OF_CUSTOMER; ++i) {
        printf("\nCUS %d", i);

        for (int j = 0; j < NUMBER_OF_RESOURCE; ++j)
            printf("\t  %d", remain_demand[i][j]);
    }

    printf("\n");

    printf("---------------------------------------------------\n");
}

// parse the command
// return 1 if error
// return 2 if request
// return 3 if release
int parse_cmd(char *cmd, int *customer_id, int update[]) {
    char *token;
	int i = 0;
    int flag;

    // take the first token
    token = strtok(cmd, " "); 

    if (!token) 
        return 1;

    while (token) {
        if (i >= NUMBER_OF_RESOURCE + 2)
            return 1;

        if (i == 0) {
            if (!strcmp(token, "RQ")) 
                flag = 2;
            else if (!strcmp(token, "RL"))
                flag = 3;
            else 
                return 1;
        }

        else if (i == 1) 
            *customer_id = atoi(token);

        else 
            update[i - 2] = atoi(token);
        
        token = strtok(NULL, " ");
        ++i;
    }

    return flag;
}   

int is_safe() {
    int avalible_tmp[NUMBER_OF_RESOURCE];
    int finish_tmp[NUMBER_OF_CUSTOMER];
    int is_enough; // 1 if need_i <= work
    int res = TRUE; // 1 if the state is safe
    int finish; // 1 if the customer is finished

    // copy from available_res
    for (int i = 0; i < NUMBER_OF_RESOURCE; ++i) 
        avalible_tmp[i] = available_res[i];

    // test if customers are satisfied
    for (int i = 0; i < NUMBER_OF_CUSTOMER; ++i) {
        finish = TRUE;

        for (int j = 0; j < NUMBER_OF_RESOURCE; ++j)
            finish &= (remain_demand[i][j] == 0);

        finish_tmp[i] = finish;
    }
    
    // find an index
    for (int i = 0; i < NUMBER_OF_CUSTOMER; ++i) {
        is_enough = TRUE;

        for (int j = 0; j < NUMBER_OF_RESOURCE; ++j)
            is_enough &= (remain_demand[i][j] <= avalible_tmp[j]);

        if (!finish_tmp[i] && is_enough) {
            // update avaliable_tmp
            for (int j = 0; j < NUMBER_OF_RESOURCE; ++j) 
                avalible_tmp[j] += allocated_res[i][j];

            // update finish_tmp
            finish_tmp[i] = TRUE;

            // every time avaliable_tmp is updated, back to front
            i = 0;
        }
    }

    for (int i = 0; i < NUMBER_OF_CUSTOMER; ++i) 
        res &= finish_tmp[i];

    return res;
}

int request_resource(int customer_id, int update[]) {
    // check if the request is too greedy
    for (int i = 0; i < NUMBER_OF_RESOURCE; ++i) {
        if (remain_demand[customer_id][i] < update[i] || available_res[i] < update[i])
            return -1;
    }

    // update
    for (int i = 0; i < NUMBER_OF_RESOURCE; ++i) {
        available_res[i] -= update[i];
        allocated_res[customer_id][i] += update[i];
        remain_demand[customer_id][i] -= update[i];
    }

    if (!is_safe()) {
        // if not, recover the update by releasing the resources
        // release_resource(customer_id, update);
        release_resource(customer_id, update);

        return -1;
    }

    return 0;
}

int release_resource(int customer_id, int update[]) {
    // check if the release is affordable
    for (int i = 0; i < NUMBER_OF_RESOURCE; ++i) {
        if (allocated_res[customer_id][i] < update[i])
            return -1;
    }

    for (int i = 0; i < NUMBER_OF_RESOURCE; ++i) {
        available_res[i] += update[i];
        allocated_res[customer_id][i] -= update[i];
        remain_demand[customer_id][i] += update[i];
    }

    return 0;
}

int main(int argc, char *argv[]) {
    printf("*********************\n");
    printf("NUMBER_OF_RESOURCE: %d\n", NUMBER_OF_RESOURCE);
    printf("NUMBER_OF_CUSTOMER: %d\n", NUMBER_OF_CUSTOMER);
    printf("*********************\n");
    
    // pass the number of resources
    // 1th resource, 2th resource, ...
    if (argc != NUMBER_OF_RESOURCE + 1) {
        printf("Invalid input! Please check the number of input\n");
        return 1;
    }

    // initialize available_res
    for (int i = 0; i < NUMBER_OF_RESOURCE; ++i) 
        available_res[i] = atoi(argv[i + 1]);

    // read in a testing file to initialize max_demand
    // each line represents the maximum request for each customer
    FILE *fp = fopen("test.txt", "r");	
    
    for (int i = 0; i < 20; ++i) {
		fscanf(fp, "%d", &max_demand[i/4][i%4]);
        remain_demand[i/4][i%4] = max_demand[i/4][i%4];
    }

	fclose(fp);

    // have the user enter conmands
    // 'RQ' for requesting resources
    // 'RL' for releasing resources
    // '*' for outputing the values
    char cmd[MAX_CMD];
    int update[NUMBER_OF_RESOURCE];
    int customer_id;

    while (TRUE) {
        printf("Enter the command>");
		fflush(stdout);

        // read user input
        fgets(cmd, MAX_CMD, stdin);
        cmd[strlen(cmd) - 1] = '\0';

        // special cases
        if (!strcmp(cmd, "exit")) 
            return 0;
        
        else if (!strcmp(cmd, "*")) 
            display_info();
        
        else {
            int flag;
            flag = parse_cmd(cmd, &customer_id, update);

            switch (flag) {
                case 1:
                    printf("Invalid command!\n");
                    break;
                
                case 2:
                    if (request_resource(customer_id, update) == -1)
                        printf("Failed in request!\n");
                    else 
                        printf("Request accepted!\n");

                    break;

                case 3:
                    release_resource(customer_id, update);
                    break;
                
                default:
                    printf("Unknown error occurs\n");
            }
        }
    }

    return 0;
}