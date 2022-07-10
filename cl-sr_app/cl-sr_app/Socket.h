#pragma once

#define WIN32_LEAN_AND_MEAN

#include <WinSock2.h>
#include "SocketHandle.h"
#include "IPVersion.h"
#include "cl_sr_result.h"
#include "SocketOptions.h"
#include "IPEndpoint.h"

namespace cl_sr
{
	class Socket
	{
	public:
		Socket(IPVersion ipVersion = IPVersion::IPv4,
			   SocketHandle handle = INVALID_SOCKET);

		cl_srResult Create();
		cl_srResult Close();
		cl_srResult Bind(IPEndpoint endpoint);
		cl_srResult Listen(IPEndpoint endpoint, int backlog = 5);
		cl_srResult Accept(Socket& outSocket);
		cl_srResult Connect(IPEndpoint endpoint);
		cl_srResult Send(void* data, int numberOfBytes, int& bytesSent);
		cl_srResult Recv(void* destination, int numberOfBytes, int& bytesRecieved);
		SocketHandle GetHandle();
		IPVersion GetIPVersin();

	private:
		cl_srResult SetSocketOption(SocketOption option, BOOL value);
		IPVersion ipVersion = IPVersion::IPv4;
		SocketHandle handle = INVALID_SOCKET;
	};
}