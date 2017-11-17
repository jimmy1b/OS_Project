/*
TCSS422 - Operating Systems
Problem 3

Group Members:
Taylor Riccetti
Alisher Baimenov
*/

#include <time.h>
#include "OS.h"


unsigned int sysStack;
unsigned int currentPC;
unsigned int dispatchCount;
PCB_p * privilegedPCBs[4]; // array of PCBs
unsigned int privilegedCount;
unsigned int quantum;
unsigned int quantumCounter;
unsigned int processCounter;

// The os simulator, runs the main loop.
int OS_Simulator(FIFO_Queue_p newProcesses, FIFO_Queue_p dieingProcesses, PriorityQ_p * readyProcesses, PCB_p * runningProcess) {
    srand(time(NULL));
    int iteration = 1;

    // Main Loop
    // One cycle is one quantum
    for( ; ; ) { // for spider
        // stops making processes after 48 and if there are at least 4 Privileged pcbs
        if(processCounter < 48 && privilegedCount < 4) {
            createNewProcesses(newProcesses);
            processCounter += fifo_size(newProcesses);
        }

        // set new processes to ready
        while(!fifo_is_empty(newProcesses)) {
            // dequeue and print next pcb
            PCB_p pcb = fifo_dequeue(newProcesses);
            set_state(pcb, ready);

            // enqueue
            pq_enqueue(*readyProcesses, pcb);
        }

        // Simulate Process Running
        currentPC += get_pc(*runningProcess) + (rand() % getCyclesFromPriority(get_priority(*runningProcess)));

        // Push to SysStack
        sysStack = currentPC;

        pseudoISR(readyProcesses, dieingProcesses, runningProcess);

        quantumCounter--;


        printf("Iteration: %d\n", iteration);
        print_priority_queue(*readyProcesses);
        int i;
        for(i = 0; i < 4; i++) {
            if (privilegedPCBs[i] == NULL) {
                // skip
            } else {
                print_pcb(privilegedPCBs[i]);
            }
        }
        iteration ++;
        if (iteration == 300)  break;
    }
}

// The psuedo-ISR, sets the state of the running process,
// calls the scheduler and updates the PC.
int pseudoISR(PriorityQ_p * readyProcesses, FIFO_Queue_p dieingProcesses, PCB_p* runningProcess) {

    // Sets the state to interrupted.
    set_state(*runningProcess, interrupted);

    // Terminate 15% chance to terminate the processes.
    if(rand() % 100 < 15 && !isPrivileged(*runningProcess)) {
        set_state(*runningProcess, halted);
    }

    // save pc to pcb
    set_pc(*runningProcess, currentPC);

    // scheduler up call
    scheduler(readyProcesses, dieingProcesses, runningProcess, get_state(*runningProcess));

    // IRET (update current pc)
    currentPC = sysStack;
    return SUCCESSFUL;
}

// Runs the scheduler to handle interrupts.
int scheduler(PriorityQ_p * readyProcesses, FIFO_Queue_p dieingProcesses, PCB_p* runningProcess, int interrupt) {
    switch(interrupt) {
        case interrupted:
            dispatcher(readyProcesses, runningProcess);
            break;
        case halted: // if the state is interrupted move to dieing processes and then call the dispatcher.
            fifo_enqueue(dieingProcesses, *runningProcess);
            dispatcher(readyProcesses, runningProcess);
            break;
        default:
            // error handling as needed
            break;
    }

    // housekeeping if needed
    // If there are 10 terminated processes, clear them now.
    if(fifo_size(dieingProcesses) == 10) {
        while(!fifo_is_empty(dieingProcesses)) {
            PCB_p dieingPCB = fifo_dequeue(dieingProcesses);
            destroy_pcb(dieingPCB);
        }
    }
    // after some time S move all processes into Q0.
    if(quantumCounter == 0) {
        moveProcesses(readyProcesses);
        quantumCounter = quantum;
    }

    return SUCCESSFUL;
}

// Dispatched the running process to appropriate queue.
int dispatcher(PriorityQ_p * readyProcesses, PCB_p* runningProcess) {
    // increment and check
    dispatchCount++;

    // update context if the pcb was not halted.
    if(get_state(*runningProcess) != halted) {
        // update the pc counter.
        set_pc(*runningProcess, sysStack);
        // set state.
        set_state(*runningProcess, ready);

        // Increments the cycles of the process.
        unsigned int cycles = get_cycles(*runningProcess);
        unsigned char priority = get_priority(*runningProcess);

        if (cycles % getCyclesFromPriority(priority) == 0) {
            if (priority == MAX_PRIORITY) {
                set_priority(*runningProcess, 0); // will go back to the highest priority queue
            } else {
                set_priority(*runningProcess, priority + 1);
            }
        }
        cycles++;
        set_cycles(*runningProcess, cycles);

        // enqueue
        pq_enqueue(*readyProcesses, *runningProcess);
    }

    // dequeue
    *runningProcess = pq_dequeue(*readyProcesses);

    // update state to running
    // set state
    set_state(*runningProcess, running);

    sysStack = get_pc(*runningProcess);

    return SUCCESSFUL;
}

void moveProcesses (PriorityQ_p * readyProcesses) {
    PriorityQ_p tempQueue = create_pq();
    while(!pq_isEmpty(*readyProcesses)) {

        PCB_p tempPCB = pq_dequeue(*readyProcesses);
        set_priority(tempPCB, 0);
        pq_enqueue(tempQueue, tempPCB);
    }

    *readyProcesses = tempQueue;
}

// Creates a random number of processes to be added to the
// the list of new processes.
int createNewProcesses(FIFO_Queue_p newProcesses) {
    int i;
    for(i = 0; i < rand() % 5; i++) {
        PCB_p pcb = create_pcb();

        // 20% chance that the pcb will become privileged.
        if(rand() % 100 < 20 && privilegedCount < 4)  {
            setPrivileged(pcb);
            privilegedPCBs[privilegedCount] = pcb;
            privilegedCount++;
        }
        fifo_enqueue(newProcesses, pcb);
    }
}

// Returns the number of cycles that each queue uses.
unsigned int getCyclesFromPriority(unsigned int priority) {
    return 2 * (priority + 1);
}

// Main, to jump start the OS simulation and initialize variables.
int main() {
    // Seed RNG
    srand(time(NULL));

    // Initialize Vars
    FIFO_Queue_p newProcesses = create_fifo_queue();
    PriorityQ_p readyProcesses = create_pq();
    PCB_p runningProcess = create_pcb();
    FIFO_Queue_p dieingProcesses = create_fifo_queue();

    // set a process to running
    set_state(runningProcess, running);

    currentPC = 0;
    sysStack = 0;
    dispatchCount = 0;
    privilegedCount = 0;
    quantum = 3 * getCyclesFromPriority(7); // multiple of middle queues quantum size.
    quantumCounter = quantum;
    processCounter = 1;

    // main loop
    OS_Simulator(newProcesses, dieingProcesses, &readyProcesses, &runningProcess);

    // free resources
    destroy(newProcesses);
    destroy(dieingProcesses);
    destroy_pq(readyProcesses);
    destroy_pcb(runningProcess);
}
