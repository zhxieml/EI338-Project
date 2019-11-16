#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define ref_ans 1022    // (1111111110)_2; used for validating

int is_valid[27];
int grids[9][9];

/* structure for passing data to threads */
typedef struct {
    int row;
    int column;
} parameters;

/* threads call this function */
void *runner(void *param);

int main() {
    FILE *fp = fopen("test.txt", "r");	

    parameters data[3][9];
    pthread_t tids[3][9];
    pthread_attr_t attrs[3][9];

    int res = 1;
    
    /*load the sudoku girds */
	for (int i = 0; i < 81; ++i) {
		fscanf(fp, "%d", &grids[i/9][i%9]);
	}

	fclose(fp);

    /* assign threads for column validators */
    for (int i = 0; i < 9; ++i) {
        /* set the data */
        data[0][i].row = -1;
        data[0][i].column = i;

        /* set the default attributes of the threads */
        pthread_attr_init(&attrs[0][i]);

        /* create the thread */
        pthread_create(&tids[0][i], &attrs[0][i], runner, &data[0][i]);

        /* wait for the thread to exit */
        pthread_join(tids[0][i], NULL);
    }

    /* assign threads for row validators */
    for (int i = 0; i < 9; ++i) {
        /* set the data */
        data[1][i].row = i;
        data[1][i].column = -1;

        /* set the default attributes of the threads */
        pthread_attr_init(&attrs[1][i]);

        /* create the thread */
        pthread_create(&tids[1][i], &attrs[1][i], runner, &data[1][i]);

        /* wait for the thread to exit */
        pthread_join(tids[1][i], NULL);
    }

    /* assign threads for subgrid validators */
    for (int i = 0; i < 9; ++i) {
        /* set the data */
        data[2][i].row = i / 3;
        data[2][i].column = i % 3;

        /* set the default attributes of the threads */
        pthread_attr_init(&attrs[2][i]);

        /* create the thread */
        pthread_create(&tids[2][i], &attrs[2][i], runner, &data[2][i]);

        /* wait for the thread to exit */
        pthread_join(tids[2][i], NULL);
    }

    /* check if all the validators give positive results */
    for (int i = 0; i < 27; ++i) {
        res &= is_valid[i];
    }

    printf((res?"Valid\n":"Invalid\n"));

    return 0;
}

void *runner(void *param) {
    int ans = 0;
    int validator_index;
    parameters *param_tmp = (parameters*) param;

    /* validate for columns */
    if (param_tmp->row == -1) {
        for (int i = 0; i < 9; ++i) {
            ans |= (1 << grids[i][param_tmp->column]);
        }
        
        validator_index = param_tmp->column;
    }

    /* validate for rows */
    else if (param_tmp->column == -1) {
        for (int i = 0; i < 9; ++i) {
            ans |= (1 << grids[param_tmp->row][i]);
        }

        validator_index = param_tmp->row + 9;
    }

    /* validate for rows */
    else {
        for (int i = 0; i < 9; ++i) {
            ans |= (1 << grids[i/3+param_tmp->row*3][i%3+param_tmp->column*3]);
        }

        validator_index = param_tmp->column + param_tmp->row * 3 + 18;
    }

    is_valid[validator_index] = (ans == ref_ans);
}