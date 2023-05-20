#include <semaphore.h>
#include <stdlib.h>
#include <string.h>
#include "bounded_buffer.h"


BoundedBuffer* CreateBuffer(int size) {
    BoundedBuffer* buffer = malloc(sizeof(BoundedBuffer));
    buffer->buffer = malloc(sizeof(char*) * size);
    buffer->size = size;
    sem_init(&buffer->mutex, 0, 1);
    sem_init(&buffer->empty, 0, size);
    sem_init(&buffer->full, 0, 0);
    buffer->in = 0;
    buffer->out = 0;
    return buffer;
}

void insertI(BoundedBuffer* buffer, char* item) {
    sem_wait(&buffer->empty);
    sem_wait(&buffer->mutex);
    buffer->buffer[buffer->in] = strdup(item);
    buffer->in = (buffer->in + 1) % buffer->size;
    sem_post(&buffer->mutex);
    sem_post(&buffer->full);
}

char* removeI(BoundedBuffer* buffer) {
    sem_wait(&buffer->full);
    sem_wait(&buffer->mutex);
    char* item = strdup(buffer->buffer[buffer->out]);
    free(buffer->buffer[buffer->out]);
    buffer->out = (buffer->out + 1) % buffer->size;
    sem_post(&buffer->mutex);
    sem_post(&buffer->empty);
    return item;
}
