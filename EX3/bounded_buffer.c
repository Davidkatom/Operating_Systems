//TODO write name
//
// Created by david on 5/20/23.
//

#include "bounded_buffer.h"
#include <semaphore.h>
#include <malloc.h>

BoundedBuffer* createBuffer(int size) {
    BoundedBuffer *buffer = malloc(sizeof(BoundedBuffer));
    buffer->buffer = malloc(sizeof(int) * size);
    buffer->size = size;
    sem_init(&buffer->mutex, 0, 1);
    sem_init(&buffer->empty, 0, size);
    sem_init(&buffer->full, 0, 0);
    buffer->in = 0;
    buffer->out = 0;
    return buffer;
}

void insertI(BoundedBuffer *buffer, int item) {
    sem_wait(&buffer->empty);
    sem_wait(&buffer->mutex);
    buffer->buffer[buffer->in] = item;
    buffer->in = (buffer->in + 1) % buffer->size;
    sem_post(&buffer->mutex);
    sem_post(&buffer->full);
}

int removeI(BoundedBuffer *buffer) {
    sem_wait(&buffer->full);
    sem_wait(&buffer->mutex);
    int item = buffer->buffer[buffer->out];
    buffer->out = (buffer->out + 1) % buffer->size;
    sem_post(&buffer->mutex);
    sem_post(&buffer->empty);
    return item;
}
