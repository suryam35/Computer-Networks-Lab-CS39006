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
#include <errno.h>
#include <dirent.h>
#include <sys/wait.h>
#include <netdb.h>
#include <pthread.h>

#define PORT 8080

int recvtimeout(int s, char *buf, int timeout, struct sockaddr_in client_addr) {
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
	int error = 0;
	n = select(s+1, &fds, NULL, NULL, &tv);
	if (n == 0) {
		error = -2; // timeout!
	}
	if(error == -2) {
		return -2;
	}
	if (n == -1) {
		error = -1; // error
	}
	if(error == -1) {
		return -1;
	}
	// data must be here, so do a normal recv()
	int len = sizeof(client_addr);
	return recv(s, buf, 100, 0);
	// return recv(s, buf, len, 0);
}

int main() {
	while(1) {
		printf("myClient> ");
		char command[2000];
		bzero(command, 2000);
		scanf("%[^\n]%*c", command);

		char command1[100];
		bzero(command1, 100);
		for(int i = 0; i < 3; i++) {
			command1[i] = command[i];
		}

		if(strcmp(command1, "del") == 0) {
			char filename[100];
			bzero(filename, 100);
			for(int i = 4; i < strlen(command); i++) {
				filename[i-4] = command[i];
			}

			int client_socket;
			struct sockaddr_in client_addr;
			bzero((char *)&client_addr, sizeof(client_addr));  // set the address to 0
			client_socket = socket(AF_INET, SOCK_STREAM, 0);   // create the socket
			if(client_socket < 0) {
				perror("Client Socket Creation Failed!!");
				exit(1);
			}

			client_addr.sin_family = AF_INET;
			client_addr.sin_port = htons(PORT);
			client_addr.sin_addr.s_addr = INADDR_ANY;

			if(connect(client_socket, (struct sockaddr *) &client_addr,
									sizeof(client_addr)) < 0) {
				close(client_socket);
				perror("Error in connection\n");
				exit(1);
			}

			int x = send(client_socket, command1, 100, 0);
			x = send(client_socket, filename, 100, 0);

			// wait for receive from server
			char buf[100];
			bzero(buf, 100);
			int rec = recvtimeout(client_socket, buf, 2, client_addr);
			if(rec <= 0) {
				printf("There is error in deleting file from server, waited for 2 sec as well!\n");
				close(client_socket);
				break;
			}
			else {
				printf("Message from server: %s\n", buf);
				close(client_socket);
				break;
			}

		}

		else {
			int i = 0;
			char command1[100];
			bzero(command1, 100);
			int j = 0;
			while(i < strlen(command) && command[i] != ' ') {
				command1[j++] = command[i];
				i++;
			}
			i++;
			char filename[100];
			bzero(filename, 100);
			j = 0;
			while(i < strlen(command) && command[i] != ' ') {
				filename[j++] = command[i++];
			}
			i++;
			j = 0;
			int x = 0, y = 0;
			char xnum[100], ynum[100];
			bzero(xnum, 100);
			bzero(ynum, 100);
			while(i < strlen(command) && command[i] != ' ') {
				xnum[j++] = command[i];
				x = 10 * x + command[i]-'0';
				i++;
			}
			i++;
			j = 0;
			while(i < strlen(command) && command[i] != ' ') {
				ynum[j++] = command[i];
				y = 10 * y + command[i]-'0';
				i++;
			}
			int client_socket;
			struct sockaddr_in client_addr;
			bzero((char *)&client_addr, sizeof(client_addr));  // set the address to 0
			client_socket = socket(AF_INET, SOCK_STREAM, 0);   // create the socket
			if(client_socket < 0) {
				perror("Client Socket Creation Failed!!");
				exit(1);
			}

			client_addr.sin_family = AF_INET;
			client_addr.sin_port = htons(PORT);
			client_addr.sin_addr.s_addr = INADDR_ANY;

			if(connect(client_socket, (struct sockaddr *) &client_addr,
									sizeof(client_addr)) < 0) {
				close(client_socket);
				perror("Error in connection\n");
				exit(1);
			}

			int bytes_sent = send(client_socket, command1, 100, 0);
			bytes_sent = send(client_socket, filename, 100, 0);
			bytes_sent = send(client_socket, xnum, 100, 0);
			bytes_sent = send(client_socket, ynum, 100, 0);

			char buf[100];
			while(1) {
				bzero(buf, 100);
				int bytes_recv = recvtimeout(client_socket, buf, 2, client_addr);
				if(bytes_recv <= 0) {
					printf("Error from server in transferring file\n");
					break;
				}
				printf("%s", buf);
				if(bytes_recv <= 99) {
					break;
				}
			}
			printf("\n");
			close(client_socket);
			break;
		}
	}
	


	return 0;
}