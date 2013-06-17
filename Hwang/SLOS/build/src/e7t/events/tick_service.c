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
 * Module      : tick_service.c
 * Description : Header File
 * Platform    : Evaluator7T
 * History     : 
 *
 * 2000-03-25 Andrew N. Sloss
 * - implemented a tick service
 *
 *****************************************************************************/


/*****************************************************************************
 * IMPORT
 *****************************************************************************/

#include "../headers/macros.h"

#include "../../headers/api_types.h"
#include "../../scheduler_c.h"

/*****************************************************************************
 * MACRO'S
 *****************************************************************************/

/* none ... */
 
/*****************************************************************************
 * GLOBAL
 *****************************************************************************/

UINT countdown;

/*****************************************************************************
 * EXTERN's
 *****************************************************************************/

/* none... */

/*****************************************************************************
 * GLOBALS
 *****************************************************************************/

/* none... */

/*****************************************************************************
 * ROUTINES
 *****************************************************************************/

/* -- eventTickInit -----------------------------------------------------------
 *                                                            
 * Description  : Initialises the counter timer in milliseconds
 *
 * Parameters   : UINT msec - sets periodic timer in milliseconds
 * Return       : none...
 * Notes        : none...
 *                                                                             
 */
 
void eventTickInit (UINT msec)
{
/* ----------------------------------------------------------------------
 *
 * Initalize the Tick hardware on the Samsung part.
 *
 * ----------------------------------------------------------------------
 */

*TMOD = 0;
*INTPND = 0x00000000; 	// Clear pending interrupts .............

/* ----------------------------------------------------------------------
 *
 * Set the countdown value depending on msec.
 *
 * ----------------------------------------------------------------------
 */

  switch (msec)
  {
  case 2: /* fast ... */
  countdown = 0x000ffff0;
  break;
  default: /* slow ... */
  countdown = 0x00effff0;
  break;	
  }
}

/* -- eventTickService --------------------------------------------------------
 *
 * Description : interrupt service routine for timer0 interrupt.
 *
 * Parameters  : none...
 * Return      : none...
 * Notes       : 
 * 
 * timer interrupt everytime the counter reaches 0. To reset
 * the timer TDATA0 has to have a new initialization value.
 * Finally the last act is to unmask the timer interrupt on 
 * the Samsung KS3250C100.
 *
 */ 

#define tIOPDATA    (volatile unsigned int *)(SYSCFG + 0x5008)
#define LEDBANK     *tIOPDATA


#define LED_4_ON    LEDBANK |= 0x00000010
#define LED_4_OFF   LEDBANK = LEDBANK &~ 0x00000010

unsigned int xLED = 0;

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
#define	ICIP		0x00

#define	__REG	*(volatile unsigned long *)
#define	__REG32	*(volatile unsigned long *)
#define	__REG16	*(volatile unsigned short *)
#define	__REG8	*(volatile unsigned char *)

#define LED_PORT 0x10600000

extern int interrupt_mask;

#define DIS_INT	interrupt_mask = __REG32(ICMR);	\
						__REG32(ICMR) = 0
#define EN_INT		__REG32(ICMR) = interrupt_mask

extern unsigned char led;

unsigned int acc_tick;
extern int eval_pid;
extern char eval_flag;
extern int PCB_CurrentTask;
extern unsigned int eval_time;
extern Pcb* cur_ppcb;

void eventTickService(void) 
{ 
	static char task_led;
	volatile int i;
	static int eval_i;

	/* -- reset timer interrupt... */ ////////////////////////////////////
	disable_irq();	// IRQ Disable
	//DIS_INT;

	// pin 26 : OS Timer 0

	if((__REG32(INTER_BASE+ICIP) & 0x04000000) != 0)	// pin 26 : OS Timer 0
	{
		acc_tick += __REG32(TIMER_BASE+OSCR);	// accumulate

		__REG32(TIMER_BASE+OSMR0) = 0xFFFFFFFF;
		__REG32(TIMER_BASE+OSSR) &= 0;
		__REG32(TIMER_BASE+OSSR) |= 1;

		if(eval_flag)
		{
			if(eval_i > 30)
			{	
				eval_i = 0;
				eval_flag = 0;
			}
			else
			{
				PutNum(cur_ppcb->pid);
				PutString(" -> ");
				eval_i++;
			}
		}

		__REG32(TIMER_BASE+OSCR) = 0;
		__REG32(TIMER_BASE+OSMR0) = 0x00000800;
	}

	enable_irq();	// IRQ Enable*/
	//EN_INT;


//*INTPND	= 1<<10;
//*TDATA0	= countdown; 
/*
  if (xLED==0)
  { 
  LED_4_ON; 
  xLED=1; 
  } 
  else 
  {
  LED_4_OFF; 
  xLED=0;
  } 

// -- unmask the interrupt source.... 
	*/
//*(volatile unsigned int*)INTMSK &= ~((1<<INT_GLOBAL)|(1<<10)|(1<<0));
} 

/* -- eventTickStart ----------------------------------------------------------
 *
 * Description  : switches on the periodic tick event
 *
 * Parameters   : none...
 * Return       : none...
 * Notes        : none...
 *
 */  
 
//void eventTickStart(void)
//{	
//*TDATA0   = countdown; /* load Counter Timer ... */
//*TMOD     |= 0x1;      /* enable interval interrupt ... */
  
/* -- unmask the interrupt source.. */ 
  	
//*(volatile unsigned int*)INTMSK &= ~((1 << INT_GLOBAL) | (1<<10) | (1<<0));     
//}

