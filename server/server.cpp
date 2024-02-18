#include <iostream>
#include <windows.h>
#include <ws2tcpip.h>
#include <string>
#include <vector>

#pragma comment (lib, "Ws2_32.lib")

#define DEFAULT_BUFLEN 512
#define DEFAULT_PORT "27015"
#define PAUSE 1000

using namespace std;

int main() {
    setlocale(0, "");
    system("title SERVER SIDE");
    cout << "процесс сервера запущен!\n";
    Sleep(PAUSE);

    WSADATA wsaData;
    int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (iResult != 0) {
        cout << "WSAStartup failed with error: " << iResult << "\n";
        return 1;
    }
    else {
        cout << "подключение Winsock.dll прошло успешно!\n";
        Sleep(PAUSE);
    }

    struct addrinfo hints;
    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    hints.ai_flags = AI_PASSIVE;

    struct addrinfo* result = NULL;
    iResult = getaddrinfo(NULL, DEFAULT_PORT, &hints, &result);
    if (iResult != 0) {
        cout << "getaddrinfo failed with error: " << iResult << "\n";
        WSACleanup();
        return 2;
    }
    else {
        cout << "получение адреса и порта сервера прошло успешно!\n";
        Sleep(PAUSE);
    }

    SOCKET ListenSocket = INVALID_SOCKET;
    ListenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
    if (ListenSocket == INVALID_SOCKET) {
        cout << "socket failed with error: " << WSAGetLastError() << "\n";
        freeaddrinfo(result);
        WSACleanup();
        return 3;
    }
    else {
        cout << "создание сокета на сервере прошло успешно!\n";
        Sleep(PAUSE);
    }

    iResult = bind(ListenSocket, result->ai_addr, (int)result->ai_addrlen);
    if (iResult == SOCKET_ERROR) {
        cout << "bind failed with error: " << WSAGetLastError() << "\n";
        freeaddrinfo(result);
        closesocket(ListenSocket);
        WSACleanup();
        return 4;
    }
    else {
        cout << "внедрение сокета по IP-адресу прошло успешно!\n";
        Sleep(PAUSE);
    }

    freeaddrinfo(result);

    iResult = listen(ListenSocket, SOMAXCONN);
    if (iResult == SOCKET_ERROR) {
        cout << "listen failed with error: " << WSAGetLastError() << "\n";
        closesocket(ListenSocket);
        WSACleanup();
        return 5;
    }
    else {
        cout << "начинается прослушка информации от клиента. пожалуйста, запустите клиентское приложение! (client.exe)\n";
    }

    SOCKET ClientSocket = INVALID_SOCKET;

    while (true) {
        ClientSocket = accept(ListenSocket, NULL, NULL);
        if (ClientSocket == INVALID_SOCKET) {
            cout << "accept failed with error: " << WSAGetLastError() << "\n";
            closesocket(ListenSocket);
            WSACleanup();
            return 6;
        }
        else {
            cout << "соединение с клиентским приложением установлено успешно!\n";
        }

        char recvbuf[DEFAULT_BUFLEN];
        int recvbuflen = DEFAULT_BUFLEN;

        // Получаем команду от клиента
        iResult = recv(ClientSocket, recvbuf, recvbuflen, 0);
        if (iResult > 0) {
            recvbuf[iResult] = '\0';

            // Обрабатываем команду
            string command(recvbuf);
            string response;

            if (command == "1") {
                response = "Привет!";
            }
            else if (command == "2") {
                response = "Погода отличная!";
            }
            else if (command == "3") {
                response = "Сейчас 12:30.";
            }
            else if (command == "4") {
                response = "До свидания!";
            }
            else if (command == "5") {
                response = "Пока ничего нового";
            }
            else if (command == "6") {
                response = "Почему у кошек много друзей? Потому что они всегда приносят мышиные подарки!";
            }
            else if (command == "7") {
                response = "Результат равен 4.";
            }
            else if (command == "8") {
                response = "Слоны — единственные животные, которые не могут прыгать.";
            }
            else if (command == "9") {
                response = "Конечно! Попробуйте этот рецепт торта: ...";
            }
            else if (command == "10") {
                response = "Выход из программы";
            }
            else {
                response = "Неизвестная команда";
            }

            // Отправляем ответ клиенту
            iResult = send(ClientSocket, response.c_str(), response.length(), 0);
            if (iResult == SOCKET_ERROR) {
                cout << "send failed with error: " << WSAGetLastError() << "\n";
                closesocket(ClientSocket);
                WSACleanup();
                return 7;
            }
            else {
                cout << "Ответ отправлен клиенту: " << response << "\n";
            }
        }
        else if (iResult == 0) {
            cout << "Соединение закрыто клиентом\n";
        }
        else {
            cout << "recv failed with error: " << WSAGetLastError() << "\n";
            closesocket(ClientSocket);
            WSACleanup();
            return 8;
        }
    }

    closesocket(ListenSocket);
    WSACleanup();

    return 0;
}
