#ifndef BOUNDED_BUFFER_H
#define BOUNDED_BUFFER_H

#include <semaphore.h>

typedef struct {
    int *buffer;
    int size;
    sem_t mutex, empty, full;
    int in, out;
} BoundedBuffer;

BoundedBuffer* createBuffer(int size);
void insertI(BoundedBuffer *buffer, int item);
int removeI(BoundedBuffer *buffer);

#endif /* BOUNDED_BUFFER_H */
