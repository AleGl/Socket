#include "./SocketTCP.hpp"
#define IP_BROADCAST "192.168.6.255"

int main(int argc, char* argv[]) {

	if (argc != 3) {
		printf("USAGE: %s PORT MSG_ANSWER\n", argv[0]);
		return(-1);
	}

	char* answer = argv[2];
	int port = atoi(argv[1]);

	ServerTCP* server = new ServerTCP(port, true);
	ConnessioneTCP* conn;
	conn = server->accetta();
	conn->ricevi();
	conn->invia(answer);
	delete(conn);

	/*
	if (server->broadcast(true)) {
		printf("Errore sul broadcast\n");
	}

	client->setIp(IP_BROADCAST);
	server->invia(answer);
	*/

	delete(server);

	return 0;
}