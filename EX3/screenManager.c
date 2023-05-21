//
// Created by david on 5/21/23.
//

#include <malloc.h>
#include <string.h>
#include "screenManager.h"

ScreenManager *CreateScreenManager(){
    ScreenManager* sm = malloc(sizeof(ScreenManager));
    sm->buffer = CreateBuffer(10000);
    return sm;
}

void* PrintToScreen(void *args){
    ScreenManager* sm = (ScreenManager*)args;
    int i = 3;
    while(1){
        char* item = removeI(sm->buffer);
        if(strcmp(item, "DONE") == 0){
            i--;
            if(i == 0)
                break;
            continue;
        }
        printf("%s\n",item);
    }
    return NULL;
}