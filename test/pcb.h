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

#ifndef PCB_H
#define PCB_H

// Pointer to the cpu_context structure.
typedef struct CPU_context_s *CPU_context_p;

typedef struct PCB_s * PCB_p;

enum state_type {new, ready, running, interrupted, waiting, halted};

/* constructor */
// Creates a pcb with the PCB pointer and the CPU_context pointer
PCB_p create_pcb();

/* deconstructor */
// Deallocates the memory for the pcb passed in.
void destroy_pcb(PCB_p p);

/* functions */

//Check if PC equals to any of the values of I/O array
//Returns 1 if io_1 contains,
//Returns 2 if io_2 contains,
//Returns 0 otherwise.
int io_contains_pc(PCB_p pcb);

// Assigns the PID.
void set_pid(PCB_p pcb, unsigned int num);

// Returns the pid of the pcb.
unsigned int get_pid(PCB_p pcb);

//Returns second of creation of pcb.
int get_creation_sec(PCB_p pcb);

//Sets max_pc value.
void set_max_pc(PCB_p pcb, unsigned int new_max_pc);

//Returns max_pc value.
unsigned int get_max_pc(PCB_p pcb);

//Sets terminate value of the pcb.
void set_terminate(PCB_p pcb, unsigned int new_terminate);

//Returns the terminate value of the pcb.
unsigned int get_terminate(PCB_p pcb);

// Sets the state_type of the pcb passed in.
void set_state(PCB_p pcb, enum state_type type);

// Returns the state of the pcb.
enum state_type get_state(PCB_p pcb);

// Sets the priority of the pcb.
void set_priority(PCB_p pcb, unsigned char priority);

// gets the priority of the pcb.
unsigned char get_priority(PCB_p pcb);

// Returns the cycles the pcb has run.
unsigned int get_cycles(PCB_p pcb);

// Sets the cycles this pcb has run.
void set_cycles(PCB_p pcb, unsigned int newCycles);

// returns the pcbs pc value.
unsigned int get_pc(PCB_p pcb);

// Sets the pcbs pc value to the given pc.
void set_pc(PCB_p pcb, unsigned int pc);

// Returns whether or no the pcb is privileged.
int isPrivileged(PCB_p pcb);

// Sets the pcb to privileged.
void setPrivileged(PCB_p pcb);

// Prints a string representation of the pcb passed in.
void print_pcb_file(PCB_p pcb, FILE * fp);

void print_pcb(PCB_p pcb);

void print_context(CPU_context_p context, FILE * fp);

void set_term_count(PCB_p pcb, unsigned int new_term_count);

unsigned int get_term_count(PCB_p pcb);

const char* get_state_name(enum state_type state);

#endif
