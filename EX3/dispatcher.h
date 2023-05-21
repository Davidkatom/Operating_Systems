//
// Created by david on 5/20/23.
//

#ifndef EX3_DISPATCHER_H
#define EX3_DISPATCHER_H
#include "bounded_buffer.h"
#include "screenManager.h"
#include "producer.h"
#include <malloc.h>
#include <limits.h>
#include <string.h>
#include <pthread.h>
typedef struct{
    int type;
    BoundedBuffer* buffer;
    ScreenManager* sm;
}CoEditor;

typedef struct {
    Producer** prods;
    int numOfProds;
    CoEditor* sportsEditor;
    CoEditor* newsEditor;
    CoEditor* weatherEditor;

}Dispatcher;

void* CoEdit(void* args);
Dispatcher *CreateDispatchers(Producer** prods, int numOfProds, ScreenManager *sm, int queueSize);
void* ProcessProducers(void* args);
#endif //EX3_DISPATCHER_H
