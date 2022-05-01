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
#define MAX_THRD 100
pthread_t tid[MAX_THRD];

void *runner(void *param) {
	int client_socket = *(int *)param;
	char command[100];
	bzero(command, 100);
	int bytes_recv = recv(client_socket, command, 100, 0);
	if(strcmp(command, "del") == 0) {
		char filename[100];
		bzero(filename, 100);
		bytes_recv = recv(client_socket, filename, 100, 0);
		int rm = remove(filename);
		if(rm != 0) {
			close(client_socket);
			pthread_exit(0);
		}
		else {
			char msg[100] = "delete success";
			int bytes_sent = send(client_socket, msg, 100, 0);
		}
	}
	else {
		char filename[100];
		bzero(filename, 100);
		bytes_recv = recv(client_socket, filename, 100, 0);
		char xnum[100], ynum[100];
		bzero(xnum, 100);
		bzero(ynum, 100);
		bytes_recv = recv(client_socket, xnum, 100, 0);
		bytes_recv = recv(client_socket, ynum, 100, 0);
		int file_d = open(filename, O_RDONLY);
		if(file_d < 0) {
			close(client_socket);
			pthread_exit(0);
		}
		else {
			int file_size = lseek(file_d, 0, SEEK_END);
			int x = 0, y = 0;
			for(int i = 0; i < strlen(xnum); i++) {
				x = 10 * x + xnum[i]-'0';
			}
			for(int i = 0; i < strlen(ynum); i++) {
				y = 10 * y + ynum[i]-'0';
			}
			if(x >= file_size || y >= file_size) {
				close(client_socket);
				pthread_exit(0);
			}
			else {
				lseek(file_d, x, SEEK_SET);
				char buffer[100];
				int to_send = y-x+1;
				while(1) {
					bzero(buffer, 100);
					if(to_send <= 99) {
						read(file_d, buffer, to_send);
						int bytes_sent = send(client_socket, buffer, to_send, 0);
						if(bytes_sent < 0) {
							close(client_socket);
							pthread_exit(0);
						}
						break;
					}
					else {
						read(file_d, buffer, 100);
						int bytes_sent = send(client_socket, buffer, 100, 0);
						if(bytes_sent < 0) {
							close(client_socket);
							pthread_exit(0);
						}
						to_send -= 100;
					}
				}
				printf("Bytes <%d> to <%d> of filename <%s> sent\n", x, y, filename);
			}
		}
	}

	close(client_socket);
	pthread_exit(0);
}

int main() {
	int server_socket, client_socket;
	int opt = 1;
	struct sockaddr_in server_addr, client_addr;

	// reset the addresses
	bzero((char *)&server_addr, sizeof(server_addr));
	bzero((char *)&client_addr, sizeof(client_addr));

	// create the server socket
	server_socket = socket(AF_INET, SOCK_STREAM, 0);
	if(server_socket < 0){
		perror("Server Socket Creation Failed");
		exit(1);
	}

	// do the forceful binding of the socket to the address
	if(setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT,&opt, sizeof(opt))) {
		perror("setsockopt");
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

	printf("\nYAAY!!! I'm running the TCP server at %d\n\n", PORT);

	// listen on the server
	if(listen(server_socket, 5) < 0) {
		close(server_socket);
		perror("Error in Listen()");
		exit(1);
	}

	fd_set fds;

	FD_ZERO(&fds);

	int maxfd = server_socket + 1;
	int client = 0;

	while(1) {
		FD_SET(server_socket, &fds);
		int n_fd = select(maxfd, &fds, NULL, NULL, NULL);
		if(FD_ISSET(server_socket, &fds)) {
			int client_len = sizeof(client_addr);

			int new_tcp_client_socket = accept(server_socket, (struct sockaddr*)&client_addr, &client_len);
			if(new_tcp_client_socket < 0) {
				close(server_socket);
				perror("Failure in Accept()");
				// exit(1);
			}
			else {
				pthread_attr_t attr; //Set of thread attributes
    			pthread_attr_init(&attr);
    			int *param;
    			param = &new_tcp_client_socket;
    			int R = pthread_create(&tid[client], &attr, runner, param);
    			if(R < 0) {
    				exit(1);
    			}
    			client++;
			}
		}
	}


	return 0;
}