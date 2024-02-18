#define WIN32_LEAN_AND_MEAN

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

void printCommands() {
    cout << "Доступные команды:\n";
    cout << "1. Привет\n";
    cout << "2. Как погода?\n";
    cout << "3. Сколько времени?\n";
    cout << "4. Пока\n";
    cout << "5. Что нового?\n";
    cout << "6. Расскажи анекдот\n";
    cout << "7. 2 + 2\n";
    cout << "8. Скажи мне что-нибудь интересное\n";
    cout << "9. Подскажи хороший рецепт\n";
    cout << "10. Выход\n";
}

int main() {
    setlocale(0, "");
    system("title CLIENT SIDE");
    cout << "процесс клиента запущен!\n";
    Sleep(PAUSE);

    printCommands();

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

    struct addrinfo* result = NULL;
    iResult = getaddrinfo("localhost", DEFAULT_PORT, &hints, &result);
    if (iResult != 0) {
        cout << "getaddrinfo failed with error: " << iResult << "\n";
        WSACleanup();
        return 2;
    }
    else {
        cout << "получение адреса и порта сервера прошло успешно!\n";
        Sleep(PAUSE);
    }

    SOCKET ConnectSocket = INVALID_SOCKET;
    ConnectSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
    if (ConnectSocket == INVALID_SOCKET) {
        cout << "socket failed with error: " << WSAGetLastError() << "\n";
        freeaddrinfo(result);
        WSACleanup();
        return 3;
    }
    else {
        cout << "создание сокета на клиенте прошло успешно!\n";
        Sleep(PAUSE);
    }

    iResult = connect(ConnectSocket, result->ai_addr, (int)result->ai_addrlen);
    if (iResult == SOCKET_ERROR) {
        cout << "connect failed with error: " << WSAGetLastError() << "\n";
        closesocket(ConnectSocket);
        ConnectSocket = INVALID_SOCKET;
        WSACleanup();
        return 4;
    }
    else {
        cout << "подключение к серверу прошло успешно!\n";
    }

    freeaddrinfo(result);

    // Отправляем команды на сервер
    string command;
    while (true) {
        cout << "Введите команду (1-10): ";
        getline(cin, command);

        iResult = send(ConnectSocket, command.c_str(), command.length(), 0);
        if (iResult == SOCKET_ERROR) {
            cout << "send failed with error: " << WSAGetLastError() << "\n";
            closesocket(ConnectSocket);
            WSACleanup();
            return 5;
        }

        cout << "Отправлена команда на сервер: " << command << "\n";
        Sleep(PAUSE);

        if (command == "10")
            break;

        // Получаем ответ от сервера
        char recvbuf[DEFAULT_BUFLEN];
        int recvbuflen = DEFAULT_BUFLEN;

        iResult = recv(ConnectSocket, recvbuf, recvbuflen, 0);
        if (iResult > 0) {
            recvbuf[iResult] = '\0';
            cout << "Ответ от сервера: " << recvbuf << "\n";
            Sleep(PAUSE);
        }
        else if (iResult == 0) {
            cout << "Соединение закрыто сервером\n";
            break;
        }
        else {
            cout << "recv failed with error: " << WSAGetLastError() << "\n";
            closesocket(ConnectSocket);
            WSACleanup();
            return 6;
        }
    }

    closesocket(ConnectSocket);
    WSACleanup();

    return 0;
}
