
#include "ServerClass.h"

/*bool bflag
bool recvflag;
bflag = true; recvflag = true;*/


Server_UDP::Server_UDP()
{
	//const char* addr;
	string addr = "10.42.0.171"; // IP of EMlid at DRONE-CONSULT network is "192.168.0.12" and at rpi network is "10.42.0.171"
	//serv_addr = addr;
	serv_port = 9999;
	
	sockfd = socket(AF_INET, SOCK_DGRAM,0);
	serv.sin_family = AF_INET;
	serv.sin_port = htons(9999);
	serv.sin_addr.s_addr = inet_addr("192.168.0.18"); // IP address of rpi in in hotspot mode is "10.42.0.1"
	l = sizeof(client);
	cout << "Server object created" << endl;
	//bflag =  true;
	//recvflag = true;
}

/*Server_UDP::Server_UDP(char* ip_addr, int* port)
{
	serv_addr = ip_addr;
	serv_port = *port;	
	
	sockfd = socket(AF_INET, SOCK_DGRAM,0);
	serv.sin_family = AF_INET;
	serv.sin_port = htons(serv_port);
	serv.sin_addr.s_addr = inet_addr(serv_addr);
	l = sizeof(client);
}*/

Server_UDP::~Server_UDP()
{
	close(sockfd);
}

bool Server_UDP::Server_start()
{
	if(bind(sockfd, (struct sockaddr*)&serv, sizeof(serv)) < 0)
	{
		cout << "Binding failed" << endl;
		//bflag = false;
		return 0;
	}
	else
	{
		cout << "Binding is completed" << endl;
		//bflag = true;
		return 1;
	}
}

bool Server_UDP::Server_recv(char* buf, int size)
{
	int rcv = recvfrom(sockfd, buf, size, 0, (struct sockaddr* )&client, &l);
	if (rcv < 0)
	{
		cout << "Error in receiving buffer." << endl;
		//recvflag = false;
		return 0;
	}
	else
	{
		//recvflag = true;
		cout << "Message received is: " << buf << endl;
		return 1;
	}
}

bool Server_UDP::Server_send(char *buf, int size)
{
//
}

/*char Server_UDP::Get_buffer()
{
	return(buffer);
}
*/

// Test Server_UDP Class
/*
int  main()
{

	char buffer[1024];

	Server_UDP udp_obj;
	bool bflag = udp_obj.Server_start();
	bool recvflag = true;
	while(bflag&&recvflag)
	{
		recvflag = udp_obj.Server_recv(buffer, 1024);
		//buffer = udp_obj->Get_buffer();
	}
	udp_obj.~Server_UDP();
	return 0;
}*/




	
