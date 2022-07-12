#pragma once

#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#define WIN32_LEAN_AND_MEAN

#pragma comment(lib, "ws2_32.lib")

#include <WinSock2.h>
#include <stdio.h>
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

resultType outputType = resultType::Unknown;

using namespace std;

//������ ��� ������� � �������� ����� ���������
mutex safeDataMTX;
WSADATA wsaData;
SOCKET sock = INVALID_SOCKET;
const char hostReq[] = "ip-api.com";
char tempBuffer[8192];
//__________________________________________

//������, ����������� ��������� ��������� ��������� � ��������� ������
bool isESCPressed = 0; //������ ������� ������� ESC
vector<char> szBuffer; //�����, �������� ����� ������� ��� ������������ ������������� 
string arrMessage; //������������� ��������� ���� ���������� ��� ������������ ������ � ���� ������� ������� ����
vector<string> messageVector; //������, �������� ������ ��������� ������ �������
//__________________________________________

//������� ����������� ������ �� ������������ ������� ��������� ���������. ������������ ��� ��������� ������, 
//������ �������� ����� ������� ������ ��� ���������� ��������� ��������� ���������.
bool verify_buf(vector<char>* buff)
{
    if (search(buff->begin(), buff->end(), "HTTP", "HTTP" + strlen("HTTP")) - buff->begin() == 0 && 
        search(buff->begin(), buff->end(), "}", "}" + strlen("}")) - buff->begin() == buff->size() - 1)
    {
        return 1;
    }
    
    return 0;
}
//__________________________________________

//������� ����������� ������ �� ������������ ������� ��������� ���������. ������������ ��� ��������� ������, 
//������ �������� ����� ������� ������ ��� ���������� ��������� ��������� ���������.
bool verify_string(string *s)
{
    if (s->find("HTTP") == 0 && s->find('}') == s->size() - 1)
    {
        return 1;
    }
    return 0;
}
//__________________________________________

//���������� ������, ������� ��������� ������������� ������� �� ������ 
void sendDataThread()
{
    //�������� ��� ������ ����������� �� ������� �� ������� ESC
    while (!_kbhit() || _getch() != KEY_ESC)
    {
        //����� HTML-������� �� ������
        const char szMsg[] = "GET /json/24.48.0.1 HTTP/1.1\r\nHost: ip-api.com\r\nConnection: keep-alive\r\n\r\n";
        //__________________________________________

        //�������� ��������� � �������� �� ����� ��������
        if (!send(sock, szMsg, strlen(szMsg), 0)) 
        {
            cout << "������ �������� ������� �� ������. ������ ����� 3 �������.\n" << endl;
        }
        else
        {
            cout << "������ ������� ���������. ����� ������ ����� 3 �������.\n" << endl;
        }
        //__________________________________________

        //������ ������ � ���������� �����������. ������������� mutex'� �������� ������ �� ������������� � 
        //������ ������ � ����������.
        safeDataMTX.lock();
        while (true)
        {
            //������ ������ �� ������
            int result = recv(sock, tempBuffer, 8192, 0);
            //__________________________________________

            //���� ��������� ��������� ����������
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
            //__________________________________________
            
            //���� ������ ���������� � ��������� �������
            if (result && outputType == resultType::vecString)
                arrMessage += tempBuffer;

            if (result && outputType == resultType::vecChar)
                szBuffer.insert(szBuffer.end(), tempBuffer, tempBuffer + sizeof(char) * result / sizeof(tempBuffer[0]));
            //__________________________________________

            //����������� ����������� ���������. � ������ ��������� ����� ��������� ���������� ��������� ������������,
            //����� ����� ���������� ���������� ������ ������. ���������� ������������� mutex'�
            if (result < 8192 || (outputType == resultType::vecChar ? verify_buf(&szBuffer) : verify_string(&arrMessage)))
            {   
                //������ ������� ������ ������� � ������. ������� �������� �������� ���� ������.
                if (outputType == resultType::vecString)
                    messageVector.push_back(arrMessage);
                //__________________________________________

                cout << "������� ������� ����� �� �������.\n" << endl;
                safeDataMTX.unlock();
                break;
            }
            //__________________________________________
        }
        //API ������������ ���������� �������� �� 45 � ������, ������� ����������� ����� � ������� ��� 
        //������ ������ � ������.
        this_thread::sleep_for(chrono::milliseconds(3000));
        //__________________________________________
    }
    //����� �� ����� �������� ������� �� ESC � ����� �� ���������, ������� ��������������� ����,
    //����������� �� ������������� ����������.
    isESCPressed = 1;
    //__________________________________________

    return;
}

int main(const int argc, const char* argv[]) {

    //����������� ��� ����������� ����������� ������ � �������
    setlocale(LC_ALL, "Rus");
    //__________________________________________

    //���� �������� ������� �� ���������� ���������
    cout << "������ ���������� ���������? (y/n)\n" << endl;

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
    //__________________________________________

    //���� ������ ������� �������������� ��������� ������� �������
    cout << "\n�������� ��� ��������� ��������.\n\n1. vector<char> = ['H', 'T', 'T', 'P', ...]\n2. vector<string> = [\"HTTP...\", ...]\n" << endl;

    while (true)
    {
        int a;
        cin >> a;

        if (a == 1)
            outputType = resultType::vecChar;
        else if (a == 2)
        {
            outputType = resultType::vecString;
            arrMessage.clear();
        }
        else
        {
            cout << "�������� ��������. ��������� �������. (1/2)\n" << endl;
            continue;
        }

        break;
    }
    //__________________________________________

    //������������� WinSock API
    bool status = WSAStartup(MAKEWORD(2, 2), &wsaData);

    if (status)
    {
        cout << "\n������ ������� WinSock API. ��� ������: " << WSAGetLastError() << endl;
        return 0;
    }
    else
        cout << "\nWinSock API ������� ���������������." << endl;
    //__________________________________________

    //�������� ������ ������������ WSA
    if (LOBYTE(wsaData.wVersion) != 2 || HIBYTE(wsaData.wVersion) != 2)
    {
        cout << "�� ������� �������� ����������� ������ WinSock API." << endl;
        return 0;
    }
    else
        cout << "������ WinSock API ���������." << endl;
    //__________________________________________

    //���� �������� ������ 
    sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sock == INVALID_SOCKET)
    {
        cout << "�� ������� ������� �����" << endl;
        return 0;
    }
    else
        cout << "����� ������� ������." << endl;
    //__________________________________________

    //���� ��������� ���������� ������. ����������� �������� ������ ��� ��������� �������� ������� ����������
    //����������� ����, �� ��� ���� ����� ����� �������� :)
    BOOL val = TRUE;
    int result = setsockopt(sock, IPPROTO_TCP, TCP_NODELAY, (const char*)&val, sizeof(val));

    if (result != 0)
    {
        cout << "�� ������� ���������� ��������� ������. ��� ������: " << WSAGetLastError() << endl;
        return 0;
    }
    else
        cout << "��������� ������ ���������." << endl;
    //__________________________________________

    //���� ������� ���������� � �������
    HOSTENT* host = gethostbyname(hostReq);
    if (host == nullptr) {
        closesocket(sock);
        cout << "�� ������� �������� ������ � �������. ��� ������: " << WSAGetLastError() << endl;
        getchar();
        return 0;
    }
    else
        cout << "������ � ������� ��������." << endl;
    //__________________________________________

    //���� ������� ���������� � �������
    SOCKADDR_IN sin;
    ZeroMemory(&sin, sizeof(sin));
    sin.sin_port = htons(PORT);
    sin.sin_family = AF_INET;
    memcpy(&sin.sin_addr.S_un.S_addr, host->h_addr_list[0], sizeof(sin.sin_addr.S_un.S_addr));
    //__________________________________________

    //���� ��������� ���������� � ��������
    if (connect(sock, (const sockaddr*)&sin, sizeof(sin)) != 0) {
        closesocket(sock);
        cout << "�� ������� ���������� ���������� � ��������.\n" << endl;
        getchar();
        return 0;
    }
    else
        cout << "���������� �����������!\n" << endl;
    //__________________________________________

    //����� ��� �������� ����������� ����������� ������������� ���� ���������� �� ������
    this_thread::sleep_for(chrono::milliseconds(500));
    //__________________________________________

    //������ ������� ������. ��� ��� ��� �� ����� �����-���� ���������, � ����� ��������� ����������� ���,
    //��������� ����� detach'�� � ��������� ��������
    thread sendThread(sendDataThread);
    sendThread.detach();
    //__________________________________________

    //�������� ����� ���������, ��������� ����������� ��������� �� ������ ������
    while (true) {
        
        //���� ���������� ������ �� ������� ������� ESC
        if (isESCPressed)
            break;
        //__________________________________________
       
        //���� ������ ������� ��������� � �������. ��������� ���������, ���������� � ��������� �������.
        //��� ��������� ������������ ������ ����������� ������� �������� ����� ������ � ������������ � ��
        //� ������� mutex'� ������ ���������� �� ��������� �� ����� ������.
        if (outputType == resultType::vecChar && szBuffer.size()) {
            safeDataMTX.lock();
            for (int i = 0; i < szBuffer.size(); i++)
            {
                cout << szBuffer[i];
            }
            cout << "\n\n===============================\n�������� ����� ������ � ������.\n===============================\n" << endl;
            szBuffer.clear();
            safeDataMTX.unlock();
        }
        else if (messageVector.size())
        {
            safeDataMTX.lock();
            cout << messageVector[0] << endl << endl;
            cout << "\n\n===============================\n�������� ����� ������ � ������.\n===============================\n" << endl;
            arrMessage.clear();
            messageVector.clear();
            safeDataMTX.unlock();
        }
        //__________________________________________
    }
    //__________________________________________

    //���������� �������� ������ ����� ���������� ��������
    closesocket(sock);
    //__________________________________________

    cout << "����� ���������� ������ ������� ��������." << endl;
    cout << "�������� ����� ��������� ��������." << endl;

    return 0;
}