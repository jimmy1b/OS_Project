#ifndef RESOURCE_H
#define RESOURCE_H

typedef struct Resource_s * Resource_p;

Resource_p create_Resource();

int get(Resource_p mdp);

int put (Resource_p mdp, int new_data);
#endif // RESOURCE_H
 
