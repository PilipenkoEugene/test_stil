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
#include <algorithm>
#include <thread>
#include <vector>
#include <chrono>
#include <mutex>
#include <conio.h>

#include "resultType.h"

#define PORT 80

const int KEY_ESC = 27;

using namespace std;

mutex safeDataMTX;
WSADATA wsaData;
SOCKET sock = INVALID_SOCKET;

const char hostReq[] = "ip-api.com";
bool isESCPressed = 0;
char tempBuffer[8192];
vector<char> szBuffer;// (tempBuffer, tempBuffer + result);

bool verify_buf(vector<char> *buff)
{
    if (search(buff->begin(), buff->end(), "HTTP", "HTTP" + strlen("HTTP")) - buff->begin() == 0 && find(buff->begin(), buff->end(), '}') - buff->begin() == buff->size() - 1)
    {
        return 1;
    }
    return 0;
}

void sendDataThread()
{
    // Send data to server
    while (!_kbhit() || _getch() != 27)
    {
        const char szMsg[] = "GET /json/24.48.0.1 HTTP/1.1\r\nHost: ip-api.com\r\nConnection: keep-alive\r\n\r\n";

        if (!send(sock, szMsg, strlen(szMsg), 0)) 
        {
            cout << "������ �������� ������� �� ������. ������ ����� 3 �������.\n" << endl;
        }
        else
        {
            cout << "������ ������� ���������. ����� ������ ����� 3 �������.\n" << endl;
        }
        safeDataMTX.lock();
        while (true)
        {
            int result = recv(sock, tempBuffer, 8192, 0);

            if (result == SOCKET_ERROR)
            {
                cout << "������ ��������� ������. ��� ������:" << WSAGetLastError() << endl;
                safeDataMTX.unlock();
                break;
            }

            if (result == 0)
            {
                cout << "������ ������� ����������." << endl;
                safeDataMTX.unlock();
                return;
            }

            if (result)
                szBuffer.insert(szBuffer.end(), tempBuffer, tempBuffer + sizeof(char) * result / sizeof(tempBuffer[0]));
            
            if (result < 8192 || verify_buf(&szBuffer))
            {
                cout << "������� ������� ����� �� �������.\n" << endl;
                safeDataMTX.unlock();
                break;
            }
        }

        this_thread::sleep_for(chrono::milliseconds(3000));
    }
    isESCPressed = 1;
    return;
}

int main(const int argc, const char* argv[]) {

    setlocale(LC_ALL, "Rus");

    bool status = WSAStartup(MAKEWORD(2, 2), &wsaData);

    if (status)
    {
        cout << "������ ������� WinSock API. ��� ������: " << WSAGetLastError() << endl;
        return 0;
    }
    else 
        cout << "WinSock API ������� ���������������." << endl;

    if (LOBYTE(wsaData.wVersion) != 2 || HIBYTE(wsaData.wVersion) != 2)
    {
        cout << "�� ������� �������� ����������� ������ WinSock API." << endl;
        return 0;
    }
    else 
        cout << "������ WinSock API ���������." << endl;

    sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sock == INVALID_SOCKET) 
    {
        cout << "�� ������� ������� �����" << endl;
        return 0;
    }
    else 
        cout << "����� ������� ������." << endl;

    BOOL val = TRUE;
    int result = setsockopt(sock, IPPROTO_TCP, TCP_NODELAY, (const char*) &val, sizeof(val));
    
    if (result != 0)
    {
        cout << "�� ������� ���������� ��������� ������. ��� ������: " << WSAGetLastError() << endl;
        return 0;
    }
    else
        cout << "��������� ������ ���������." << endl;

    // Get Server info
    HOSTENT* host = gethostbyname(hostReq);
    if (host == nullptr) {
        closesocket(sock);
        cout << "�� ������� �������� ������ � �������. ��� ������: " << WSAGetLastError() << endl;
        getchar();
        return 0;
    }
    else
        cout << "������ � ������� ��������." << endl;

    // Define server info
    SOCKADDR_IN sin;
    ZeroMemory(&sin, sizeof(sin));
    sin.sin_port = htons(PORT);
    sin.sin_family = AF_INET;  
    memcpy(&sin.sin_addr.S_un.S_addr, host->h_addr_list[0], sizeof(sin.sin_addr.S_un.S_addr));

    // Connect to server
    if (connect(sock, (const sockaddr*)&sin, sizeof(sin)) != 0) {
        closesocket(sock);
        cout << "�� ������� ���������� ���������� � ��������." << endl;
        getchar();
        return 0;
    }
    else
        cout << "���������� �����������!" << endl;

    cout << "\n������ ���������� ���������? (y/n)" << endl;

    while (true)
    {
        string ans;
        cin >> ans;

        if (ans == "y" || ans == "Y")
            break;
        else if (ans == "n" || ans == "N")
            return 0;
        else
            cout << "������������ �����. (y/n)" << endl;
    }

    thread sendThread(sendDataThread);
    sendThread.detach();
    
    while (true) {
        if (isESCPressed)
            break;

        if (szBuffer.size()) {
            safeDataMTX.lock();
            for (int i = 0; i < szBuffer.size(); i++)
            {
                cout << szBuffer[i];
            }
            cout << "\n\n===============================\n�������� ����� ������ � ������.\n===============================\n" << endl;

            szBuffer.clear();
            safeDataMTX.unlock();
        }
    }

    closesocket(sock);

    cout << "����� ���������� ������ ������� ��������." << endl;
    cout << "�������� ����� ��������� ��������." << endl;

    return 0;
}