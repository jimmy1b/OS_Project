#include <stdlib.h>
#include <stdio.h>
#include "mutex.h"
#include "pcb.h"
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
        if(!res->mutex) {
            free(res);
            return NULL;
        }
        res->data = 0;
        return res;
    }
} 

int destroy_resource(Resource_p res) {
    if(!res) return -1;
    destroy_mutex(res->mutex);
    free(res);
    return 0;
}

// return 0 if able to lock, -1 if not
int get(Resource_p res) {
    int result = res->data;
    return result;
}

// // return 0 if able to lock, -1 if not
// int get(Resource_p res, PCB_p pcb) {
//     if (lock(res->mutex, pcb) == 1) {
//         int result = res->data;
//         unlock(res->mutex, pcb);
//         return result;
//     } else {
//         return -1;
//     }
// }

// return 0 if able to lock, -1 if not
void put(Resource_p res, int new_data) {
    res->data = new_data;
}

// // return 0 if able to lock, -1 if not
// int put(Resource_p res, PCB_p pcb, int new_data) {
//     if (lock(res->mutex, pcb) == 1) {
//         res->data = new_data;
//         unlock(res->mutex, pcb);
//         return 0;
//     } else {
//         return -1;
//     }
// }

mutex_p getMutex(Resource_p res) {
    if(!res) return NULL;
    return res->mutex;
}