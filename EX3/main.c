#include "bounded_buffer.h"
#include "producer.h"
#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#include "dispatcher.h"

#define BUFFER_SIZE 5
#define NUM_ITEMS 10
#define NUM_PRODUCERS 3




//void* con(void* args) {
//    Producer* prod = (Producer*)args;  // Cast the argument back to the desired type
//    BoundedBuffer* buffer = prod->buffer;
//    while(1){
//        char* item = removeI(buffer);
//        if(strcmp(item,"DONE") == 0){
//            free(item); // Remember to free the memory allocated for the string
//            return NULL;
//        }
//
//        printf("Consumed item: %s\n", item);
//        free(item); // Remember to free the memory allocated for the string
//    }
//}

int main() {
    pthread_t dispatcherThread;
    pthread_t* producerThreads= malloc(sizeof(pthread_t)*NUM_PRODUCERS) ;

    Producer** producers = malloc(sizeof(Producer*) * NUM_PRODUCERS);
    for(int i = 0; i <= NUM_PRODUCERS; i++){
        Producer* producer = CreateProducer(i,10);
        producers[i] = producer;
        pthread_create(&producerThreads[i], NULL, CreateItems, producer);
    }
    Dispatcher* dispatcher = CreateDispatchers(producers,NUM_PRODUCERS);
    pthread_create(&dispatcherThread, NULL, ProcessProducers, dispatcher);

    //pthread_join(producerThread, NULL);
    pthread_join(dispatcherThread, NULL);
    for(int i = 0; i <= NUM_PRODUCERS; i++){
        pthread_join(producerThreads[i], NULL);
    }

    return 0;
}
