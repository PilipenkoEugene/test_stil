#include "network.h"
#include <iostream>

using namespace std;

bool cl_sr::Network::Init()
{
	WSAData wsaData;

	bool status = WSAStartup(MAKEWORD(2, 2), &wsaData);

	if (status != 0)
	{
		cerr << "Не удалось запустить WinSock API" << endl;
		return false;
	}
	 
	if (LOBYTE(wsaData.wVersion) != 2 || HIBYTE(wsaData.wVersion) != 2)
	{
		cerr << "Не удалось получить необходимую версию WinSock API dll" << endl;
		return false;
	}

	return true;
}

void cl_sr::Network::Shutdown()
{
	WSACleanup();
}
