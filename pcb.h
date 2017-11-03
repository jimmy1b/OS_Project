typedef struct cpu_context {// CPU state for the LC-3 processor
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
} CPU_context_s;// _s means this is a structure definition

typedef CPU_context_s* CPU_context_p;// _p means that this is a pointer to a structure

enum state_type {new, ready, running, interrupted, waiting, halted, zombie, blocked};

typedef struct pcb {// Process control block
  unsigned int pid; // process identification
  enum state_type state; // process state (running, waiting, etc.)
  unsigned int parent; // parent process pid
  unsigned char priority; // 0 is highest –15is lowest.
  unsigned char * mem; // start of process in memory
  unsigned int size; // number of bytes in process
  unsigned char channel_no; // which I/O device or service Q// if process is blocked, which fifo_queue it is in
  CPU_context_p context; // set of cpu registers// other items to be added as needed.

  unsigned int MAX_PC;
  time_t creation;
  time_t termination;
  unsigned int terminate;
  unsigned int term_count;
  unsigned int * IO_1_TRAPS;
  unsigned int * IO_2_TRAPS;

} PCB_s;

typedef PCB_s* PCB_p;

PCB_p constructor();
int destructor(PCB_p pcb);
unsigned int getPid(PCB_p pcb);
unsigned int getParent(PCB_p pcb);
unsigned char getPriority(PCB_p pcb);
unsigned char * getMem(PCB_p pcb);
unsigned int getSize(PCB_p pcb);
unsigned char getChannel_no(PCB_p pcb);
unsigned int getPC(PCB_p pcb);

unsigned int get_MAX_PC(PCB_p pcb);
int set_MAX_PC(PCB_p pcb, int the_Max_pc);
time_t get_creation(PCB_p pcb);
int set_creation(PCB_p pcb, time_t theTime);
time_t get_termination(PCB_p pcb);
int set_termination(PCB_p pcb, time_t theTime);
unsigned int get_terminate(PCB_p pcb);
int set_terminate(PCB_p pcb, unsigned int theInt);
unsigned int get_term_count(PCB_p pcb);
int set_term_count(PCB_p pcb, unsigned int theInt);
int *  get_IO_1_TRAPS(PCB_p pcb);
int set_IO_1_TRAPS(PCB_p pcb, int * theArr);
int * get_IO_2_TRAPS (PCB_p pcb);
int set_IO_2_TRAPS(PCB_p pcb, int * theArr);

int setState(PCB_p pcb, enum state_type new_state);
int assignPid(PCB_p pcb, unsigned int * currentPid);
int setPid(PCB_p pcb, unsigned int pid);
int setParent(PCB_p pcb, unsigned int parent);
int setPriority(PCB_p pcb, unsigned char priority);
int setMem(PCB_p pcb, unsigned char * mem);
int setSize(PCB_p pcb, unsigned int size);
int setChannel_no(PCB_p pcb, unsigned char channel_no);
int setPC(PCB_p pcb, unsigned int the_pc);
char * toString(PCB_p pcb);
char * context_toString(CPU_context_p context);
