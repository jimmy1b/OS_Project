//Mutex Data Structure.

#include <stdlib.h>
#include <stdio.h>
#include "mutex.h"

//Mutex structure.
struct mutex_s {
    PCB_p pcb;
    FIFO_Queue_p waiting_q;
    enum lock_state state;

} mutex_s;
//Creates and returns mutex.
mutex_p create_mutex() {
    mutex_p mutex = (mutex_p) malloc(sizeof(mutex_s));
    if (!mutex) {
        return NULL;
    } else {
        mutex->pcb = NULL;
        mutex->waiting_q = create_fifo_queue();
        mutex->state = unlocked;

        return mutex;
    }
}

//Locks mutex.
//Do threads acquire locks or processes?
//Returns 0 if put to waiting Queue.
//Return 1 if acquired lock.
int lock(mutex_p mutex, PCB_p pcb) {
    if (mutex->state == locked) {
        fifo_enqueue(mutex->waiting_q, pcb);
        set_state(pcb, waiting);
        return 0; //failed to acquire lock.
    } else {
        if (fifo_is_empty(mutex->waiting_q)) {
            mutex->pcb = pcb;
            mutex->state = locked;
            return 1;
        } else {
            fifo_enqueue(mutex, pcb);
            set_state(pcb, waiting);
            return 0;
        }
    }
}

//Attempts to lock mutex.
//Returns 0 for fail,
//1 means lock is available.
int trylock(mutex_p mutex) {
    //0 state is locked, 1 is unlocked.
    if (mutex->state == locked) {
        return 0;
    } else {
        return 1;
    }
}

int unlock(mutex_p mutex) {
    if (fifo_is_empty(mutex->waiting_q)) {
        mutex->pcb = NULL;
        mutex->state = unlocked;
        return 0;
    } else {
        mutex->pcb = fifo_dequeue(mutex->waiting_q);
        set_state(mutex->pcb, running);
        mutex->state = locked;
        return 1;
    }
}

int TestAndSet(int *old_ptr, int new) {
    int old = *old_ptr;
    *old_ptr = new;
    return old;
}
