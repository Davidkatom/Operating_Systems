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

Producer* producer(int id, int size){
    Producer *prod = malloc(sizeof(BoundedBuffer));
    prod->buffer = createBuffer(size);
    prod->id = id;

    char* type = TYPES[rand()%3 + 1];
    printf("%s",type);

}