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

//Данные для сетевой и поточной части прогрыммы
mutex safeDataMTX;
WSADATA wsaData;
SOCKET sock = INVALID_SOCKET;
const char hostReq[] = "ip-api.com";
char tempBuffer[8192];
//__________________________________________

//Данные, позволяющие корректно завершать программу и считывать данные
bool isESCPressed = 0; //Статус нажатия клавиши ESC
vector<char> szBuffer; //Буфер, хранящий ответ сервера при посимвольном представлении 
string arrMessage; //Промежуточное хранилище байт информации при предствлении вывода в виде единого массива байт
vector<string> messageVector; //Вектор, хранящий полное сообщение ответа сервера
//__________________________________________

//Функция верификации буфера на соответствие формату итогового сообщения. Используется при получении ответа, 
//размер которого равен размеру буфера для исключения получения неполного сообщения.
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

//Функция верификации строки на соответствие формату итогового сообщения. Используется при получении ответа, 
//размер которого равен размеру буфера для исключения получения неполного сообщения.
bool verify_string(string *s)
{
    if (s->find("HTTP") == 0 && s->find('}') == s->size() - 1)
    {
        return 1;
    }
    return 0;
}
//__________________________________________

//Реализация потока, который выполняет переодические запросы на сервер 
void sendDataThread()
{
    //Основной код потока выполняется до нажатия на клавишу ESC
    while (!_kbhit() || _getch() != KEY_ESC)
    {
        //Текст HTML-запроса на сервер
        const char szMsg[] = "GET /json/24.48.0.1 HTTP/1.1\r\nHost: ip-api.com\r\nConnection: keep-alive\r\n\r\n";
        //__________________________________________

        //Отправка сообщения и проверка на успех операции
        if (!send(sock, szMsg, strlen(szMsg), 0)) 
        {
            cout << "Ошибка отправки запроса на сервер. Повтор через 3 секунды.\n" << endl;
        }
        else
        {
            cout << "Запрос успешно отправлен. Новый запрос через 3 секунды.\n" << endl;
        }
        //__________________________________________

        //Чтение ответа и перезапись результатов. Использования mutex'а защищает данные от использования в 
        //момент чтения и перезаписи.
        safeDataMTX.lock();
        while (true)
        {
            //Чтение данных из сокета
            int result = recv(sock, tempBuffer, 8192, 0);
            //__________________________________________

            //Блок обработки возможных исключений
            if (result == SOCKET_ERROR)
            {
                cout << "Ошибка получения пакета. Код ошибки:" << WSAGetLastError() << endl;
                safeDataMTX.unlock();
                break;
            }

            if (result == 0)
            {
                cout << "Сервер оборвал соединение." << endl;
                safeDataMTX.unlock();
                return;
            }
            //__________________________________________
            
            //Блок записи результата в требуемом формате
            if (result && outputType == resultType::vecString)
                arrMessage += tempBuffer;

            if (result && outputType == resultType::vecChar)
                szBuffer.insert(szBuffer.end(), tempBuffer, tempBuffer + sizeof(char) * result / sizeof(tempBuffer[0]));
            //__________________________________________

            //Верификация полученного сообщения. В случае получения конца сообщения выполнение программы продолжиться,
            //иначе будет продолжено считывание потока данных. Происходит разблокировка mutex'а
            if (result < 8192 || (outputType == resultType::vecChar ? verify_buf(&szBuffer) : verify_string(&arrMessage)))
            {   
                //Запись полного ответа сервера в вектор. Элемент является массивом байт данных.
                if (outputType == resultType::vecString)
                    messageVector.push_back(arrMessage);
                //__________________________________________

                cout << "Успешно получен ответ от сервера.\n" << endl;
                safeDataMTX.unlock();
                break;
            }
            //__________________________________________
        }
        //API ограничивает количество запросов до 45 в минуту, поэтому установлена пауза с запасом для 
        //чтения данных с экрана.
        this_thread::sleep_for(chrono::milliseconds(3000));
        //__________________________________________
    }
    //Выход из цикла означает нажатия на ESC и выход из программы, поэтому устанавливается флаг,
    //указывающий на необходимость завершения.
    isESCPressed = 1;
    //__________________________________________

    return;
}

int main(const int argc, const char* argv[]) {

    //Локализация для корректного отображения текста в консоли
    setlocale(LC_ALL, "Rus");
    //__________________________________________

    //Блок ожидания команды на выполнение программы
    cout << "Начать выполнение программы? (y/n)\n" << endl;

    while (true)
    {
        string ans;
        cin >> ans;

        if (ans == "y" || ans == "Y")
            break;
        else if (ans == "n" || ans == "N")
            return 0;
        else
            cout << "Некорректный ответ. (y/n)" << endl;
    }
    //__________________________________________

    //Блок выбора формата предоставления итогового формата запроса
    cout << "\nВыберите тип выходного значения.\n\n1. vector<char> = ['H', 'T', 'T', 'P', ...]\n2. vector<string> = [\"HTTP...\", ...]\n" << endl;

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
            cout << "Неверное значение. Повторите попытку. (1/2)\n" << endl;
            continue;
        }

        break;
    }
    //__________________________________________

    //Инициализация WinSock API
    bool status = WSAStartup(MAKEWORD(2, 2), &wsaData);

    if (status)
    {
        cout << "\nОшибка запуска WinSock API. Код ошибки: " << WSAGetLastError() << endl;
        return 0;
    }
    else
        cout << "\nWinSock API успешно инициализирован." << endl;
    //__________________________________________

    //Проверка версии запрошенного WSA
    if (LOBYTE(wsaData.wVersion) != 2 || HIBYTE(wsaData.wVersion) != 2)
    {
        cout << "Не удалось получить необходимую версию WinSock API." << endl;
        return 0;
    }
    else
        cout << "Версия WinSock API актуальна." << endl;
    //__________________________________________

    //Блок создания сокета 
    sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sock == INVALID_SOCKET)
    {
        cout << "Не удалось создать сокет" << endl;
        return 0;
    }
    else
        cout << "Сокет успешно создан." << endl;
    //__________________________________________

    //Блок установки параметров сокета. Отключается алгоритм Нейгла для ускорения отправки пакетов программой
    //Перегружаем сеть, но нам ведь важна любая скорость :)
    BOOL val = TRUE;
    int result = setsockopt(sock, IPPROTO_TCP, TCP_NODELAY, (const char*)&val, sizeof(val));

    if (result != 0)
    {
        cout << "Не удалось установить настройки сокета. Код ошибки: " << WSAGetLastError() << endl;
        return 0;
    }
    else
        cout << "Настройки сокеты применены." << endl;
    //__________________________________________

    //Блок запроса информации о сервере
    HOSTENT* host = gethostbyname(hostReq);
    if (host == nullptr) {
        closesocket(sock);
        cout << "Не удалось получить данные о сервере. Код ошибки: " << WSAGetLastError() << endl;
        getchar();
        return 0;
    }
    else
        cout << "Данные о сервере получены." << endl;
    //__________________________________________

    //Блок задания информации о сервере
    SOCKADDR_IN sin;
    ZeroMemory(&sin, sizeof(sin));
    sin.sin_port = htons(PORT);
    sin.sin_family = AF_INET;
    memcpy(&sin.sin_addr.S_un.S_addr, host->h_addr_list[0], sizeof(sin.sin_addr.S_un.S_addr));
    //__________________________________________

    //Блок установки соединения с сервером
    if (connect(sock, (const sockaddr*)&sin, sizeof(sin)) != 0) {
        closesocket(sock);
        cout << "Не удалось установить соединение с сервером.\n" << endl;
        getchar();
        return 0;
    }
    else
        cout << "Соединение установлено!\n" << endl;
    //__________________________________________

    //Пауза для задержки отображения результатов инициализации всех параметров на экране
    this_thread::sleep_for(chrono::milliseconds(500));
    //__________________________________________

    //Запуск второго потока. Так как нам не важен какой-либо результат, а потом корректно завершается сам,
    //отпускаем поток detach'ем в свободное плавание
    thread sendThread(sendDataThread);
    sendThread.detach();
    //__________________________________________

    //Основной поток программы, постоянно проверяющий считанные из сокета данные
    while (true) {
        
        //Блок завершения работы по нажатию клавищи ESC
        if (isESCPressed)
            break;
        //__________________________________________
       
        //Блок вывода полного сообщения в консоль. Выводится результат, записанный в выбранном формате.
        //Для избежания переполнения памяти выполняется очистка хранилищ после вывода в соответствии с ТЗ
        //С помощью mutex'а данные защищаются от изменения во время вывода.
        if (outputType == resultType::vecChar && szBuffer.size()) {
            safeDataMTX.lock();
            for (int i = 0; i < szBuffer.size(); i++)
            {
                cout << szBuffer[i];
            }
            cout << "\n\n===============================\nОжидание новых данных в буфере.\n===============================\n" << endl;
            szBuffer.clear();
            safeDataMTX.unlock();
        }
        else if (messageVector.size())
        {
            safeDataMTX.lock();
            cout << messageVector[0] << endl << endl;
            cout << "\n\n===============================\nОжидание новых данных в буфере.\n===============================\n" << endl;
            arrMessage.clear();
            messageVector.clear();
            safeDataMTX.unlock();
        }
        //__________________________________________
    }
    //__________________________________________

    //Корректное закрытие сокета после выполнения прораммы
    closesocket(sock);
    //__________________________________________

    cout << "Поток считывания данных успешно завершен." << endl;
    cout << "Основной поток корректно завершен." << endl;

    return 0;
}