#include "bounded_buffer.h"
#include "producer.h"
#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#include "dispatcher.h"

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

    char buffer[256];
    int producerCount = 0;
    int coEditorQueueSize = 0;

    while(fgets(buffer, sizeof(buffer), config)) {
        if(strstr(buffer, "PRODUCER") != NULL)
            producerCount++;
        else if(strstr(buffer, "Co-Editor queue size") != NULL)
            sscanf(buffer, "Co-Editor queue size = %d", &coEditorQueueSize);
    }

    rewind(config);

    pthread_t dispatcherThread;
    pthread_t* producerThreads= malloc(sizeof(pthread_t)*producerCount) ;
    Producer** producers = malloc(sizeof(Producer*) * producerCount);

    int i = 0;
    int numItems, queueSize;
    while(fgets(buffer, sizeof(buffer), config)) {
        if(strstr(buffer, "PRODUCER") != NULL) {
            fgets(buffer, sizeof(buffer), config);
            sscanf(buffer, "%d", &numItems);

            fgets(buffer, sizeof(buffer), config);
            sscanf(buffer, "queue size = %d", &queueSize);

            Producer* producer = CreateProducer(i+1,numItems, queueSize);
            producers[i] = producer;
            pthread_create(&producerThreads[i], NULL, CreateItems, producer);
            i++;
        }
    }


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

//    pthread_join(sportsEditor, NULL);
//    pthread_join(weatherEditor, NULL);
//    pthread_join(newsEditor, NULL);
    pthread_join(screenManagerThread, NULL);

    fclose(config);
    return 0;
}
