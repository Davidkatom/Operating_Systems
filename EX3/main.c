#include "bounded_buffer.h"
#include "producer.h"
#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#include "dispatcher.h"

#define BUFFER_SIZE 5
#define NUM_ITEMS 10
#define NUM_PRODUCERS 10
int main() {
    pthread_t dispatcherThread;
    pthread_t* producerThreads= malloc(sizeof(pthread_t)*NUM_PRODUCERS) ;

    Producer** producers = malloc(sizeof(Producer*) * NUM_PRODUCERS);
    for(int i = 0; i <= NUM_PRODUCERS; i++){
        Producer* producer = CreateProducer(i,6);
        producers[i] = producer;
        pthread_create(&producerThreads[i], NULL, CreateItems, producer);
    }
    Dispatcher* dispatcher = CreateDispatchers(producers,NUM_PRODUCERS);
    pthread_create(&dispatcherThread, NULL, ProcessProducers, dispatcher);

    pthread_t sportsEditor;
    pthread_t weatherEditor;
    pthread_t newsEditor;


    pthread_create(&sportsEditor, NULL, CoEdit, dispatcher->sportsEditor);
    pthread_create(&weatherEditor, NULL, CoEdit, dispatcher->weatherEditor);
    pthread_create(&newsEditor, NULL, CoEdit, dispatcher->newsEditor);


    pthread_join(dispatcherThread, NULL);

    pthread_join(sportsEditor, NULL);
    pthread_join(weatherEditor, NULL);
    pthread_join(newsEditor, NULL);



    return 0;
}