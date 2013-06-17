#define	__REG	*(volatile unsigned long *)
#define	__REG32	*(volatile unsigned long *)
#define	__REG16	*(volatile unsigned short *)
#define	__REG8	*(volatile unsigned char *)

#define LED_PORT 0x10600000

#define SEV12_port	0x10300000
#define SEV43_port	0x10400000
#define ZERO 0x3F
#define ONE 0x06
#define TWO 0x5B
#define THREE 0x4F
#define FOUR 0x66
#define FIVE 0x6D
#define SIX 0x7D
#define SEVEN 0x27
#define EIGHT 0x7F
#define NINE 0x67

void _start()
{
	__asm("b main");
}

#define seg(y, x) \
			if(x == 1)			y = ONE;	\
			else if(x == 2)	y = TWO;	\
			else if(x == 3)	y = THREE;	\
			else if(x == 4)	y = FOUR;	\
			else if(x == 5)	y = FIVE;	\
			else if(x == 6)	y = SIX;	\
			else if(x == 7)	y = SEVEN;	\
			else if(x == 8)	y = EIGHT;	\
			else if(x == 9)	y = NINE;	\
			else 					y = ZERO;


int main()
{
	int i;
	unsigned char led;
	unsigned char led_100, led_10, led_1;

	led = 99;
	while(1)
	{
		if(led > 1)
			led--;
		else
			led = 99;

		seg(led_10, (led / 10) % 10);
		seg(led_1, (led % 100) % 10);

		__REG32(SEV43_port) = led_10 << 8 | led_1;

		for(i=0;i<30000;i++);
	}
}

