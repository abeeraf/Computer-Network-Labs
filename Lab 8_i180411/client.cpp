#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <assert.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <iostream>
#include <cstring>
#include <string>

using namespace std;

int main()
{
	int sockfd = socket(AF_INET,SOCK_STREAM,0);	
	assert(sockfd != -1 );
	
	char buff[200];
	char port[10];
	struct sockaddr_in saddr;
	
	cout << "Server 1 = 2000\nServer 2 = 3000\nEnter port number of server: ";
	fgets(port, sizeof(port), stdin);

	if (strncmp(port, "2000", 4) == 0) 
	{
	
		memset(&saddr, 0, sizeof(saddr));
		saddr.sin_family = AF_INET;
		saddr.sin_port = htons(2000);
		saddr.sin_addr.s_addr = inet_addr("127.0.0.1");

		int res = connect(sockfd, (struct sockaddr*)&saddr, sizeof(saddr));
		assert(res != -1);
	
		while (1) 
		{
			printf("Do you want an EVEN ticket or an ODD ticket?\nEnter 'END' to exit\n");
			fgets(buff, sizeof(buff), stdin);

			if (strncmp(buff, "END", 3) == 0)
			exit(0);
			
			send(sockfd,buff,strlen(buff),0);
			memset(buff,0,200);
			recv(sockfd,buff,200,0);
			puts(buff);

			if (strncmp(buff, "Tickets SOLD OUT!", sizeof("Tickets SOLD OUT!")) == 0)
			exit(0);
		}
	}
	
	else if (strncmp(port, "3000", 4) == 0)
	{
		memset(&saddr, 0, sizeof(saddr));
		saddr.sin_family = AF_INET;
		saddr.sin_port = htons(3000);
		saddr.sin_addr.s_addr = inet_addr("127.0.0.1");

		int res = connect(sockfd, (struct sockaddr*)&saddr, sizeof(saddr));
		assert(res != -1);
	
		while (1) 
		{
			printf("Do you want an EVEN ticket or an ODD ticket?\nEnter 'END' to exit\n");
			fgets(buff, sizeof(buff), stdin);

			if (strncmp(buff, "END", 3) == 0)
			exit(0);
			send(sockfd,buff,strlen(buff),0);
			memset(buff,0,200);
			recv(sockfd,buff,200,0);
			puts(buff);

			if (strncmp(buff, "Tickets SOLD OUT!", sizeof("Tickets SOLD OUT!")) == 0)
			exit(0);
		}
	}
	
	close(sockfd);
}
