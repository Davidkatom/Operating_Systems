#include "bounded_buffer.h"
#include "producer.h"
#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <string.h>

#define BUFFER_SIZE 5
#define NUM_ITEMS 10

BoundedBuffer* buffer;



void* con(void* args) {
    Producer* prod = (Producer*)args;  // Cast the argument back to the desired type
    BoundedBuffer* buffer = prod->buffer;
    while(1){
        char* item = removeI(buffer);
        if(strcmp(item,"DONE") == 0){
            free(item); // Remember to free the memory allocated for the string
            return NULL;
        }

        printf("Consumed item: %s\n", item);
        free(item); // Remember to free the memory allocated for the string
    }
}

int main() {
    //buffer = createBuffer(5);
    Producer* producer = CreateProducer(0,10);
    pthread_t producerThread, consumerThread;
    pthread_create(&producerThread, NULL, CreateItems, producer);
    //pthread_create(&consumerThread, NULL, pr, producer);
    pthread_create(&consumerThread, NULL, con, producer);


    pthread_join(producerThread, NULL);
    pthread_join(consumerThread, NULL);


    return 0;
}
