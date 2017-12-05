#include <stdlib.h>
#include <stdio.h>
#include "mutex.h"
#include "Resource.h"

struct Resource_s {
    mutex_p mutex;
     unsigned int data;

} Resource_s;

Resource_p create_resource() {
    Resource_p res = (resource_p) malloc(sizeof(Resource_s));
    if (!res) {
        return NULL;
    } else {
        res->mutex = create_mutex();
        res->data = 0;
        return res;
    }
} 

int get(Resource_p res, PCB_p pcb) {
    if (lock(res->mutex, pcb) == 1) {
        int result = res->data;
        unlock(res->mutex);
        return result;
    } else {
        return -1
    }
}

int put (Resource_p res, PCB_p pcb, int new_data) {
    lock(res->mutex, pcb);
    res->data = new_data;
    unlock(res->mutex);
}
