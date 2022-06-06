#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include "SocketAPI.h"
#include <iostream>
#include <assert.h>
#include <strsafe.h>
#include <io.h>
#include <ws2tcpip.h>

SocketAPI::SocketAPI()
{
	this->portnum = 8080;
	this->server = NULL;
}

int SocketAPI::OpenSocket()
{
	WSADATA wsaData;
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
	{
		return 1;
	}
	int Error = 0;
	this->sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (this->sockfd < 0)
	{
		Error = -1;
		std::cout << "Error Code :" << errno << std::endl;
	}
	return Error;
}

int SocketAPI::GetHost(std::string hostname)
{
	int Error = 0;
	this->server = gethostbyname(hostname.c_str());
	if (this->server == NULL)
	{
		Error = -1;
		std::cout << "Error Code :" << errno << std::endl;
	}
	return Error;
}

int SocketAPI::ClientConnect()
{
	int Error = 0;
	memset((char*)&this->serv_addr, 0, sizeof(this->serv_addr));
	this->serv_addr.sin_family = AF_INET;
	memcpy((char*)&this->serv_addr.sin_addr.s_addr, (char*)this->server->h_addr, this->server->h_length);
	this->serv_addr.sin_port = htons(portnum);
	int conRet = connect(this->sockfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr));
	if (conRet < 0)
	{
		Error = -1;
		std::cout << "Error Code :" << errno << std::endl;
	}
	return Error;
}

int SocketAPI::ClientRead()
{
	int Error = 0;
	//int RdRet = recv(this->sockfd, this->buffer2, sizeof(this->buffer2), 0);
	memset(this->buffer, 0, 12000);
	//int RdRet = _read(this->sockfd, this->buffer, 9999);
	int RdRet = 0;
	resultString = "";
	do 
	{
		RdRet = recv(this->sockfd, this->buffer, sizeof(this->buffer), 0);
		for (int i = 0; i <= RdRet; ++i)
		{
			if (this->buffer[i] >= 32 || this->buffer[i] == '\n' || this->buffer[i] == '\r') { //only write valid chars
				resultString += this->buffer[i]; //final string
			}
		}
		if (this->buffer[RdRet] == '\0') { //buf[len] is the last position in the received chanks 
			break;
		}
	} while (true);
	if (RdRet < 0)
	{
		Error = -1;
		std::cout << "Error Code :" << errno << std::endl;
	}
	return Error;
}

void SocketAPI::CloseSocket()
{
	_close(this->sockfd);
}