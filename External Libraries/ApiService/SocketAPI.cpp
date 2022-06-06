#include <BaseTsd.h>
typedef SSIZE_T ssize_t;
#define CURL_STATICLIB
#define _CRT_NONSTDC_NO_DEPRECATE
#define _CRT_SECURE_NO_WARNINGS
#include "pch.h"
#include "SocketAPI.h"
#include<io.h>
#include<sys/types.h>
#include<string>
#include<errno.h>
#include<WinSock2.h>

#include <assert.h>
#include <strsafe.h>


SocketAPI::SocketAPI()
{
	cli_addr = (sockaddr_in*)calloc(1, sizeof(sockaddr));
}


void WriteEventLogEntry(PWSTR scmMessage, WORD wType, PWSTR m_name)
{
	HANDLE hEventSource = NULL;
	LPCWSTR lscmStrings[2] = { NULL, NULL };

	hEventSource = RegisterEventSource(NULL, m_name);
	if (hEventSource)
	{
		lscmStrings[0] = m_name;
		lscmStrings[1] = scmMessage;

		ReportEvent(hEventSource,
			wType,
			0,
			0,
			NULL,
			2,
			0,
			lscmStrings,
			NULL
		);

		DeregisterEventSource(hEventSource);
	}
}
void WriteErrorLogEntry(PWSTR scmFunction, DWORD dwError, PWSTR m_name)
{
	wchar_t szMessage[260];
	StringCchPrintf(szMessage, ARRAYSIZE(szMessage), L"%s failed with error 0x%08lx", scmFunction, dwError);
	WriteEventLogEntry(szMessage, EVENTLOG_ERROR_TYPE, m_name);
}
int SocketAPI::OpenSocket(PWSTR m_name)
{

	WSADATA wsaData;
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
	{
		return 1;
	}
	int error = 0;
	//create socket
	this->sockfd = socket(AF_INET, SOCK_STREAM, 6);
	if (this->sockfd < 0)
	{
		error = -1;
		WriteEventLogEntry(L"CentrobankAPI: Can't create socket", EVENTLOG_INFORMATION_TYPE, m_name);
		return error;
	}
	//clear address structure
	memset((char*)&this->serv_addr, 0, sizeof(this->serv_addr));
	this->portno = 8080;
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = INADDR_ANY;
	serv_addr.sin_port = htons(portno);

	return error;
}

int SocketAPI::BindServer()
{
	int Error = 0;
	int bindRet = bind(sockfd, (struct sockaddr*)&this->serv_addr, sizeof(this->serv_addr));
	if (bindRet < 0)
	{
		Error = -1;
	}
	return Error;
}

int SocketAPI::ListenServer()
{
	int Error = 0;
	int lisret = listen(sockfd, 5);
	if (lisret < 0)
	{
		Error = -1;
	}
	return Error;

}

int SocketAPI::AcceptServer()
{
	int Error = 0;
	this->newsockfd = accept(this->sockfd, (struct sockaddr*)&this->cli_addr, &this->clientlen);
	
	
	
	if (this->newsockfd < 0)
	{
		Error = -1;
	}
	return Error;
}


int SocketAPI::SendServer(std::string msg, PWSTR m_name)
{
	int Error = 0; 
	//SSIZE_T sndRet = send(this->newsockfd, sizeof(msg.c_str()), msg.length(), 0);
	SSIZE_T sndRet = send(this->newsockfd, msg.c_str(), msg.length(), 0);
	if (sndRet < 0)
	{
		Error = -1;
	}


	return Error;
}


int SocketAPI::ReceiveServer()
{
	int Error = 0;
	memset(buffer, 0, 256);
	// posix костыли:)
	SSIZE_T rdRet = _read(this->newsockfd, buffer, 256);
	if (rdRet < 0)
	{
		Error = -1;
	}
	return Error;
}

void SocketAPI::CloseSocket()
{
	_close(this->sockfd);
}