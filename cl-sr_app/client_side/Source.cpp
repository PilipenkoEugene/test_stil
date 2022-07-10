// client_side.cpp : This file contains the 'main' function. Program execution begins and ends there.

#include <cl-sr_app\pr_lib_cl-sr.h>
#include <iostream>

using namespace cl_sr;
using namespace std;

int main()
{
	setlocale(LC_ALL, "Rus");

	if (Network::Init())
	{
		cout << "WinSock API инициализирован" << endl;
		Socket socket;

		if (socket.Create() == cl_srResult::Success)
		{
			cout << "Сокет успешно создан" << endl;
			if (socket.Connect(IPEndpoint("127.0.0.1", 4790)) == cl_srResult::Success)
			{
				cout << "Подключение к серверу успешно завершено." << endl;
				char buffer[256];
				strcpy_s(buffer, "New message from client side.\0");
				int bytesSent = 0;
				int result = cl_srResult::Success;

				while (result == cl_srResult::Success)
				{
					result = socket.Send(buffer, 256, bytesSent);
					cout << "Попытка отправки данных..." << endl;
					Sleep(500);
				}
				cout << "Данные успешно отправлены" << endl;

			}
			else
			{
				cerr << "Не удалось выполнить подключение." << endl;
			}
			socket.Close();
		}
		else
		{
			cerr << "Ошибка создания сокета" << endl;
		}
	}
	Network::Shutdown();

	system("pause");
	return 0;
}