//
// Created by david on 5/20/23.
//

#include <malloc.h>
#include <stdlib.h>
#include <time.h>
#include "producer.h"
const char* TYPES[] = {
        "SPORTS",
        "WEATHER",
        "NEWS"
};

Producer* CreateProducer(int id, int num_products, int queue_size){
    Producer *prod = malloc(sizeof(Producer));
    prod->buffer = CreateBuffer(queue_size);
    //prod->buffer = buffer;
    prod->id = id;
    prod->size = num_products;
    return prod;
}
void* CreateItems(void* args){
    Producer* prod = (Producer*)args;  // Cast the argument back to the desired type
    char* type = TYPES[rand()%3];
    //printf("%s",type);
    for(int i = 0; i < prod->size; i++){
        char item[50];
        sprintf(item,"Producer %d %s %d", prod->id, type, i);
        insertI(prod->buffer,item);
    }
    insertI(prod->buffer,"DONE");
    //printf("Poducer %d DONE\n", prod->id);
    return NULL;
}