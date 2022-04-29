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

#define PORT 8044

const int BUFF_SIZE = 100;

int main(int argc, char *argv[]) {
	// get the file name
	char *file_name = argv[1];

	// open the file with a file descriptor
	int file_descriptor = open(file_name, O_RDONLY, 0);
	if(file_descriptor < 0) {
		perror("File not found");
		exit(1);
	}

	// create the client socket
	int client_socket;
	struct sockaddr_in client_addr;
	bzero((char *)&client_addr, sizeof(client_addr));
	client_socket = socket(AF_INET, SOCK_DGRAM, 0);
	if(client_socket < 0) {
		perror("Client Socket Creation Failed!!");
		exit(1);
	}

	// create the client address
	client_addr.sin_family = AF_INET;
	client_addr.sin_port = htons(PORT);
	client_addr.sin_addr.s_addr = INADDR_ANY;


	char buffer[BUFF_SIZE];
	int bytes_read = 0;
	while(1){
		bytes_read = read(file_descriptor, buffer, BUFF_SIZE-1);  // read data from the file
		buffer[bytes_read] = '\0';
		// send this data to the server
		sendto(client_socket, buffer, BUFF_SIZE, 0, (const struct sockaddr *) &client_addr, sizeof(client_addr));
		// reset the buffer
		bzero(buffer, sizeof(buffer));
		if(bytes_read <= 0) {
			break;
		}
	}
	int len = sizeof(client_addr);

	// receive the number of words
	recvfrom(client_socket, buffer, BUFF_SIZE, 0, ( struct sockaddr *) &client_addr, &len);
	printf("Number of words = %s\n", buffer);

	// receive the number of characters
	recvfrom(client_socket, buffer, BUFF_SIZE, 0, ( struct sockaddr *) &client_addr, &len);
	printf("Number of characters = %s\n", buffer);

	// receive the number of sentences
	recvfrom(client_socket, buffer, BUFF_SIZE, 0, ( struct sockaddr *) &client_addr, &len);
	printf("Number of sentences = %s\n", buffer);

	// close the client socket and file descriptor
	printf("\n\nClosing Client\n\n");
	close(file_descriptor);
	close(client_socket);
}