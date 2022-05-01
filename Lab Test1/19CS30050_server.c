// Name: Suryam Arnav Kalra
// Roll No.: 19CS30050

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <fcntl.h>

typedef struct cid {
	int port;
	int ip;
} cid;

#define PORT 8080

int main() {
	int tcp_server_socket;
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
	if(listen(tcp_server_socket, 3) < 0) {
		close(tcp_server_socket);
		perror("Error in Listen()");
		exit(1);
	}

	int clientsockfd[3];
	for(int i = 0; i < 3; i++) {
		clientsockfd[i] = -1;
	}

	cid clientid[3];
	int numclient = 0;

	fd_set fds;

	while(1) {
		int maxfd = tcp_server_socket;
		FD_ZERO(&fds);
		FD_SET(tcp_server_socket, &fds);
		for(int i = 0; i < 3; i++) {
			if(clientsockfd[i] != -1) {
				FD_SET(clientsockfd[i], &fds);
				if(clientsockfd[i] > maxfd) {
					maxfd = clientsockfd[i];
				}
			}
		}
		maxfd += 1;
		int n_fd = select(maxfd, &fds, NULL, NULL, NULL);

		if(FD_ISSET(tcp_server_socket, &fds)) {
			bzero((char *)&client_addr, sizeof(client_addr));
			int client_len = sizeof(client_addr);
			int new_tcp_client_socket = accept(tcp_server_socket, (struct sockaddr*)&client_addr, &client_len);
			if(new_tcp_client_socket < 0) {
				close(tcp_server_socket);
				perror("Failure in Accept()");
				exit(1);
			}
			clientsockfd[numclient] = new_tcp_client_socket;
			clientid[numclient].port = client_addr.sin_port;
			clientid[numclient].ip = client_addr.sin_addr.s_addr;
			// set the ip
			numclient++;
			printf("Server received a new connection from client <%d : %d>\n", clientid[numclient-1].port, clientid[numclient-1].ip);
		}

		for(int i = 0; i < numclient; i++) {
			if(clientsockfd[i] < maxfd) {
				if(FD_ISSET(clientsockfd[i], &fds)) {
					int port = clientid[i].port, ip = clientid[i].ip;
					char buffer[1000], new_buffer[1000];
					bzero(buffer, 1000);
					bzero(new_buffer, 1000);
					int bytes_recv = recv(clientsockfd[i], buffer, 999, 0);
					buffer[bytes_recv] = '\0';
					printf("Server: Received message <%s> from client <%d : %d>\n", buffer, port, ip);
					// append port and ip to new_buffer
					int k = 0;
					int tport = port, tip = ip;
					while(tport) {
						new_buffer[k++] = tport%10+'0';
						tport /= 10;
					}
					new_buffer[k++] = ' ';
					while(tip) {
						new_buffer[k++] = tip%10 + '0';
						tip /= 10;
					}
					new_buffer[k++] = ' ';
					for(int l = 0; l < strlen(buffer); l++) {
						new_buffer[k++] = buffer[l];
					}
					new_buffer[k] = '\0';
					if(numclient == 1) {
						printf("Server: Insufficient clients, message <%s> from client <%d : %d> dropped\n", buffer, port, ip);
					}
					else {
						for(int j = 0; j < numclient; j++) {
							if(j == i) {
								continue;
							}
							send(clientsockfd[j], new_buffer, 1000, 0);
							printf("Server: Sent message <%s> from client <%d : %d> to client <%d : %d>\n", new_buffer, port, ip, clientid[j].port, clientid[j].ip);
						}
					}
				}
			}
		}
	}
}