/*
TCSS422 - Operating Systems
Problem 4

Group Members:
Joshua Lau
Alisher Baimenov
*/

#include <time.h>
#include <pthread.h>
#include <assert.h>
#include "Resource.h"
#include "OS.h"
#define PROCESSNUMBER 45
#define NOIOPROCESSNUMBER 20
#define RESOURCEPROCESSNUMBER 10
#define NANO_SECOND_MULTIPLIER  100000000000  // 1 millisecond = 1,000,000 Nanoseconds

unsigned int sysStack;
unsigned int currentPC;
unsigned int dispatchCount;
PCB_p privilegedPCBs[4]; // array of PCBs
unsigned int privilegedCount;
unsigned int quantum;
unsigned int quantumCounter;
unsigned int processCounter;
unsigned int noIOCounter;
unsigned int prodConCounter;
unsigned int mutualCounter;
int thePriority, theTime, IO1time, IO2time, IO1priority, IO2Priority, deadlock;

PCB_p IO1Process;
PCB_p IO2Process;
FIFO_Queue_p IO1Queue;
FIFO_Queue_p IO2Queue;

FIFO_Queue_p newProcesses;
FIFO_Queue_p dieingProcesses;
pthread_mutex_t priorityMutex;

pthread_mutex_t timerMutex;
pthread_mutex_t Io1Mutex;
pthread_mutex_t Io2Mutex;


Resource_p prodConR[10];
Resource_p mutualR1[10];
Resource_p mutualR2[10];


// The os simulator, runs the main loop.
int OS_Simulator(PriorityQ_p * readyProcesses, PCB_p * runningProcess) {


    int iteration = 1;
    int processed = 0;
    int t, a, b, i, j;
    //startTimer(get_priority(*readyProcesses));
    // Main Loop
    // One cycle is one quantum
    for( ; ; ) {
            //srand(time(NULL));
        //makes new processes, puts them into newProcesses queue
        // stops making processes after 48 and if there are at least 4 Privileged pcbs
        prepareNewProcesses();

        processed = 0;

        for( ; ; ) {
            if(*runningProcess != NULL && get_state(*runningProcess) != waiting) {
                set_pc(*runningProcess, get_pc(*runningProcess) + 1);
                //printf("%d\n", get_pc(*runningProcess));
            // } else if (!pq_isEmpty(*readyProcesses)) {
            //     //*runningProcess = pq_dequeue(*readyProcesses);
            }
            pthread_mutex_trylock(&timerMutex);
            t = theTime;

            pthread_mutex_unlock(&timerMutex);
            if (t == 1) {
                quantumCounter--;

                printf("Timer\n\n");
                pthread_mutex_trylock(&timerMutex);
                theTime = 0;

                pthread_mutex_unlock(&timerMutex);
            //printf("%d\nmax pc : %d\n", get_pc(*runningProcess), get_max_pc(*runningProcess));
           // print_priority_queue(*readyProcesses);
	       		pseudoISR(readyProcesses, runningProcess);

		        break;
		    }
            //IO timer int check
            pthread_mutex_trylock(&Io1Mutex);
            a = IO1time;
            pthread_mutex_unlock(&Io1Mutex);
            pthread_mutex_trylock(&Io2Mutex);
            b = IO2time;
            pthread_mutex_unlock(&Io2Mutex);

            // a = IOTimer(readyProcesses);
            // printf("a is: %d\n", a);
		    if (a == 1 && IO1Process != NULL) {
//printf("Check20\n");
		    	//throw io 1 interrupt
                set_state(IO1Process, ready);
                scheduler(readyProcesses, &IO1Process, get_state(IO1Process));
                IO1Process = fifo_dequeue(IO1Queue);
                printf("\n");
                printf("IO1QUEUE is: ");
                print_fifo_queue(IO1Queue);
                pseudoISR(readyProcesses, runningProcess);
		    	break;
		    }

            if (b == 1 && IO2Process != NULL) {
		    	//throw io 2 interrupt

                print_fifo_queue(IO2Queue);
                set_state(IO2Process, ready);
                scheduler(readyProcesses, &IO2Process, get_state(IO2Process));
                IO2Process = fifo_dequeue(IO2Queue);
                printf("\n");
                printf("IO2QUEUE is: ");
                print_fifo_queue(IO2Queue);
                pseudoISR(readyProcesses, runningProcess);
		    	break;
		    }

            //printf("processed is: %d, state is: %d", processed, get_state(*runningProcess));
            while(!processed && get_state(*runningProcess) != waiting) {
                //producer function
                if(get_type(*runningProcess) == producer) {
                    int r = get_pcb_resource(*runningProcess);
                    printf("(pid: %d) producer %d requested lock in resource pcResource%d - ", get_pid(*runningProcess), r, r);
                    if(lock(getMutex(prodConR[r]), *runningProcess) == 1) {
                        if(checkTime()) break;
                        int g = increment(prodConR[r]);
                        if(checkTime()) break;
                        unlock(getMutex(prodConR[r]), *runningProcess);
                        printf("(pid: %d) producer %d incremented data in resource pcResource%d: %d\n", get_pid(*runningProcess), r, r, g);
                    }

                //consumer function
                } else if(get_type(*runningProcess) == consumer) {
                    int r = get_pcb_resource(*runningProcess);
                    printf("(pid: %d) consumer %d requested lock in resource pcResource%d - ", get_pid(*runningProcess), r, r);
                    if(lock(getMutex(prodConR[r]), *runningProcess) == 1) {
                        if(checkTime()) break;
                        int g = get(prodConR[r]);
                        if(checkTime()) break;
                        unlock(getMutex(prodConR[r]), *runningProcess);
                        printf("(pid: %d) consumer %d read data from resource pcResource%d: %d\n", get_pid(*runningProcess), r, r, g);
                    }

                //mutual exclusive process function
                } else if(get_type(*runningProcess) == mutual) {
                    //deadlock

                    int p = get_pair(*runningProcess);
                    if(deadlock && p) {
                        int r = get_pcb_resource(*runningProcess);
                        printf("(pid: %d) mutual%d%d requested lock in resource m2Resource%d - ", get_pid(*runningProcess), p, r, r);
                        if(lock(getMutex(mutualR2[r]), *runningProcess) == 1) {
                            if(checkTime()) break;
                            printf("(pid: %d) mutual%d%d requested lock in resource m1Resource%d - ", get_pid(*runningProcess), p, r, r);
                            if(lock(getMutex(mutualR1[r]), *runningProcess) == 1) {
                                if(checkTime()) break;
                                get(mutualR1[r]);
                                if(checkTime()) break;
                                get(mutualR2[r]);
                                if(checkTime()) break;
                                unlock(getMutex(mutualR2[r]), *runningProcess);
                                if(checkTime()) break;
                                unlock(getMutex(mutualR1[r]), *runningProcess);
                                printf("(pid: %d) mutual%d%d got 2 resources: m1Resource%d, m2Resource%d\n", get_pid(*runningProcess), p, r, r, r, r);
                            }

                        }

                    //no deadlock
                    } else {
                        int r = get_pcb_resource(*runningProcess);
                        printf("(pid: %d) mutual%d%d requested lock in resource m1Resource%d - ", get_pid(*runningProcess), p, r, r);
                        if(lock(getMutex(mutualR1[r]), *runningProcess) == 1) {
                            if(checkTime()) break;
                            printf("(pid: %d) mutual%d%d requested lock in resource m2Resource%d - ", get_pid(*runningProcess), p, r, r);
                            if(lock(getMutex(mutualR2[r]), *runningProcess) == 1) {
                                if(checkTime()) break;
                                get(mutualR1[r]);
                                if(checkTime()) break;
                                get(mutualR2[r]);
                                if(checkTime()) break;
                                unlock(getMutex(mutualR2[r]), *runningProcess);
                                if(checkTime()) break;
                                unlock(getMutex(mutualR1[r]), *runningProcess);
                                printf("(pid: %d) mutual%d%d got 2 resources: m1Resource%d, m2Resource%d\n", get_pid(*runningProcess), p, r, r, r, r);
                            }
                        }
                    }
                }
                processed = 1;
            }

            //IO Trap
            int iotrap = io_contains_pc(*runningProcess);
            //printf("iotrap is: %d\n", iotrap);
            //assert(IO1Process != NULL);
            //assert(IO2Process != NULL);
            if (iotrap == 1) {
                //printf("IO1\n");
                set_state(*runningProcess, waiting);
                if (IO1Process != NULL) {
                    fifo_enqueue(IO1Queue, *runningProcess);
                } else {
                    //remember
                    IO1Process = *runningProcess;
                    pthread_mutex_trylock(&Io1Mutex);
                    IO1time = 0;
                    pthread_mutex_unlock(&Io1Mutex);
                }
                *runningProcess = NULL;
                printf("printing IO1QUEUE:");
                print_fifo_queue(IO1Queue);

                scheduler(readyProcesses, runningProcess, get_state(*runningProcess));


                //printf("Check22\n");

                //print_priority_queue(*readyProcesses);
                break;
            } else if (iotrap == 2) {
                //printf("IO2\n");
                set_state(*runningProcess, waiting);

                if (IO2Process != NULL) {
                    fifo_enqueue(IO2Queue, *runningProcess);
                    //IO2Process = fifo_dequeue(IO2Queue);
                } else {
                  IO2Process = *runningProcess;
                  pthread_mutex_trylock(&Io2Mutex);
                  IO2time = 0;
                  pthread_mutex_unlock(&Io2Mutex);
                }
                *runningProcess = NULL;
                printf("PRINTING IO2QUEUE:");
                print_fifo_queue(IO2Queue);
                scheduler(readyProcesses, runningProcess, get_state(*runningProcess));
                //printf("Check23\n");


                //print_priority_queue(*readyProcesses);
                break;
            }
            if(*runningProcess != NULL) {

                // if (get_terminate(*runningProcess) > 0 && get_pc(*runningProcess) >= get_max_pc(*runningProcess)) {
                //     //printf("term\n");
                //     set_term_count(*runningProcess, get_term_count(*runningProcess) + 1);
                //     set_pc(*runningProcess, 0);
                if (get_term_count(*runningProcess) > get_terminate(*runningProcess)) {
                        set_state(*runningProcess, halted);
                        scheduler(readyProcesses, runningProcess, get_state(*runningProcess));
                    break;
                }
            }
        }
        if (deadlockMonitor() != 0) {
            printf("THERE IS A DEADLOCK\n");
        }
        
		iteration ++;
		//printf("ITERATION IS: %d\n", iteration);
        if (iteration == 10000){
            printf("we ended\n");
            break;
        }
    }
}

//Returns 0 for NO deadlock,
//Returns another number for a deadlock.
int deadlockMonitor() {
    int i;
    int count = 0;
    for (i = 0; i < RESOURCEPROCESSNUMBER; i++) {
        if (get_mutex_pcb(getMutex(mutualR1[i])) != NULL
            && get_mutex_pcb(getMutex(mutualR2[i])) != NULL
            && (get_pid(get_mutex_pcb(getMutex(mutualR1[i]))) !=
                get_pid(get_mutex_pcb(getMutex(mutualR2[i]))))){
                count++;
            }
    }
    return count;
}

int IOTimer(PriorityQ_p * readyProcesses) {
	// if (io_contains_pc(*runningProcess) == 1) {//enter logic to enqueue pcb into IO1queue if value matches the IO1 array.
 //        fifo_enqueue(IOQueue, *runningProcess);
	// }
    pthread_mutex_trylock(&Io1Mutex);
    int check = IO1time;
    //printf("check is: %d\n", check);
    pthread_mutex_unlock(&Io1Mutex);
    pthread_mutex_trylock(&Io2Mutex);
    int check2 = IO2time;
    pthread_mutex_unlock(&Io2Mutex);
    //printf("check is: %d\n", check);
    //printf("check2 is: %d\n", check2);
    if (check == 1) {
        printf("I/O 1 complete\n");
        pthread_mutex_trylock(&Io1Mutex);
        IO1time = 0;
        pthread_mutex_unlock(&Io1Mutex);
        if(IO1Process != NULL) {
            set_state(IO1Process, ready);
            printf("in IOTimer, check = 1, after this I go to scheduler\n");
            scheduler(readyProcesses, &IO1Process, get_state(IO1Process));

        }
        return 1;
    } else if (check2 == 1) {
        printf("I/O 2 complete\n");
        pthread_mutex_trylock(&Io2Mutex);
        IO2time = 0;
        pthread_mutex_unlock(&Io2Mutex);
        if(IO2Process != NULL) {
            set_state(IO2Process, ready);
            scheduler(readyProcesses, &IO2Process, get_state(IO2Process));
        }
        return 2;
    }
    return 0;
}

// The psuedo-ISR, sets the state of the running process,
// calls the scheduler and updates the PC.
int pseudoISR(PriorityQ_p * readyProcesses, PCB_p* runningProcess) {

    // Sets the status to interrupted.
    if (get_state(*runningProcess) != waiting && get_state(*runningProcess) != halted) set_state(*runningProcess, interrupted);

    sysStack = get_pc(*runningProcess);

    // save pc to pcb
    //set_pc(*runningProcess, sysStack);

    // scheduler up call
    //printf("yea14\n");
    scheduler(readyProcesses, runningProcess, get_state(*runningProcess));

    // IRET (update current pc)
    //currentPC = sysStack;
//printf("Check11\n");

    return SUCCESSFUL;
}
int startTimer(int priority) {
	theTime = getCyclesFromPriority(priority);
}

int timer(PCB_p pcb) {

	// if (theTime <= 0) {
  //       //startTimer(get_priority(pcb));
  //       return 1;
  //   }
	// else {
	// 	theTime -= 1;
	// 	return 0;
	// }
}

// Runs the scheduler to handle interrupts.
int scheduler(PriorityQ_p * readyProcesses, PCB_p* runningProcess, int interrupt) {
//scheduler(readyProcesses, *runningProcess, 0)
    //printf("\n%d\n", interrupt);

//printf("somewhere in scheduler\n");
//printf("interrupt is: %d\n", interrupt);

  //printf("pid is %d\n", get_pid(*runningProcess));


    switch(interrupt) {
        //case 1:

        case interrupted:
        //printf("Check1\n");
            dispatcher(readyProcesses, runningProcess);


            break;
        //case 2:

        case halted: // if the state is interrupted move to dieing processes and then call the dispatcher.

            fifo_enqueue(dieingProcesses, *runningProcess);

            dispatcher(readyProcesses, runningProcess);
          //  printf("Check2\n");
            break;

        case waiting: //IO trap
        //case 3:

            dispatcher(readyProcesses, runningProcess);
            break;
        //case 4:
        case ready: //IO completion
	    //printf("Check4\n");
            //printf("case ready started\n");
            pq_enqueue(*readyProcesses, *runningProcess);
            //printf("case ready finished\n");
            break;
        case none://no pcb yet
            dispatcher(readyProcesses, runningProcess);
        default:
            // error handling as needed
            break;
    }

    //enqueues new processes
    while(!fifo_is_empty(newProcesses)) {



      //print_fifo_queue(newProcesses);

      //print_pq(*readyProcesses);
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
         //printf("Check69\n");
           PCB_p dieingPCB = fifo_dequeue(dieingProcesses);
           destroy_pcb(dieingPCB);
       }

   }


    // after some time S move all processes into Q0.
    if(quantumCounter <= 0) {
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

    if(*runningProcess != NULL &&
            get_state(*runningProcess) != halted &&
            get_state(*runningProcess) != waiting) {
        //printf("Check3\n");
        // update the pc counter.
        //set_pc(*runningProcess, sysStack);
        // set state.
        set_state(*runningProcess, ready);


        // Increments the cyassertcles of the process.
        //unsigned int cycles = get_cycles(*runningProcess);

        int priority = get_priority(*runningProcess);
        //printf("\n%d", priority);

        //if (cycles % getCyclesFromPriority(priority) == 0) {
        if (priority == MAX_PRIORITY) {
            set_priority(*runningProcess, 0); // will go back to the highest priority queue
        } else {
          //  printf("\n%d", priority + 1);
            set_priority(*runningProcess, priority + 1);
        }

        //}
        //cycles++;
        //set_cycles(*runningProcess, cycles);

        // enqueue

    }

    if(*runningProcess != NULL && get_state(*runningProcess) != halted) {
        pq_enqueue(*readyProcesses, *runningProcess);
    }

    //handle IO

    //printf("Check3\n");

    // dequeue
    //printf("size of PQ is: %d\n", pq_size(*readyProcesses));
    //print_pq(*readyProcesses);

    if (pq_size(*readyProcesses) == 0) {
            //PQ is empty...What should we do?
        pq_enqueue(*readyProcesses, create_pcb());
        // printf("size of PQ is: %d\n", pq_size(*readyProcesses));
    }
    *runningProcess = pq_dequeue(*readyProcesses);
    pthread_mutex_lock(&priorityMutex);
    thePriority = get_priority(*runningProcess);
    pthread_mutex_unlock(&priorityMutex);

  //  printf("%d\n", processCounter);
    //print_priority_queue(*readyProcesses);

    // update state to running
    // set state
    if(*runningProcess != NULL) {
        set_state(*runningProcess, running);
        //printf("Check3\n");
        sysStack = get_pc(*runningProcess);

        return SUCCESSFUL;

    } else {
        return FAILED;
        printf("dispatcher HERE?\n");
    }
    //printf("Check4\n");

    return SUCCESSFUL;
}

void prepareNewProcesses() {
    if(processCounter <= PROCESSNUMBER) {// && privilegedCount < 4) {
        createNewProcesses(newProcesses, 0);
        //processCounter += fifo_size(newProcesses);
    }

    if(noIOCounter <= NOIOPROCESSNUMBER) {
        createNewProcesses(newProcesses, 1);

    }

    if(prodConCounter < RESOURCEPROCESSNUMBER) {
        createNewProcesses(newProcesses, 2);
    }

   if(mutualCounter < RESOURCEPROCESSNUMBER) {
        createNewProcesses(newProcesses, 3);
    }
}

void moveProcesses (PriorityQ_p * readyProcesses) {
    FIFO_Queue_p save = create_fifo_queue();

    //PriorityQ_p tempQueue = create_pq();
    while(!pq_isEmpty(*readyProcesses)) {
        PCB_p tempPCB = pq_dequeue(*readyProcesses);

        set_priority(tempPCB, 0);
        fifo_enqueue(save, tempPCB);
    }
    while(!fifo_is_empty(save)) {
        PCB_p tempPCB = fifo_dequeue(save);
        pq_enqueue(*readyProcesses, tempPCB);
    }
    destroy(save);

}

// Creates a random number of processes to be added to the
// the list of new processes.
/*type 0 = normal
       1 = NO IO
       2 = Producer-Consumer
       3 = Mutual Resource Pairs*/
int createNewProcesses(FIFO_Queue_p newProcesses, int type) {
    int i;
    if (type == 0) {//create normal processes
		for(i = 0; i < rand() % 5; i++) {
			PCB_p pcb = create_pcb();
			fifo_enqueue(newProcesses, pcb);
      processCounter++;
        }
    } else if (type == 1) {//create NO IO.
		for(i = 0; i < rand() % 5; i++) {
			PCB_p pcb = create_noio_pcb();
            fifo_enqueue(newProcesses, pcb);
            noIOCounter++;
		}
		// producer, consumer pairs
	} else if (type == 2) {
		for(i = 0; i < rand() % 3; i++) {
            if(prodConCounter < RESOURCEPROCESSNUMBER) {
                PCB_p pcb1 = create_prod_pcb(prodConCounter);
                fifo_enqueue(newProcesses, pcb1);
                PCB_p pcb2 = create_cons_pcb(prodConCounter);
                fifo_enqueue(newProcesses, pcb2);
                prodConCounter++;
            } else {
                break;
            }
		}
		// mutual resource pairs
	} else if (type == 3) {
		for(i = 0; i < rand() % 3; i++) {
			if(mutualCounter < RESOURCEPROCESSNUMBER) {
                PCB_p pcb1 = create_mutual_pcb(mutualCounter, 0);
                fifo_enqueue(newProcesses, pcb1);
                PCB_p pcb2 = create_mutual_pcb(mutualCounter, 1);
                fifo_enqueue(newProcesses, pcb2);
                mutualCounter++;
            } else {
                break;
            }
		}
	}
}

// Returns the number of cycles that each queue uses.
unsigned int getCyclesFromPriority(unsigned int priority) {
    return 8 * (priority + 1);
}
void *timerFunc(void *t) {
  //timer startup
  for(;;) {
    pthread_mutex_lock(&priorityMutex);

    int length = getCyclesFromPriority(thePriority)* NANO_SECOND_MULTIPLIER * 100000;
    pthread_mutex_unlock(&priorityMutex);// * NANO_SECOND_MULTIPLIER/ 10000);
    struct timespec timing;
    timing.tv_sec = 0;
    timing.tv_nsec = length;

    nanosleep(&timing, NULL);
    //printf(" TIMER\n");
    pthread_mutex_lock(&timerMutex);
    theTime = 1;
    pthread_mutex_unlock(&timerMutex);

  }


}

int checkTime() {
    pthread_mutex_trylock(&timerMutex);
    int b = theTime;
    pthread_mutex_unlock(&timerMutex);
    return b;

}
void *IO1Func(void *t) {
  //sleep(30);
  struct timespec timing;
  //timer startup
  for(;;) {
    int ioval = 1;
    pthread_mutex_lock(&Io1Mutex);
    ioval = IO1time;
    //printf("ioval is: %d\n", ioval);

    pthread_mutex_unlock(&Io1Mutex);
    if(ioval == 0 ) {
      int length = ((rand() % 3) + 2) * 3 * getCyclesFromPriority(7) * NANO_SECOND_MULTIPLIER * 1000;
    // * NANO_SECOND_MULTIPLIER/ 10000);

      timing.tv_sec = 3;
      timing.tv_nsec = length;
      nanosleep(&timing, NULL);

      //printf(" IO1\n");
      pthread_mutex_lock(&Io1Mutex);
      IO1time = 1;

      pthread_mutex_unlock(&Io1Mutex);
    }
  }


}
void *IO2Func(void *t) {
  //sleep(30);
  struct timespec timing;
  //timer startup
  for(;;) {
    int ioval = 1;
    pthread_mutex_trylock(&Io2Mutex);
    ioval = IO2time;
    pthread_mutex_unlock(&Io2Mutex);// * NANO_SECOND_MULTIPLIER/ 10000);
    if(ioval == 0) {
      int length = ((rand() % 3) + 2) * 3 * getCyclesFromPriority(7) * NANO_SECOND_MULTIPLIER;
      timing.tv_sec = 3;
      timing.tv_nsec = length;
      nanosleep(&timing, NULL);
      printf(" IO2\n");
      pthread_mutex_trylock(&Io2Mutex);
      IO2time = 1;
      pthread_mutex_unlock(&Io2Mutex);
    }
  }
}

// Main, to jump start the OS simulation and initialize variables.
int main() {
    // Seed RNG
    srand(time(NULL));
    //IO and Timer threads
    pthread_t timerThread, IO1Thread, IO2Thread;
    //timerThread = pthread_create(timerThread, NULL, timer);'
    pthread_mutex_init(&timerMutex, NULL);
    pthread_mutex_init(&Io1Mutex, NULL);
    pthread_mutex_init(&Io2Mutex, NULL);
    pthread_mutex_init(&priorityMutex, NULL);
    // Initialize Vars
    newProcesses = create_fifo_queue();
    PriorityQ_p readyProcesses = create_pq();

    PCB_p runningProcess = NULL;

    //FIFO_Queue_p dieingProcesses = create_fifo_queue();
    dieingProcesses = create_fifo_queue();
    PCB_p IO1Process = NULL;
    PCB_p IO2Process = NULL;
    IO1Queue = create_fifo_queue();
    IO2Queue = create_fifo_queue();
    // set a process to running
    thePriority = 0;

    set_state(runningProcess, running);
    int i;
    for(i = 0; i < 10; i++) {
        prodConR[i] = create_resource();
        mutualR1[i] = create_resource();
        mutualR2[i] = create_resource();
    }

    deadlock = 0;
    currentPC = 0;
    sysStack = 0;
    dispatchCount = 0;
    privilegedCount = 0;
    quantum = 3 * getCyclesFromPriority(7); // multiple of middle queues quantum size.
    quantumCounter = quantum;
    processCounter = 0;
    noIOCounter = 0;
    prodConCounter = 0;
    mutualCounter = 0;
    //create threads

    int timerthreadcreated = pthread_create(&timerThread, NULL, timerFunc, NULL);
    int IO1threadcreated = pthread_create(&IO1Thread, NULL, IO1Func, NULL);
    int IO2threadcreated = pthread_create(&IO2Thread, NULL, IO2Func, NULL);


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
    for(i = 0; i < 10; i++) {
        destroy_resource(prodConR[i]);
        destroy_resource(mutualR1[i]);
        destroy_resource(mutualR2[i]);
    }
    pthread_exit(NULL);
}
