void add_n( fifo_queue new_procs);
int checkTerm();
// void set_rand_pids(unsigned int rand_pids[]);
int contains( int * arr,  int value, int size);

PCB_p find_first_empty(priority_queue pq);
int dispatcher(PCB_p * current, priority_queue pq);
int scheduler(int schedule_bit, PCB_p * current, priority_queue pq, fifo_queue new_procs, fifo_queue old_procs, int * quantum, fifo_queue IO_Queue);

char * to_string_3(int iteration_count, priority_queue pq);
int reset_all_priority(priority_queue pq);
unsigned int increment_pc(PCB_p * current, priority_queue pq, unsigned int * quantum_count);

int terminate( PCB_p * current, priority_queue pq, fifo_queue new_procs, fifo_queue old_procs, int * quantum, fifo_queue IO_Queue);
int timer_interrupt(PCB_p * current, priority_queue pq, fifo_queue new_procs, fifo_queue old_procs, int * quantum, fifo_queue IO_Queue);
void pseudo_iret(unsigned int * pc);

PCB_p IO_ret(priority_queue pq, fifo_queue IO_Queue);
int IO_Trap(PCB_p * current, priority_queue pq, fifo_queue new_procs, fifo_queue old_procs, int * quantum, fifo_queue IO_Queue);
