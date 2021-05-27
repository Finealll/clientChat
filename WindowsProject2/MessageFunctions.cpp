#pragma once
#include "Global.h"
#include "ServerWork.h"
#include "Entities.h"

SOCKET GetSocket();

int SendData(const char* msg, int size = 8192) {
    if (send(GetSocket(), msg, size, 0) == SOCKET_ERROR) {       //Отправляем сообщение
        int num = WSAGetLastError();
        MessageBox(NULL, L"Error sending message", 0, 0);
        return 0;
    }
    else
        return 1;
}


int LoginUser(wchar_t login[32], wchar_t password[32]) {
    //Отправляем сообщение для логина юзера
    // [0] = 1, [1] = 1, [2] = 1 [32-96) - логин, [96-160) - пароль

    char message[160];
    message[0] = 1;
    message[1] = 1;
    message[2] = 1;
    int szl = sizeof(wchar_t);

    memcpy(&message[32], login, wcslen(login) * sizeof(wchar_t));
    memcpy(&message[96], password, wcslen(password) * sizeof(wchar_t));

    SendData(message, 160);
    return 1;
}


int GetStartUserData() {
    //Отправляем сообщение для получения юзердаты после успешного логина
    // [0] = 1, [1] = 1, [2] = 2

    char message[3];
    message[0] = 1;
    message[1] = 1;
    message[2] = 2;

    SendData(message, 3);
    return 1;
}


int SendTextMessage(Message mess) {
    //Отправляем сообщение на сервер
    //[0] = 2, [1] = 1, [2] = 1, [6-14) - user_id отправителя, [14-22) - user_id получателя, [22-30) - chatId, [32-8132) - текст
    char message[8132];

    message[0] = 2;
    message[1] = 1;
    message[2] = 1;

    memcpy(&message[6], &mess.ownerId, sizeof(long long));
    memcpy(&message[14], &mess.recipientId, sizeof(long long));
    memcpy(&message[22], &mess.chat_id, sizeof(long long));
    memcpy(&message[32], &mess.text, wcslen(mess.text) * sizeof(wchar_t));

    SendData(message, wcslen(mess.text) * sizeof(wchar_t) + 32);
    return 1;
}


int RegistrateUser(User user) {
    char message[303];

    message[0] = 1;
    message[1] = 2;
    message[2] = 1;

    memcpy(&message[32], user.fName, wcslen(user.fName) * sizeof(wchar_t));
    memcpy(&message[64], user.lName, wcslen(user.lName) * sizeof(wchar_t));
    memcpy(&message[96], user.pName, wcslen(user.pName) * sizeof(wchar_t));
    memcpy(&message[128], user.email, strlen(user.email));
    memcpy(&message[160], user.phone, strlen(user.phone));
    memcpy(&message[171], user.login, wcslen(user.login) * sizeof(wchar_t));
    memcpy(&message[235], user.password, wcslen(user.password) * sizeof(wchar_t));
    message[299] = user.birthday.wYear / 100;
    message[300] = user.birthday.wYear % 100;
    message[301] = user.birthday.wMonth;
    message[302] = user.birthday.wDay;

    SendData(message,  303);
    return 1;

}


int GetNewChatByPhone(char phone[12]){
    char message[43];

    message[0] = 3;
    message[1] = 1;

    memcpy(&message[32], phone, 11);

    SendData(message, 43);

    return 1;
}






