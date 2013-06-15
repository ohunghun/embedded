#include "../core/mutex.h"

#include "../headers/api_types.h"
#include "../devices/ddf_io.h"
#include "../e7t/devices/ddf_types.h"

#include "../e7t/events/swis.h"
#include "../headers/api_device.h"
#include "../serial.h"

#define	__REG32	*(volatile unsigned long *)
#define SEV12_port	0x10300000
#define SEV43_port	0x10400000
#define NONELIGHT 0x00000000
#define ALLLIGHT 0x00007F00

void C_EntryTask4(void)
{
	unsigned int i;	
	
	while(1)
	{
		__REG32(SEV43_port) = NONELIGHT;
		for(i = 0; i < 10000000; i++);
		__REG32(SEV43_port) = ALLLIGHT;
		for(i = 0; i < 10000000; i++);
	}
}
