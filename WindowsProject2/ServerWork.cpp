#include "Global.h"

SOCKET Connection;              //Сокет сервера


void ServerInit() {             //Инит сервера, проверка и подгрузка библиотек
    WSAData wsaData;
    WORD DLLVersion = MAKEWORD(2, 1);
    if (WSAStartup(DLLVersion, &wsaData))
        MessageBox(NULL, L"Error: failed load library Winsock", L"Error", MB_OK | MB_ICONERROR);
}


bool Connect()                  //Подключение к серверу
{
    SOCKADDR_IN addr;

    if (int sockfd = socket(AF_INET, SOCK_STREAM, 0) < 0) {
        MessageBox(NULL, L"Error : Could not create socket", 0, 0);
    }

    int nSizeOfADDR = sizeof(addr);
    addr.sin_addr.s_addr = inet_addr("192.168.0.103");
    addr.sin_port = htons(3001);
    addr.sin_family = AF_INET;

    Connection = socket(AF_INET, SOCK_STREAM, 0);
    if (connect(Connection, (SOCKADDR*)&addr, sizeof(addr)) != 0)
    {
        MessageBox(NULL, L"Error: failed connect to server", L"Error", MB_OK | MB_ICONERROR);
        return false;
    }
    else
    {
        //MessageBox(NULL, L"Connect to Server", L"INFO", MB_OK | MB_ICONINFORMATION);
        return true;
    }
}


SOCKET GetSocket() {
    return Connection;
}
