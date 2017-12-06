#include <stdlib.h>
#include <stdio.h>
#include "Resource.h"

struct Resource_s {
    mutex_p mutex;
     unsigned int data;

} Resource_s;

Resource_p create_resource() {
    Resource_p res = (Resource_p) malloc(sizeof(Resource_s));
    if (!res) {
        return NULL;
    } else {
        res->mutex = create_mutex();
        res->data = 0;
        return res;
    }
}

// return 0 if able to lock, -1 if not
int get(Resource_p res, PCB_p pcb) {
    if (lock(res->mutex, pcb) == 1) {
        int result = res->data;
        unlock(res->mutex, pcb);
        return result;
    } else {
        return -1;
    }
}

// return 0 if able to lock, -1 if not
int put (Resource_p res, PCB_p pcb, int new_data) {
    if (lock(res->mutex, pcb) == 1) {
        res->data = new_data;
        unlock(res->mutex, pcb);
        return 0;
    } else {
        return -1;
    }
}
