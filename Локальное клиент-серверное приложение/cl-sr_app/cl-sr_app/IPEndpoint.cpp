#include "IPEndpoint.h"
#include "IPVersion.h"

#include <WS2tcpip.h>
#include <iostream>
#include <assert.h>

using namespace std;

IPEndpoint::IPEndpoint(const char* ip, unsigned short port)
{
	this->port = port;
	in_addr addr; 

	int result = inet_pton(AF_INET, ip, &addr);

	if (result == 1)
	{
		if (addr.S_un.S_addr != INADDR_NONE)
		{
			ip_string = ip;
			hostname = ip;
			ipversion = IPVersion::IPv4;
			ip_bytes.resize(sizeof(ULONG));
			memcpy(&ip_bytes[0], &addr.S_un.S_addr, sizeof(ULONG));

			ipversion = IPVersion::IPv4;
			return;
		}
	}

	addrinfo hints = {};
	hints.ai_family = AF_INET;
	addrinfo* hostinfo = nullptr;
	result = getaddrinfo(ip, NULL, &hints, &hostinfo);
	cout << result << endl;

	if (result == 0)
	{
		
		sockaddr_in* host_addr = reinterpret_cast<sockaddr_in*>(hostinfo->ai_addr);

		//host_addr->sin_addr.S_un.S_addr
		ip_string.resize(16);
		inet_ntop(AF_INET, &host_addr->sin_addr, &ip_string[0], 16);

		hostname = ip; 

		ULONG ip_long = host_addr->sin_addr.S_un.S_addr;
		ip_bytes.resize(sizeof(ULONG));
		memcpy(&ip_bytes[0], &ip_long, sizeof(ULONG));

		ipversion = IPVersion::IPv4;

		freeaddrinfo(hostinfo);

		return;
	}
}

cl_sr::IPEndpoint::IPEndpoint(sockaddr* addr)
{
	assert(addr->sa_family == AF_INET);
	sockaddr_in* addrv4 = reinterpret_cast<sockaddr_in*>(addr);	
	ipversion = IPVersion::IPv4;
	port = ntohs(addrv4->sin_port);
	ip_bytes.resize(sizeof(ULONG));
	memcpy(&ip_bytes[0], &addrv4->sin_addr, sizeof(ULONG));

	ip_string.resize(16);
	inet_ntop(AF_INET, &addrv4->sin_addr, &ip_string[0], 16);

	hostname = ip_string;

}

IPVersion IPEndpoint::GetIPVersion()
{
	return ipversion;
}

vector<uint8_t> IPEndpoint::GetIPBytes()
{
	return ip_bytes;
}

string cl_sr::IPEndpoint::getHostName()
{
	return hostname;
}

string IPEndpoint::getIPString()
{
	return ip_string;
}

unsigned short IPEndpoint::GetPort()
{
	return port;
}

sockaddr_in cl_sr::IPEndpoint::GetSockAddrIPv4()
{
	assert(ipversion == IPVersion::IPv4);
	sockaddr_in addr = {};
	addr.sin_family = AF_INET;
	memcpy(&addr.sin_addr, &ip_bytes[0], sizeof(ULONG));
	addr.sin_port = htons(port);

	return addr;
}

void cl_sr::IPEndpoint::Print()
{
	switch (ipversion)
	{
	case IPVersion::IPv4:
		cout << "Используется IPv4" << endl;
		break;
	case IPVersion::IPv6:
		cout << "Используется IPv6" << endl;
		break;
	default:
		cout << "Неизвестный формат адреса." << endl;
	}
	
	cout << "Hostname: " << hostname << endl;
	cout << "IP: " << ip_string << endl;
	cout << "Port: " << port << endl;
	cout << "IP bytes: " << endl;

	for (auto digit : ip_bytes)
	{
		cout << (int)digit << endl;
	}
}
