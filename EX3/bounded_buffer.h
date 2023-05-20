#ifndef BOUNDED_BUFFER_H
#define BOUNDED_BUFFER_H

#include <semaphore.h>

typedef struct {
    char** buffer;
    int size;
    sem_t mutex, empty, full;
    int in, out;
} BoundedBuffer;

BoundedBuffer* CreateBuffer(int size);
void insertI(BoundedBuffer *buffer, char* item);
char* removeI(BoundedBuffer *buffer);

#endif /* BOUNDED_BUFFER_H */
