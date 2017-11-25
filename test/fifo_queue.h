/*
TCSS422 - Operating Systems
Problem 4

Group Members:
Joshua Lau
Alisher Baimenov
*/


#ifndef FIFO_QUEUE_H
#define FIFO_QUEUE_H

#include "pcb.h"


typedef struct FIFO_Queue_s * FIFO_Queue_p;

typedef struct Node_s * Node_p;

/* Constructor */
FIFO_Queue_p create_fifo_queue();

/* Deconstructor */
void destroy(FIFO_Queue_p fifo);

/* Functions */
int fifo_is_empty(FIFO_Queue_p fifo);

unsigned int fifo_size(FIFO_Queue_p fifo);

void fifo_enqueue(FIFO_Queue_p fifo, PCB_p data);

PCB_p fifo_dequeue(FIFO_Queue_p fifo);

void print_fifo_queue(FIFO_Queue_p fifo);

#endif
