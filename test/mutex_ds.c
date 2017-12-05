#include <stdlib.h>
#include <stdio.h>
#include "mutex.h"
#include "mutex_ds.h"

struct mutex_ds_s {
    mutex_p mutex;
    PCB_p pcb;
    int data;

} mutex_ds_s;

mutex_ds_p create_mutex_ds() {
    mutex_ds_p mdp = (mutex_ds_p) malloc(sizeof(mutex_ds_s));
    if (!mdp) {
        return NULL;
    } else {
        mdp->mutex = create_mutex();
        mdp->pcb = create_pcb();
        mdp->data = 0;
        return mdp;
    }
} 

int get(mutex_ds_p mdp) {
    lock(mdp->mutex, mdp->pcb);
    int result = mdp->data;
    unlock(mdp->mutex);
    return result;
}

int put (mutex_ds_p mdp, int new_data) {
    lock(mdp->mutex, mdp->pcb);
    mdp->data = new_data;
    unlock(mdp->mutex);
}
