#pragma once
#ifndef GLOBAL_H
#define GLOBAL_H

#define _CRT_SECURE_NO_WARNINGS

#pragma comment(lib,"Comctl32.lib") 
#pragma comment(lib, "Ws2_32.lib")

#include <windows.h>  
#include <vector>
#include <Commctrl.h>
#include <windowsx.h>
#include <queue>
#include "resource.h"

using namespace std;


#define AUTH_BUTTON 1           //Окно авторизации - переделать
#define AUTH_REGISTRATION_BUTTON 3           //Окно авторизации - зарегестрироваться
#define AUTH_TEXTBOX 2

#define MAIN_CHATSLIST 20
#define MAIN_BACK_BUTTON 21			//Кнопка назад
#define MAIN_ADD_CHAT_BUTTON 22		//Кнопка добавления чата

#define MAIN_MESSAGE_EDIT 51	//Под едит сообщения
#define MAIN_MESSAGESEND_BUTTON 52	//Под кнопку отправки


#define WM_GET_CHATS_SIZES	32890	//Под обновление данных, lParam - текущее окно
#define WM_REDRAW_CHATS		32891	//Под перерисовку чатов
#define WM_REDRAW_MESSAGES		32892	//Под перерисовку сообщений


// Цветовая палитра
#define DARK_BLUE_SOLID RGB(14,22,33)
#define BLUE_SOLID RGB(23, 33, 43)
#define WHITE_SOLID RGB(255, 255, 255)


#endif // !GLOBAL_H