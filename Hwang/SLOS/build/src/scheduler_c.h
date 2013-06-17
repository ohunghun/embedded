typedef struct _Pcb{
	int pid;
	int reg[20];
	struct _Pcb* next;
	char priority;
	void* task_entry;
	void* stack;
	char ttl;	// time to live;
} Pcb;

Pcb* createPcb(int pid);
