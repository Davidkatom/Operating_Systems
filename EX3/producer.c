//
// Created by david on 5/20/23.
//

#include <malloc.h>
#include <stdlib.h>
#include "producer.h"
const char* TYPES[] = {
        "SPORTS",
        "WEATHER",
        "NEWS"
};

Producer* CreateProducer(int id, int size){
    Producer *prod = malloc(sizeof(Producer));
    prod->buffer = CreateBuffer(5);
    //prod->buffer = buffer;
    prod->id = id;
    prod->size = size;
    return prod;
}
void* CreateItems(void* args){
    Producer* prod = (Producer*)args;  // Cast the argument back to the desired type
    char* type = TYPES[rand()%3];
    //printf("%s",type);
    for(int i = 0; i < prod->size; i++){
        char item[50];
        sprintf(item,"Producer %d %s %d", prod->id, type, i);
        printf("Produces item: %s\n", item);
        insertI(prod->buffer,item);
    }
    insertI(prod->buffer,"DONE");

    return NULL;
}