// server_side.cpp : This file contains the 'main' function. Program execution begins and ends there.

#include <cl-sr_app\pr_lib_cl-sr.h>
#include <iostream>

using namespace cl_sr;
using namespace std;

int main()
{
	setlocale(LC_ALL, "Rus");

	//setsockopt();

	if (Network::Init())
	{
		cout << "WinSock API ���������������" << endl;
		Socket socket;
	
		if(socket.Create() == cl_srResult::Success)
		{
			cout << "����� ������� ������" << endl;

			if (socket.Listen(IPEndpoint("127.0.0.1", 4790)) == cl_srResult::Success)
			{
				cout << "����� ������� ������ � �������. ���� 4790." << endl;

				Socket newConnection;
				if (socket.Accept(newConnection) == cl_srResult::Success)
				{
					cout << "����� ���������� �����������." << endl;

					char buffer[256];
					int bytesReceived = 0;
					int result = cl_srResult::Success;

					while (result == cl_srResult::Success)
					{
						result = newConnection.Recv(buffer, 256, bytesReceived);
						if (result != cl_srResult::Success)
						{
							cout << "������ ��������� ������" << endl;
							break;
						}
						cout << "�������� ���������: " << buffer << endl;
					}

					newConnection.Close();
				}
				else
				{
					int error = WSAGetLastError();
					cerr << "������ �������� ������ �����������." << endl;
				}
			}
			else
			{
				cerr << "������ ������������� ������. ���� 4790." << endl;
			}
			socket.Close();
		}
		else
		{	
			cerr << "������ �������� ������" << endl;
		}
	}

	/*IPEndpoint test("www.google.com", 8080);
	if (test.GetIPVersion() == IPVersion::IPv4)
	{
		cout << "Hostname: " << test.getHostName() << endl;
		cout << "IPv4: " << test.getIPString() << endl;
		cout << "Port: " << test.GetPort() << endl;
		cout << "IP bytes..." << endl;
		for (auto& digit : test.GetIPBytes())
		{
			cout << (int)digit << endl;
		}
	}
	else
	{
		cerr << "����� ����� �� � ������� IPv4" << endl;
	}*/

	Network::Shutdown();
	system("pause");
	return 0;
} 