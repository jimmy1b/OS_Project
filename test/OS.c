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
#define NANO_SECOND_MULTIPLIER  1000000  // 1 millisecond = 1,000,000 Nanoseconds

unsigned int sysStack;
unsigned int currentPC;
unsigned int dispatchCount;
PCB_p privilegedPCBs[4]; // array of PCBs
unsigned int privilegedCount;
unsigned int quantum;
unsigned int quantumCounter;
unsigned int processCounter;
unsigned int noIOCounter;
int thePriority, theTime, IO1time, IO2time, IO1priority, IO2Priority;

PCB_p IO1Process;
PCB_p IO2Process;
FIFO_Queue_p IO1Queue;
FIFO_Queue_p IO2Queue;

FIFO_Queue_p newProcesses;
FIFO_Queue_p newNoIos;
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

    srand(time(NULL));

    int iteration = 1;
    int t, a, b, i, j;
    //startTimer(get_priority(*readyProcesses));
    // Main Loop
    // One cycle is one quantum
    for( ; ; ) { // for spider
        //printf("Numb\n");
        // stops making processes after 48 and if there are at least 4 Privileged pcbs
        if(processCounter <= PROCESSNUMBER) {// && privilegedCount < 4) {
            createNewProcesses(newProcesses, 0);
            processCounter += fifo_size(newProcesses);
        }
        if(processCounter < noIOCounter) {
          createNewProcesses(newNoIos, 1);

          noIOCounter += fifo_size(newNoIos);
        }




        //Simulate Process Running
        //set_pc(*runningProcess, get_pc(*runningProcess) + 1);

        // Push to SysStack
        //sysStack = currentPC;
//printf("Check5\n");









        for( ; ; ) {
            //timer int check


            //t = timer(*runningProcess);

            if(*runningProcess != NULL) {
                set_pc(*runningProcess, get_pc(*runningProcess) + 1);
                //printf("%d\n", get_pc(*runningProcess));
            // } else if (!pq_isEmpty(*readyProcesses)) {
            //     //*runningProcess = pq_dequeue(*readyProcesses);
            }





            pthread_mutex_trylock(&timerMutex);
            t = theTime;

            pthread_mutex_unlock(&timerMutex);
            if (t == 1) {

                //printf("Timer\n");
                pthread_mutex_trylock(&timerMutex);
                theTime = 0;

                pthread_mutex_unlock(&timerMutex);
            printf("%d\nmax pc : %d\n", get_pc(*runningProcess), get_max_pc(*runningProcess));
            print_priority_queue(*readyProcesses);

	       		pseudoISR(readyProcesses, runningProcess);

                //startTimer(get_priority(*runningProcess));




		        break;
		    }

            //IO timer int check
            pthread_mutex_trylock(&Io1Mutex);
            a = IO1time;
            pthread_mutex_unlock(&Io1Mutex);
            pthread_mutex_trylock(&Io2Mutex);
            b = IO2time;
            pthread_mutex_unlock(&Io2Mutex);

		   // a = IOTimer
		    if (a == 1 && IO1Process != NULL) {
//printf("Check20\n");
		    	//throw io 1 interrupt
                set_state(IO1Process, ready);
                scheduler(readyProcesses, &IO1Process, get_state(IO1Process));
                // if(!fifo_is_empty){
                //   IO1Process = fifo_dequeue(IO1Queue);
                // }

                print_fifo_queue(IO1Queue);
                pseudoISR(readyProcesses, runningProcess);
//printf("Check5\n");

		    	break;
		    }

            if (b == 1 && IO2Process != NULL) {
//printf("Check21\n");
		    	//throw io 2 interrupt
                printf("yeayea14\n");
                print_fifo_queue(IO2Queue);
                set_state(IO2Process, ready);
                scheduler(readyProcesses, &IO2Process, get_state(IO2Process));
                if(!fifo_is_empty){
                    IO2Process = fifo_dequeue(IO2Queue);
                }
                print_fifo_queue(IO2Queue);
                pseudoISR(readyProcesses, runningProcess);
//printf("Check6\n");

		    	break;
		    }

            //IO Trap
            int iotrap = 3;//io_contains_pc(*runningProcess);
            //assert(IO1Process != NULL);
            //assert(IO2Process != NULL);
            if (iotrap == 1) {
                printf("IO1\n");
                set_state(*runningProcess, waiting);
                scheduler(readyProcesses, runningProcess, get_state(*runningProcess));
                if (IO1Process != NULL) {
                    fifo_enqueue(IO1Queue, *runningProcess);
                } else {
                    //remember
                    IO1Process = *runningProcess;
                }
                print_fifo_queue(IO1Queue);

                printf("Check22\n");

                //print_priority_queue(*readyProcesses);
                break;
            } else if (iotrap == 2) {
                //printf("IO2\n");
                set_state(*runningProcess, waiting);

                if (IO2Process != NULL) {
                    fifo_enqueue(IO2Queue, *runningProcess);
                } else {
                IO2Process = fifo_dequeue(IO2Queue);
                }
                print_fifo_queue(IO2Queue);
                scheduler(readyProcesses, runningProcess, get_state(*runningProcess));
                printf("Check23\n");


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
    //printf("Check10\n");

                        break;

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
    quantumCounter--;
    printf("\nQuantum counter%d\n", quantumCounter);
		iteration ++;
		//printf("ITERATION IS: %d\n", iteration);
        if (iteration == 1000 && ((pq_isEmpty(*readyProcesses) && processCounter >= PROCESSNUMBER))){
          printf("we ended\n");
        break;
      }
    }
}

int IOTimer(PriorityQ_p * readyProcesses) {
	// if (io_contains_pc(*runningProcess) == 1) {//enter logic to enqueue pcb into IO1queue if value matches the IO1 array.
 //        fifo_enqueue(IOQueue, *runningProcess);
	// }
  pthread_mutex_lock(&Io1Mutex);
  int check = IO1time;
  pthread_mutex_unlock(&Io1Mutex);
  pthread_mutex_lock(&Io2Mutex);
  int check2 = IO2time;
  pthread_mutex_unlock(&Io2Mutex);
  if (check == 1) {
    printf("I/O 1 complete\n");
    pthread_mutex_lock(&Io1Mutex);
    IO1time = 0;
    pthread_mutex_unlock(&Io1Mutex);
    if(IO1Process != NULL) {
      set_state(IO1Process, ready);
      scheduler(readyProcesses, &IO1Process, get_state(IO1Process));
    }
    return 1;
  } else if (check2 == 1) {
    printf("I/O 2 complete\n");
    pthread_mutex_lock(&Io2Mutex);
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
	// if (IO1time <= 0 && IO1Process != NULL) {
  //       printf(" I/O 1 complete\n");
  //       IO1time = ((rand() % 3) + 3) * getCyclesFromPriority(7) ;
  //     	if(!fifo_is_empty(IO1Queue)) {
  //     		IO1Process = fifo_dequeue(IO1Queue);
  //     		}
  //
  //       set_state(IO1Process, ready);
  //       scheduler(readyProcesses, &IO1Process, get_state(IO1Process));
  //
  //       // IO1Process = fifo_dequeue(IO1Queue);
  //       return 1;

	// } else if (IO2time <= 0 && IO2Process != NULL) {
  //       printf(" I/O 2 complete\n");
	// //printf("Check1\n");
  //       IO2time = ((rand() % 3) + 3) * getCyclesFromPriority(7) ;
	// //printf("Check2\n");
	// if(!fifo_is_empty(IO2Queue)) {
	// 	IO2Process = fifo_dequeue(IO2Queue);
	// 	}
	// //printf("Check3\n");
  //       // set_state(IO2Process, ready);
  //       // scheduler(readyProcesses, IO1Process, get_state(IO1Process));
  //
  //       // IO2Process = fifo_dequeue(IO2Queue);
  //       return 2;
  //
  //   } else {
  //
  //       IO1time -= 1;
  //       IO2time -= 1;
  //       return 0;
  //   }



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
//printf("Check6\n");
    sysStack = get_pc(*runningProcess);
//printf("Check50\n");
    // save pc to pcb
    //set_pc(*runningProcess, sysStack);
//printf("Check51\n");
    // scheduler up call

    scheduler(readyProcesses, runningProcess, get_state(*runningProcess));
//printf("Check52\n");
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
    //printf("\n%d\n", interrupt);

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
            pq_enqueue(*readyProcesses, *runningProcess);
            break;
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
    while(!fifo_is_empty(newNoIos)) {

            // dequeue and print next pcb
            PCB_p pcb = fifo_dequeue(newNoIos);
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
        pq_enqueue(*readyProcesses, *runningProcess);

    }
    //handle IO

    //printf("Check3\n");

    // dequeue
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
        return FAILED
    }
    //printf("Check4\n");

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
int createNewProcesses(FIFO_Queue_p newProcesses, int type) {
    int i;
    if (type == 0) {//create normal processes
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
    } else if (type == 1) {
		for(i = 0; i < rand() % 5; i++) {
			PCB_p pcb = create_noio_pcb();


          // 20% chance that the pcb will become privileged.
          /*if(rand() % 100 < 20 ) {//&& privilegedCount < 4)  {
              setPrivileged(pcb);
              privilegedPCBs[privilegedCount] = pcb;
              privilegedCount++;
          })*/
          fifo_enqueue(newProcesses, pcb);
		}
		// producer, consumer pairs
	} else if (type == 2) {
		for(i = 0; i < 10; i++) {
			PCB_p pcb1 = create_prod_pcb(i);
			fifo_enqueue(newProcesses, pcb1);
			PCB_p pcb2 = create_cons_pcb(i);
			fifo_enqueue(newProcesses, pcb2);



          // 20% chance that the pcb will become privileged.
          /*if(rand() % 100 < 20 ) {//&& privilegedCount < 4)  {
              setPrivileged(pcb);
              privilegedPCBs[privilegedCount] = pcb;
              privilegedCount++;
          })*/
          
		}

		// mutual resource pairs
	} else if (type == 3) {
		for(i = 0; i < 10; i++) {
			PCB_p pcb1 = create_mutual_pcb(i);
			fifo_enqueue(newProcesses, pcb1);
			PCB_p pcb2 = create_mutual_pcb(i);
			fifo_enqueue(newProcesses, pcb2);

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

    int length = getCyclesFromPriority(thePriority)* NANO_SECOND_MULTIPLIER * 1000;
    pthread_mutex_unlock(&priorityMutex);// * NANO_SECOND_MULTIPLIER/ 10000);
    struct timespec timing;
    timing.tv_sec = 0;
    timing.tv_nsec = length;



    nanosleep(&timing, NULL);

    //printf(" TIMER\n");
    pthread_mutex_trylock(&timerMutex);
    theTime = 1;
    pthread_mutex_unlock(&timerMutex);

  }


}
void *IO1Func(void *t) {
  sleep(30);
  struct timespec timing;
  //timer startup
  for(;;) {
    pthread_mutex_trylock(&Io1Mutex);
    int ioval = IO1time;

    pthread_mutex_unlock(&Io1Mutex);
    if(ioval == 0 ) {
      int length = ((rand() * 3) + 2) * 3 * getCyclesFromPriority(7) * NANO_SECOND_MULTIPLIER * 1000;
    // * NANO_SECOND_MULTIPLIER/ 10000);

      timing.tv_sec = 11;
      timing.tv_nsec = length;
      nanosleep(&timing, NULL);

      printf(" IO1\n");
      pthread_mutex_trylock(&Io1Mutex);
      IO1time = 1;

      pthread_mutex_unlock(&Io1Mutex);
    }






  }


}
void *IO2Func(void *t) {
  sleep(30);
  struct timespec timing;
  //timer startup
  for(;;) {
    pthread_mutex_trylock(&Io2Mutex);
    int ioval = IO2time;
    pthread_mutex_unlock(&Io2Mutex);// * NANO_SECOND_MULTIPLIER/ 10000);
    if(ioval == 0) {
      int length = ((rand() * 3) + 2) * 3 * getCyclesFromPriority(7) * NANO_SECOND_MULTIPLIER;
      timing.tv_sec = 5;
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
    // timerMutex = PTHREAD_MUTEX_INITIALIZER;
    // Io1Mutex = PTHREAD_MUTEX_INITIALIZER;
    // Io2Mutex = PTHREAD_MUTEX_INITIALIZER;



    // Initialize Vars
    newProcesses = create_fifo_queue();
    newNoIos = create_fifo_queue();
    PriorityQ_p readyProcesses = create_pq();

    PCB_p runningProcess = create_pcb();

    //FIFO_Queue_p dieingProcesses = create_fifo_queue();
    dieingProcesses = create_fifo_queue();
    // PCB_p IO1Process = create_pcb();
    // PCB_p IO2Process = create_pcb();
    IO1Queue = create_fifo_queue();
    IO2Queue = create_fifo_queue();
    // set a process to running
    thePriority = 0;

    set_state(runningProcess, running);
	 for(int i = 0; i < 10; i++) {
		prodConR[i] = create_resource();
		mutualR1[i] = create_resource();
		mutualR2[i] = create_resource();
    }

    currentPC = 0;
    sysStack = 0;
    dispatchCount = 0;
    privilegedCount = 0;
    quantum = 3 * getCyclesFromPriority(7); // multiple of middle queues quantum size.
    quantumCounter = quantum;
    processCounter = 1;
    //create threads
    int timerthreadcreated = pthread_create(&timerThread, NULL, timerFunc, (void*)runningProcess);
    int IO1threadcreated = pthread_create(&IO1Thread, NULL, IO1Func, (void*)runningProcess);
    int IO2threadcreated = pthread_create(&IO2Thread, NULL, IO2Func, (void*)runningProcess);

    for(int i = 0; i < 10; i++) {
		prodConR[i] = create_resource();
		mutualR1[i] = create_resource();
		mutualR2[i] = create_resource();
    }
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
    pthread_exit(NULL);
}
