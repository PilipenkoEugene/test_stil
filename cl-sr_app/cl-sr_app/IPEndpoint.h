#pragma once
#include "IPVersion.h"
#include <string>
#include <vector>

using namespace std;
using namespace cl_sr;

namespace cl_sr
{
	class IPEndpoint
	{
	public:
		IPEndpoint(const char* ip, unsigned short port);
		IPEndpoint(sockaddr* addr);
		IPVersion GetIPVersion();
		vector<uint8_t> GetIPBytes();
		string getHostName();
		string getIPString();
		unsigned short GetPort();
		sockaddr_in GetSockAddrIPv4();
		void Print();
	private:
		IPVersion ipversion = IPVersion::Unknown;
		string hostname = "";
		string ip_string = "";
		unsigned short port = 0;
		vector<uint8_t> ip_bytes;
	};
}