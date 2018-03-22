#ifndef __ADDRESS_HPP
#define __ADDRESS_HPP

#include "./Address.hpp"
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>

#define LOOPBACK "127.0.0.1"
#define MYSELF_IP "0.0.0.0"

#define MAX_MSG 4096
void errore(int, char*);
class ClientTCP {
	private: int sockId;
	public: ClientTCP();
			~ClientTCP();
			bool connetti(Address server);
			bool invia(char* msg);
			char* ricevi();
			bool inviaRaw(void* msg, int length);
			void* riceviRaw(int* length);
			bool broadcast(bool status);
};

ClientTCP::ClientTCP(){
	sockId = socket(AF_INET,SOCK_STREAM,0);
}

ClientTCP::~ClientTCP(){
	close(sockId);
}

/*
restituisce vero nel caso in cui si siano verificati degli errori
*/
bool ClientTCP::connetti(Address server){
	struct sockaddr_in structServer = server.getAddress(); 
	if(connect(sockId, 
			  (struct sockaddr*) &structServer,
			  (socklen_t) sizeof(struct sockaddr_in))){
		return true;
	}

	return false;
}

bool ClientTCP::invia(char* msg){
	int bufferLen;
	bufferLen = strlen(msg) + 1;

	return inviaRaw(msg, bufferLen);
}

char* ClientTCP::ricevi(){
	char* buffer;
	int ret;

	buffer = (char*) riceviRaw(&ret);
	if(!buffer)
		return buffer;
	buffer[ret + 1] = '\0';

	return buffer;
}

/*
restituisce vero nel caso in cui si siano verificati degli errori
*/
bool ClientTCP::inviaRaw(void* msg, int length){
	int ret = send(sockId, msg, length, 0);
	if(ret != length)
		return true;

	return false;
}

void* ClientTCP::riceviRaw(int* length){
	void* buffer = malloc(MAX_MSG + 1);
	*length = recv(sockId, buffer, MAX_MSG, 0);
	if(*length <= 0){
		free(buffer);
		return NULL;
	}

	return buffer;
}

bool ClientTCP::broadcast(bool status){
	int len_so = sizeof(int); 
	int val = status? 1: 0;
	return (setsockopt(sockId, 
						SOL_SOCKET,
						SO_BROADCAST, 
						&val, 
						(socklen_t) len_so) == 0);
}

/* gestisce gli errori bloccanti */
void errore(int retCode, char* description){
	printf("ERRORE bloccante su %s.\n", description);
	printf("%d:%s\n", errno, strerror(errno));
	printf("Exiting with retun code %d.\n", retCode);
	exit(retCode);
}

#endif //__ADDRESS_HPP