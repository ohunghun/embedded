///////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ���ý� ���� ó������ ���� �Ǵ� �����̴�.
//
// 1. Vector ���̺� �ۼ�
// 2. Cpu ����
// 3. GPIO ����
// 4. �޸� ���� �������� ����
// 5. �޸� ī��(ROM -> RAM)
// 6. C �Լ��� �̵�
///////////////////////////////////////////////////////////////////////////////////////////////////////////////


/* �ڵ��� ident */
.ident "$Id: start.S,v$"

#include <reg.h>				// xscale ���� �������͵��� ���ǵǾ� �ִ�.
#include <config.h>				// ���� ���������� ����Ǿ� �ִ�.

.text

/* vector ���̺� �ۼ� */
.globl 	_start
_start:
	b	reset
	b	Undefined_Instruction		// ���ǵ��� �ʴ� ������ ���� �ν�Ʈ���Ϳ� ��ġ�Ǿ����� ����ȴ�.
	//b	Software_Interrupt      	// ����Ʈ�������� ���ͷ�Ʈ ����(SWI)�� ����Ǿ��� �� ����ȴ�.
	ldr pc, [pc, #0x30]
	b	Prefetch_Abort		// ����Ÿ�� ���� ��ġ �Ҷ� ���� ������ �߻��ϸ� ����ȴ�.
	b	Data_Abort			// ����Ÿ ������ �߻��ϸ� ����ȴ�.
	b	Not_Used			// ������ �ʴ´�.
	//b	IRQ				// IRQ ���ͷ�Ʈ�� �߻��Ǹ� ����ȴ�.
	ldr pc, [pc, #0x24]
	b	FIQ				// IRQ ���ͷ�Ʈ���� �켱���� �ִ� ���ͷ�Ʈ�� �߻��Ǹ� ����ȴ�.

reset:

///////////////////////////////////////////////////////////////////////////////////////////////////
// cpu�� �����Ѵ�.
///////////////////////////////////////////////////////////////////////////////////////////////////
xscale_init:
	///////////////////////////////////////////////////////////////////////////////////////////
	//���۹����� ���� �ټ� ��Ʈ=10011
	//IRQ, FRQ, ������
	//32 bit ���ɾ� ó��
	//���� ������ ���� �̵�, coprocessor ������ �����ϰ� �Ѵ�.
	///////////////////////////////////////////////////////////////////////////////////////////

	mov     r0, #0x000000D3  //11010011
        msr     cpsr_c, r0

	//mcr ������ ���� co processror�� ������ �Ѵ�.
	//�����μ��� ������ ���� ���ؾȰ��� �κ��� �����Ƿ� �ķ� �̷��.

        //ldr     r0, =0x2001
        //mcr    p15, 0, r0, c15, c1, 0 //r0�� ���� c15�� ���� �ִ´�.


	//Intel has macro call (CPWAIT) here, but they don't define it
        //disable MMU, instruction and data caches, and the write buffer
        ldr     r0, =0x00000078
	mcr     p15, 0, r0, c1, c0, 0
        //Intel has macro call (CPWAIT) here, but they don't define it

        //flush instruction and data tlb
        ldr     r0, =0x00000000
        mcr     p15, 0, r0, c8, c7, 0

        //flush instruction and data caches
        mcr     p15, 0, r0, c7, c7, 0

        //drain the write buffer
        mcr     p15, 0, r0, c7, c10, 4

        //intel does it, so I will too...
        .word 0xa00c0494	// swi handler �ּ�
        .word 0xa00c04cc	// irq handler �ּ�
        nop

        //grant manager access to all domains
        mvn     r0, #0
        mcr     p15, 0, r0, c3, c0, 0

        ldr     r12, =INTERRUPT_CONTROL_BASE

        ldr     r0, =0x00000000
        str     r0, [r12, #ICMR] 	// ���� ���� ��� ���ͷ�Ʈ�� �߻����� �ʰ� ����ũ �Ѵ�
        str     r0, [r12, #ICLR] 	// Interrupt controller Level Register
	                           	// ICLR�� ��� 0�� �������ν� FIQ�� �ƴ� IRQ�������� ���ͷ�Ʈ�� �����Ѵ�.

        ldr     r12, =CLOCK_MANAGER_BASE

        ldr     r0, =CKEN_VALUE
        str     r0, [r12, #CKEN] 	//CKEN-Clock enable Register�� CKEN6��Ʈ�� �����Ͽ�.
					//AC97, SSP, FFUART, BTUART, USB, MMC, LCD

        ldr     r0, =OSCC_VALUE	//OSCC-Oscillator Configure Register
        str     r0, [r12, #OSCC]	//OON��Ʈ�� 1�� ���� �Ͽ�. 32.768Hz oscillator�� enable�Ѵ�.
					//�ѹ� ������ ���� Reset�� �����ϰ� �̰��� �ٲܼ� ����.


        ldr     r0, =CCCR_VALUE	//CCCR-Core Clock Configure Register
        str     r0, [r12, #CCCR]	//Memory Frequncy is 99.53Mhz ~ 3.6864Mhz
					//Run Mode Frequncy = Memory Frequncy * 2
					//Toubo Mode Frequncy = Run Mode Fre * 2

        mov     r1, #3		//cp14�� Ŭ���� �Ŀ��� ���õǾ� �ִ�
        mcr     p14, 0, r1, c6, c0, 0//cp14�� 6�� �������Ϳ� 3�� �ִ´�.
					//FCS��Ʈ�� �����Ͽ� ����� ���������� ���뿡���� Frequncy�� �����Ѵ�.
					//TOURBO ��Ʈ�� �����Ͽ� �ͺ� ���� ����.


	ldr	r1, =OSCR		//OSCR-OS Timer Counter Register�� ó������ 0�� �ִ´�.
	ldr	r0, =0
	str	r0, [r1]

        ldr     r0, =0x300
wait_for_clock:			//Ŭ�� ������ 300���� �غ��� �κ����� Ŭ�� �½�Ʈ�� �Ѵ�.
        ldr	r2, [r1]		//������ �̺κе� ����ڿ� ���� �ʿ����� �������� �ִ�.
        cmp     r0, r2
        bne     wait_for_clock



init_gpio:
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//�̰����� QPIO�� �ʱ�ȭ ���ִ� ������ 81���� QPIO�� �ִ� 3���� alternate function�� ������ �����Ƿ�.
	//�ش��ϴ� alternate function�� ���� ���־�� ���� �����Ѵ�.
	//���忡 ���õ� ������ ��� alternate function�� �����Ͽ�. ���� ���� ���ۿ� �̻��̾����� �Ѵ�.
	//81���� GPIO�� ó���ϱ� ���� 9�� ���������� 3�� set�� ������ �ִ�.
	//GPAF�������Ϳ� alternate function�� ���� �ϰ�(2�� ��Ʈ ���->2���� �� 6���� ����), GPDR, GPSR,GPCR��
	//function�� Ư���� �°� �����Ѵ�.
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	ldr     r12, =GPIO_BASE

        ldr     r0, =GAFR0L_VALUE
        str     r0, [r12, #GAFR0_L]
        ldr     r0, =GAFR0U_VALUE
        str     r0, [r12, #GAFR0_U]

        ldr     r0, =GAFR1L_VALUE
        str     r0, [r12, #GAFR1_L]
        ldr     r0, =GAFR1U_VALUE
        str     r0, [r12, #GAFR1_U]

        ldr     r0, =GAFR2L_VALUE
        str     r0, [r12, #GAFR2_L]
        ldr     r0, =GAFR2U_VALUE
        str     r0, [r12, #GAFR2_U]

        ldr     r0, =GPSR0_VALUE
        str     r0, [r12, #GPSR0]
        ldr     r0, =GPSR1_VALUE
        str     r0, [r12, #GPSR1]
        ldr     r0, =GPSR2_VALUE
        str     r0, [r12, #GPSR2]
        ldr     r0, =GPCR0_VALUE
        str     r0, [r12, #GPCR0]
        ldr     r0, =GPCR1_VALUE
        str     r0, [r12, #GPCR1]
        ldr     r0, =GPCR2_VALUE
        str     r0, [r12, #GPCR2]

        ldr     r0, =GPDR0_VALUE
        str     r0, [r12, #GPDR0]
        ldr     r0, =GPDR1_VALUE
        str     r0, [r12, #GPDR1]
        ldr     r0, =GPDR2_VALUE
        str     r0, [r12, #GPDR2]

	///////////////////////////////////////////////////////////////////////////////////////////////////////////
	//PSSR - Power Manager Sleep Status Register
	//RDH - Read-Disable Hold is set
	//RDH - RDH�� GPIO input�� ������ ���� ��Ÿ����. (GPIO�� ������ ����� clear�ؾ���)
	//PH - Periperal Control Hold
	//PH - GPIO ���� GPIO configuration�� ���� �����Ǿ�����, ������忡 �ִ��� �˷��ش�. (peripheral interface�� �������� ����� clear �ؾ���)
	//������ ��Ʈ���� 1�� �������ν� �� status flags�� clear �Ҽ� �ִ�.
	///////////////////////////////////////////////////////////////////////////////////////////////////////////

	ldr     r1, =PSSR
	ldr     r2, =0x37		//RDH, PH, VFS,BFS,SSS�� ��� �����Ѵ�.
	str     r2, [r1]

init_sdram:
	//�޸� ���� �������͸� �����ϴ� �κ��̴�.
	//���� Ŀ���� �����Ͽ���, Ư���� �缳���� ������, �̺κ��� �ٲ��� �����Ƿ�, �������� ������ ���� ���� ��������
	//�� ���� ������ �����ؾ� �Ѵ�(Ư�� DRAM�κ�)

        ldr     r12, =MEM_CTL_BASE

        ldr     r0, =MSC0_VALUE
        str     r0, [r12, #MSC0]

 	//Intel's code reads it back to make sure it works...
    	//ldr     r0, [r12, #MSC0]

        ldr     r0, =MSC1_VALUE
        str     r0, [r12, #MSC1]
        ldr     r0, [r12, #MSC1]

        ldr     r0, =MSC2_VALUE
        str     r0, [r12, #MSC2]
        ldr     r0, [r12, #MSC2]

        ldr     r0, =MECR_VALUE
        str     r0, [r12, #MECR]

        ldr     r0, =MCMEM0_VALUE
        str     r0, [r12, #MCMEM0]

        ldr     r0, =MCMEM1_VALUE
        str     r0, [r12, #MCMEM1]

        ldr     r0, =MCATT0_VALUE
        str     r0, [r12, #MCATT0]

        ldr     r0, =MCATT1_VALUE
        str     r0, [r12, #MCATT1]

        ldr     r0, =MCIO0_VALUE
        str     r0, [r12, #MCIO0]

        ldr     r0, =MCIO1_VALUE
        str     r0, [r12, #MCIO1]


	//////////////////////////////////////////////////////////////////////////////////////////////////////////
	// MDREFR ��Ʈ�� �����ϴµ� �־ ��Ȯ�ϰ� �����ǰ� �ϱ�����. ���� �ܰ��� �����Ѵ�.
	// ���� ����
	// 1. SDLCK1.2.3�� not free runnung �׸��� DRI���� �����Ѵ�.
	// 2. �츮 ����� SRAM�� ���ԵǹǷ� SXCNFG�� ���� �����Ͽ��� �Ѵ�.
	//	MDREFR������ MSC������ �����ص� ���ھ����� ���δ�.
	//	�̰��� ���� �����ϸ�, �̹� MSC� ���õ� ���� �����Ǿ� ������ ���� ���� ���õȴ�.
	// 3. Self Reflash �� ������ �ؾ��Ѵ�. �̰������� �����Ǿ���.
	//
	// 4. SDCLK�� ���� �־� �޸𸮸� ���� ��ų �غ� �Ѵ�. Self Reflash, APD���� ����
	// 5. MDREFR�� ������ ���� �����Ѵ�.
	//////////////////////////////////////////////////////////////////////////////////////////////////////////

        ldr     r0, =MDREFR_VALUE
        ldr     r3, [r12, #MDREFR]	//r3�� ���� �����Ǿ� �ִ� MDREFR�� ���� �����Ѵ�.
        ldr     r1, =0xFFF
        and    r0, r0, r1		//������ DRI ������ r0�� �ִ´�.

	// r1 = 0xfff
	// r3 = ���� �����Ǿ� �ִ� MDREFR�� ��
	// r0 = ������ DRI�� ��.
        bic     r3, r3, r1
	bic	r3, r3, #0x03800000		//DRI ������ �ռ��� 25,24,23��Ʈ�� Ŭ�����Ѵ�. ���� SDCLK0,1, 2�� ��� not free running���� ����
        orr     r3, r3, r0

        str     r3, [r12, #MDREFR]		//�̹� ������ DRI���� �����Ѵ�.


	// �츮 ����� SRAM�� ���ԵǹǷ� SXCNFG�� ���� �����Ͽ��� �Ѵ�.

        ldr     r0, =MDREFR_VALUE
	ldr	r1, =0xF6000		//23,22,21,20,18,17 ��Ʈ�� 1�� �����Ѵ�.
					//23 ��Ʈ - Static MemoryŬ��
					//22 ��Ʈ - Self Reflash�� Enable
					//21 ��Ʈ - Reserve
					//20 ��Ʈ - APD(Auto Power Down)�� 1�� ����, ���� ���� �ʴ� SDRAM, Static Memory�� Power Down
					//18 ��Ʈ - SDCLK2�� Enable
					//17 ��Ʈ - SDCLK1 �� �޸� Ŭ���� ������ �����Ѵ�.
	orr	r3, r3, r0		//���� ���� �����Ѵ�.

        str     r3, [r12, #MDREFR]
	ldr	r3, [r12, #MDREFR]		//�ֱ� ���� ������ ���� �����´�.

	bic     r3, r3, #0x00400000		//Self-Reflash�� desabel�Ѵ�.
        str     r3, [r12, #MDREFR]  	//K1RUN���� ��Ʈ�� Self-Refreah�� ������ ���¿��� �����ϰ�, �ο��� �ɶ����� �����Ǿ���Ѵ�.

	ldr	r0, =MDREFR_VALUE	//������ ���� ��Ʈ�� ���� �����Ѵ�.
	ldr	r1, =0x03809000		//���� ������ ���� E1PIN�� ������(Self-Reflash�� ������ �ϴ� ��������)�ȴ�.
	and	r0, r0, r1   			//SDCLK������ ����
	orr	r3, r3, r0
        str     r3, [r12, #MDREFR]
        nop
        nop

	//���� ������ ���� MDREFR���������� ��簪�� ������ ������. �ϴܺο��� Self-Reflash��  Enable�Ѵ�.

	//MDCNFG���� 0,1,2,16,17�� Ŭ�����Ͽ� ������ ��Ų�� ���� �����Ѵ�. �׷��� �������� SDRAM patition�� enable�Ѵ�.
        
	ldr     r0, =MDCNFG_VALUE

        bic     r0, r0, #0x00000003		//������ �ϴ� �κ�
        bic     r0, r0, #0x00030000		//

        bic     r0, r0, #0x00000004		// 32bits bus width�� ����
        str     r0, [r12, #MDCNFG]		//���� ����, ���� �ο��� �ϴ� �κ��� ����.

        ldr     r0, =OSCR
	mov	r1, #0
	str	r1, [r0]			//OSCR�� ���� r1�� ������ �ִ�.

	//pause for approx 200 usecs
        ldr     r4, =0x300			//Ÿ�̸Ӹ� �̿��ؼ� �����̸� �ִ� �κ��̴�.
sdram_dly:
        ldr     r1, [r0]
        cmp    r4, r1
        bgt     sdram_dly			//

        //turn everything off
        //mov     r0, #0x78
        //mcr     p15, 0, r0, c1, c0, 0

        //Access memory that has not been enabled for CBR refresh cycles (8)
	ldr	r0, =BOOTLOADER_RAM_BASE
	str     r0, [r0]
        str     r0, [r0]
        str     r0, [r0]
        str     r0, [r0]
        str     r0, [r0]
        str     r0, [r0]
        str     r0, [r0]
        str     r0, [r0]

        //they mention dcache should
        //be enabled here if it is desired (we don't care)

	//	ldr	r0, =0x0000007A
	//	mcr	p15, 0, r0, c1, c0, 0

        ldr     r0, [r12, #MDCNFG]
        //orr     r0, r0, #0x00000001
	orr     r0, r0, #0x00000003
        str     r0, [r12, #MDCNFG]		//SDRAM patition0, 1�� enable ��Ų��.

        //write MDMRS again
        ldr     r0, =MDMRS_VALUE
        str     r0, [r12, #MDMRS]

        ldr     r0, [r12, #MDREFR]
	ldr	r11, =0xFFEFFFFF		//Auto Power Down�� �����ؼ� �޸� ���� Ŭ������ �ڵ����� Enable�Ѵ�.
	and	r0, r0, r11
        str     r0, [r12, #MDREFR]

	//////////////////////////////////////////////////////////////////////////////////////
	//�ҿ� �ִ� ��Ʈ�δ��� �������� ������ C�Լ��� �����Ѵ�.
	/////////////////////////////////////////////////////////////////////////////////////
	ldr	r0, =0
	ldr	r1, =BOOTLOADER_RAM_RUNNING
	ldr	r2, =BOOTLOADER_LENGTH


rom2ram:
	ldr	r3, [r0]
	str	r3, [r1]
	add	r0, r0, #4
	add	r1, r1, #4
	subs	r2, r2, #4
	bne	rom2ram

	ldr	sp, =STACK_POINTER


	ldr	r0, =c_main
	mov	pc, r0

boot_falut:
	b	reset



Undefined_Instruction:	// �˼� ���� ���� ó���� ���� ���� ��ƾ

Software_Interrupt:		// ����Ʈ���� ���ͷ�Ʈ�� �߻��Ǿ������� ���� ���� ��ƾ

Prefetch_Abort:		// ����Ÿ�� ���� ��ġ �Ҷ� ���� ������ �߻��ϸ� ����ȴ�.

Data_Abort:			// ����Ÿ ������ �߻��ϸ� ����ȴ�.

Not_Used:			// ������ �ʴ´�.

IRQ:				// IRQ ���ͷ�Ʈ�� �߻��Ǹ� ����ȴ�.

FIQ:				// FIQ ���ͷ�Ʈ�� �߻��Ǹ� ����ȴ�.