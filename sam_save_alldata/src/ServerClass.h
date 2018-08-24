#ifndef SERVER_CLASS_H
#define SERVER_CLASS_H

// Writing a Server Class for UDP protocol

#include <iostream>
#include <ostream>
#include <arpa/inet.h>
#include <unistd.h>

#include <sys/socket.h>
#include <sys/types.h>

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
using namespace std;

class Server_UDP
{

public: 
	Server_UDP();
	//Server_UDP(char* ip_addr, int* port);
	~Server_UDP();
	bool Server_start();
	//int Server_close();
	bool Server_recv(char* buf, int size);
	bool Server_send(char* buf, int size);
	char Get_buffer();

private:
	char* serv_addr;
	int recvlen;
	int serv_port;
	int sockfd;
	struct sockaddr_in serv, client;
	socklen_t l;
	char buffer[1024];
	//bool bflag;
	//bool recvflag;
};
#endif
