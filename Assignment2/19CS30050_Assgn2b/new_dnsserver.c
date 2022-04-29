// Name: Suryam Arnav Kalra
// Roll No.: 19CS30050
// CONCURRENT TCP SERVER + ITERATIVE UDP SERVER


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <dirent.h>
#include <sys/wait.h>
#include <netdb.h>

#define PORT 8080

const int BUFF_SIZE = 100;

int main(int argc, char *argv[]) {
	int tcp_server_socket, udp_server_socket;
	struct sockaddr_in server_addr, client_addr;
	int opt = 1;
	// reset the addresses
	bzero((char *)&server_addr, sizeof(server_addr));
	bzero((char *)&client_addr, sizeof(client_addr));

	tcp_server_socket = socket(AF_INET, SOCK_STREAM, 0);
	if(tcp_server_socket < 0) {
		perror("TCP Socket not created!");
		exit(1);
	}

	if(setsockopt(tcp_server_socket, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT,&opt, sizeof(opt))) {
		perror("setsockopt");
		exit(1);
	}

	
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(PORT);
	server_addr.sin_addr.s_addr = INADDR_ANY;
	// bind socket to server address
	if(bind(tcp_server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
		close(tcp_server_socket);
		perror("TCP Binding failed!");
		exit(1);
		
	}

	printf("\nYAAY!!! I'm running the TCP server at %d\n\n", PORT);

	// listen on the server
	if(listen(tcp_server_socket, 5) < 0) {
		close(tcp_server_socket);
		perror("Error in Listen()");
		exit(1);
	} 


	udp_server_socket = socket(AF_INET, SOCK_DGRAM, 0);
	if(udp_server_socket < 0){
		perror("UDP Socket Creation Failed");
		exit(1);
	}

	// bind the socket to the address
	if(bind(udp_server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0){
		close(udp_server_socket);
		perror("UDP Binding failed");
		exit(1);
	}

	printf("\nYAAY!!! I'm running the UDP server at %d\n\n", PORT);

	// int clilen = sizeof(client_addr);

	fd_set fds;

	FD_ZERO(&fds);

	int maxfd = 0;
	if(tcp_server_socket < udp_server_socket) {
		maxfd = udp_server_socket;
	}
	else {
		maxfd = tcp_server_socket;
	}

	maxfd = maxfd + 1;

	// run the server infinitely
	while(1) {

		FD_SET(tcp_server_socket, &fds);

		FD_SET(udp_server_socket, &fds);

		int n_fd = select(maxfd, &fds, NULL, NULL, NULL);

		// if the tcp socket is ready to receive
		if(FD_ISSET(tcp_server_socket, &fds)) {

			int pid;
			int client_len = sizeof(client_addr);

			int new_tcp_client_socket = accept(tcp_server_socket, (struct sockaddr*)&client_addr, &client_len);
			if(new_tcp_client_socket < 0) {
				close(tcp_server_socket);
				perror("Failure in Accept()");
				exit(1);
			}

			// close(tcp_socket_fd);
			printf("\nPROCESSING TCP REQUEST\n");
			if((pid = fork()) == 0) {
				close(tcp_server_socket); 

				int client_len = sizeof(client_addr);

				char buffer[BUFF_SIZE];
				int bytes_read = 0;
				
				// receive the data from the client
				bytes_read = recv(new_tcp_client_socket, buffer, BUFF_SIZE, 0);
				if(bytes_read < 0) {
					perror("Error in tcp receive");
					exit(1);
				}
				printf("TCP Receive: %s\n", buffer);
				struct hostent *h;
				h = gethostbyname(buffer);

				if(h == NULL) {
					// printf("IP : %s\n", inet_ntoa(*(struct in_addr*)h->h_addr));
					printf("Invalid hostname from client\n");
					buffer[0] = 'e', buffer[1] = 'r', buffer[2] = 'r', buffer[3] = 'o', buffer[4] = 'r', buffer[5] = '\0';
					send(new_tcp_client_socket, buffer, BUFF_SIZE, 0);
				}
				else {
					char *array;
					int count = 0;

				    do {
				    	if(!(h->h_addr_list[count])) {
				    		break;
				    	}
				    	array = inet_ntoa(*((struct in_addr*) h->h_addr_list[count])); 
				    	if(!array) {
				    		break;
				    	}
				    	printf("Sent IP %d : %s\n",++count,array);

				    	send(new_tcp_client_socket, (char *)array, BUFF_SIZE, 0); 

				    }while(array != NULL);
				    char temp[BUFF_SIZE];
				    temp[0] = '\0';
				    send(new_tcp_client_socket, temp, BUFF_SIZE, 0);
				}

				close(new_tcp_client_socket);
				exit(0);
			}
			close(new_tcp_client_socket);
		}

		// if the udp socket is ready to receive
		if(FD_ISSET(udp_server_socket, &fds)) {

		    printf("\nPROCESSING UDP REQEST\n");
		  
		    int client_len = sizeof(client_addr);

			char buffer[BUFF_SIZE];
			int bytes_read = 0;
			
			// receive the data from the client
			bytes_read = recvfrom(udp_server_socket, buffer, BUFF_SIZE, 0, ( struct sockaddr *) &client_addr, &client_len);
			if(bytes_read < 0) {
				perror("Error in UDP receive");
				exit(1);
			}
			printf("UDP Receive: %s\n", buffer);
				
			struct hostent *h;
			h = gethostbyname(buffer);

			if(h == NULL) {
				// printf("IP : %s\n", inet_ntoa(*(struct in_addr*)h->h_addr));
				printf("Invalid hostname from client\n");
				buffer[0] = 'e', buffer[1] = 'r', buffer[2] = 'r', buffer[3] = 'o', buffer[4] = 'r', buffer[5] = '\0';
				sendto(udp_server_socket, buffer, BUFF_SIZE, 0, (const struct sockaddr *) &client_addr, sizeof(client_addr));
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

			    do {
			    	if(!(h->h_addr_list[count])) {
			    		break;
			    	}
			    	array = inet_ntoa(*((struct in_addr*) h->h_addr_list[count])); 
			    	if(!array) {
			    		break;
			    	}
			    	printf("Sent IP %d : %s\n",++count,array);

			    	sendto(udp_server_socket, (char *)array, BUFF_SIZE, 0, (struct sockaddr *) &client_addr, sizeof(client_addr)); 

			    }while(array != NULL);
			    char temp[BUFF_SIZE];
			    temp[0] = '\0';
			    sendto(udp_server_socket, temp, BUFF_SIZE, 0, (struct sockaddr *) &client_addr, sizeof(client_addr));
			}
		}

	}
	// close(server_socket);
}