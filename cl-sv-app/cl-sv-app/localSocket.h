#pragma once

#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#define WIN32_LEAN_AND_MEAN

#pragma comment(lib, "ws2_32.lib")

#include <WinSock2.h>
#include <stdio.h>
#include <stdlib.h>
#include <Windows.h>
#include <WS2tcpip.h>
#include <iostream>

#include "resultType.h"

#define PORT 80

using namespace std;

class LocalSocket
{
public:
	LocalSocket(WSADATA wsaData, SOCKET sock);
	static resultType InitWSA();
	SOCKET createSocket();
	WSAData getWSAData();
	static void Shutdown();

private:
	WSAData wsaDate;
	SOCKET sock = INVALID_SOCKET;
};