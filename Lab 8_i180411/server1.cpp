#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <assert.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <iostream>
#include <vector>
#include <algorithm>

using namespace std;
#define MAXFD 100

//Add a file descriptor to the fds array
void fds_add(int fds[], int fd) {
	for(int x = 0; x < MAXFD; x++)	
	{
		if (fds[x] == -1) 
		{
	      		fds[x] = fd;
		 	break;
		}
	}
}

string charPtrToString(char* toConvert) {
	
	string str;
	for (int x = 0; toConvert[x]; x++)
	str += toConvert[x];
	return str;
}

int main() {

	int allocatedTickets = 2;
	int index;
	vector<int> declinedClients;
	string reservedTickets[100]; // tickets taken by every client
	int sockfd = socket(AF_INET,SOCK_STREAM,0);
	assert(sockfd != -1);
	
    	cout<<"Server 1 is online"<<endl;
    
	struct sockaddr_in saddr, caddr;
	memset(&saddr, 0, sizeof(saddr));
	saddr.sin_family = AF_INET;
	saddr.sin_port = htons(2000); // port number
	saddr.sin_addr.s_addr = inet_addr("127.0.0.1");

	int res = bind(sockfd, (struct sockaddr*)&saddr, sizeof(saddr));
	assert(res != -1);
	
	//Create listening queue
	listen(sockfd, 5);
	
	//Define fdset collection
	fd_set fdset;

	//Define fds array
	int fds[MAXFD];
	for (int x = 0; x < MAXFD; x++)
	fds[x]=-1;

	//Add a file descriptor to the fds array
	fds_add(fds, sockfd);

	while (1) 
	{
		FD_ZERO(&fdset); //Clear the fdset array to 0
		int maxfd = -1;

		//For loop finds the maximum subscript for the ready event in the fds array
		for (int x = 0; x < MAXFD; x++) 
		{
			if (fds[x] == -1)
			continue;

			FD_SET(fds[x], &fdset);

			if (fds[x] > maxfd)
			maxfd = fds[x];
		}

		struct timeval tv = {0, 0};	
		int n = select(maxfd + 1, &fdset, NULL, NULL, &tv); //Select system call, where we only focus on read events
		
		if (n == -1)	//fail
		perror("select error");
	
		else 
		{
			//check all file descriptors 
			for(int x = 0; x < MAXFD; x++) 
			{
				if (allocatedTickets > 12) 
				{
					cout << "All tickets SOLD OUT";
					for (int x = 0; x < MAXFD; x++) 
					{

						if (fds[x] != -1) 
						{
							if (FD_ISSET(fds[x], &fdset)) 
							{
								char buff[128];
								recv(fds[x], buff, 127, 0);

								string msg;
								msg = "\nTickets SOLD OUT.";
								send(fds[x], msg.c_str(), sizeof(msg), 0);

								close(fds[x]);
							}
						}

						string numStr = reservedTickets[x];
						if (numStr.length() > 0) 
						{
							cout << "\n\nClient " << x << " had tickets: ";
							for (int y = 0; y < numStr.length(); y++) 
							{
								char z = numStr[y];

								if (z == '+' and y != 0)
								cout << ",";
								else if (z != '+')
								cout << z;
							}
							cout << endl;
						}
					}

					if (!declinedClients.empty()) 
					{
						cout <<endl<< "Clients who did not recieve tickets: ";
						for (int x = 0; x < declinedClients.size(); x++)
						cout << declinedClients[x] << " ";
					}				
					cout << endl;
					cout << "Task completed." << endl;
					exit(0);
				}

				if (fds[x] == -1)	//event not ready
				continue;
				
				//Determine if the event corresponding to the file descriptor is ready
				if (FD_ISSET(fds[x], &fdset)) 
				{
			   
					//There are two kinds of cases for judging file descriptors
				   
			   		//A file descriptor is a socket, meaning accept if a new client requests a connection
					if (fds[x] == sockfd) 
					{
						//accept
						struct sockaddr_in caddr;
						int len = sizeof(caddr);
						int c = accept(sockfd, (struct sockaddr *)&caddr, (socklen_t*)&len);	//Accept new client connections
						if (c < 0)
						continue;
						fds_add(fds, c);//Add the connection socket to the array where the file descriptor is stored
					}

					//Receive data recv when an existing client sends data
					else 
					{
						char buff[128] = {0};
						string msg;
						int res = recv(fds[x], buff, 127, 0);

						if(res <= 0) 
						{
							close(fds[x]);
							fds[x] = -1;
							printf("\nOne Client Over\n");
						}

						else
						{
							index = fds[x];
							if (strncmp(buff, "odd", 3) == 0) 
							{
								cout << "This server cannot reserve odd tickets." << endl;
								send(fds[x],"Tickets are not available.", 26, 0);

								vector<int>::iterator it = find (declinedClients.begin(), declinedClients.end(), fds[x]);
								if (it == declinedClients.end())
								declinedClients.push_back(fds[x]);
								break;
							}

							else if (strncmp(buff, "EVEN", 4) == 0) 
							{
								cout << endl << "Client " << index << " requesting a ticket." << endl;

								if (allocatedTickets <= 12) 
								{
									reservedTickets[index] += "+";
									reservedTickets[index] += to_string(allocatedTickets);

									msg = "Ticket ";
									msg += to_string(allocatedTickets);
									msg += " reserved";
									send(fds[x], msg.c_str(), sizeof(msg), 0);
									cout << msg << " by Client "<< to_string(index) << endl;
									allocatedTickets += 2;
								}

								else 
								{
									msg = "No ticket available.";
									cout << msg << endl;
									send(fds[x], msg.c_str(), sizeof(msg), 0);
								}
							}

							else 
							{
								msg = "Invalid option selected.";
								cout << msg << endl;
								send(fds[x], msg.c_str(), sizeof(msg), 0);
							}
						}
					}
				}
			}
		}
	}
}
