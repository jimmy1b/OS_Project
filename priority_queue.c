#include <stdio.h>
#include <stdlib.h>
#include "pcb.h"
#include "fifo_queue.h"
#include "priority_queue.h"
#include <string.h>

//This is used for the number of priority levels
#define QUEUE_SIZE 16

priority_queue priority_queue_constructor() {
  priority_queue ready_q = malloc(sizeof(fifo_queue) * QUEUE_SIZE);
  int i;
  for (i = 0; i < QUEUE_SIZE; i++) {
    ready_q[i] = fifo_queue_constructor();
  }
  return ready_q;
}

fifo_queue get_queue(priority_queue pq, unsigned char priority) {
  if (pq == NULL) return NULL;
  return *(pq + priority);
}

int priority_queue_destructor(priority_queue q) {
  if (q == NULL) return -1;
  int i;
  for (i = 0; i < QUEUE_SIZE; i++) {
    fifo_destructor(q[i]);
  }
  free(q);
  return 0;
}

int enqueue_ready(priority_queue theready_q, unsigned char priority, PCB_p the_pcb) {
  if (theready_q == NULL) return -1;
  fifo_queue q = theready_q[priority];
  q_enqueue(q, the_pcb);
  return 0;
}

PCB_p dequeue_ready(priority_queue theready_q, unsigned char priority) {
  if (theready_q == NULL) return NULL;
  fifo_queue q = theready_q[priority];
  if (q_is_empty(q)==0) {
    return NULL;
  }
  return q_dequeue(q);
}

int empty_ready(priority_queue theready_q, unsigned char priority) {
  if (theready_q == NULL) return -1;
  return q_is_empty(theready_q[priority]);
}

char * pq_toString(priority_queue pq) {
  if (pq == NULL) return NULL;
  char * ret = malloc(1024 * 32);
  memset(ret, 0, 1024 * 32);
  int i;
  for (i = 0; i < 16; i++) {
    char * q_name = malloc(32);
    memset(q_name, 0, strlen(q_name));
    sprintf(q_name, "Queue %d: ", i);
    strcat(ret, q_name);
    free(q_name);

    char * tmp = q_toString(pq[i]);
    strcat(ret, tmp);
    free(tmp);
  }
  return ret;
}
