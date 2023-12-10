#include <stdio.h>
#include <stdbool.h>
#include <malloc.h>
#include <pthread.h>
#include <stdlib.h>

typedef struct buffer_t {
    int *array;
    int capacity;
    int index;
} buffer_t;

void buffer_init(buffer_t *buff, int capacity) {
    buff->array = malloc(sizeof(int) * capacity);
    buff->capacity = capacity;
    buff->index = 0;
}

void buffer_destroy(buffer_t *buff) {
    free(buff->array);
}

bool buffer_push(buffer_t *buff, int data) {
    if(buff->index < buff->capacity) {
        buff->array[buff->index] = data;
        buff->index++;
        return true;
    }
    return false;
}

int buffer_pull(buffer_t *buff) {
    if(buff->index > 0) {
        return buff->array[--buff->index];
    }
    return -1;
}

typedef struct thread_data_t {
    pthread_mutex_t mutex;
    pthread_cond_t producer;
    pthread_cond_t consumer;
    buffer_t buff;
}thread_data_t;

void thread_data_init(thread_data_t *data, int capacity) {
    pthread_mutex_init(&data->mutex, NULL);
    pthread_cond_init(&data->consumer, NULL);
    pthread_cond_init(&data->producer, NULL);
    buffer_init(&data->buff, capacity);
}

void thread_data_destroy(thread_data_t *data) {
    pthread_mutex_destroy(&data->mutex);
    pthread_cond_destroy(&data->consumer);
    pthread_cond_destroy(&data->producer);
    buffer_destroy(&data->buff);
}

int generujCisla(int min, int max) {
    float cislo = rand() / (float) RAND_MAX;
    return min + cislo * (max - min);
}

void* consumer_fun(void* data) {
    thread_data_t* data_t = (thread_data_t*) data;

    for(int i = 0; i < 10; i++) {
        pthread_mutex_lock(&data_t->mutex);
        int cislo = generujCisla(1,10);
        //printf("cislo je %d \n", cislo);
        while(!buffer_push(&data_t->buff, cislo)) {
            pthread_cond_wait(&data_t->consumer, &data_t->mutex);
        }

        pthread_mutex_unlock(&data_t->mutex);
        pthread_cond_signal(&data_t->producer);

    }
}

void prvociselnyRozklad(int cislo) {
    printf("->");
    for(int i = 2; i <= cislo; ++i) {
        while(cislo % i == 0) {
            printf(" %d", i);
            cislo /= i;
        }
    }
}

void* producer_fun(void* data) {
    thread_data_t* data_t = (thread_data_t*) data;

    for(int i = 0; i < 10; i++) {
        pthread_mutex_lock(&data_t->mutex);

        while(data_t->buff.index == 0) {
            pthread_cond_wait(&data_t->producer, &data_t->mutex);
        }

        int cislo = buffer_pull(&data_t->buff);
        printf("rozklad pre cislo %d", cislo);
        prvociselnyRozklad(cislo);
        printf("\n");
        pthread_mutex_unlock(&data_t->mutex);
        pthread_cond_signal(&data_t->consumer);
    }
}


int main() {

    thread_data_t data;
    thread_data_init(&data, 10);

    pthread_t producer, consumer;

    pthread_create(&consumer, NULL, consumer_fun, &data);
    pthread_create(&producer, NULL, producer_fun, &data);


    pthread_join(producer, NULL);
    pthread_join(consumer, NULL);

    thread_data_destroy(&data);

    return 0;
}
