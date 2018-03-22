#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#define MAX_MSG 4096

//Prototipi
void errore(int, char*);

int main(int argc, char* argv[]) {
	if (argc != 4) {
		printf("USAGE:%s IP PORT MSG\n", argv[0]);
		return(-1);
	}

	int sock_id;
	char buffer[MAX_MSG + 1];

	sock_id = socket(AF_INET, SOCK_STREAM, 0);

	struct sockaddr_in server;

	server.sin_family = AF_INET;
	inet_aton(argv[1], &server.sin_addr);
	server.sin_port = htons(atoi(argv[2]));
	for (int i = 0; i < 8; i++) {
		server.sin_zero[i] = 0;
	}

	if (connect(sock_id, 
			    (struct sockaddr*)&server, 
			    (socklen_t)sizeof(struct sockaddr_in))) {
		errore(-2, "connect()");
	}

	int ret = send(sock_id, argv[3], strlen(argv[3]) + 1, 0);
	if (ret != strlen(argv[3]) + 1) {
		errore(-3, "send()");
	}

	ret = recv(sock_id, buffer, MAX_MSG, 0);
	if (ret <= 0) {
		errore(-4, "recv()");
	}

	buffer[ret] = '\0';

	printf("[%s:%d] %s\n", argv[1], atoi(argv[2]), buffer);

	close(sock_id);
	
	return 0;
}

/* gestisce gli errori bloccanti */
void errore(int retCode, char* description){
	printf("ERRORE bloccante su %s.\n", description);
	printf("%d:%s\n", errno, strerror(errno));
	printf("Exiting with retun code %d.\n", retCode);
	exit(retCode);
}