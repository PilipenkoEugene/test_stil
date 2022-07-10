#include "Socket.h"
#include <assert.h>
#include <iostream>

using namespace cl_sr;
using namespace std;

cl_sr::Socket::Socket(IPVersion ipVersion, SocketHandle handle)
    :ipVersion(ipVersion), handle(handle)
{
    assert(ipVersion == IPVersion::IPv4);
}

cl_srResult cl_sr::Socket::Create()
{
    assert(ipVersion == IPVersion::IPv4);

    if (handle != INVALID_SOCKET)
    {
        return cl_srResult::NotYetImplemented;
    }

    handle = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    if (handle == INVALID_SOCKET)
    {
        int error = WSAGetLastError();
        return cl_srResult::NotYetImplemented;
    }

    if (SetSocketOption(SocketOption::TCP_NoDelay, TRUE) != cl_srResult::Success)
    {
        return cl_srResult::NotYetImplemented;
    }

    return cl_srResult::Success;
}

cl_srResult cl_sr::Socket::Close()
{
    if (handle == INVALID_SOCKET)
    {
        return cl_srResult::NotYetImplemented;
    }

    int result = closesocket(handle);

    if (result != 0)
    {
        int error = WSAGetLastError();
        return cl_srResult::NotYetImplemented;
    }

    handle = INVALID_SOCKET;
    return cl_srResult::Success;
}

cl_srResult cl_sr::Socket::Bind(IPEndpoint endpoint)
{
    sockaddr_in addr = endpoint.GetSockAddrIPv4();
    int result = bind(handle, (sockaddr*)(&addr), sizeof(sockaddr_in));
    if (result != 0)
    {
        int error = WSAGetLastError();
        return cl_srResult::NotYetImplemented;
    }
    return cl_srResult::Success;
}

cl_srResult cl_sr::Socket::Listen(IPEndpoint endpoint, int backlog)
{
    if (Bind(endpoint) != cl_srResult::Success)
    {
        return cl_srResult::NotYetImplemented;
    }

    int result = listen(handle, backlog);

    if (result != 0)
    {
        int error = WSAGetLastError();
        return cl_srResult::NotYetImplemented;
    }

    return cl_srResult::Success;
}

cl_srResult cl_sr::Socket::Accept(Socket& outSocket)
{
    sockaddr_in addr = {};
    int len = sizeof(sockaddr_in);

    SocketHandle acceptedConnectionHandle = accept(handle, (sockaddr*)&addr, &len);
    if (acceptedConnectionHandle == INVALID_SOCKET)
    {
        int error = WSAGetLastError();
        return cl_srResult::NotYetImplemented;
    }

    cout << "Создано новое подключение." << endl;

    IPEndpoint newConnectionEndPoint((sockaddr*)&addr);
    newConnectionEndPoint.Print();

    outSocket = Socket(IPVersion::IPv4, acceptedConnectionHandle);
    return cl_srResult::Success;
}

cl_srResult cl_sr::Socket::Connect(IPEndpoint endpoint)
{
    sockaddr_in addr = endpoint.GetSockAddrIPv4();
    int result = connect(handle, (sockaddr*) &addr, sizeof(sockaddr_in));
    
    if (result != 0)
    {
        int error = WSAGetLastError();
        return cl_srResult::NotYetImplemented;
    }
    return cl_srResult::Success;
}

cl_srResult cl_sr::Socket::Send(void* data, int numberOfBytes, int& bytesSent)
{
    bytesSent = send(handle, (const char*) data, numberOfBytes, NULL);

    if (bytesSent == SOCKET_ERROR)
    {
        int error = WSAGetLastError();
        return cl_srResult::NotYetImplemented;
    }
    return cl_srResult::Success;
}

cl_srResult cl_sr::Socket::Recv(void* destination, int numberOfBytes, int& bytesRecieved)
{
    bytesRecieved = recv(handle, (char*)destination, numberOfBytes, NULL);

    if (bytesRecieved == 0)
    {
        return cl_srResult::NotYetImplemented;
    }

    if (bytesRecieved == SOCKET_ERROR)
    {
        int error = WSAGetLastError();
        return cl_srResult::NotYetImplemented;
    }

    return cl_srResult::Success;
}

SocketHandle cl_sr::Socket::GetHandle()
{
    return handle;
}

IPVersion cl_sr::Socket::GetIPVersin()
{
    return ipVersion;
}

cl_srResult cl_sr::Socket::SetSocketOption(SocketOption option, BOOL value)
{
    int result = 0;
    switch (option)
    {
    case SocketOption::TCP_NoDelay:
        result = setsockopt(handle, IPPROTO_TCP, TCP_NODELAY, (const char*) &value, sizeof(value));
        break;
    default:
        return cl_srResult::NotYetImplemented;
    }

    if (result != 0)
    {
        int error = WSAGetLastError();
        return cl_srResult::NotYetImplemented;
    }
    return cl_srResult::Success;
}
