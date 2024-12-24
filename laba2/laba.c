#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>

#include <time.h>
#include <limits.h>

#define MAX_THREADS 8  // Максимальное количество потоков

typedef struct {
    int *array;
    int left;
    int right;
    int max_threads;
} ThreadData;

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
int active_threads = 0;


void *thread_merge_sort(void *arg);

void merge(int *array, int left, int mid, int right) {
    int n1 = mid - left + 1;
    int n2 = right - mid;

    int *L = calloc(n1, sizeof(int));
    int *R = calloc(n2, sizeof(int));

    if (L == NULL || R == NULL) {
        fprintf(stderr, "Memory allocation error\n");
        exit(EXIT_FAILURE);
    }

    memcpy(L, &array[left], n1 * sizeof(int));
    memcpy(R, &array[mid + 1], n2 * sizeof(int));

    int i = 0, j = 0, k = left;
    while (i < n1 && j < n2) {
        if (L[i] <= R[j]) {
            array[k++] = L[i++];
        } else {
            array[k++] = R[j++];
        }
    }

    while (i < n1) array[k++] = L[i++];
    while (j < n2) array[k++] = R[j++];

    free(L);
    free(R);
}

void merge_sort(int *array, int left, int right, int max_threads) {
    if (left < right) {
        int mid = left + (right - left) / 2;

        if (max_threads > 1) {
            pthread_t thread;
            ThreadData data = {array, left, mid, (max_threads > 1) ? max_threads / 2 : 1};

            pthread_mutex_lock(&mutex);
            if (active_threads >= MAX_THREADS) {
                pthread_mutex_unlock(&mutex);
                merge_sort(array, left, mid, 1);
            } else {
                active_threads++;
                pthread_mutex_unlock(&mutex);

                pthread_create(&thread, NULL, thread_merge_sort, &data);
                merge_sort(array, mid + 1, right, (max_threads > 1) ? max_threads / 2 : 1);

                pthread_join(thread, NULL);

                pthread_mutex_lock(&mutex);
                active_threads--;
                pthread_mutex_unlock(&mutex);
            }
        } else {
            merge_sort(array, left, mid, 1);
            merge_sort(array, mid + 1, right, 1);
        }

        merge(array, left, mid, right);
    }
}

void *thread_merge_sort(void *arg) {
    ThreadData *data = (ThreadData *)arg;
    merge_sort(data->array, data->left, data->right, data->max_threads);
    return NULL;
}

void write_array(int *array, int size) {
    for (int i = 0; i < size; i++) {
        printf("%d ", array[i]);
    }
    printf("\n");
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("Not enough arguments\n");
        exit(EXIT_FAILURE);
    }

    long temp = strtol(argv[1], NULL, 10);
    if (temp <= 0 || temp > INT_MAX) {
        printf("Invalid first argument\n");
        exit(EXIT_FAILURE);
    }
    int array_size = (int)temp;

    temp = strtol(argv[2], NULL, 10);
    if (temp <= 0 || temp > MAX_THREADS) {
        printf("Invalid second argument\n");
        exit(EXIT_FAILURE);
    }
    int max_threads = (int)temp;

    int *array = calloc(array_size, sizeof(int));
    if (array == NULL) {
        printf("Memory error\n");
        exit(EXIT_FAILURE);
    }

    srand(time(NULL));
    for (int i = 0; i < array_size; i++) {
        array[i] = rand() % 100;
    }

    printf("Original array: ");
    write_array(array, array_size);

    merge_sort(array, 0, array_size - 1, max_threads);

    printf("Sorted array: ");
    write_array(array, array_size);

    free(array);
    pthread_mutex_destroy(&mutex);
    return 0;
}