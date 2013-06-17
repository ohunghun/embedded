/*
 *  ____________________________________________________________________
 * 
 *  Copyright (c) 2002, Andrew N. Sloss, Chris Wright and Dominic Symes
 *  All rights reserved.
 *  ____________________________________________________________________
 * 
 *  NON-COMMERCIAL USE License
 *  
 *  Redistribution and use in source and binary forms, with or without 
 *  modification, are permitted provided that the following conditions 
 *  are met: 
 *  
 *  1. For NON-COMMERCIAL USE only.
 * 
 *  2. Redistributions of source code must retain the above copyright 
 *     notice, this list of conditions and the following disclaimer. 
 * 
 *  3. Redistributions in binary form must reproduce the above 
 *     copyright notice, this list of conditions and the following 
 *     disclaimer in the documentation and/or other materials provided 
 *     with the distribution. 
 * 
 *  4. All advertising materials mentioning features or use of this 
 *     software must display the following acknowledgement:
 * 
 *     This product includes software developed by Andrew N. Sloss,
 *     Chris Wright and Dominic Symes. 
 * 
 *   THIS SOFTWARE IS PROVIDED BY THE CONTRIBUTORS ``AS IS'' AND ANY 
 *   EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
 *   IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR 
 *   PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE CONTRIBUTORS BE 
 *   LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, 
 *   OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, 
 *   PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, 
 *   OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY 
 *   THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR 
 *   TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT 
 *   OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY 
 *   OF SUCH DAMAGE. 
 * 
 *  If you have questions about this license or would like a different
 *  license please email :
 * 
 * 	andrew@sloss.net
 * 
 * 
 */
 
/*****************************************************************************
 * Simple Little Operating System - SLOS
 *****************************************************************************/

/*****************************************************************************
 *
 * Module       : task1_start.c
 * Description  : Shell TASK routine that toggles the LED2
 * Platform     : e7t
 * History      :
 *
 * 2000-03-27 Andrew N. Sloss
 * - implemented on Evalautor7T
 * - added header and changed the delay time
 *
 *****************************************************************************/

/*****************************************************************************
 * IMPORT
 *****************************************************************************/

#include "../core/mutex.h"

#include "../headers/api_types.h"
#include "../devices/ddf_io.h"
#include "../e7t/devices/ddf_types.h"

#include "../e7t/events/swis.h"
#include "../headers/api_device.h"
#include "../serial.h"

#include "task1_start.h"
#include "task2_start.h"
#include "task3_start.h"
#include "task4_start.h"
#include "../malloc/malloc.h"
#include "../scheduler_c.h"

#define GREEN1      0
#define RED         2
#define ORANGE      1
#define GREEN2      3

/*****************************************************************************
 * STATICS
 *****************************************************************************/

device_treestr *red_host;
UID rled;

/*****************************************************************************
 * ROTUINES
 *****************************************************************************/

/* -- openRedLED --------------------------------------------------------------
 *
 * Description   : opens the communication port to the red LED
 *
 * Parameters    : none..
 * Return        : BOOLean - successful | unsucessful 
 * Notes         :
 *
 */

BOOL openRedLED(void) 
{ 
unsigned int delay;
/* --------------------------------------------------------------
 *
 * eventIODeviceOpen - 
 *
 * open the LED driver node...
 * 
 * --------------------------------------------------------------
 */

red_host = eventIODeviceOpen(&rled,DEVICE_LED_E7T,RED);

  /* check that a device driver is found ... */

  /*
  if (red_host==0) {
    while (1) 
    {
    delay=0xBEEF0401;
    };
  return FALSE;
  }
  */

  /* check the UID .................. */

  switch (rled) 
  {
  case DEVICE_IN_USE:
  case DEVICE_UNKNOWN:
  
    /* while (1) 
    {
    delay=0xBEEF0201;
    }; */

	return FALSE;
  } 
  	
return TRUE; 
}

/* -- switchOnRedLED ----------------------------------------------------------
 *
 * Description  : switches on the RED LED
 *
 * Parameter    : none...
 * Return       : none...
 * Notes	:
 *
 */

void switchOnRedLED(void)
{eventIODeviceWriteBit(red_host,rled,(UINT)1);}

/* -- switchOffRedLED ---------------------------------------------------------
 *
 * Description  : switches off the RED LED
 *
 * Parameter    : none...
 * Return       : none...
 * Notes	:
 *
 */

void switchOffRedLED(void)
{eventIODeviceWriteBit(red_host,rled,(UINT)0);}

/* -- C_EntryTask1 ------------------------------------------------------------
 *
 * Description  : C entry to Task1
 *
 * Parameter    : none...
 * Return       : none...
 * Notes	:
 *
 * Never terminates
 */

#define	__REG	*(volatile unsigned long *)
#define	__REG32	*(volatile unsigned long *)
#define	__REG16	*(volatile unsigned short *)
#define	__REG8	*(volatile unsigned char *)
#define ICMR	0x40D00004

extern int interrupt_mask;

#define DIS_INT	interrupt_mask = __REG32(ICMR);	\
						__REG32(ICMR) = 0
#define EN_INT		__REG32(ICMR) = interrupt_mask

extern unsigned int pcb_exist[3];
extern unsigned int running_task[4];
extern int task_to_pcb[4];
extern void* PCB_PtrTask1;
extern void* PCB_PtrTask2;
extern void* PCB_PtrTask3;
void(*task)(void);
void* pcb;
void* stack;

void startTask(char num, void(*task_addr)(void), char irq)
{
	int i;
	
	if(irq != 0)
		disable_irq();	// disable irq

	num -= 49;
	if(running_task[num] == 1)
		PutString("The task is already running. \r\n");
	else{
		// task number test
		for(i = 0; i < 3; i++)
		{
			if(pcb_exist[i] == 0)
				break;
		}
		if(i == 3){
			PutString("Too many tasks. \r\n");
			return;
		}

		// set task function address
		if(task_addr == 0)
		{
			switch(num)
			{
			case 0:
				task = C_EntryTask1;
				break;
			case 1:
				task = C_EntryTask2;
				break;
			case 2:
				task = C_EntryTask3;
				break;
			default:
				break;
			}
		}
		else
			task = task_addr;

		// set pcb & stack address
		switch(i)
		{
		case 0:
			pcb = &PCB_PtrTask1;
			stack = (void*)0xA1610000;
			break;
		case 1:
			pcb = &PCB_PtrTask2;
			stack = (void*)0xA1620000;
			break;
		case 2:
			pcb = &PCB_PtrTask3;
			stack = (void*)0xA1630000;
			break;
		default:
			break;
		}		

		running_task[num] = 1;
		pcb_exist[i] = 1;
		task_to_pcb[num] = i;

		__asm__("STMFD r13!, {r0-r3}");
		__asm__("LDR r0,=task");
		__asm__("LDR r0,[r0]");
		__asm__("LDR r1,=pcb");
		__asm__("LDR r1,[r1]");
		__asm__("LDR r2,=stack");
		__asm__("LDR r2,[r2]");
		__asm__("BL pcbSetUp");
		__asm__("LDMFD r13!, {r0-r3}");

		PutString("Start the task ");
		PutChar(num + 49);
		PutString(". \r\n");
	}

	if(irq != 0)
		enable_irq();	// enable irq
}

extern Pcb* cur_ppcb;

void killTask(char num, char irq)
{	
	int intmask;
	int pid;
	Pcb* ppcb, *pre;

	// Disable interrupt
	if(irq != 0)
	{
		DIS_INT;
	}
	
	// Find target task pcb
	pid = num - 48;
	pre = ppcb = cur_ppcb;
	do{
		if(ppcb->pid == pid)
			break;

		pre = ppcb;		
		ppcb = ppcb->next;
	}while(ppcb != cur_ppcb);
	if(ppcb->pid != pid)
	{
		PutString("The task is already dead. \r\n");
	}

	// Adjust link and free the memory.
	else
	{
		pre->next = ppcb->next;

		free(ppcb->task_entry);
		free(ppcb->stack);
		free(ppcb);
	}

	// Enable interrupt
	if(irq != 0)
	{
		EN_INT;
	}
}

extern int eval_pid;
extern char eval_flag;
void evaluateTask()
{	
	
}

extern unsigned int task_priority[4];
void priorityChange(char num, char priority)
{
	Pcb* ppcb;
	char pid;

	pid = num - 48;
	ppcb = cur_ppcb;
	do{
		if(ppcb->pid == pid)
			break;
		ppcb = ppcb->next;
	}while(ppcb != cur_ppcb);
	if(ppcb->pid != pid)
		return;

	ppcb->priority = priority - 48;
}

void printTask()
{
	Pcb* ppcb;

	ppcb = cur_ppcb;
	do{
		PutString("pid : ");
		PutNum(ppcb->pid);
		PutString(" 's priority is ");
		PutNum(ppcb->priority);
		PutString("\r\n");
		ppcb = ppcb->next;
	}while(ppcb != cur_ppcb);
}

extern int g_pid;
void makeTask()
{
	unsigned int size;
	void* addr, *addr_tmp;
	char priority;
	char c;
	int intmask;
	Pcb* ppcb;

	// Disable interrupt
	DIS_INT;

	// Receive the task size.
	size = GetChar();
	size |= (GetChar() << 8);
	size |= (GetChar() << 16);
	size |= (GetChar() << 24);

	// Receive priority of the task.
	priority = GetChar();

	PutString("p : ");
	PutNum(priority);
	PutString(",  size : ");
	PutNum(size);
	PutString("\r\n");

	// Copy bin file to the memory.
	addr = addr_tmp = malloc((int)size);
	while(size > 0)
	{
		c = GetChar();
		__REG8(addr_tmp++) = c;
		size--;
	}

	PutString("copy complete\r\n");

	// Create a New PCB. And adjust link
	ppcb = createPcb(g_pid++);
	ppcb->next = cur_ppcb->next;
	cur_ppcb->next = ppcb;
	ppcb->priority = priority;
	ppcb->task_entry = addr;

	// Set PCB's registers.
	task = (void(*)(void))addr;
	pcb = &(ppcb->reg[18]);
	stack = malloc(104) + 100;
	ppcb->stack = stack;
	__asm__("STMFD r13!, {r0-r3}");
	__asm__("LDR r0,=task");
	__asm__("LDR r0,[r0]");
	__asm__("LDR r1,=pcb");
	__asm__("LDR r1,[r1]");
	__asm__("LDR r2,=stack");
	__asm__("LDR r2,[r2]");
	__asm__("BL pcbSetUp");
	__asm__("LDMFD r13!, {r0-r3}");

	PutString("task added.\r\n");
	for(size = 0; size < 30000; size++);

	// Enable interrupt
	EN_INT;
}

char checkCmd(char* str)
{
	char num;

	if(str[0] != '!')
		return 0;

	num = str[2];

	switch(str[1])
	{
	case 's':
		startTask(num, 0, 1);
		break;
	case 'k':
		killTask(num, 1);
		break;
	case 'e':
		eval_flag = 1;
		break;
	case 'p':
		priorityChange(num, str[3]);
		break;
	case 'a':
		printTask();
		break;
	case 'm':
		makeTask();
		break;
	default:
		return 0;
	}

	return 1;
}

void C_EntryTask1(void)
{
	char str[200];
	int i;

	//SerialInit(); 				//serial Initialize
	
	//printf("Start Serial Operation!!");	// print simple string to console 
	PutString("Start Serial Operation!!");	// print simple string to console 


	while(1)
	{
		PutString("$Serial>");	// print "Serial" to get user input
		GetString(str);	// get user input
		if(!checkCmd(str))
		{
			PutString("\r\n#ECHO>>>");	
			PutString(str);	// echo
			PutString("\r\n");	// new line
		}
			
	}
}

/*
void C_EntryTask1(void)
{
volatile int delay;

  if (openRedLED ()==TRUE)
  {
    switchOnRedLED ();
    while (1) 
    {
    aWAIT;
    switchOnRedLED ();

    // dummy time delay...
    for (delay=0; delay<0x20ffff; delay++) {} 
  
    aSIGNAL;
    switchOffRedLED ();
    
    // dummy time delay...
    for (delay=0; delay<0x20ffff; delay++) {}
    }
  }



  while (1) 
  {
  delay=0xBEEF0001;
  };
}
*/
