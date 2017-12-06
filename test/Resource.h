#ifndef RESOURCE_H
#define RESOURCE_H

#include "mutex.h"

typedef struct Resource_s * Resource_p;

Resource_p create_resource();

int destroy_resource(Resource_p res);

int get(Resource_p res);

void put (Resource_p res, int new_data);

mutex_p getMutex(Resource_p res);
#endif // RESOURCE_H
 
