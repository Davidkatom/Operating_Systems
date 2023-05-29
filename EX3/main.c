#include "bounded_buffer.h"
#include "producer.h"
#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#include "dispatcher.h"
//David Shnaiderov 209198308

int main(int argc, char* argv[]) {
    if (argc < 2) {
        printf("Usage: %s <config file>\n", argv[0]);
        return 1;
    }

    FILE* config = fopen(argv[1], "r");
    if (config == NULL) {
        printf("Error: unable to open config file '%s'\n", argv[1]);
        return 1;
    }

    int producerCount = 0;
    int coEditorQueueSize = 0;
    char c;
    // count the number of producers (count '\n' characters and divide by 3)
    for (c = getc(config); c != EOF; c = getc(config)) {
        if (c == '\n') {
            producerCount++;
        }
    }
    producerCount = producerCount / 4; // each producer has 3 lines

    rewind(config);

    pthread_t dispatcherThread;
    pthread_t* producerThreads= malloc(sizeof(pthread_t)*producerCount) ;
    Producer** producers = malloc(sizeof(Producer*) * producerCount);

    int i = 0;
    int numItems, queueSize, id;
    char buffer[256];
    //printf("ProducerCount = %d\n", producerCount);
    pthread_mutex_t lock;
    pthread_mutex_init(&lock, NULL); // initialize the mutex

    while(i < producerCount) {
        fgets(buffer, sizeof(buffer), config);  // producer ID is not used
        sscanf(buffer, "%d", &id);
        //printf("id: %d\n",id);
        fgets(buffer, sizeof(buffer), config);  // number of products
        sscanf(buffer, "%d", &numItems);
        //printf("items: %d\n",id);

        fgets(buffer, sizeof(buffer), config);  // queue size
        sscanf(buffer, "%d", &queueSize);
        //printf("queueSize: %d\n",id);

        Producer* producer = CreateProducer(id,numItems, queueSize, &lock);
        producers[i] = producer;
        pthread_create(&producerThreads[i], NULL, CreateItems, producer);
        i++;
        fgets(buffer, sizeof(buffer), config);  // empty line
    }



//    i = 0;
//    while(i < producerCount) {
//        pthread_create(&producerThreads[i], NULL, testprint, producers[i]);
//        i++;
//    }


    // Co-Editor queue size is the last line
    fgets(buffer, sizeof(buffer), config);
    sscanf(buffer, "%d", &coEditorQueueSize);

    ScreenManager *screenManager = CreateScreenManager();
    Dispatcher* dispatcher = CreateDispatchers(producers,producerCount, screenManager, coEditorQueueSize);
    pthread_create(&dispatcherThread, NULL, ProcessProducers, dispatcher);

    pthread_t sportsEditor;
    pthread_t weatherEditor;
    pthread_t newsEditor;
    pthread_t screenManagerThread;

    pthread_create(&sportsEditor, NULL, CoEdit, dispatcher->sportsEditor);
    pthread_create(&weatherEditor, NULL, CoEdit, dispatcher->weatherEditor);
    pthread_create(&newsEditor, NULL, CoEdit, dispatcher->newsEditor);

    pthread_create(&screenManagerThread, NULL, PrintToScreen, screenManager);

    pthread_join(dispatcherThread, NULL);
    pthread_join(screenManagerThread, NULL);

    fclose(config);

//      free(producers);
    free(producerThreads);
    free(dispatcher->sportsEditor->buffer);
    free(dispatcher->weatherEditor->buffer);
    free(dispatcher->newsEditor->buffer);
    free(dispatcher->sportsEditor);
    free(dispatcher->weatherEditor);
    free(dispatcher->newsEditor);
    free(dispatcher);
    free(screenManager->buffer);
    free(screenManager);
    return 0;
}
