//
// Created by david on 5/20/23.
//

#ifndef EX3_PRODUCER_H
#define EX3_PRODUCER_H

#include "bounded_buffer.h"

typedef struct {
    int id;
    BoundedBuffer* buffer;
}Producer;
#endif //EX3_PRODUCER_H
