#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <unistd.h>
#include <netinet/in.h>
#include <stdio.h>
#define BUF_SIZE 512


int confirmCheckSum(unsigned char *buf,int length,unsigned char checksum);

struct data
{
	int filesize;
	int seq;
	unsigned char checksum;
	char temp[3];
	int buf[BUF_SIZE];
};

struct ack
{
	int seq;
};

int currentSeq;

int main(int argc, char **argv)
{
	int sockfd;
	int clilen1,clilen2;
	int state;
	int n;
	int sum,res;
	struct data add_data;
	struct ack add_ack;    

	struct sockaddr_in serveraddr, clientaddr;

	FILE *fp;

	currentSeq=0;

	if((fp = fopen("receivefile.aa","w")) == NULL) {
		printf("File Read Error!\n");
		return 0;
	}

	clilen1 = sizeof(clientaddr);
	clilen2 = sizeof(clientaddr);
	sockfd = socket(AF_INET, SOCK_DGRAM, 0);
	if (sockfd < 0)
	{
		perror("socket error : ");
		exit(0);
	}

	bzero(&serveraddr, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
	serveraddr.sin_port = htons(1234);

	bzero(&clientaddr, sizeof(clientaddr));
	clientaddr.sin_family = AF_INET;
	clientaddr.sin_addr.s_addr = inet_addr("192.168.123.150");
	clientaddr.sin_port = htons(1235);

	state = bind(sockfd, (struct sockaddr *)&serveraddr, 
			sizeof(serveraddr));
	if (state == -1)
	{
		perror("bind error : ");
		exit(0);
	}

	while(1)
	{
		n =recvfrom(sockfd, (void *)&add_data, sizeof(add_data), 0, (struct sockaddr *)&serveraddr, &clilen1);

		if(confirmCheckSum(add_data.buf,add_data.filesize,add_data.checksum)) printf("checksum confirm!!\n");
		else{ 
			printf("checksum error!!\n"); 
			continue; //re receive
		}

		if(currentSeq==add_data.seq){ //expect seq
			fwrite(add_data.buf,1,add_data.filesize,fp);
			currentSeq++;
		}
		add_ack.seq=add_data.seq+1; 
		sendto(sockfd, (void *)&add_ack, sizeof(add_ack), 0, (struct sockaddr *)&clientaddr, clilen2);

		if(add_data.filesize<BUF_SIZE){
			printf("receive Success!!\n");
			break;
		}
	}
	fclose(fp);
	close(sockfd);
}


int confirmCheckSum(unsigned char *buf,int length,unsigned char checksum){
	int checksumtotal=0,i=0;
	unsigned char bytesum;
	for(i=0;i<length;i++){
		checksumtotal+=(int)buf[i];
	}

	bytesum=(unsigned char)checksumtotal;

	if((char)(bytesum+checksum)==0x00){
		return 1;
	}

	return 0;       

}




