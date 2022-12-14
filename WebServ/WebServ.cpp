#include <stdio.h>
#include <strstream>
#include <string>
#ifndef LINUX_MODE
#include <winsock2.h>
#endif


#include "pserver.h"
#include "procesatramas.h"
#include "Area81.h"

void ClearLog()
{
	DeleteFile("WebServ.log");
}

int main()
{
#ifndef LINUX_MODE
	WSADATA wsaData;
	if (WSAStartup(MAKEWORD(2, 1), &wsaData) != 0)
	{
		printf("Error WSAStartup\n");
		return 0;
	}
#endif

	ClearLog();

	CServer server;
	server.AddTcp<CArea81>(8080);
	if (!server.WaitAllProcessUP(5000))
	{
		printf("Error!\n");
		exit(0);
	}
	while (1)
	{
		Sleep(10);
	}
    return 0;
}

