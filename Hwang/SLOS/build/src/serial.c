#include "serial.h"

#define	__REG	*(volatile unsigned long *)
#define	__REG32	*(volatile unsigned long *)
#define	__REG16	*(volatile unsigned short *)
#define	__REG8	*(volatile unsigned char *)

#define	TICKS_PER_SECOND 32768
#define	RCNR	 __REG(0x40900000)

//Interrupt Control Registers
#define	INTERRUPT_CONTROL_BASE 0x40D00000

#define	ICIP		0x00
#define	ICMR		0x04
#define	ICLR		0x08
#define	ICFP		0x0C
#define	ICPR		0x10
#define	ICCR		0x14

#define	CLOCK_MANAGER_BASE 0x41300000

#define	CCCR		0x00
#define	CKEN		0x04
#define	OSCC		0x08
#define	OSCR		0x40A00010


//GPIO registers
#define	GPIO_BASE	0x40E00000

#define	GPLR0		0x00
#define	GPLR1		0x04
#define	GPLR2		0x08
#define	GPDR0		0x0C
#define	GPDR1		0x10
#define	GPDR2		0x14
#define	GPSR0		0x18
#define	GPSR1		0x1C
#define	GPSR2		0x20
#define	GPCR0		0x24
#define	GPCR1		0x28
#define	GPCR2		0x2C
#define	GRER0		0x30
#define	GRER1		0x34
#define	GRER2		0x38
#define	GFER0		0x3C
#define	GFER1		0x40
#define	GFER2		0x44
#define	GDER0		0x48
#define	GDER1		0x4C
#define	GDER2		0x50
#define	GAFR0_L	0x54
#define	GAFR0_U	0x58
#define	GAFR1_L	0x5C
#define	GAFR1_U	0x60
#define	GAFR2_L	0x64
#define	GAFR2_U	0x68

//Memory Control Registers
#define	MEM_CTL_BASE      0x48000000

#define	MDCNFG	0x00
#define	MDREFR	0x04
#define	MSC0		0x08
#define	MSC1		0x0C
#define	MSC2		0x10
#define	MECR		0x14
#define	SXCNFG	0x1C
#define	MCMEM0	0x28
#define	MCMEM1	0x2C
#define	MCATT0	0x30
#define	MCATT1	0x34
#define	MCIO0		0x38
#define	MCIO1		0x3C
#define	MDMRS		0x40

// Power management
#define	RCSR		0x40F00030
#define	RCSR_SLEEP	0x00000004
#define	PSPR		0x40F00008
#define	PSSR		0x40F00004
//#define	PSSR_PH	0x00000010
//#define	PSSR_RDH	0x00000020
//#define	PSSR_STATUS_MASK  0x00000007

//FFUART Registers
#define	FFUART_BASE       0x40100000

#define	FFRBR		0x00
#define	FFTHR		0x00
#define	FFIER		0x04
#define	FFIIR		0x08
#define	FFFCR		0x08
#define	FFLCR		0x0C
#define	FFMCR		0x10
#define	FFLSR		0x14
#define	FFMSR		0x18
#define	FFSPR		0x1C
#define	FFISR		0x20
#define	FFDLL		0x00
#define	FFDLH		0x04

// Status bits.
#define	STATUS_BUSY			0x00800080
#define	STATUS_ERR_FILTER		0x007E007E
#define	STATUS_LOCK_DETECT		0x00020002
#define	STATUS_VOLT_RANG_ERR		0x00080008
#define	STATUS_CMD_SEQ_ERR		0x00300030
#define	STATUS_PGM_ERR			0x00100010
#define	STATUS_LOCK_ERR			0x00100010
#define	STATUS_ERASE_ERR		0x00200020
#define	STATUS_UNLOCK_ERR		0x00200020
#define	STATUS_LOCK_ERR_FILTER	0x00380038





void SerialInit(void)
{
/* 
 *  Initialize UART here 
 *  SET FFLCR, FFFCR, FFDLL, FFLSR , etc...
 */

	// pin 34 in, pin 39 out
	__REG32(GPIO_BASE + GPDR1) &= 0xFFFFFFFB;
	__REG32(GPIO_BASE + GPDR1) |= 0x00000080;

	// set alternate function
	__REG32(GPIO_BASE + GAFR1_L) = 0x00008010;

	// set serial clock enable
	__REG32(CLOCK_MANAGER_BASE + CKEN) |= 0x00000040;

	// set LCR (8 data bits, no parity, 1 stop bit, and we can access to DLL and DLH)
	__REG32(FFUART_BASE + FFLCR) = 0x00000083;

	// set DLL (115200bps)
	__REG32(FFUART_BASE + FFDLL) = 0x00000008;
	__REG32(FFUART_BASE + FFDLH) = 0x00000000;

	// now we can access to TBR, RBR, IER
	__REG32(FFUART_BASE + FFLCR) &= 0xFFFFFF7F;

	// set FCR
	__REG32(FFUART_BASE + FFFCR) = 0x00000007;

   return;
}

void PutChar(const char ch)
{
/* Wait until FIFO empty..
 * Store 1byte character to FFTHR register's low 1byte
 */
	while((__REG32(FFUART_BASE + FFLSR) & 0x00000040) == 0);	// check TEMT bit of LSR is 1
	
	__REG32(FFUART_BASE + FFTHR) = ch;	// put our data to THR

   return;
}

char GetChar()
{
	while((__REG32(FFUART_BASE + FFLSR) & 0x00000001) == 0);	// check DR bit of LSR is 1

	return __REG32(FFUART_BASE + FFRBR);	// get data from RBR
}


/*
 * Print characters by using PutChar
 */

void PutString(const char *ch)
{
	while(*ch != '\0')
		PutChar(*ch++);
}

void GetString(char* str)
{
	char c;

	do
	{
		c = GetChar();
		PutChar(c);		// you can see that you have inputted
		*str++ = c;
	}while(c != '\r' && c != '\0');	// if you push the 'ENTER' key, this routine will finish
	*str++ = '\n';
	*str = '\0';
}

void PutNum(unsigned int num)
{
	char n;
	int i;
	
	for(i = 28; i >= 0; i -= 4)
	{
		n = (num >> i) & 0x0000000F;
		if(n > 9)
			n += 55;
		else
			n += 48;

		PutChar(n);
	}
}
