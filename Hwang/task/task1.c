#define	__REG	*(volatile unsigned long *)
#define	__REG32	*(volatile unsigned long *)
#define	__REG16	*(volatile unsigned short *)
#define	__REG8	*(volatile unsigned char *)

#define LED_PORT 0x10600000

void _start()
{
	__asm("b main");
}

int main()
{
	int i;
	char led;

	led = 0;
	while(1)
	{
		led++;
		__REG32(LED_PORT) = led;
		for(i=0;i<30000;i++);
	}
}
