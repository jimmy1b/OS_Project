/*
TCSS422 - Operating Systems
Problem 4

Group Members:
Joshua Lau
Alisher Baimenov
*/


#ifndef PRIORITYQ_H_
#define PRIORITYQ_H_

#include "fifo_queue.h"
#include "pcb.h"

// The maximum priority allowed for a process
#define MAX_PRIORITY 15

typedef struct PriorityQ_s * PriorityQ_p;

/*Creates Priority Queue*/
PriorityQ_p create_pq();

/*Destroys this Priority Queue.*/
void destroy_pq(PriorityQ_p queue);

/* returns 0 for false, 1 for true */
int pq_isEmpty(PriorityQ_p queue);

/*Returns number of elements in this Priority Queue.*/
unsigned int pq_size(PriorityQ_p queue);

/*Adds given pcb to the Priority Queue.*/
void pq_enqueue(PriorityQ_p queue, PCB_p data);

/*Removes pcb with highest priority from the queue
and returns it.*/
PCB_p pq_dequeue(PriorityQ_p queue);

/*Prints Priority Queue.*/
void print_pq(PriorityQ_p queue);

/*Prints Priority Queue.*/
void print_priority_queue(PriorityQ_p queue);

#endif /* PRIORITYQ_H_ */
