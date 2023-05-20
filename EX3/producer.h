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
}Producer;
//Producer *CreateProducer(int id, int size);
Producer* CreateProducer(int id, int size);
void *CreateItems(void* prod);
#endif //EX3_PRODUCER_H
