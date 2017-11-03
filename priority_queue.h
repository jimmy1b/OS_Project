typedef fifo_queue * priority_queue;

priority_queue priority_queue_constructor();
int enqueue_ready(priority_queue theready_q, unsigned char priority, PCB_p the_pcb);
PCB_p dequeue_ready(priority_queue theready_q, unsigned char priority);
int empty_ready(priority_queue theready_q, unsigned char priority);
int all_empty_ready(priority_queue theready_q);
int priority_queue_destructor(priority_queue q);
char * pq_toString(priority_queue pq);
fifo_queue get_queue(priority_queue pq, unsigned char priority);