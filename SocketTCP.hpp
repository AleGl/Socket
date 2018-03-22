#include "./Address.hpp"
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <list>

#define MAX_CONNS 50
#define MAX_MSG 4096

class SocketTCP {
	protected:	int socket_id;
	public:		SocketTCP();						//socket()
				~SocketTCP();						//close()
				bool broadcast(bool broadcast);		//setsockopt()
};


class ConnessioneTCP {
private:	int conn_id;
			Address* address;
			bool client;
public:		ConnessioneTCP(int conn_id, Address* address, bool client);
			~ConnessioneTCP();						//shutdown()

			bool invia(char* msg);
			bool invia_raw(void* msg, int length);	//send()
			char* ricevi();
			void* ricevi_raw(int* length);	//recv()
};

class ServerTCP: public SocketTCP {
private:	std::list<ConnessioneTCP*> connessioni; 
public:		ServerTCP(int port, bool loopback);		//bind(), listen()
			~ServerTCP();							
			ConnessioneTCP* accetta();				//accept()
			
};

class ClientTCP: public SocketTCP {
	private:	ConnessioneTCP* connessione;
	public:		ClientTCP();					
				~ClientTCP();					

				bool connetti(Address* address);	//connect()
				bool invia(char* msg);
				bool invia_raw(void* msg, int length);	
				char* ricevi();
				void* ricevi_raw(int* length);
};


SocketTCP::SocketTCP() {
	socket_id = socket(AF_INET, SOCK_STREAM, 0);
}

SocketTCP::~SocketTCP() {
	close(socket_id);
}

bool SocketTCP::broadcast(bool broadcast) {
	int len_so = sizeof(int); 
	int val = broadcast? 1: 0;
	return (setsockopt(socket_id, 
						SOL_SOCKET,
						SO_BROADCAST, 
						&val, 
						(socklen_t) len_so) == 0);
}

ConnessioneTCP::ConnessioneTCP(int conn_id, Address* address, bool client) {
	this->conn_id = conn_id;
	this->address = address;
	this->client = client;
}

ConnessioneTCP::~ConnessioneTCP() {
	delete(this->address);

	if(!this->client){
		shutdown(conn_id, SHUT_RDWR);
	}
}

bool ConnessioneTCP::invia(char* msg) {
	int bufferLen;
	bufferLen = strlen(msg) + 1;

	return invia_raw(msg, bufferLen);
}

bool ConnessioneTCP::invia_raw(void* msg, int length) {
	int ret = send(conn_id, msg, length, 0);
	if(ret != length)
		return true;

	return false;
}

char* ConnessioneTCP::ricevi() {
	char* buffer;
	int ret;

	buffer = (char*) ricevi_raw(&ret);
	if(!buffer)
		return buffer;
	buffer[ret + 1] = '\0';

	return buffer;
}

void* ConnessioneTCP::ricevi_raw(int* length) {
	void* buffer = malloc(MAX_MSG + 1);
	*length = recv(conn_id, buffer, MAX_MSG, 0);
	if(*length <= 0){
		free(buffer);
		return NULL;
	}

	return buffer;
}


ServerTCP::ServerTCP(int port, bool loopback):SocketTCP() {
	struct sockaddr_in myself;
	myself.sin_family = AF_INET;
	inet_aton(loopback?"127.0.0.1":"0.0.0.0", &myself.sin_addr);
	myself.sin_port = htons(port);
	for (int i = 0; i < 8; i++) {
		myself.sin_zero[i] = 0;	
	}
	
	if (bind(socket_id, (struct sockaddr*)&myself, (socklen_t)sizeof(struct sockaddr_in))) {
		printf("Errore sulla bind()\n");
	}

	if(listen(socket_id, MAX_CONNS)) {
		printf("Errore sulla listen()\n");
	}
}

ServerTCP::~ServerTCP(){}

ConnessioneTCP* ServerTCP::accetta() {
	Address* address;
	struct sockaddr_in client;
	int length = sizeof(struct sockaddr);
	int conn_id = accept(socket_id, (struct sockaddr*)&client, (socklen_t*)&length);
	address = new Address(client);
	ConnessioneTCP* conn = new ConnessioneTCP(conn_id, address, false);
	connessioni.push_back(conn);
	return conn;
}

ClientTCP::ClientTCP():SocketTCP() {}

ClientTCP::~ClientTCP(){
	delete(this->connessione);
}

bool ClientTCP::connetti(Address* address) {
	struct sockaddr_in server = address->getAddress();
	if (connect(socket_id, 
			    (struct sockaddr*)&server, 
			    (socklen_t)sizeof(struct sockaddr_in))) {
		return false;
	}

	ConnessioneTCP* conn = new ConnessioneTCP(socket_id, address, true);
	this->connessione = conn;

	return true;
}

bool ClientTCP::invia(char* msg) {
	connessione->invia(msg);
}

bool ClientTCP::invia_raw(void* msg, int length) {
	connessione->invia_raw(msg, length);
}

char* ClientTCP::ricevi() {
	connessione->ricevi();
}

void* ClientTCP::ricevi_raw(int* length) {
	connessione->ricevi_raw(length);
}

