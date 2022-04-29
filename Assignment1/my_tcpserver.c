// Name: Suryam Arnav Kalra
// Roll No.: 19CS30050
// TCP SERVER


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

	// run the server infinitely
	while(1) {

		// accept the connection from the socket
		int client_len = sizeof(client_addr);
		if ((client_socket = accept(server_socket, (struct sockaddr *)&client_addr, &client_len)) < 0) {
			close(server_socket);
			perror("Failure in Accept()");
			exit(1);
		}

		char buffer[BUFF_SIZE];
		int bytes_read = 0;
		int word_count = 0, character_count = 0, sentence_count = 0, i = 0;
		int current = 0;
		while(1) {
			bytes_read = recv(client_socket, buffer, BUFF_SIZE, 0);  // receive the data from the server
			i = 0;
			if(buffer[0] == '\0') {
				break;
			}
			// count the number of words, characters and sentences
			while(i < bytes_read && buffer[i] != '\0') {
				// if(buffer[i] == ' ' || buffer[i] == '\n') {
				// 	word_count++;
				// }
				// if(buffer[i] == '.') {
				// 	sentence_count++;
				// }
				// i++;
				if(buffer[i] == ' ' || buffer[i] == '\n' || buffer[i] == '.') {
					if(current >= 1) {
						word_count++;
					}
					current = 0;
					if(buffer[i] == '.') {
						sentence_count++;
					}
				}
				else {
					current++;
				}
				i++;
			}
			character_count += i;
			if(bytes_read <= 1) {
				break;
			}
		}
		bzero(buffer, sizeof(buffer));  // reset the buffer

		// convert word count to character array and send to the client
		i = 0;
		while(word_count) {
			buffer[i++] = word_count%10+'0';
			word_count /= 10;
		}
		for(int j = 0; j < i/2; j++) {
			char temp = buffer[j];
			buffer[j] = buffer[i-j-1];
			buffer[i-j-1] = temp;
		}
		buffer[i] = '\0';
		send(client_socket, buffer, BUFF_SIZE, 0);

		// convert character count to character array and send to the client
		i = 0;
		while(character_count) {
			buffer[i++] = character_count%10+'0';
			character_count /= 10;
		}
		for(int j = 0; j < i/2; j++) {
			char temp = buffer[j];
			buffer[j] = buffer[i-j-1];
			buffer[i-j-1] = temp;
		}
		buffer[i] = '\0';
		send(client_socket, buffer, BUFF_SIZE, 0);

		// convert sentence count to character array and send to the client
		i = 0;
		while(sentence_count) {
			buffer[i++] = sentence_count%10+'0';
			sentence_count /= 10;
		}
		for(int j = 0; j < i/2; j++) {
			char temp = buffer[j];
			buffer[j] = buffer[i-j-1];
			buffer[i-j-1] = temp;
		}
		buffer[i] = '\0';
		send(client_socket, buffer, BUFF_SIZE, 0);
		close(client_socket);  // close the client socket
	}
	// close(server_socket);
}