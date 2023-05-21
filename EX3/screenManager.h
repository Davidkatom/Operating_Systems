//
// Created by david on 5/21/23.
//

#ifndef EX3_SCREENMANAGER_H
#define EX3_SCREENMANAGER_H

#include "bounded_buffer.h"

typedef struct{
    BoundedBuffer *buffer;
}ScreenManager;
ScreenManager *CreateScreenManager();
void* PrintToScreen(void *args);
#endif //EX3_SCREENMANAGER_H
