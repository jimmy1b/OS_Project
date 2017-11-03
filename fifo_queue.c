#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "pcb.h"
#include "fifo_queue.h"


fifo_queue fifo_queue_constructor() {
  fifo_queue q = malloc(sizeof(fifo_q));
  q->head = NULL;
  q->tail = NULL;
  q->count = 0;
  q->quantum = 0;
  return q;
}

int fifo_destructor(fifo_queue q) {
  if (q == NULL) return -1;
  while (q->count > 0) {
    PCB_p temp = q_dequeue(q);
    destructor(temp);
  }
  free(q);
  return 0;
}

int q_enqueue(fifo_queue the_queue, PCB_p theitem) {
  if (the_queue == NULL) return -1;
  q_node* node = (q_node*) malloc(sizeof(q_node));
  node->next = NULL;
  node->data = theitem;
  the_queue->count++;
  // printf("\nbefore\n");
  if (the_queue->head==NULL) {
    the_queue->head=node;
    the_queue->tail=node;
    return 0;
  }
  // printf("after\n");
  the_queue->tail->next = node;
  // printf("after\n");
  the_queue->tail = node;
  // printf("return\n");
  return 0;
}

int q_is_empty(fifo_queue the_queue) {
  if (the_queue == NULL) return -1;
  return the_queue->count == 0 ? 0 : 1;
}

PCB_p q_peek(fifo_queue q) {
  if (q == NULL) return NULL;
  return q->head->data;
}

PCB_p q_dequeue(fifo_queue the_queue) {
  if (the_queue == NULL) return NULL;
  if (q_is_empty(the_queue)==0) {
      return NULL;
  }
  q_node* node = the_queue->head;
  if (the_queue->head==the_queue->tail) {
    the_queue->head = NULL;
    the_queue->tail = NULL;
  } else {
    the_queue->head = the_queue->head->next;
  }
  PCB_p tmp = node->data;
  free(node);
  the_queue->count--;
  return tmp;
}

int get_count(fifo_queue the_queue) {
  if (the_queue == NULL) return -1;
  return the_queue->count;
}

char * q_toString(fifo_queue q) {
  if (q == NULL) return NULL;
  char * list_rep = malloc(1024);
  memset(list_rep, 0, 1024);
  char * init_str = malloc(256);
  memset(list_rep, 0, 256);
  int count = q->count;
  sprintf(init_str, "Count=%d: ", q->count);
  strcat(list_rep, init_str);
  free(init_str);

  // special case
  if (count == 0) {
    strcat(list_rep, "-*\n");
  }

  int i;
  for (i = 0; i < count; i++) {
    char * inner = malloc(16);
    memset(inner, 0, 16);
    PCB_p p = q_dequeue(q);
    if (i == count - 1) {
      char* pcb_rep = toString(p);
      sprintf(inner, "P%d-*\n", getPid(p));
      strcat(list_rep, inner);
      free(pcb_rep);
    } else {
      sprintf(inner, "P%d->", getPid(p));
      strcat(list_rep, inner);
    }
    q_enqueue(q, p);
    free(inner);
  }

  return list_rep;
}

int q_setquantum(fifo_queue q, int val) {
  if (q == NULL) return 1;
  q->quantum = val;
  return 0;
}
