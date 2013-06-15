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

extern unsigned int pcb_exist[3];
extern unsigned int running_task[4];
extern int task_to_pcb[4];
extern void* PCB_PtrTask1;
extern void* PCB_PtrTask2;
extern void* PCB_PtrTask3;
void(*task)(void);
void* pcb;
void* stack;

void startTask(char num)
{
	//void(*task)(void);
	//void* pcb;
	//void* stack;
	int i;
	
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
		case 3:
			task = C_EntryTask4;
			break;
		default:
			break;
		}

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

	enable_irq();	// enable irq
}

void killTask(char num)
{	
	int i;
	disable_irq();	// disable irq

	num -= 49;
	if(running_task[num] == 0)
		PutString("The task is already dead. \r\n");
	else{
		running_task[num] = 0;
		pcb_exist[task_to_pcb[num]] = 0;
		task_to_pcb[num] = -1;

		PutString("Kill the task ");
		PutChar(num + 49);
		PutString(". \r\n");
	}

	enable_irq();	// enable irq
}

char checkCmd(char* str)
{
	char num;

	if(str[0] != '!')
		return 0;

	num = str[2];

	if(str[1] == 's')
		startTask(num);
	else if(str[1] == 'k')
		killTask(num);
	else
		return 0;

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
