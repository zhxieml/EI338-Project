#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

int data[10];
int sorted_data[10];

/* structure for passing data to threads */
typedef struct {
    int low;
    int high;
} parameters;

/* sorting threads call this function */
void *sorting(void *param);

/* merging threads call this function */
void *merging(void *param);

int main() {
    FILE *fp = fopen("test_2.txt", "r");

    parameters sort_data[2], merge_data;
    pthread_t sort_tids[2], merge_tid;
    pthread_attr_t sort_attrs[2], merge_attr;

    int m = 5;

    /*load the sudoku girds */
	for (int i = 0; i < 10; ++i) {
		fscanf(fp, "%d", &data[i]);
	}

    /* initialize data */
    sort_data[0].low = 0;
    sort_data[0].high = m;
    sort_data[1].low = m;
    sort_data[1].high = 10;
    merge_data.low = 0;
    merge_data.high = 10;
    
    /* set the default attributes of the threads */
    pthread_attr_init(&sort_attrs[0]);
    pthread_attr_init(&sort_attrs[1]);
    pthread_attr_init(&merge_attr);

    /* create the thread */
    pthread_create(&sort_tids[0], &sort_attrs[0], sorting, &sort_data[0]);
    pthread_create(&sort_tids[1], &sort_attrs[1], sorting, &sort_data[1]);

    /* wait for the thread to exit */
    pthread_join(sort_tids[0], NULL);
    pthread_join(sort_tids[1], NULL);

    /* merge */
	pthread_create(&merge_tid, &merge_attr, merging, &merge_data);
	pthread_join(merge_tid, NULL);

	/* output */
	for (int i = 0; i < 10; ++i) {
        printf("%d ", sorted_data[i]);
    }
		
	printf("\n");

    return 0;
}

/* sort data */
void *sorting(void *param) {
    parameters *param_tmp = (parameters*) param;
    int min_index, tmp;
	
    for (int i = param_tmp->low; i < param_tmp->high - 1; ++i) {
		min_index = i;
		
        for(int j = i + 1; j < param_tmp->high; ++j) {
			if(data[min_index] > data[j]) {
				min_index = j;
			}
		}

		tmp = data[i];
		data[i] = data[min_index];
		data[min_index] = tmp;
	}
}

/* merge data */
void *merging(void *param) {
    parameters *param_tmp = (parameters*) param;

	int m = (param_tmp->low + param_tmp->high) / 2;
	int i = param_tmp->low;
	int j = m;
	int k = i;

    while (i < m || j < param_tmp->high)
    {        
        if (i == m) {
            sorted_data[k++] = data[j++];
            continue;
        }
        
        if (j == param_tmp->high) {
            sorted_data[k++] = data[i++];
            continue;
        }
        
        if (data[i] < data[j]) {
            sorted_data[k++] = data[i++];
        }
        else {
            sorted_data[k++] = data[j++];
        }
    }
}