#pragma once
#include "Global.h"


struct User {				//��������� ��������� �����
	long long user_id;
	wchar_t fName[16];
	wchar_t lName[16];
	wchar_t pName[16];
	SYSTEMTIME birthday;
	char phone[11];
	char email[32];
	wchar_t bio[256];
	wchar_t login[32];
	wchar_t password[32];       //��� ������
};


struct Message {			//��������� ��������� ���������
	long long ownerId;
	long long recipientId;
	long long message_id;
	long long chat_id;
	wchar_t text[8100];
	SYSTEMTIME time;
	bool isDeleted;

	HWND hwnd;
	HMENU menuId;
};


struct Chat {				//��������� ��������� ��� � ����������� � ������
	User* user;
	vector<Message> messageItems;
	long long chatId;

	HWND hwnd;
	HMENU menuId;
};


struct Chats {				//������� ��������� �������������� � ������
	int cChats = 10000;
	int cMessages = 100000;
	int currentWindow = 1;			//������� ���� - 1 - ���� �����, 2 - ���� ����
	vector<Chat> chats;
	vector<User*> users;
	Chat* currentChat;			//��������� �� ������� �������� ���
	HWND* hList;

	int GetChatSize() {			//���������� ���������� �����
		return chats.size();
	}

	int GetMessagesSize() {			//���������� ���������� ���������
		return currentChat->messageItems.size();
	}


	int GetCurrentUserId() {			//���������� user_id �������� ����
		return currentChat->user->user_id;
	}

	int GetCurrentChatId() {			//���������� chatId �������� ����
		return currentChat->chatId;
	}

	//������ � ������ � ���� �����

	int AddChat(long long user_id, long long chatId) {
		//�������� ���� �� ��� ��� � ����� ������
		for (int i = 0; i < chats.size(); i++) {
			if (chats[i].user->user_id == user_id)
				return E_FAIL;
		}
		//��������� ������� ����� � ����
		User* us = nullptr;
		for (int i = 0; i < users.size(); i++) {
			if (users[i]->user_id == user_id) {
				us = users[i];
				goto next;
			}
		}
		//��� ��� ��������� � �������
		MessageBox(0, L"HI", 0, 0);
		//...
		next:
		Chat chat;
		chat.menuId = (HMENU)cChats++;
		chat.chatId = chatId;
		chat.user = us;
		chats.push_back(chat);

		if (currentWindow == 1) {
			SendMessage(*hList, WM_GET_CHATS_SIZES, 0, 1);
			//ReDrawChats(*hList);
		}
			
		return S_OK;
	}

	//��������� ��������� � ������ � ����
	int AddMessage(Message message) {
		for (int i = 0; i < chats.size(); i++) {
			if (chats[i].chatId == message.chat_id) {
				message.menuId = (HMENU)cMessages++;
				chats[i].messageItems.push_back(message);
				if (currentWindow == 2) {
					SendMessage(*hList, WM_GET_CHATS_SIZES, 0, 2);
					//ReDrawMessages(*hList);
				}
				return S_OK;
			}
		}
		return E_FAIL;
	}



	//��������� ����� � ������
	int AddUser(User* user) {
		for (int i = 0; i < users.size(); i++) {
			if (users[i]->user_id == user->user_id)
				return E_FAIL;
		}
		users.push_back(user);
		return S_OK;
	}

	int ReDrawMessages(HWND hwnd, int startPos = 0) {
		DestroyMessagesWindow();
		DrawMessagesWindow(hwnd, startPos);
		return S_OK;
	}


	int ReDrawChats(HWND hwnd, int startPos = 0) {
		DestroyChatsWindow();
		DrawChatsWindow(hwnd, startPos);
		return S_OK;
	}


	//���������� ���� � ������
	int DrawChatsWindow(HWND hwnd, int startPos=0, int h = 70) {
		for (int i = 0; i < chats.size(); i++) {
			chats[i].hwnd = CreateWindowEx(0, L"static", chats[i].user->fName, WS_CHILD | WS_BORDER | SS_NOTIFY, 0, i*h, 300, h, hwnd, chats[i].menuId, 0, 0);
			ShowWindow(chats[i].hwnd, SW_SHOW);
		}
		RECT rc;
		GetClientRect(GetParent(*hList), &rc);
		SendMessage(hwnd, WM_GET_CHATS_SIZES, 0, 1);
		SendMessage(GetParent(*hList), WM_SIZE, 0, MAKELPARAM(rc.right, rc.bottom));
		return S_OK;
	}
	
	//������� ���� � ������
	int DestroyChatsWindow() {
		for (int i = 0; i < chats.size(); i++) {
			DestroyWindow(chats[i].hwnd);
		}
		return S_OK;
	}

	//�������� �� ������ ����
	int SizeWidthChatsWindow(LPARAM lParam, int h = 70){
		for (int i = 0; i < chats.size(); i++) {
			SetWindowPos(chats[i].hwnd, HWND_TOP, 0, 0, GET_X_LPARAM(lParam), h, SWP_NOMOVE);
		}
		return S_OK;
	}

	//������������ ��������� �������� ����
	int DrawMessagesWindow(HWND hwnd, int startPos = 0, int h = 70) {
		RECT rc;
		GetClientRect(GetParent(hwnd), &rc);
		int counter = 0;
		for (int i = currentChat->messageItems.size() - 1; i >= 0 ; i--) {
			//currentChat->messageItems[i].hwnd = CreateWindowEx(0, L"FineallMessageWindowClass", currentChat->messageItems[i].text, WS_CHILD | WS_BORDER, 0, startPos + counter++ * h, 0, h, hwnd, currentChat->messageItems[i].menuId, 0, 0);
			currentChat->messageItems[i].hwnd = CreateWindowEx(0, L"static", currentChat->messageItems[i].text, WS_CHILD | WS_BORDER, 0, startPos + counter++ * h, 0, h, hwnd, currentChat->messageItems[i].menuId, 0, 0);
			ShowWindow(currentChat->messageItems[i].hwnd, SW_SHOW);
		}

		SendMessage(hwnd, WM_GET_CHATS_SIZES, 0, 2);
		SendMessage(GetParent(*hList), WM_SIZE, 0, MAKELPARAM(rc.right, rc.bottom));
		return S_OK;
	}

	//������� ���� � ����������� ����
	int DestroyMessagesWindow() {
		for (int i = 0; i < currentChat->messageItems.size(); i++) {
			DestroyWindow(currentChat->messageItems[i].hwnd);
		}
		return S_OK;
	}

	//�������� �� ������ ���������
	int SizeWidthMessagesWindow(LPARAM lParam, int h = 70) {
		for (int i = 0; i < currentChat->messageItems.size(); i++) {
			SetWindowPos(currentChat->messageItems[i].hwnd, HWND_TOP, 0, 0, GET_X_LPARAM(lParam), h, SWP_NOZORDER | SWP_NOMOVE);
		}
		return S_OK;
	}

	//������������� ������� ���
	int SetCurrentChat(int menuId) {
		for (int i = 0; i < chats.size(); i++) {
			if (chats[i].menuId == (HMENU)menuId) {
				currentChat = &chats[i];
				return S_OK;
			}
		}
		return E_FAIL;
	}
	
	int MapChatsPos(int iVscrollInc, int h=70) {
		RECT rc;
		for (int i = 0; i < chats.size(); i++) {
			GetClientRect(chats[i].hwnd, &rc);
			MapWindowPoints(chats[i].hwnd, GetParent(chats[i].hwnd), (LPPOINT)&rc, 2);
			SetWindowPos(chats[i].hwnd, 0, 0, -h * iVscrollInc, 0, 0, SWP_NOSIZE);
		}
		return S_OK;
	}


	
};





