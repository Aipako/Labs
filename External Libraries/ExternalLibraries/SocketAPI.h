#pragma once
#include <WinSock2.h>
#include<string>
class SocketAPI
{
public:
	int sockfd, portnum;
	struct sockaddr_in serv_addr;
	struct hostent* server;
	char buffer[12000];
	std::string resultString;
	SocketAPI();
	int OpenSocket();
	int GetHost(std::string hostname);
	int ClientConnect();
	int ClientRead();
	void CloseSocket();
};

