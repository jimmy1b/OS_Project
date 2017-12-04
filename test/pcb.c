/*
TCSS422 - Operating Systems
Problem 4

Group Members:
Joshua Lau
Alisher Baimenov

Changes:
    We added to variables the the PCB_s struct, a bool value to
    determine whether or not the PCB is privileged and added
    a cycles variable to track how many cycles the PCB has run.

    This information is specific to the PCB so we decided to store this
    information here.
*/

#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#include "pcb.h"

// Global to determine the next possible PID value for a new PCB
unsigned int currentpid = 1;

// Structure definition for the CPU State for the LC-3 Processor.
struct CPU_context_s {
    unsigned int pc;
    unsigned int ir;
    unsigned int psr;
    unsigned int r0;
    unsigned int r1;
    unsigned int r2;
    unsigned int r3;
    unsigned int r4;
    unsigned int r5;
    unsigned int r6;
    unsigned int r7;
} CPU_context_s;

// Structure definition for the process control block.
//Each PCB gets unique IO_1/2_traps value.
struct PCB_s {
    unsigned int pid;
    enum state_type state;
    unsigned int parent;
    unsigned char priority;
    unsigned char *mem;
    unsigned int size;
    unsigned char channel_no;
    CPU_context_p context;
    unsigned int cycles;
    _Bool privileged;
    //fields added in problem 4.
    unsigned int max_pc;
    time_t t;
    struct tm creation;
    struct tm termination;
    unsigned int terminate;
    unsigned int term_count;
    unsigned int io_1_traps[4];
    unsigned int io_2_traps[4];
    //fields added in final project.
    enum pair_type pair;
    //enum pcb_type type;
} PCB_s;

/* constructor */
// Creates a pcb with the PCB pointer and the CPU_context pointer
PCB_p create_pcb() {
    PCB_p pcb = (PCB_p) malloc(sizeof(PCB_s));
    pcb->context = (CPU_context_p) malloc(sizeof(CPU_context_s));
    if (!pcb) {
        return NULL;
    } else {
        pcb->context->pc = 0;
        pcb->context->ir = 0;
        pcb->context->psr = 0;
        pcb->context->r0 = 0;
        pcb->context->r1 = 0;
        pcb->context->r2 = 0;
        pcb->context->r3 = 0;
        pcb->context->r4 = 0;
        pcb->context->r5 = 0;
        pcb->context->r6 = 0;
        pcb->context->r7 = 0;

        pcb->pid = currentpid++;
        pcb->state = new;
        pcb->parent = 0;
        pcb->priority = 0;
        pcb->mem = 0;
        pcb->size = 0;
        pcb->channel_no = 0;
        pcb->cycles = 0;
        pcb->privileged = 0;
        //pcb->max_pc = 2345;
        pcb->t = time(NULL);
        pcb->creation = *localtime(&(pcb->t));
        //pcb->termination = ???
        //pcb->terminate = 2;
        pcb->term_count = 0;
        srand(time(NULL));
        pcb->max_pc = rand() % 4000;
        pcb->terminate = rand() % 10;
        int i;
        for (i = 0; i < 4; i++) {
            int val = rand() % (pcb->max_pc);
            if (i == 0) {
                pcb->io_1_traps[i] = val;
            } else {
                while (val == pcb->io_1_traps[i - 1]) {
                    val = rand() % (pcb->max_pc);
                }
                pcb->io_1_traps[i] = val;
            }
        }
        for (i = 0; i < 4; i++) {
            int val = rand() % (pcb->max_pc);
            if (i == 0) {
                pcb->io_2_traps[i] = val;
            } else {
                while (val == pcb->io_2_traps[i - 1]) {
                    val = rand() % (pcb->max_pc);
                }
                pcb->io_2_traps[i] = val;
            }
        }
    }
    return pcb;
}
PCB_p create_noio_pcb() {
  PCB_p pcb = (PCB_p) malloc(sizeof(PCB_s));
  pcb->context = (CPU_context_p) malloc(sizeof(CPU_context_s));
  if (!pcb) {
      return NULL;
  } else {
      pcb->context->pc = 0;
      pcb->context->ir = 0;
      pcb->context->psr = 0;
      pcb->context->r0 = 0;
      pcb->context->r1 = 0;
      pcb->context->r2 = 0;
      pcb->context->r3 = 0;
      pcb->context->r4 = 0;
      pcb->context->r5 = 0;
      pcb->context->r6 = 0;
      pcb->context->r7 = 0;

      pcb->pid = currentpid++;
      pcb->state = new;
      pcb->parent = 0;
      pcb->priority = 0;
      pcb->mem = 0;
      pcb->size = 0;
      pcb->channel_no = 0;
      pcb->cycles = 0;
      pcb->privileged = 0;
      //pcb->max_pc = 2345;
      pcb->t = time(NULL);
      pcb->creation = *localtime(&(pcb->t));
      //pcb->termination = ???
      //pcb->terminate = 2;
      pcb->term_count = 0;
      srand(time(NULL));
      pcb->max_pc = rand() % 4000;
      pcb->terminate = rand() % 10;
      int i;
      for (i = 0; i < 4; i++) {
          int val = pcb->max_pc + 1;
          pcb->io_1_traps[i] = val;
          pcb->io_2_traps[i] = val;


      }
  }
  return pcb;
}

/* deconstructor */
// Deallocates the memory for the pcb passed in.
void destroy_pcb(PCB_p p) {
    if (!p) {
        return;
    } else {
        free(p->context);
        free(p);
    }
}

/* functions */


//Check if PC equals to any of the values of I/O array
//Returns 1 if io_1 contains,
//Returns 2 if io_2 contains,
//Returns 0 otherwise.
int io_contains_pc(PCB_p pcb) {
    int pc = pcb->context->pc;
    int i;
    for (i = 0; i < 4; i++) {
        //printf("%d, %d %d\n", pc, pcb->io_1_traps[i], pcb->io_2_traps[i]);
        if (pcb->io_1_traps[i] == pc) {
            return 1;
        } else if (pcb->io_2_traps[i] == pc) {
            return 2;
        }
    }
    return 0;
}



// Assigns the PID.
void set_pid(PCB_p pcb, unsigned int num) {
    pcb->pid = num;
}

unsigned int get_pid(PCB_p pcb) {
    return pcb->pid;
}

//Pair type setter/getters.
void set_pair(PCB_p pcb, enum pair_type new_pair) {
    pcb->pair = new_pair;
}

enum pair_type get_pair(PCB_p pcb) {
    return pcb->pair;
}

//Setters and getters for max_pc
void set_max_pc(PCB_p pcb, unsigned int new_max_pc) {
    pcb->max_pc = new_max_pc;
}

unsigned int get_max_pc(PCB_p pcb) {
    return pcb->max_pc;
}

//Returns time of creation of pcb.
int get_creation_sec(PCB_p pcb) {
    return (pcb->creation).tm_sec;
}

//Sets termination time of the pcb.
void set_termination(PCB_p pcb) {
    pcb->termination = *localtime(&(pcb->t));
}

//Getters and setters of terminate field.
void set_terminate(PCB_p pcb, unsigned int new_terminate) {
    pcb->terminate = new_terminate;
}

unsigned int get_terminate(PCB_p pcb) {
    return pcb->terminate;
}

//Getters and setter of term_count.
void set_term_count(PCB_p pcb, unsigned int new_term_count) {
    pcb->term_count = new_term_count;
}

unsigned int get_term_count(PCB_p pcb) {
    return pcb->term_count;
}

// Sets the state_type of the pcb passed in.
void set_state(PCB_p pcb, enum state_type type) {
    pcb->state = type;
}

enum state_type get_state(PCB_p pcb) {
    return pcb->state;
}

// Sets the priority of the pcb.
void set_priority(PCB_p pcb, unsigned char priority) {
    pcb->priority = priority;
}

unsigned char get_priority(PCB_p pcb) {
    return pcb->priority;
}

// Returns the cycles the pcb has run.
unsigned int get_cycles(PCB_p pcb) {
    return pcb->cycles;
}

// Sets the cycles this pcb has run.
void set_cycles(PCB_p pcb, unsigned int newCycles) {
    pcb->cycles = newCycles;
}

// Returns whether or no the pcb is privileged.
int isPrivileged(PCB_p pcb) {
    return pcb->privileged;
}

// Sets the pcb to privileged.
void setPrivileged(PCB_p pcb) {
    pcb->privileged = 1;
}

// returns the pcbs pc value.
unsigned int get_pc(PCB_p pcb) {
    if(pcb == NULL) return -1;
    if(pcb->context == NULL) return -1;

    return pcb->context->pc;
}

// Sets the pcbs pc value to the given pc.
void set_pc(PCB_p pcb, unsigned int pc) {
    pcb->context->pc = pc;
}


// Prints a string representation of the pcb passed in.
void print_pcb_file(PCB_p pcb, FILE * fp) {
    fprintf(fp, "Pid: %X, Priority: %X, State:%X ", get_pid(pcb), get_priority(pcb), pcb->state);
    print_context(pcb->context,fp);
}

void print_pcb(PCB_p pcb){
    printf("PCB: PID: %u, PRIORITY: %u, PC: %u,%u\n", pcb->pid, pcb->priority, pcb->context->pc, pcb->cycles);
}

void print_context(CPU_context_p context, FILE * fp) {
    fprintf(fp, "Context Data: [pc: %u, ir: %u, psr: %u, r0: %u, r1: %u, r2: %u, r3: %u, r4: %u, r5: %u, r6: %u, r7: %u]\n",
           context->pc, context->ir, context->psr, context->r0, context->r1, context->r2, context->r3,
           context->r4, context->r5, context->r6, context->r7);
}

const char* get_state_name(enum state_type state) {
    switch (state) {
        case new: return "New";
        case ready: return "Ready";
        case running: return "Running";
        case interrupted: return "Interrupted";
        case waiting: return "Waiting";
        case halted: return "Halted";
    }
  }
