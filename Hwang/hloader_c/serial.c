
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <fcntl.h>
#include <termio.h>
#include <string.h>

int init_serial();
void close_serial();
void *recv_thread_function(void *a);
void process_sload(unsigned char* filename, char priority);
int send_string(unsigned char* send_cmd);

#define BUFFER_SIZE 	200
#define BAUDRATE	B115200 // B9600

#define SERIALDEVICE "/dev/ttyUSB0"

int com_dev = -1;
struct termios oldtio, newtio;

int main()
{
	pthread_t l_thread;
	int thread_id;
	unsigned char buf[3];
	unsigned char* tok;
	unsigned char* tok2;
	int length=0;
	int cnt=0;
	int status;
	
	unsigned char input_cmd[100];
	unsigned char send_cmd[100];
	unsigned char uboot_cmd[100];
	unsigned char cmd[100];
	
	if(init_serial()) 
		printf("success init uart \n");

	
	thread_id = pthread_create(&l_thread, NULL, recv_thread_function, (void*)0);
	pthread_detach(l_thread);
//	length = strlen(a);	
	
	while(1)
	{
		memset(send_cmd, 0x00, 100);
		memset(input_cmd, 0x00, 100);
		memset(uboot_cmd, 0x00, 100);

		printf("\nHOST_PROMPT > ");
		gets(input_cmd);
		length = strlen(input_cmd);
		if(length <= 1)
			continue;

		strcpy(uboot_cmd, input_cmd);
			
		tok = strtok(input_cmd, " ");
		
		if(!strcmp(tok, "sload"))
		{
			tok = strtok(NULL, " ");	// tok = "led.bin"
			tok2 = strtok(NULL, " ");
			if(tok2 == NULL)
				buf[0] = 9;
			buf[0] = *tok2;
			process_sload(tok, buf[0] - 48);	
		}else if(!strcmp(tok, "go"))
		{
			strcpy(send_cmd, tok);	// send_cmd = "go"
			strcat(send_cmd," ");
			tok = strtok(NULL, " ");	// tok = "0xa0d00000"
			strcat(send_cmd, tok);
			strcat(send_cmd,"\r");
			send_string(send_cmd);				
		}else if(!strcmp(tok, "preg"))
		{
			strcpy(send_cmd, tok);	// send_cmd = "preg"
			strcat(send_cmd,"\r");
			send_string(send_cmd);			
		}else if(!strcmp(tok, "mw"))
		{
			strcpy(send_cmd, tok);	// send_cmd = "mw"
			strcat(send_cmd," ");
			tok = strtok(NULL, " ");	// tok = "0xa0d00000"
			strcat(send_cmd, tok);
			strcat(send_cmd,"\r");
			send_string(send_cmd);				
		}else if(!strcmp(tok, "end"))
		{
			strcpy(send_cmd, tok);	// send_cmd = "end"
			strcat(send_cmd,"\r");
			send_string(send_cmd);	
		}else if(!strcmp(tok, "flash"))
		{
			strcpy(send_cmd, tok);	// send_cmd = "flash"
			strcat(send_cmd," booter\r");	// send_cmd = "flash booter".
			send_string(send_cmd);	
		}
		else
		{
			strcat(uboot_cmd, "\r");
		
			printf("input cmd = %s", uboot_cmd);
		
			send_string(uboot_cmd);

			if(!strcmp(uboot_cmd,"quit"))	//?
			{
				close_serial();
				printf("\nserial closed");
				pthread_exit(0);
				exit(1);
			}

		}

	}

//	sleep(1);	//serial output delay
//	close_serial();

}
void process_go(unsigned char* cmd, unsigned char* addr)
{
	
}

// filename = "led.bin"
void process_sload(unsigned char* filename, char priority)
{
	FILE *fd;
	int i;
	int status;
	unsigned char ch[2]={0x00, };
	unsigned int filesize;
	unsigned char str[10]={0x00, };
	char buf[5];

	filesize = 0;

	fd = fopen(filename, "r");
	if(fd <= 0)
	{
		fprintf(stderr, "process_sload : file open error");
		return;
	}
	
	fseek(fd, 0, SEEK_END);
	filesize = ftell(fd);
	fseek(fd, 0, SEEK_SET);

	// send command
	send_string("!m\r");

	// send file size
	buf[0] = filesize & 0x000000FF;
	buf[1] = (filesize & 0x0000FF00) >> 8;
	buf[2] = (filesize & 0x00FF0000) >> 16;
	buf[3] = (filesize & 0xFF000000) >> 24;
	write(com_dev, buf, 4);

	// send priority
	write(com_dev, &priority, 1);
	
	sleep(1);

	while(!feof(fd))
	{
		ch[0] = fgetc(fd);

		status = write(com_dev, ch, 1);
		if(status < 0)
		{
			fprintf(stderr, "\nsend_string : write error %d"
						, status);
			return;
		}
	}
	
	fclose(fd);
}

int send_string(unsigned char* send_cmd)
{
	int i, status;
	unsigned char* send_buf;
	send_buf = send_cmd;
	printf("\n");
	
	while(*send_buf != 0)
	{
		status = write(com_dev, send_buf, 1);
		if(status < 0)
		{
			fprintf(stderr, "\nsend_string : write error %d"
						, status);
			return 0;
		}
		printf("%c", *send_buf);
		send_buf++;
	}
	return 1;
}

int init_serial()
{
	com_dev = open(SERIALDEVICE, O_RDWR | O_NOCTTY);

	if(com_dev < 0)
	{
		printf("Serial port open failed\n");
		return 0;
	}

	printf("Serial port open Successed : %d\n", com_dev);

	tcgetattr(com_dev, &oldtio);
	memset(&newtio, 0, sizeof(newtio));
	newtio.c_cflag = BAUDRATE | CS8 | CLOCAL | CREAD;
	newtio.c_iflag = IGNPAR | ICRNL;
	newtio.c_oflag = 0;
	newtio.c_lflag = ICANON;
	newtio.c_cc[VTIME] = 0;
	newtio.c_cc[VMIN] = 1;
	tcflush(com_dev, TCIFLUSH);
	tcsetattr(com_dev, TCSANOW, &newtio);
	return 1;
}

void close_serial()
{
	if(com_dev < 0)
		return;
	tcsetattr(com_dev, TCSANOW, &oldtio);
	close(com_dev);
	com_dev = -1;

}

void *recv_thread_function(void *a)
{
        char readBuf[BUFFER_SIZE+1];
        int nbyte;
        fd_set read_fd;
	int i=0;
        printf("rece_thread_function_thread\n");

        while(1)
        {
                FD_ZERO(&read_fd);
                FD_SET(com_dev, &read_fd);

                if(select(com_dev+1, &read_fd, NULL, NULL, NULL) < 0)
                {
                        fprintf(stderr, "select error (in send_thread_function)\n");
                        break;
                }

                if(FD_ISSET(com_dev, &read_fd))
                        nbyte = read(com_dev, readBuf, 1);

                if(nbyte <= 0)
                {
                        fprintf(stderr, "Serial Error\n");
                        break;
                }

		printf("%c", readBuf[0]);
	}
}

