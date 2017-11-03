typedef struct q_node {
  struct q_node* next;
  PCB_p data; 
} q_node;


typedef struct fifo_q {
  int count;
  q_node* head;
  q_node* tail;
  int quantum;
} fifo_q;

typedef fifo_q * fifo_queue;

int fifo_destructor(fifo_queue q);
fifo_queue fifo_queue_constructor();
int q_enqueue(fifo_queue the_queue, PCB_p theitem);
PCB_p q_dequeue(fifo_queue the_queue);
int q_is_empty(fifo_queue the_queue);
PCB_p q_peek(fifo_queue q);
char * q_toString(fifo_queue q);
int q_setquantum(fifo_queue q, int val);
