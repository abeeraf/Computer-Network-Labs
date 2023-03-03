#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
int main() 
{
	char request[256];
	char buf[200];
	char str[]="Exit";
	// create the socket
	int sock;
	int x=0;
	while(1)
	{
		sock = socket(AF_INET, SOCK_STREAM, 0);
		//setup an address
		struct sockaddr_in server_address;
		server_address.sin_family = AF_INET;
		server_address.sin_addr.s_addr = INADDR_ANY;
		server_address.sin_port = htons(3001);

		connect(sock, (struct sockaddr *) &server_address,
		sizeof(server_address));
		printf("Enter message for the Server: \n");
		scanf("%s",&request);
		send(sock, request, sizeof(request), 0);

		if(!strcmp(str,request))
		x++;
		if(x==1)
		break;
		
	}
	close(sock);
	return 0;
}

