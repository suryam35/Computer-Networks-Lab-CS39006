// Name: Suryam Arnav Kalra
// Roll No.: 19CS30050
// TCP CLIENT


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <fcntl.h>

#define PORT 8080

const int BUFF_SIZE = 100;

int main(int argc, char *argv[]) {

	// get file name
	char *file_name = argv[1];
	// get file descriptor
	int file_descriptor = open(file_name, O_RDONLY);
	if(file_descriptor < 0) {
		perror("File not found");
		exit(1);
	}


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

	char buffer[BUFF_SIZE];
	int bytes_read = 0;
	
	// read data from the file
	while(1) {
		bytes_read = read(file_descriptor, buffer, BUFF_SIZE-1);  // read data
		buffer[bytes_read] = '\0';  // set the null character
		int x = send(client_socket, buffer, BUFF_SIZE, 0);  // send this data to the server
		bzero(buffer, sizeof(buffer));  // reset the buffer
		if(bytes_read <= 0) {
			break;
		}
	}

	// receive the number of words
	recv(client_socket, buffer, BUFF_SIZE, 0);
	printf("\nNumber of words = %s\n", buffer);

	// receive the number of characters
	recv(client_socket, buffer, BUFF_SIZE, 0);
	printf("Number of characters = %s\n", buffer);

	// receive the number of sentences
	recv(client_socket, buffer, BUFF_SIZE, 0);
	printf("Number of sentences = %s\n", buffer);

	// close the client socket and the file descriptor
	printf("\nClosing Client\n\n");
	close(file_descriptor);
	close(client_socket);
}