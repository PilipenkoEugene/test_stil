//#include "localSocket.h"
//
//LocalSocket::LocalSocket(WSADATA wsaData, SOCKET sock)
//    :wsaData(wsaData), sock(sock)
//{
//}
//
//resultType LocalSocket::InitWSA()
//{
//    bool status = WSAStartup(MAKEWORD(2, 2), &wsaData);
//
//    if (status)
//    {
//        cout << "������ ������� WinSock API. ��� ������: " << WSAGetLastError() << endl;
//        return resultType::Failed;
//    }
//
//    if (LOBYTE(wsaData.wVersion) != 2 || HIBYTE(wsaData.wVersion) != 2)
//    {
//        cout << "�� ������� �������� ����������� ������ WinSock API." << endl;
//        return resultType::Failed;
//    }
//
//    return resultType::Success;
//}
//
//SOCKET LocalSocket::createSocket()
//{
//    SOCKET sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
//    if (sock < 0) {
//        cout << "�� ������� ������� �����" << endl;
//        return INVALID_SOCKET;
//    }
//    return sock;
//}
//
//WSAData LocalSocket::getWSAData()
//{
//    return wsaData;
//}
//
//void LocalSocket::Shutdown()
//{
//    WSACleanup();
//}
