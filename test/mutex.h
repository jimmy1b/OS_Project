#ifndef MUTEX_H
#define MUTEX_H

#include "fifo_queue.h"

typedef struct mutex_s * mutex_p;

enum lock_state {locked, unlocked};

mutex_p create_mutex();

int destroy_mutex(mutex_p mutex);
PCB_p get_mutex_pcb(mutex_p mutex);
int lock(mutex_p mutex, PCB_p pcb);

int trylock(mutex_p mutex);

int unlock(mutex_p mutex, PCB_p pcb);

int TestAndSet(int *old_ptr, int new);

#endif // MUTEX_H
