#include "./ClientTCP.hpp"
#define IP_BROADCAST "192.168.6.255"

/*Metodo che testa la classe ClientTCP.cpp*/
int main(int argc, char *argv[])
{
	/*Controllo del corretto passaggio dei parametri al programma*/
	if(argc != 4){
		printf("USAGE: %s IP_SERVER PORTA_SERVER MESSAGGIO\n", argv[0]);
		return(-1);
	}

	/*Istanzio un nuovo oggetto di classe client*/
	ClientTCP* client = new ClientTCP();
	Address* server = new Address(argv[1], atoi(argv[2]));

	/*Richiedo la connessione al server*/
	client->connetti(*server);

	/*Invio il messaaggio al server*/
	client->invia(argv[3]);

	/*Stampo la risposta del server*/
	char* resp;
	printf("Ho ricevuto:\n %s", resp=client->ricevi());
	free(resp);

	/*Testo la funzione broadcast*/
	if(client->broadcast(true)){
		printf("Broadcast non attivo\n");
	}
	server->setIp(IP_BROADCAST);
	client->connetti(*server);
	client->invia(argv[3]);

	/*Richiamo il distruttore della classe client*/
	delete(client);
	delete(server);

	return 0;
}