//
// Created by david on 5/20/23.
//
#include <unistd.h>
#include "dispatcher.h"

CoEditor *CreateCoEditor(int type, ScreenManager* screenManager, int queueSize){
    CoEditor *coEditor = malloc(sizeof(CoEditor));
    coEditor->buffer = CreateBuffer(queueSize);
    coEditor->sm = screenManager;
    coEditor->type = type;
    return coEditor;
}

Dispatcher *CreateDispatchers(Producer** prods, int numOfProds, ScreenManager *sm, int queueSize){
    Dispatcher *dispatcher = malloc(sizeof(Dispatcher));

    dispatcher->newsEditor = CreateCoEditor(0,sm,queueSize);
    dispatcher->weatherEditor = CreateCoEditor(1,sm,queueSize);
    dispatcher->sportsEditor = CreateCoEditor(2,sm,queueSize);

    dispatcher->prods = prods;
    dispatcher->numOfProds = numOfProds;
    return dispatcher;
}

void* ProcessProducers(void* args) {
    Dispatcher* dispatcher = (Dispatcher*)args;
    int i = 0;
    while (1) {
        if(dispatcher->numOfProds == 0)
            break;
        if(i == dispatcher->numOfProds)
            i = 0;
        Producer* prod = dispatcher->prods[i];

        // Loop back to the first producer if we've reached the end of the array

        // Try to remove an item from the current producer's buffer
        char* item = removeI(prod->buffer);

        // If the producer is done producing items, free it and remove it from the array
        if (strcmp(item, "DONE") == 0) {

            free(prod->buffer->buffer);
            free(prod->buffer);
            free(prod);
            // Shift all elements in the array down by one
            for (int j = i; j <= dispatcher->numOfProds; j++) {
                dispatcher->prods[j] = dispatcher->prods[j + 1];
            }
            dispatcher->numOfProds--;
            prod = NULL;
            continue;
        }

        // Determine the type of the item and add it to the appropriate buffer in the dispatcher
        char type[256] = {0};  // This initializes the whole array to '\0'
        sscanf(item, "Producer: %*d %s %*d\n", type);
        if (strcmp(type, "SPORTS") == 0) {
            insertI(dispatcher->sportsEditor->buffer, item);
        } else if (strcmp(type, "WEATHER") == 0) {
            insertI(dispatcher->weatherEditor->buffer, item);

        } else if (strcmp(type, "NEWS") == 0) {
            insertI(dispatcher->newsEditor->buffer, item);

        }
        //printf("Processed %s\n", item);
        i++;
    }

    insertI(dispatcher->sportsEditor->buffer, "DONE");
    insertI(dispatcher->weatherEditor->buffer, "DONE");
    insertI(dispatcher->newsEditor->buffer, "DONE");

    return NULL;
}

void* CoEdit(void* args){
    CoEditor *editor = (CoEditor *)args;
    while(1){
        //printf("editor: %d\n",editor->type);
        char* item = removeI(editor->buffer);

        if(strcmp(item, "DONE") == 0){
            insertI(editor->sm->buffer, "DONE");
            break;
        }
        usleep(100000); // Sleep for 100,000 microseconds (0.1 seconds)
        insertI(editor->sm->buffer, item);


    }
    return NULL;
}




