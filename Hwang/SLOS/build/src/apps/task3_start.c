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
 * Module       : task3_start.c
 * Description  : this application provides the SLOS command line.
 * OS           : SLOS 0.09	
 * Platform     : e7t
 * History      :
 *
 * November 5th 2001 - Andrew N. Sloss
 * - created a simple command line calling task.
 *
 *****************************************************************************/

/*****************************************************************************
 * DATATYPE
 *****************************************************************************/

/*****************************************************************************
 * IMPORT
 *****************************************************************************/

#include "../core/all.h"
#include "../core/cli_shell.h"
#include "../serial.h"

/*****************************************************************************
 * GLOBAL
 *****************************************************************************/

/* none... */

/*****************************************************************************
 * ROUTINES
 *****************************************************************************/

/* -- banner ------------------------------------------------------------------
 *
 * Description   : prints out the SLOS standard banner
 *
 * Parameters    : none...
 * Return        : none...
 * Notes         :
 *
 */

void banner (void) 
{
printToHost ("\n\r");
printToHost ("\n\rSimple Little OS (0.09)\n\r");
printToHost ("- initialized ...... OK\n\r");
printToHost ("- running on ....... e7t \n\r");
printToHost ("\n\r");
printToHost ("\n\r");
}


/* -- C_EntryTask3 ------------------------------------------------------------
 *
 * Description : C entry point into application task3
 * 
 * Parameters  : none...
 * Return      : none...
 * Other       :
 *
 * This routine never terminates
 *
 */


#define	__REG32	*(volatile unsigned long *)
#define SEV12_port	0x10300000
#define SEV43_port	0x10400000
#define ZERO 0x0000003F
#define ONE 0x00000006
#define TWO 0x0000005B
#define THREE 0x0000004F
#define FOUR 0x00000066
#define FIVE 0x0000006D
#define SIX 0x0000007D
#define SEVEN 0x00000027
#define EIGHT 0x0000007F
#define NINE 0x00000067

extern unsigned char led;
#include "../malloc/malloc.h"
void C_EntryTask3(void) 
{
	volatile unsigned int i;
	unsigned char seg;
	unsigned char num;
	char* a;

	while(1)
	{
		a = (char*)malloc(sizeof(char)*4);
		a[0]='h';
		a[1]='h';
		a[2]='h';
		a[3]='\0';
		PutString(a);
		for(i = 0; i < 100000; i++);
		free(a);
	}

	num = 0;
	if(led & 0x80)
		num++;
	if(led & 0x40)
		num++;
	if(led & 0x20)
		num++;
	if(led & 0x10)
		num++;
	if(led & 0x08)
		num++;
	if(led & 0x04)
		num++;
	if(led & 0x02)
		num++;
	if(led & 0x01)
		num++;

	switch(num)
	{
	case 0:
		seg = ZERO;
		break;
	case 1:
		seg = ONE;
		break;
	case 2:
		seg = TWO;
		break;
	case 3:
		seg = THREE;
		break;
	case 4:
		seg = FOUR;
		break;
	case 5:
		seg = FIVE;
		break;
	case 6:
		seg = SIX;
		break;
	case 7:
		seg = SEVEN;
		break;
	case 8:
		seg = EIGHT;
		break;
	case 9:
		seg = NINE;
		break;
	}
	
	__REG32(SEV43_port) = 0;
	__REG32(SEV12_port) = seg;

	for(i=0;i<100;i++);

	/*
	while(1)
	{
		for(i=0;i<10000000;i++);
		PutString("3\r\n");
	}
	*/
}

/*
void C_EntryTask3(void) 
{
  volatile int delay;


 //Open file stream


 if(openHost (COM0)==TRUE)
 {
 banner ();
 shell  ();
 }


 //Closing the HostPort should *never* occur


closeHost ();

  while (1) 
  {
  delay=0xbeef003;
  }
}
*/
