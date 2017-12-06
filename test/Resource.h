#ifndef RESOURCE_H
#define RESOURCE_H

#include "mutex.h"

typedef struct Resource_s * Resource_p;

Resource_p create_Resource();

int get(Resource_p res, PCB_p pcb);

int put (Resource_p res, PCB_p pcb, int new_data);
#endif // RESOURCE_H
 
