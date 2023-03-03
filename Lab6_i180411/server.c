#include <stdio.h>
#include <stdlib.h>

#include <sys/socket.h>
#include <sys/types.h>

#include <netinet/in.h>

int main() 
{

	char buf[200];
	// create the server socket
	int server_socket;
	server_socket = socket(AF_INET, SOCK_STREAM, 0);

	
	// define the server address
	struct sockaddr_in server_address;
	server_address.sin_family = AF_INET;
	server_address.sin_port = htons(3001);
	server_address.sin_addr.s_addr = INADDR_ANY;

	// bind the socket to our specified IP and port
	bind(server_socket, (struct sockaddr*) &server_address, sizeof(server_address));
	listen(server_socket, 5);
	int count=0;

	while(1)
	{
		int client_socket;
		int status;

		client_socket = accept(server_socket, NULL, NULL);
		pid_t pid=fork();
		if(!pid)
		{ //The child process
		
			recv(client_socket, &buf, sizeof(buf), 0);

			if(!strcmp("Exit",buf))
			break;

			printf("\n Client Says: %s \n", buf);
			// close the socket
			close(client_socket);
		}
		else
		//Parent process
		close(client_socket);
		
	}
	return 0;
}


