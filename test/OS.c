/*
TCSS422 - Operating Systems
Problem 4

Group Members:
Joshua Lau
Alisher Baimenov
*/

#include <time.h>
#include "OS.h"


unsigned int sysStack;
unsigned int currentPC;
unsigned int dispatchCount;
PCB_p privilegedPCBs[4]; // array of PCBs
unsigned int privilegedCount;
unsigned int quantum;
unsigned int quantumCounter;
unsigned int processCounter;
int theTime, IO1time, IO2time;
PCB_p IO1Process;
PCB_p IO2Process;
FIFO_Queue_p IO1Queue;
FIFO_Queue_p IO2Queue;

FIFO_Queue_p newProcesses;
FIFO_Queue_p dieingProcesses;

// The os simulator, runs the main loop.
int OS_Simulator(PriorityQ_p * readyProcesses, PCB_p * runningProcess) {
    srand(time(NULL));

    int iteration = 1;
    int t, a, i, j;
    //startTimer(get_priority(*readyProcesses));
    // Main Loop
    // One cycle is one quantum
    for( ; ; ) { // for spider
        //printf("Numb\n");
        // stops making processes after 48 and if there are at least 4 Privileged pcbs
        if(processCounter < 4000) {// && privilegedCount < 4) {
            createNewProcesses(newProcesses);
            processCounter += fifo_size(newProcesses);
        }

        // set new processes to ready
        // while(!fifo_is_empty(newProcesses)) {
        //     // dequeue and print next pcb
        //     PCB_p pcb = fifo_dequeue(newProcesses);
        //     set_state(pcb, ready);

        //     // enqueue
        //     pq_enqueue(*readyProcesses, pcb);
        // }


        //Simulate Process Running
        //set_pc(*runningProcess, get_pc(*runningProcess) + 1);

        // Push to SysStack
        //sysStack = currentPC;
//printf("Check5\n");

        quantumCounter--;







        for( ; ; ) {
            //timer int check


            t = timer(*runningProcess);

            if(*runningProcess != NULL) {
                set_pc(*runningProcess, get_pc(*runningProcess) + 1);
            } else if (!pq_isEmpty(*readyProcesses)) {
                *runningProcess = pq_dequeue(*readyProcesses);
            }






            if (t == 1) {

                printf("Timer\n");

	       		pseudoISR(readyProcesses, runningProcess);

                startTimer(get_priority(*runningProcess));
		        //print_priority_queue(*readyProcesses);

		        break;
		    }

            //IO timer int check
		    a = IOTimer(readyProcesses);

		    if (a == 1) {
//printf("Check20\n");
		    	//throw io 1 interrupt
                set_state(IO1Process, ready);
                scheduler(readyProcesses, &IO1Process, get_state(IO1Process));
                if(!fifo_is_empty){
                  IO1Process = fifo_dequeue(IO1Queue);
                }

                //print_fifo_queue(IO1Queue);
                pseudoISR(readyProcesses, runningProcess);
//printf("Check5\n");

		    	break;
		    } else if (a == 2) {
//printf("Check21\n");
		    	//throw io 2 interrupt
                set_state(IO2Process, ready);
                scheduler(readyProcesses, &IO2Process, get_state(IO2Process));
                if(!fifo_is_empty){
                  IO2Process = fifo_dequeue(IO2Queue);
                }
                //print_fifo_queue(IO2Queue);
                pseudoISR(readyProcesses, runningProcess);
//printf("Check6\n");

		    	break;
		    }

            //IO Trap
            int iotrap = io_contains_pc(*runningProcess);
            if (iotrap == 1) {
                printf("IO1\n");
                set_state(*runningProcess, waiting);
                if (IO1Process != NULL) {
                    fifo_enqueue(IO1Queue, *runningProcess);
                } else {
                    //remember
                    IO1Process = *runningProcess;
                }
                //print_fifo_queue(IO1Queue);
                scheduler(readyProcesses, runningProcess, get_state(*runningProcess));
                printf("Check22\n");

                //print_priority_queue(*readyProcesses);
                break;
            } else if (iotrap == 2) {
                printf("IO2\n");
                set_state(*runningProcess, waiting);
                if (IO2Process != NULL) {
                    fifo_enqueue(IO2Queue, *runningProcess);
                } else {
                    IO2Process = *runningProcess;
                }
                //print_fifo_queue(IO2Queue);
                scheduler(readyProcesses, runningProcess, get_state(*runningProcess));
                printf("Check23\n");

                //print_priority_queue(*readyProcesses);
                break;
            }
            if(*runningProcess != NULL) {

                if (get_terminate(*runningProcess) > 0 && get_pc(*runningProcess) >= get_max_pc(*runningProcess)) {
                    printf("term\n");
                    set_term_count(*runningProcess, get_term_count(*runningProcess) + 1);
                    set_pc(*runningProcess, 0);
                    if (get_term_count(*runningProcess) > get_terminate(*runningProcess)) {
                        set_state(*runningProcess, halted);
                        scheduler(readyProcesses, runningProcess, get_state(*runningProcess));
    //printf("Check10\n");

                        break;
                    }
                }
            }

        }

        // moved to timer loop
		// //Terminate
		// if (get_terminate(*runningProcess) > 0) {
		// 	if (get_pc(*runningProcess) > get_max_pc(*runningProcess)) {
		// 		set_term_count(*runningProcess, get_term_count(*runningProcess) + 1);
		// 		set_pc(*runningProcess, 0);
		// 		if (get_term_count(*runningProcess) > get_terminate(*runningProcess)) {
  //                   set_state(*runningProcess, halted);
  //                   scheduler(readyProcesses, runningProcess, get_state(*runningProcess));
		// 		}
		// 	}
		// }
		iteration ++;
		//printf("ITERATION IS: %d\n", iteration);
        if (iteration == 15000 || pq_isEmpty(*readyProcesses)){
          printf("we ended\n");
        break;
      }
    }
}

int IOTimer(PriorityQ_p * readyProcesses) {
	// if (io_contains_pc(*runningProcess) == 1) {//enter logic to enqueue pcb into IO1queue if value matches the IO1 array.
 //        fifo_enqueue(IOQueue, *runningProcess);
	// }
	if (IO1time <= 0 && IO1Process != NULL) {
        printf(" I/O 1 complete\n");
        IO1time = ((rand() % 3) + 3) * getCyclesFromPriority(7) ;
	if(!fifo_is_empty(IO1Queue)) {
		IO1Process = fifo_dequeue(IO1Queue);
		}

        // set_state(IO1Process, ready);
        // scheduler(readyProcesses, IO1Process, get_state(IO1Process));

        // IO1Process = fifo_dequeue(IO1Queue);
        return 1;

	} else if (IO2time <= 0 && IO2Process != NULL) {
        printf(" I/O 2 complete\n");
	//printf("Check1\n");
        IO2time = ((rand() % 3) + 3) * getCyclesFromPriority(7) ;
	//printf("Check2\n");
	if(!fifo_is_empty(IO2Queue)) {
		IO2Process = fifo_dequeue(IO2Queue);
		}
	//printf("Check3\n");
        // set_state(IO2Process, ready);
        // scheduler(readyProcesses, IO1Process, get_state(IO1Process));

        // IO2Process = fifo_dequeue(IO2Queue);
        return 2;

    } else {

        IO1time -= 1;
        IO2time -= 1;
        return 0;
    }

}

// int IOTrap2(FIFO_Queue_p IOQueue, PCB_p* runningProcess, PriorityQ_p * readyProcesses) {
// 	if (io_contains_pc(*runningProcess) == 2) {//enter logic to enqueue pcb into IO1queue if value matches the IO1 array.
//         fifo_enqueue(IOQueue, *runningProcess);
// 	}
// 	if (IO2time <= 0) {
// 		printf(" I/O Trap 2 threw interrupt\n");
// 		IO2time = (rand() * 3 + 2) * 3 * getCyclesFromPriority(7) ;
//         if (IO2Process != NULL) {
//             pq_enqueue(readyProcesses, IO2Process);
//         }
// 		IO2Process = fifo_dequeue(IOQueue);
// 		return 1;

// 	}
// 	else {
// 		IO2time -= 1;
// 		return 0;
// 	}

// }

// The psuedo-ISR, sets the state of the running process,
// calls the scheduler and updates the PC.
int pseudoISR(PriorityQ_p * readyProcesses, PCB_p* runningProcess) {
    // Sets the status to interrupted.
    set_state(*runningProcess, interrupted);
printf("Check6\n");
    sysStack = get_pc(*runningProcess);
printf("Check50\n");
    // save pc to pcb
    set_pc(*runningProcess, sysStack);
printf("Check51\n");
    // scheduler up call
    scheduler(readyProcesses, runningProcess, get_state(*runningProcess));
printf("Check52\n");
    // IRET (update current pc)
    //currentPC = sysStack;
//printf("Check11\n");

    return SUCCESSFUL;
}
int startTimer(int priority) {
	theTime = getCyclesFromPriority(priority);
}

int timer(PCB_p pcb) {
	if (theTime <= 0) {
        //startTimer(get_priority(pcb));
        return 1;
    }
	else {
		theTime -= 1;
		return 0;
	}
}

// Runs the scheduler to handle interrupts.
int scheduler(PriorityQ_p * readyProcesses, PCB_p* runningProcess, int interrupt) {
    printf("\n%d\n", interrupt);

    switch(interrupt) {
        //case 1:

        case interrupted:
        printf("Check1\n");
            dispatcher(readyProcesses, runningProcess);
            break;
        //case 2:

        case halted: // if the state is interrupted move to dieing processes and then call the dispatcher.

            fifo_enqueue(dieingProcesses, *runningProcess);

            dispatcher(readyProcesses, runningProcess);
            printf("Check2\n");
            break;

        case waiting: //IO trap
        //case 3:

            dispatcher(readyProcesses, runningProcess);
            break;
        //case 4:
        case ready: //IO completion
	    //printf("Check4\n");
            pq_enqueue(*readyProcesses, *runningProcess);
            break;
        default:
            // error handling as needed
            break;
    }

    //enqueues new processes
    while(!fifo_is_empty(newProcesses)) {

            // dequeue and print next pcb
            PCB_p pcb = fifo_dequeue(newProcesses);
            set_state(pcb, ready);

            // enqueue
            pq_enqueue(*readyProcesses, pcb);
        }

    // housekeeping if needed
    // If there are 4 terminated processes, clear them now.
    if(fifo_size(dieingProcesses) == 4) {

        while(!fifo_is_empty(dieingProcesses)) {
            PCB_p dieingPCB = fifo_dequeue(dieingProcesses);
            destroy_pcb(dieingPCB);
        }
        printf("Check69\n");

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
    //printf("Check3\n");
    // update context if the pcb was not halted.
    if(get_state(*runningProcess) != halted &&
        get_state(*runningProcess) != waiting) {
        //printf("Check3\n");
        // update the pc counter.
        //set_pc(*runningProcess, sysStack);
        // set state.
        set_state(*runningProcess, ready);


        // Increments the cycles of the process.
        //unsigned int cycles = get_cycles(*runningProcess);
        unsigned char priority = get_priority(*runningProcess);

        //if (cycles % getCyclesFromPriority(priority) == 0) {
        if (priority == MAX_PRIORITY) {
            set_priority(*runningProcess, 0); // will go back to the highest priority queue
        } else {
            set_priority(*runningProcess, priority + 1);
        }
        //}
        //cycles++;
        //set_cycles(*runningProcess, cycles);

        // enqueue
        pq_enqueue(*readyProcesses, *runningProcess);
    }
    //printf("Check3\n");

    // dequeue
    *runningProcess = pq_dequeue(*readyProcesses);

    // update state to running
    // set state
    if(*runningProcess != NULL) {
        set_state(*runningProcess, running);
        //printf("Check3\n");
        sysStack = get_pc(*runningProcess);
    }
    //printf("Check4\n");

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
        /*if(rand() % 100 < 20 ) {//&& privilegedCount < 4)  {
            setPrivileged(pcb);
            privilegedPCBs[privilegedCount] = pcb;
            privilegedCount++;
        })*/
        fifo_enqueue(newProcesses, pcb);
    }
}

// Returns the number of cycles that each queue uses.
unsigned int getCyclesFromPriority(unsigned int priority) {
    return 20 * (priority + 1);
}

// Main, to jump start the OS simulation and initialize variables.
int main() {
    // Seed RNG
    srand(time(NULL));

    // Initialize Vars
    newProcesses = create_fifo_queue();
    PriorityQ_p readyProcesses = create_pq();
    PCB_p runningProcess = create_pcb();
    //FIFO_Queue_p dieingProcesses = create_fifo_queue();
    dieingProcesses = create_fifo_queue();
    PCB_p IO1Process = create_pcb();
    PCB_p IO2Process = create_pcb();
    IO1Queue = create_fifo_queue();
    IO2Queue = create_fifo_queue();
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
    OS_Simulator(&readyProcesses, &runningProcess);

    // free resources
    destroy(newProcesses);
    destroy(dieingProcesses);
    destroy_pq(readyProcesses);
    destroy_pcb(runningProcess);
    destroy(IO1Queue);
    destroy(IO2Queue);
    destroy_pcb(IO1Process);
    destroy_pcb(IO2Process);
}
