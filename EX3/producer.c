//
// Created by david on 5/20/23.
//

#include <malloc.h>
#include <stdlib.h>
#include <time.h>
#include "producer.h"
#include <unistd.h>
#include <pthread.h>

const char* TYPES[] = {
        "SPORTS",
        "WEATHER",
        "NEWS"
};

Producer* CreateProducer(int id, int num_products, int queue_size, pthread_mutex_t* lock){
    Producer *prod = malloc(sizeof(Producer));
    prod->buffer = CreateBuffer(queue_size);
    //prod->buffer = buffer;
    prod->id = id;
    prod->size = num_products;
    prod->lock = lock;
    return prod;
}
void* CreateItems(void* args){
    Producer *prod;
    prod = (Producer *) args;  // Cast the argument back to the desired type
    int counts[] = {0, 0, 0};
    for(int i = 0; i < prod->size; i++){
        //pthread_mutex_lock(prod->lock);   // lock the mutex
        int rnd = rand()%3;
        char* type = TYPES[rnd];
        char item[50];
        sprintf(item,"Producer: %d %s %d", prod->id, type, counts[rnd]);
        //printf("%s\n",item);
        counts[rnd]+=1;
        insertI(prod->buffer,item);
        //pthread_mutex_unlock(prod->lock); // unlock the mutex

        //sem_post(&mutex);

    }
    insertI(prod->buffer,"DONE");
    //printf("Poducer %d DONE\n", prod->id);
    return NULL;
}

void *testprint(void* args){
    Producer *prod;
    prod = (Producer *) args;  // Cast the argument back to the desired type
    for(int i = 0; i<8;i++){
        pthread_mutex_lock(prod->lock);   // lock the mutex
        usleep(100000);
        char output[6];
        printf("TEST\r\n");
        fflush(stdout); // flush the buffer

        pthread_mutex_unlock(prod->lock); // unlock the mutex
    }
    return NULL;
}

