#ifndef RESOURCE_H
#define RESOURCE_H

#include "mutex.h"

typedef struct Resource_s * Resource_p;

Resource_p create_resource();

int destroy_resource(Resource_p res);

int get(Resource_p res, PCB_p pcb);

void put (Resource_p res, PCB_p pcb, int new_data);
#endif // RESOURCE_H
 
