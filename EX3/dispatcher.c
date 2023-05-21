//
// Created by david on 5/20/23.
//

#include <unistd.h>
#include "dispatcher.h"

CoEditor *CreateCoEditor(){
    CoEditor *coEditor = malloc(sizeof(CoEditor *));
    coEditor->buffer = CreateBuffer(10000);
    return coEditor;
}

Dispatcher *CreateDispatchers(Producer** prods, int numOfProds){
    Dispatcher *dispatcher = malloc(sizeof(Dispatcher));

    dispatcher->newsEditor = CreateCoEditor();
    dispatcher->weatherEditor = CreateCoEditor();
    dispatcher->sportsEditor = CreateCoEditor();

    dispatcher->prods = prods;
    dispatcher->numOfProds = numOfProds;
    return dispatcher;
}

void* ProcessProducers(void* args) {
    Dispatcher* dispatcher = (Dispatcher*)args;

    // Assuming that the producers are in an array and terminated by a NULL pointer
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

            // Shift all elements in the array down by one
            for (int j = i; j <= dispatcher->numOfProds; j++) {
                dispatcher->prods[j] = dispatcher->prods[j + 1];
            }
            dispatcher->numOfProds--;
            //free(prod);
            prod = NULL;
            continue;
        }

        // Determine the type of the item and add it to the appropriate buffer in the dispatcher
        char type[50];
        sscanf(item, "Producer %*d %s %*d", type);
        if (strcmp(type, "SPORTS") == 0) {
            insertI(dispatcher->sportsEditor->buffer, item);
        } else if (strcmp(type, "WEATHER") == 0) {
            insertI(dispatcher->weatherEditor->buffer, item);
        } else if (strcmp(type, "NEWS") == 0) {
            insertI(dispatcher->newsEditor->buffer, item);
        }
        printf("Processed %s\n", item);
        i++;
    }

    return NULL;
}

void* CoEdit(void* args){
    CoEditor *editor = (CoEditor *)args;
    int i = 0;
    while(1){
        char* item = removeI(editor->buffer);
        usleep(100000); // Sleep for 100,000 microseconds (0.1 seconds)
        printf("Edited number %d\n", i);
        i++;
    }
    return NULL;
}




