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

#define PORT 8080

int main() {
	int client_socket;
	struct sockaddr_in client_addr;
	bzero((char *)&client_addr, sizeof(client_addr));  // set the address to 0
	client_socket = socket(AF_INET, SOCK_STREAM, 0);   // create the socket
	if(client_socket < 0) {
		perror("Client Socket Creation Failed!!");
		exit(1);
	}

	int val, err;

	val = fcntl(client_socket, F_GETFL, 0);
	err = fcntl(client_socket, F_SETFL, val | O_NONBLOCK);
    if (err != 0) {
        printf("Socket cannot be made nonblocking, exiting\n");
        exit(0);
    }

	// create the socket address
	client_addr.sin_family = AF_INET;
	client_addr.sin_port = htons(PORT);
	client_addr.sin_addr.s_addr = INADDR_ANY;

	while ( connect(client_socket, (struct sockaddr *) &client_addr,
							sizeof(client_addr)) < 0) {
            if (errno == EINPROGRESS || errno == EALREADY) {
                printf("Delay in making connection? Let me sleep and retry after 1 sec\n");
                sleep(1);
            }
            else {
			printf("Unable to connect to server\n");
			exit(0);
	    	  }
         }

    int N = 1;
    while(1) {
    	int T = 1 + rand()%3;
    	sleep(T);
    	char msg[1000];
    	bzero(msg, 1000);
    	msg[0] = 'M', msg[1] = 'e', msg[2] = 's', msg[3] = 's', msg[4] = 'a', msg[5] = 'g', msg[6] = 'e', msg[7] = ' ';
    	if(N < 6) {
    		msg[8] = N + '0';
    		msg[9] = '\0';
    		send(client_socket, msg, 999, 0);
    		N++;
    		printf("Message %d sent\n", N-1);
    	}

    	char buf[1000];
    	bzero(buf, 1000);
    	int flag = 0;
    	while ( recv(client_socket, buf, 1000, 0) < 0) {
           if (errno == EWOULDBLOCK || errno == EAGAIN) {
				flag = 1;
				break;
            }
	    	else {
                /* some otiher error */
                printf("Fatal error in recv. Exiting\n");
                exit(0);
            }
		}
		if(flag == 1) {
			continue;
		}
		char ip[50], port[10];
		int i  =0, j  = 0;
		while(i < strlen(buf) && buf[i] != ' ') {
			port[j++] = buf[i];
			i++;
		}
		i++;
		port[j] = '\0';
		j = 0;
		while(i < strlen(buf) && buf[i] != ' ') {
			ip[j++] = buf[i++];
		}
		i++;
		ip[j] = '\0';

		printf("Client: Received message <%s> from client <%s : %s>\n", buf, port, ip);
    }

}


// If the receive is blocking, we can use a timeout receive using select call in which we can 
// set a timeout value of 2 sec and if we do not have any data till then, we will timeout and continue
// taking new messages from the keyboard. 