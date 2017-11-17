/*
TCSS422 - Operating Systems
Problem 3

Group Members:
Taylor Riccetti
Alisher Baimenov
*/

#include <stdio.h>
#include <stdlib.h>
#include "fifo_queue.h"
#include "pcb.h"
#include "PriorityQ.h"

// value to denote current interupt is a timer interupt
#define TIMER_INTERUPT 1

// value to denote a successful function return
#define SUCCESSFUL 0

// Maximum size used for an ouput buffer string
#define MAX_BUFFER_SIZE 1024

// Simulates the OS.
int OS_Simulator(FIFO_Queue_p, FIFO_Queue_p, PriorityQ_p *, PCB_p *);

// Runs the pseudo ISR, calls the scheduler and updates the PC.
int pseudoISR(PriorityQ_p *,FIFO_Queue_p , PCB_p*);

// Handles the interrupts.
int scheduler(PriorityQ_p *, FIFO_Queue_p , PCB_p* , int);

// Dispatches the next ready process and enqueues the old process.
int dispatcher(PriorityQ_p *, PCB_p*);

// Creates a random number (0 - 5) of processes.
int createNewProcesses(FIFO_Queue_p);

// Moves proccesses into the highest priority queue.
void moveProcesses (PriorityQ_p *);

// Returns the number of cycles that each queue uses.
unsigned int getCyclesFromPriority(unsigned int);