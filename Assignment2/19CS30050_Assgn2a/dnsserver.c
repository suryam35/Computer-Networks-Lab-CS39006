// Name: Suryam Arnav Kalra
// Roll No.: 19CS30050
// UDP SERVER


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <netdb.h>
#include <sys/time.h>

#define PORT 8044

const int BUFF_SIZE = 100;

int main(int argc, char *argv[]) {
	int server_socket;
	struct sockaddr_in server_addr, client_addr;

	// reset the addresses
	bzero((char *)&server_addr, sizeof(server_addr));
	bzero((char *)&client_addr, sizeof(client_addr));

	// create the server socket
	server_socket = socket(AF_INET, SOCK_DGRAM, 0);
	if(server_socket < 0){
		perror("Server Socket Creation Failed");
		exit(1);
	}

	// create the server address
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(PORT);
	server_addr.sin_addr.s_addr = INADDR_ANY;

	// bind the socket to the address
	if(bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0){
		close(server_socket);
		perror("Binding failed");
		exit(1);
	}

	printf("\nYAAY!!! I'm running the UDP server at %d\n\n", PORT);

	// run the server infinitely
	while(1) {
		int client_len = sizeof(client_addr);

		char buffer[BUFF_SIZE];
		int bytes_read = 0;
		
		// receive the data from the client
		bytes_read = recvfrom(server_socket, buffer, BUFF_SIZE, 0, ( struct sockaddr *) &client_addr, &client_len);
			
		struct hostent *h;
		h = gethostbyname(buffer);

		if(h == NULL) {
			// printf("IP : %s\n", inet_ntoa(*(struct in_addr*)h->h_addr));
			printf("Invalid hostname from client\n");
			buffer[0] = 'e', buffer[1] = 'r', buffer[2] = 'r', buffer[3] = 'o', buffer[4] = 'r', buffer[5] = '\0';
			sendto(server_socket, buffer, BUFF_SIZE, 0, (const struct sockaddr *) &client_addr, sizeof(client_addr));
		}
		else {
			char *array;
			int count = 0;

			// struct in_addr **addr_list;
			// int i;

			// addr_list = (struct in_addr **) h->h_addr_list;
			
			// for(i = 0; addr_list[i] != NULL; i++) 
			// {
			// 	//Return the first one;
			// 	printf("IP : %s\n", inet_ntoa(*addr_list[i]));
			// }

		    do{
		    	if(!(h->h_addr_list[count])) {
		    		break;
		    	}
		    	array = inet_ntoa(*((struct in_addr*) h->h_addr_list[count])); 
		    	if(!array) {
		    		break;
		    	}
		    	printf("Sent IP %d : %s\n",++count,array);

		    	sendto(server_socket, (char *)array, BUFF_SIZE, 0, (struct sockaddr *) &client_addr, sizeof(client_addr)); 

		    }while(array != NULL);
		    char temp[BUFF_SIZE];
		    temp[0] = '\0';
		    sendto(server_socket, temp, BUFF_SIZE, 0, (const struct sockaddr *) &client_addr, sizeof(client_addr));
		}

		// sendto(server_socket, buffer, BUFF_SIZE, 0, (const struct sockaddr *) &client_addr, sizeof(client_addr));

	}
	// close(server_socket);
}