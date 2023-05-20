//
// Created by david on 5/20/23.
//

#ifndef EX3_DISPATCHER_H
#define EX3_DISPATCHER_H
#include "bounded_buffer.h"
#include "producer.h"
#include <malloc.h>
#include <limits.h>
#include <string.h>
#include <pthread.h>

typedef struct {
    Producer** prods;
    BoundedBuffer* newsBuffer;
    BoundedBuffer* weatherBuffer;
    BoundedBuffer* sportsBuffer;

}Dispatcher;
Dispatcher *CreateDispatchers(Producer** prods);
void* ProcessProducers(void* args);
#endif //EX3_DISPATCHER_H
