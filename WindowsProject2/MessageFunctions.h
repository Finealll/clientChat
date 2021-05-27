#pragma once
#include "Global.h"
#include "Entities.h"

int SendData(const char* msg, int size = 1024);
int LoginUser(wchar_t login[32], wchar_t password[32]);
int GetStartUserData();
int SendTextMessage(Message mess);
int RegistrateUser(User user);
int GetNewChatByPhone(char phone[12]);