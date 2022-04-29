// Name: Suryam Arnav Kalra
// Roll No.: 19CS30050
// UDP CLIENT


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <sys/time.h>

#define PORT 8080

const int BUFF_SIZE = 100;


int recvtimeout(int s, char *buf, int timeout) {
	fd_set fds;
	int n;
	struct timeval tv;
	// set up the file descriptor set
	FD_ZERO(&fds);
	FD_SET(s, &fds);
	// set up the struct timeval for the timeout
	tv.tv_sec = timeout;
	tv.tv_usec = 0;
	// wait until timeout or data received
	n = select(s+1, &fds, NULL, NULL, &tv);
	if (n == 0) return -2; // timeout!
	if (n == -1) return -1; // error
	// data must be here, so do a normal recv()
	return recv(s, buf, BUFF_SIZE, 0);
}

int main(int argc, char *argv[]) {
	// get the file name
	char dnsname[100];
	printf("Enter the DNS Name: ");
	scanf("%s", dnsname);


	// create the client socket
	int client_socket;
	struct sockaddr_in client_addr;
	bzero((char *)&client_addr, sizeof(client_addr));  // set the address to 0
	client_socket = socket(AF_INET, SOCK_STREAM, 0);   // create the socket
	if(client_socket < 0) {
		perror("Client Socket Creation Failed!!");
		exit(1);
	}

	// create the socket address
	client_addr.sin_family = AF_INET;
	client_addr.sin_port = htons(PORT);
	client_addr.sin_addr.s_addr = INADDR_ANY;

 	// connect the socket
	if(connect(client_socket, (struct sockaddr*)&client_addr, sizeof(client_addr)) < 0 ){
		close(client_socket);
		perror ("Connection Failed When Creating from Client");
		exit(1);
	}

	// send the dns name to the server
	int x = send(client_socket, dnsname, BUFF_SIZE, 0);
	if(x < 0) {
		perror("ERROR in sending data tcp");
		exit(1);
	}

	while(1) {
		char buf[BUFF_SIZE];
		int bytes_received = recvtimeout(client_socket, buf, 2);
		if(bytes_received == -2) {
			printf("Server Timeout\n");
			break;
		}
		if(bytes_received == -1) {
			printf("Error in select\n");
			break;
		}
		if(bytes_received == 0 || buf[0] == '\0') {
			break;
		}
		if(strcmp(buf, "error") == 0) {
			printf("Error in DNS name\n");
			break;
		}
		printf("IP : %s\n", buf);
	}
	printf("\n\nClosing Client\n\n");
	close(client_socket);
}