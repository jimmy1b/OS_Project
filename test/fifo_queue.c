/*
TCSS422 - Operating Systems
Problem 4

Group Members:
Joshua Lau
Alisher Baimenov
*/


#include <stdlib.h>
#include <string.h>
#include <stdio.h>

// #include "pcb.h"
#include "fifo_queue.h"

struct FIFO_Queue_s {
    Node_p front;
    Node_p rear;
    unsigned int count;
} FIFO_Queue_s;

struct Node_s{
    PCB_p data;
    Node_p next;
} Node_s;

FIFO_Queue_p create_fifo_queue() {
    FIFO_Queue_p queue = malloc(sizeof(struct FIFO_Queue_s));
    if(queue == 0) {
        return 0;
    }
    queue->count = 0;
    queue->front = NULL;
    queue->rear = NULL;
    return queue;
}

/* Deconstructor */
void destroy(FIFO_Queue_p fifo) {
    while(!fifo_is_empty(fifo)) {
        PCB_p p = fifo_dequeue(fifo);
	destroy_pcb(p);
    }
 
    free(fifo);
}

/* Functions */
int fifo_is_empty(FIFO_Queue_p fifo) {
    return fifo->count <= 0;
}

void fifo_enqueue(FIFO_Queue_p fifo, PCB_p data) {
    if(fifo == NULL || data == NULL) {
        return;
    }
    Node_p new_node = malloc(sizeof(struct Node_s));
    new_node->data = data;
    if(fifo_is_empty(fifo)) {
        fifo->rear = new_node;
        fifo->front = fifo->rear;
    } else {
        fifo->rear->next = new_node;
        fifo->rear = new_node;
    }
    fifo->rear->next = NULL;
    fifo->count++;
}

PCB_p fifo_dequeue(FIFO_Queue_p fifo) {
    if (!fifo_is_empty(fifo)) {
        Node_p removed_node = fifo->front;
        PCB_p pcb = removed_node->data;
        Node_p next_front = fifo->front->next;
        free(fifo->front);
        fifo->front = next_front;
        fifo->count--;
        return pcb;
    } else {
        return NULL;
    }
}

unsigned int fifo_size(FIFO_Queue_p fifo) {
    return fifo->count;
}

void print_fifo_queue(FIFO_Queue_p fifo) {
    if(fifo == NULL) {
        return;
    }

    Node_p front = fifo->front;

    int index = 1;
    if(fifo->count != 0) {
        printf( "Count=%d: ", fifo->count);
        while (front != NULL) {
            unsigned int process = get_pid(front->data);
            printf( "p%d", process);
            printf( "->");
            index++;
            front = front->next;
        }
        printf("*");
    } else {
        printf("Count=%d: ->* ", fifo->count);
}
}
