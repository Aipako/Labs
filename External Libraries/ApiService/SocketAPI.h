#pragma once
#include <windows.h>
#include <io.h>
#include <sys/types.h>
#include <string>
#include <errno.h>
#include <ws2tcpip.h>
class SocketAPI
{
public:
	int sockfd, newsockfd, portno;
	socklen_t clientlen;
	char buffer[256];
	struct sockaddr_in serv_addr, *cli_addr;

	SocketAPI();
	int OpenSocket(PWSTR m_name);
	int BindServer();
	int ListenServer();
	int AcceptServer();
	int SendServer(std::string msg, PWSTR m_name);
	int ReceiveServer();
	void CloseSocket();
};

