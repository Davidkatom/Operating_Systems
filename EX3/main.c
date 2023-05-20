#include "bounded_buffer.h"
#include <stdio.h>
#include <pthread.h>
#include <malloc.h>

#define BUFFER_SIZE 5
#define NUM_ITEMS 10

BoundedBuffer *buffer;

void *producer(void *arg) {
    for (int i = 0; i < NUM_ITEMS; i++) {
        printf("Producing item %d\n", i);
        insert(buffer, i);
    }
    return NULL;
}

void *consumer(void *arg) {
    for (int i = 0; i < NUM_ITEMS; i++) {
        int item = remove(buffer);
        printf("Consumed item %d\n", item);
    }
    return NULL;
}

int main() {
    buffer = createBuffer(BUFFER_SIZE);

    pthread_t producerThread, consumerThread;
    pthread_create(&producerThread, NULL, producer, NULL);
    pthread_create(&consumerThread, NULL, consumer, NULL);

    pthread_join(producerThread, NULL);
    pthread_join(consumerThread, NULL);

    free(buffer->buffer);
    free(buffer);

    return 0;
}
