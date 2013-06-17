#include "scheduler_c.h"

extern void* PCB_PtrCurrentTask;
extern void* PCB_PtrNextTask;
extern unsigned int task_remainslice[4];
extern unsigned int task_priority[4];

extern Pcb* cur_ppcb;

void kernelScheduler_c(void)
{
	static int i;
	Pcb* next;

	// Check ttl
	if(cur_ppcb->ttl > 0)
	{
		cur_ppcb->ttl--;
		__asm("LDMFD	sp!, {r0-r3,r12,r14} ");
		__asm("MOVS    pc,r14");
	}
	else
		cur_ppcb->ttl = cur_ppcb->priority;	

	next = cur_ppcb->next;

	// context switch
	PCB_PtrCurrentTask = &cur_ppcb->reg[18];
	PCB_PtrNextTask = &next->reg[18];
	cur_ppcb = next;
	__asm("LDMFD	sp!, {r0-r3,r12,r14} ");
	__asm("LDR		r13, =PCB_PtrCurrentTask");
	__asm("LDR		r13, [r13]");
	__asm("SUB		r13, r13, #60");
	__asm("STMIA	r13, {r0-r14}^");
	__asm("MRS     r0, SPSR");
	__asm("STMDB   r13, {r0,r14}");
	__asm("LDR     r13, =PCB_PtrNextTask");
	__asm("LDR     r13, [r13]");
	__asm("SUB     r13, r13,#60");
	__asm("LDMDB   r13, {r0,r14}");
	__asm("MSR     spsr_cxsf, r0");
	__asm("LDMIA   r13, {r0-r14}^");
	__asm("LDR     r13, =PCB_TopOfIRQStack");
	__asm("LDR     r13, [r13]");
	__asm("MOVS    pc,r14");
}

Pcb* createPcb(int pid)
{
	Pcb* newPcb = (Pcb*)malloc(sizeof(Pcb));
	newPcb->pid = pid;
	newPcb->next = 0;
	newPcb->priority = 0;
	newPcb->task_entry = 0;
	newPcb->stack = 0;
	newPcb->ttl = 0;
	return newPcb;
}
