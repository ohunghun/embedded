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
 * Module       : cinit.c
 * Description  : initialize the command line and task1 
 * OS           : SLOS 0.09
 * Platform     : e7t
 * History      :
 *
 * 10th November 2001 Andrew N. Sloss
 * - created
 *
 * 11th December 2001 Andrew N. Sloss
 * - updated the cinit with comments and new structure
 *
 *****************************************************************************/

/*****************************************************************************
 * IMPORTS
 *****************************************************************************/

#include "all.h"
#include "../e7t/events/event_init.h"
#include "../serial.h"
#include "../malloc/malloc.h"

/*****************************************************************************
 * MACROS
 *****************************************************************************/

/* trace - note this should go into a separate file ... */

#define DEVICESINIT    1
#define SERVICESINIT   2
#define TICKINIT       3
#define CINITINIT      4
#define TICKSTART      5
#define ENTERTASK      6

#define LOW_LEVEL_INITIALIZATION   1
#define BOOT_SLOS                  2

/*****************************************************************************
 * DATATYPES
 *****************************************************************************/

/* none... */

/*****************************************************************************
 * STATICS
 *****************************************************************************/

unsigned int STATE;
	
/*****************************************************************************
 * ROUTINUES
 *****************************************************************************/

/* -- cinit_init --------------------------------------------------------------
 *
 * Description : platform initialization C code is placed here
 *
 * Parameters  : none...
 * Return      : none...
 * Notes       : none...
 *		  
 */

void cinit_init(void)
{

STATE = LOW_LEVEL_INITIALIZATION;

 /* -------------------------------------------------------------------
  * Initialize all the device drivers
  * 
  * This routine has to be called before any of the device driver can 
  * be used. It complies with version 0.01 of the Wright Device Driver 
  * Framework (v0.01).
  *
  * ------------------------------------------------------------------- 
  */

lltrace(eventIODeviceInit(),DEVICESINIT);

 /* -------------------------------------------------------------------
  * 
  * Initialize all the unique services /pod/events/e7t/init.h
  * In SLOS Services are devices that cause interrupts.
  *
  * ------------------------------------------------------------------- 
  */

lltrace(eventServicesInit(),SERVICESINIT);

 /* -------------------------------------------------------------------
  * 
  * Initialize the timer to interrupt every 2 milliseconds
  * 
  * ------------------------------------------------------------------- 
  */
 
lltrace(eventTickInit(2),TICKINIT);
}



////////////////////////////////////////////////////////////////////////////////////

// 타이머에 관련된 레지스터 주소 정의
#define	INTERRUPT_CONTROL_BASE 0x40D00000
#define INTER_BASE INTERRUPT_CONTROL_BASE
#define TIMER_BASE 0x40A00000
#define OSMR0 0
#define OSMR1 4
#define OSCR 0x10
#define OSSR 0x14
#define OIER 0x1C
#define	ICCR		0x14
#define	ICMR		0x04
#define	ICLR		0x08

#define	__REG	*(volatile unsigned long *)
#define	__REG32	*(volatile unsigned long *)
#define	__REG16	*(volatile unsigned short *)
#define	__REG8	*(volatile unsigned char *)



// 타이머 0를 준비시킨다.
void os_timer_start()
{
	__REG32(TIMER_BASE+OIER) |= 1; // TIMER 0
	__REG32(TIMER_BASE+OSSR) = 1;	// Timer Status Clear
}
// 타이머 0를 1초 간격으로 동작시키도록 준비한다.
void os_timer_init()
{
	__REG32(INTER_BASE+ICCR) = 1;	// Disable Idle Mask = DIM = idle mode on
	__REG32(INTER_BASE+ICMR) |= 0x04000000;	// TIMER 0 Mask
	__REG32(INTER_BASE+ICLR) &= 0xFBFFFFFF;	// IRQ mode

	__REG32(TIMER_BASE+OSCR) = 0;		
	__REG32(TIMER_BASE+OSMR0) = 0x00000800;	// 1 sec 38400
}

// enable irq
void enable_irq()
{
	__asm__ __volatile__("mrs	r1, cpsr");
	__asm__ __volatile__("bic	r1, r1, #0x80");
	__asm__ __volatile__("msr	cpsr, r1");
}

// disable irq
void disable_irq()
{
	__asm__ __volatile__("mrs	r1, cpsr");
	__asm__ __volatile__("orr r1, r1, #0x80");
	__asm__ __volatile__("msr	cpsr, r1");
}

/////////////////////////////////////////////////////////////////////////////////////



/* -- C_Entry -----------------------------------------------------------------
 *
 * Description: C entry point
 * 
 * Parameters  : none...
 * Return      : return 1 if successful
 * Notes       : none...
 *
 */

unsigned int pcb_exist[3];
unsigned int running_task[4];
int task_to_pcb[4];
unsigned int task_priority[4];
unsigned int task_remainslice[4];
int eval_pid;
char eval_flag;
unsigned int eval_time;
#include "../scheduler_c.h"
Pcb* cur_ppcb;
int g_pid;
int interrupt_mask;

int C_Entry(void)
{
	
 /* --------------------------------------------------------------------
  * 
  * Initalize all the internal data structures for device drivers and
  * services. 
  *
  * Terminology:
  *
  *  - services are called by interrupt... 
  *  - device drivers are called by applications
  *
  * -------------------------------------------------------------------
  */

pcb_exist[0] = 1;
pcb_exist[1] = 0;
pcb_exist[2] = 0;

running_task[0] = 1;
running_task[1] = 0;
running_task[2] = 0;
running_task[3] = 0;

task_to_pcb[0] = 0;
task_to_pcb[1] = -1;
task_to_pcb[2] = -1;
task_to_pcb[3] = -1;

mallocinit();

g_pid = 1;
cur_ppcb = createPcb(g_pid++);
cur_ppcb->next = cur_ppcb;

//lltrace(cinit_init(),CINITINIT);
SerialInit();
// 타이머 설정

os_timer_init();
os_timer_start();
enable_irq();	

 /* -------------------------------------------------------------------
  *
  * Start the periodic timer. This will not effect the system until 
  * IRQ or FIQ interrupts are enabled.
  * 
  * -------------------------------------------------------------------
  */

//lltrace(eventTickStart(),TICKSTART);

 /* ------------------------------------------------------------------- 
  *
  * Turn On interrupts (I_bit=0) and change mode to User Mode.
  *
  * -------------------------------------------------------------------
  */

STATE=BOOT_SLOS;

  asm volatile ("MSR  CPSR_c,#0x50");
 /* -------------------------------------------------------------------
  *
  * Enter into Application/Task1 
  * 
  * -------------------------------------------------------------------
  -*/

PutString("start \r\n");
lltrace(C_EntryTask1(),ENTERTASK); 

//while(1);


 /* -------------------------------------------------------------------
  *
  * THIS CODE SHOULD NEVER BE REACHED 
  *
  * -------------------------------------------------------------------
  */
 
return 0;
}

