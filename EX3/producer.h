//
// Created by david on 5/20/23.
//

#ifndef EX3_PRODUCER_H
#define EX3_PRODUCER_H

#include "bounded_buffer.h"

typedef struct {
    int id;
    int size;
    BoundedBuffer* buffer;
    pthread_mutex_t* lock;

}Producer;
//Producer *CreateProducer(int id, int size);
Producer* CreateProducer(int id, int num_products, int queue_size, pthread_mutex_t* lock);
void *CreateItems(void* prod);
void *testprint(void* args);
#endif //EX3_PRODUCER_H
