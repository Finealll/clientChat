#ifndef UNICODE
#define UNICODE
#endif 

#include "Global.h"
#include "MessageFunctions.h"
#include "ServerWork.h"
#include "Entities.h"

#include <iostream>


HINSTANCE gInst;            //Глобальный инстанс

HWND hAuthwnd;                  //Окно входа
HWND hMainwnd;                  //Окно чата

HWND hAuthButton;               //Кнопка войти
HWND hAuthLoginTextBox;          //Инпут для логина
HWND hAuthPasswordTextBox;          //Инпут для пароля
HWND hAuthRegistrateButton;         //Кнопка зарегестрироваться

HWND hMessageTextEdit;               //Edit под сообщение
HWND hMessageSendButton;             //Кнопка под отправку

HWND hBackButton;               //Назад на главную
HWND hNewChatButton;               //Назад на главную

HWND hRegistrationDialog;       //Диалоговое окно регистрации
HWND hAddChatDialog;            //Диалоговое окно добавления чата

HWND hChatsList;			//Окно под список чатов

Chats chats;
User myself;                //Описатель себя

int windowId;           //iD окна, 1 - чаты, 2 - чат
int chatId;           //iD открытого чата

const wchar_t MAIN_CLASS_NAME[] = L"FineallChatMainWindowClass";
const wchar_t AUTH_CLASS_NAME[] = L"FineallChatAuthWindowClass";
const wchar_t CHATLIST_CLASS_NAME[] = L"FineallChatListWindowClass";
const wchar_t MESSAGE_CLASS_NAME[] = L"FineallMessageWindowClass";

LRESULT CALLBACK AuthWindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK MainWindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK ChatListWindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
BOOL CALLBACK RegistrateUserProc(HWND hwndDlg, UINT message, WPARAM wParam, LPARAM lParam);
BOOL CALLBACK AddChatProc(HWND hwndDlg, UINT message, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK MessageProc(HWND hwndDlg, UINT message, WPARAM wParam, LPARAM lParam);
//void SendUserMessage(LPWSTR str);
void MessageHandler();
void DataHandler();
//HWND CreateMessageHWND(int x, int y, LPCWSTR text);

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow)
{
    gInst = hInstance;

    ServerInit();
    if (!Connect()) {
        MessageBox(0, L"Невозможно подключиться к серверу!", 0, 0);
        return 0;
    }
    INITCOMMONCONTROLSEX icex;           // Structure for control initialization.
    icex.dwICC = ICC_LISTVIEW_CLASSES;
    InitCommonControlsEx(&icex);

    WNDCLASS authWc = { };
    WNDCLASS mainWc = { };
    WNDCLASS listWc = { };
    WNDCLASS messageWc = { };

    authWc.lpfnWndProc = AuthWindowProc;
    authWc.hInstance = hInstance;
    authWc.lpszClassName = AUTH_CLASS_NAME;

    mainWc.lpfnWndProc = MainWindowProc;
    mainWc.hInstance = hInstance;
    mainWc.lpszClassName = MAIN_CLASS_NAME;
    mainWc.hbrBackground = CreateSolidBrush(DARK_BLUE_SOLID);
    mainWc.style = CS_VREDRAW | CS_HREDRAW;

    listWc.lpfnWndProc = ChatListWindowProc;
    listWc.hInstance = hInstance;
    listWc.lpszClassName = CHATLIST_CLASS_NAME;
    listWc.hbrBackground = CreateSolidBrush(DARK_BLUE_SOLID);
    listWc.style = CS_VREDRAW | CS_HREDRAW;


    messageWc.lpfnWndProc = MessageProc;
    messageWc.hInstance = hInstance;
    messageWc.lpszClassName = MESSAGE_CLASS_NAME;
    messageWc.hbrBackground = CreateSolidBrush(BLUE_SOLID);
    messageWc.style = CS_VREDRAW | CS_HREDRAW;

    RegisterClass(&authWc);
    RegisterClass(&mainWc);
    RegisterClass(&listWc);
    RegisterClass(&messageWc);


    CreateThread(0, 0, (LPTHREAD_START_ROUTINE)MessageHandler, 0, 0, 0);
    CreateThread(0, 0, (LPTHREAD_START_ROUTINE)DataHandler, 0, 0, 0);

    //Окна входа и чаты
    hAuthwnd = CreateWindowEx(0, AUTH_CLASS_NAME, L"Вход", WS_SYSMENU | WS_CAPTION | WS_MINIMIZEBOX, CW_USEDEFAULT, CW_USEDEFAULT, 
        475, 330,  NULL, NULL, hInstance, NULL);
    hMainwnd = CreateWindowEx(0, MAIN_CLASS_NAME, L"Супер крутой чат пяу пяу пяу", WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, 500, 800, NULL, NULL, hInstance, NULL);
   

    ShowWindow(hAuthwnd, nCmdShow);

    // Run the message loop.

    MSG msg = { };
    while (GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return 0;
}


LRESULT CALLBACK MainWindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    
    switch (uMsg) {
    case WM_CREATE: {

        windowId = 1;

        RECT rc;
        GetClientRect(hwnd, &rc);

        hChatsList = CreateWindowEx(0, CHATLIST_CLASS_NAME, NULL, WS_CHILD | WS_VISIBLE | WS_VSCROLL, 0, 80, rc.right, rc.bottom - 80, hwnd, (HMENU)MAIN_CHATSLIST, 0, 0);
        hMessageSendButton = CreateWindowEx(0, L"button", L"Отправить", WS_CHILD, 0, 0, 0, 0, hwnd, (HMENU)MAIN_MESSAGESEND_BUTTON, 0, 0);
        hMessageTextEdit = CreateWindowEx(0, L"edit", 0, WS_CHILD | ES_AUTOVSCROLL | WS_VSCROLL | ES_MULTILINE, 0, 0, 0, 0, hwnd, (HMENU)MAIN_MESSAGE_EDIT, 0, 0);
        hBackButton = CreateWindowEx(0, L"button", L"Назад", WS_CHILD, 0, 0, 0, 0, hwnd, (HMENU)MAIN_BACK_BUTTON, 0, 0);
        hNewChatButton = CreateWindowEx(0, L"button", L"+", WS_CHILD | WS_VISIBLE, 0, 0, 0, 0, hwnd, (HMENU)MAIN_ADD_CHAT_BUTTON, 0, 0);

        //SendDlgItemMessage(hwnd, MAIN_MESSAGE_EDIT, EM_SETLIMITTEXT, 4050, 0);
        chats.hList = &hChatsList;


#pragma region initTestEntities
//
//        User user1, user2, user3;       //Юзеры
//        memcpy(user1.fName, L"Имя6", sizeof(L"Имя1"));       //Имя
//        memcpy(user2.fName, L"Имя7", sizeof(L"Имя2"));
//        memcpy(user3.fName, L"Имя8", sizeof(L"Имя3"));
//        user1.user_id = 6;
//        user2.user_id = 7;
//        user3.user_id = 8;
//
//
//        Message mess121, mess122, mess211;      //Сообщения, от 1 ко 2, от 2 к 1, от 1 к 2
//        mess121.message_id = 6;
//        mess211.message_id = 7;
//        mess122.message_id = 8;
//        mess121.ownerId = 6;
//        mess122.ownerId = 6;
//        mess211.ownerId = 7;
//        mess121.recipientId = 7;
//        mess122.recipientId = 7;
//        mess211.recipientId = 6;
//        mess121.chat_id = 4;
//        mess122.chat_id = 4;
//        mess211.chat_id = 4;
//        memcpy(mess121.text, L"Текст первого сообщения юзеру 7", sizeof(L"Текст первого сообщения юзеру 2"));
//        memcpy(mess122.text, L"Текст второго сообщения юзеру 7", sizeof(L"Текст второго сообщения юзеру 2"));
//        memcpy(mess211.text, L"Текст первого сообщения юзеру 6", sizeof(L"Текст первого сообщения юзеру 1"));
//        GetSystemTime(&mess121.time);
//        Sleep(100);
//        GetSystemTime(&mess211.time);
//        Sleep(100);
//        GetSystemTime(&mess122.time);
//
//
//        chats.AddUser(user1);
//        chats.AddUser(user2);
//        chats.AddUser(user3);
//
//        chats.AddChat(6, 4);
//        chats.AddChat(7, 4);
//
//        chats.AddMessage(mess121);
//        chats.AddMessage(mess211);
//        chats.AddMessage(mess122);
#pragma endregion


        chats.ReDrawChats(hChatsList);




        return 0;
    }




    case WM_COMMAND:{
        int wmId = LOWORD(wParam);      //Id окна, вызвавшее событие
        int wmEvent = HIWORD(wParam);       //Id события
        switch (wmId)
        {
        case MAIN_MESSAGESEND_BUTTON:
            wchar_t buff[8100];
            GetWindowText(hMessageTextEdit, buff, 8100);

            Message mess;
            memcpy(mess.text, buff, 8100);
            mess.ownerId = myself.user_id;
            mess.recipientId = chats.GetCurrentUserId();
            mess.chat_id = chats.GetCurrentChatId();
            SendTextMessage(mess);
            //chats.ReDrawMessages(hChatsList);
            //SendMessage(hChatsList, WM_CREATE, 0, 0);
            return 0;

        case MAIN_BACK_BUTTON:
            chats.DestroyMessagesWindow();
            chats.DrawChatsWindow(hChatsList);
            windowId = 1;
            chats.currentWindow = 1;

            ShowWindow(hMessageSendButton, SW_HIDE);
            ShowWindow(hMessageTextEdit, SW_HIDE);
            ShowWindow(hBackButton, SW_HIDE);
            ShowWindow(hNewChatButton, SW_SHOW);

            RECT rc;
            GetClientRect(hwnd, &rc);
            SendMessage(hwnd, WM_GET_CHATS_SIZES, 0, 1);
            SendMessage(hwnd, WM_SIZE, 0, MAKELPARAM(rc.right, rc.bottom));
            //SendMessage(hChatsList, WM_SIZE, 0, MAKELPARAM(rc.right, rc.bottom));
            InvalidateRect(hwnd, 0, TRUE);
            return 0;

        case MAIN_ADD_CHAT_BUTTON:
            DialogBox(gInst, MAKEINTRESOURCE(IDD_ADD_CHAT_DIALOG), hMainwnd, (DLGPROC)AddChatProc);
            return 0;

        default:
            break;
        }
        break;
        }


    case WM_SIZE: {
        int cxClient = LOWORD(lParam);
        int cyClient = HIWORD(lParam);


        if (windowId == 1) {
            SetWindowPos(hChatsList, HWND_TOP, 0, 0, cxClient, cyClient - 80, SWP_NOZORDER | SWP_NOMOVE);
            SetWindowPos(hNewChatButton, HWND_TOP, cxClient - 40, 15, 35, 35, SWP_NOZORDER);
        }
        else if (windowId == 2) {
            SetWindowPos(hChatsList, HWND_TOP, 0, 0, cxClient, cyClient - 130, SWP_NOZORDER | SWP_NOMOVE);
            SetWindowPos(hMessageTextEdit, HWND_TOP, 5, cyClient - 50, cxClient-120-5-5, 50, SWP_NOZORDER);
            SetWindowPos(hMessageSendButton, HWND_TOP, cxClient - 105, cyClient - 50, 100, 50, SWP_NOZORDER);
            SetWindowPos(hBackButton, HWND_TOP, cxClient-105, 15, 100, 35, SWP_NOZORDER);
        }
        return 0;
    }

    case WM_DESTROY: {
        //User u1 = *chats.users[0];
        //User u2 = *chats.users[1];
        //MessageBox(0, u1.fName, 0, 0);
        //MessageBox(0, u2.fName, 0, 0);
        PostQuitMessage(0);
        break;
    }

    case WM_PAINT: {
        PAINTSTRUCT ps;
        HFONT hFont;
        HDC hdc;
        RECT rect1, rect2;

        GetClientRect(hwnd, &rect1);
        GetClientRect(hwnd, &rect2);
        rect1.bottom = 80;
        rect2.top = rect2.bottom - 80;

        hFont = CreateFont(48, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, L"Arial");
        hdc = BeginPaint(hwnd, &ps);
        SelectObject(hdc, hFont);
        SetTextColor(hdc, RGB(255, 255, 255));
        SetBkMode(hdc, 1);
        FillRect(hdc, &rect1, CreateSolidBrush(BLUE_SOLID));
        if (windowId == 1)
            TextOut(hdc, 10, 10, L"Ваши чаты", wcslen(L"Ваши чаты"));
        else if (windowId == 2) {
            FillRect(hdc, &rect2, CreateSolidBrush(BLUE_SOLID));
            wchar_t name[33];
            wsprintfW(name, L"%s %s", chats.currentChat->user->fName, chats.currentChat->user->lName);
            TextOut(hdc, 10, 10, name, wcslen(name));
        }
        EndPaint(hwnd, &ps);
        return 0;

    }
    }
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}


LRESULT CALLBACK ChatListWindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{

    int iVscrollInc;
    static int iVscrollPos, iVscrollMax, cxClient, cyClient, h, cItems;

    switch (uMsg)
    {
    case WM_CREATE:

    case WM_GET_CHATS_SIZES: 
        if (windowId == 1)
            cItems = chats.GetChatSize();
        else if (windowId == 2)
            cItems = chats.GetMessagesSize();

        h = 70;
        iVscrollPos = 0;
        SetScrollRange(hwnd, SB_VERT, 0, cItems * h, FALSE);
        SetScrollPos(hwnd, SB_VERT, iVscrollPos, TRUE);
        return 0;
      

    case WM_REDRAW_CHATS:
        if (windowId == 1)
            chats.ReDrawChats(hwnd);
        return 0;

    case WM_REDRAW_MESSAGES:
        if ((windowId == 2) && (chats.currentChat->chatId == lParam))
            chats.ReDrawMessages(hwnd);
        return 0;

    case WM_VSCROLL:{
        switch (LOWORD(wParam))
        {
        case SB_TOP:
            iVscrollInc = -iVscrollPos;
            break;
        case SB_BOTTOM:
            iVscrollInc = iVscrollMax - iVscrollPos;
            break;
        case SB_LINEUP:
            iVscrollInc = -1;
            break;
        case SB_LINEDOWN:
            iVscrollInc = 1;
            break;
        case SB_PAGEUP:
            iVscrollInc = min(-1, -cyClient / h);
            break;
        case SB_PAGEDOWN:
            iVscrollInc = max(1, cyClient / h);
            break;
        case SB_THUMBTRACK:
            iVscrollInc = HIWORD(wParam) - iVscrollPos;
            break;
        default:
            iVscrollInc = 0;
        }
        iVscrollInc = max(
            -iVscrollPos,
            min(iVscrollInc, iVscrollMax - iVscrollPos)
        );
        if (iVscrollInc != 0)
        {
            iVscrollPos += iVscrollInc;
            ScrollWindow(hwnd, 0, -h * iVscrollInc, NULL, NULL);
            SetScrollPos(hwnd, SB_VERT, iVscrollPos, TRUE);
            UpdateWindow(hwnd);
        }
        return 0;
    }


    case WM_MOUSEWHEEL: {
        iVscrollInc = GET_WHEEL_DELTA_WPARAM(wParam) < 0 ? -1 : 1;

        iVscrollInc = max(
            -iVscrollPos,
            min(iVscrollInc, iVscrollMax - iVscrollPos)
        );

        if (iVscrollInc != 0)
        {
            iVscrollPos += iVscrollInc;
            ScrollWindow(hwnd, 0, -h * iVscrollInc, NULL, NULL);
            SetScrollPos(hwnd, SB_VERT, iVscrollPos, TRUE);
            UpdateWindow(hwnd);
        }
        return 0;
    }

    case WM_COMMAND: {
        int wmId = LOWORD(wParam);      //Id окна, вызвавшее событие
        int wmEvent = HIWORD(wParam);       //Id события
        if (windowId == 1 && wmId >= 10000) {                //Если сообщение на окне чатов
            windowId = 2;
            chats.currentWindow = 2;
            chats.DestroyChatsWindow();
            chats.SetCurrentChat(wmId);
            chats.DrawMessagesWindow(hChatsList);

            ShowWindow(hMessageSendButton, SW_SHOW);
            ShowWindow(hMessageTextEdit, SW_SHOW);
            ShowWindow(hBackButton, SW_SHOW);
            ShowWindow(hNewChatButton, SW_HIDE);

            cItems = chats.GetMessagesSize();
            SetScrollRange(hwnd, SB_VERT, 0, cItems * h, FALSE);
            SetScrollPos(hwnd, SB_VERT, iVscrollPos, TRUE); 

            InvalidateRect(hMainwnd, 0, TRUE);
        }
        break;
    }
    
    case WM_SIZE: {
        //Scroll
        cxClient = LOWORD(lParam);
        cyClient = HIWORD(lParam);


        if (windowId == 1) {
            chats.SizeWidthChatsWindow(lParam);
            iVscrollMax = max(0, cItems - (cyClient-80) / h - 1);
        }
        else if (windowId == 2) {
            chats.SizeWidthMessagesWindow(lParam);
            iVscrollMax = max(0, cItems - (cyClient - 160) / h);
        }


        iVscrollPos = min(iVscrollPos, iVscrollMax);

        SetScrollRange(hwnd, SB_VERT, 0, iVscrollMax, FALSE);
        SetScrollPos(hwnd, SB_VERT, iVscrollPos, TRUE);
        return 0;
    }
    

    case WM_CTLCOLORSTATIC:{
        SelectObject((HDC)wParam, CreateFont(20, 0, 0, 0, 700, 0, 0, 0, 0, 0, 0, 0, 0, L"Arial"));
        //Красим и задаем стиль инпутам
        SetTextColor((HDC)wParam, RGB(255, 255, 255));
        SetBkColor((HDC)wParam, BLUE_SOLID);
        return (LRESULT)CreateSolidBrush(BLUE_SOLID);
    }

    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;

    }
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}







LRESULT CALLBACK AuthWindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
    case WM_CREATE:
        hAuthButton = CreateWindowEx(0, L"BUTTON", L"Войти", WS_VISIBLE | WS_CHILD | BS_FLAT, 82, 227, 55, 25, hwnd,
            (HMENU)AUTH_BUTTON, NULL, NULL);
        hAuthRegistrateButton = CreateWindowEx(0, L"BUTTON", L"Зарегестрироваться", WS_VISIBLE | WS_CHILD | BS_FLAT, 142, 227, 175, 25, hwnd,
            (HMENU)AUTH_REGISTRATION_BUTTON, NULL, NULL);
        hAuthLoginTextBox = CreateWindowEx(WS_EX_CLIENTEDGE, L"EDIT", L"login1", WS_VISIBLE | WS_CHILD | WS_BORDER | ES_AUTOHSCROLL, 82, 117, 240, 40, hwnd,
            (HMENU)AUTH_TEXTBOX, NULL, NULL);
        hAuthPasswordTextBox = CreateWindowEx(WS_EX_CLIENTEDGE, L"EDIT", L"passw1", WS_VISIBLE | WS_CHILD | WS_BORDER | ES_AUTOHSCROLL, 82, 167, 240, 40, hwnd,
            (HMENU)AUTH_TEXTBOX, NULL, NULL);


        return 0;
    case WM_PAINT:
        PAINTSTRUCT ps;
        HFONT hFont;
        HDC hdc;
        hFont = CreateFont(48, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, L"Arial");
        hdc = BeginPaint(hwnd, &ps);
        SelectObject(hdc, hFont);
        SetTextColor(hdc, RGB(0, 0, 0));
        SetBkMode(hdc, 1);
        TextOut(hdc, 82, 38, L"Войдите в чатик!", strlen("Войдите в чатик!"));
        EndPaint(hwnd, &ps);
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    case WM_COMMAND:
        switch (wParam)
        {
        case AUTH_BUTTON:

            wchar_t login[32];      //Под логин
            wchar_t password[32];       //Под пароль
            GetWindowText(hAuthLoginTextBox, login, 32);
            GetWindowText(hAuthPasswordTextBox, password, 32);
            if (wcscmp(login, L"") == 0) {
                MessageBox(hwnd, L"Введите логин!", 0, 0);
                break;
            }
            if (wcscmp(login, L"") == 0) {
                MessageBox(hwnd, L"Введите пароль!", 0, 0);
                break;
            }

            
            LoginUser(login, password);
            /*
            ShowWindow(hAuthwnd, SW_HIDE);
            ShowWindow(hMainwnd, SW_SHOW);*/

            break;

        case AUTH_REGISTRATION_BUTTON:
            ShowWindow(hAuthwnd, SW_HIDE);
            DialogBox(gInst, MAKEINTRESOURCE(IDD_REGISTRATION_DIALOG), hAuthwnd, (DLGPROC)RegistrateUserProc);
            ShowWindow(hAuthwnd, SW_SHOW);
            break;
        }


        break;

    }
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}


BOOL CALLBACK RegistrateUserProc(HWND hwndDlg, UINT message, WPARAM wParam, LPARAM lParam) {
    switch (message) {
    case WM_INITDIALOG:
        hRegistrationDialog = hwndDlg;
        SendDlgItemMessage(hwndDlg, IDC_EDIT_FNAME, WM_SETTEXT, 0, (LPARAM)L"Имя");
        SendDlgItemMessage(hwndDlg, IDC_EDIT_LNAME, WM_SETTEXT, 0, (LPARAM)L"Фамилия");
        SendDlgItemMessage(hwndDlg, IDC_EDIT_PNAME, WM_SETTEXT, 0, (LPARAM)L"Отчество");
        SendDlgItemMessage(hwndDlg, IDC_EDIT_PHONE, WM_SETTEXT, 0, (LPARAM)L"Телефон");
        SendDlgItemMessage(hwndDlg, IDC_EDIT_EMAIL, WM_SETTEXT, 0, (LPARAM)L"Email");
        SendDlgItemMessage(hwndDlg, IDC_EDIT_LOGIN, WM_SETTEXT, 0, (LPARAM)L"Логин");
        SendDlgItemMessage(hwndDlg, IDC_EDIT_PASSWORD, WM_SETTEXT, 0, (LPARAM)L"Пароль");

        SendDlgItemMessage(hwndDlg, IDC_EDIT_FNAME, EM_SETLIMITTEXT, 16, 0);
        SendDlgItemMessage(hwndDlg, IDC_EDIT_LNAME, EM_SETLIMITTEXT, 16, 0);
        SendDlgItemMessage(hwndDlg, IDC_EDIT_PNAME, EM_SETLIMITTEXT, 16, 0);
        SendDlgItemMessage(hwndDlg, IDC_EDIT_PHONE, EM_SETLIMITTEXT, 11, 0);
        SendDlgItemMessage(hwndDlg, IDC_EDIT_EMAIL, EM_SETLIMITTEXT, 32, 0);
        SendDlgItemMessage(hwndDlg, IDC_EDIT_LOGIN, EM_SETLIMITTEXT, 32, 0);
        SendDlgItemMessage(hwndDlg, IDC_EDIT_PASSWORD, EM_SETLIMITTEXT, 32, 0);
        return TRUE;

    case WM_COMMAND:
        switch (LOWORD(wParam)) {
        case IDC_BUTTON_REGISTRATE: {
            User user;

            HWND hBirthday = GetDlgItem(hwndDlg, IDC_DATETIMEPICKER1);

            GetDlgItemTextW(hwndDlg, IDC_EDIT_FNAME, user.fName, 16);
            GetDlgItemTextW(hwndDlg, IDC_EDIT_LNAME, user.lName, 16);
            GetDlgItemTextW(hwndDlg, IDC_EDIT_PNAME, user.pName, 16);

            char phone[12];
            GetDlgItemTextA(hwndDlg, IDC_EDIT_PHONE, phone, 12);
            memcpy(user.phone, phone, 11);

            GetDlgItemTextA(hwndDlg, IDC_EDIT_EMAIL, user.email, 32);
            GetDlgItemTextW(hwndDlg, IDC_EDIT_LOGIN, user.login, 32);
            GetDlgItemTextW(hwndDlg, IDC_EDIT_PASSWORD, user.password, 32);
            SendMessage(hBirthday, DTM_GETSYSTEMTIME, 0, (LPARAM)&user.birthday);

            RegistrateUser(user);
            break;
        }
        }
        break;

    case WM_CLOSE:
        EndDialog(hwndDlg, 1);
        return TRUE;
    }
    return FALSE;
}


BOOL CALLBACK AddChatProc(HWND hwndDlg, UINT message, WPARAM wParam, LPARAM lParam) {
    switch (message) {
    case WM_INITDIALOG:
        hAddChatDialog = hwndDlg;
        return TRUE;

    case WM_COMMAND:
        switch (LOWORD(wParam)) {
        case IDC_ADDNEWCHAT_BUTTON: {
            char phone[12];

            GetDlgItemTextA(hwndDlg, IDC_ADDNEWCHAT_EDIT, phone, 12);

            //Проверки на телефон

            GetNewChatByPhone(phone);
            break;
        }
        }
        break;

    case WM_CLOSE:
        EndDialog(hwndDlg, 1);
        return TRUE;
    }
    return FALSE;
}


LRESULT CALLBACK MessageProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg)
    {
    case WM_CREATE: {
        Message* message = (Message*)((LPCREATESTRUCT)lParam)->lpCreateParams;
        User* user = chats.GetNameByUserId(message->ownerId);
        message->hwndEdit = CreateWindowEx(0, L"EDIT", message->text, WS_VISIBLE | WS_CHILD | WS_VSCROLL | ES_AUTOVSCROLL | ES_MULTILINE, 5, 5, 300, 70, hwnd, 0, gInst, 0);
        wchar_t upLine[1024];
        wsprintfW(upLine, L"%s %d/%d/%d %d:%d", user->fName, message->time.wDay, message->time.wMonth, message->time.wYear, message->time.wHour, message->time.wMinute);
        message->hwndUpLine = CreateWindowEx(0, L"STATIC", upLine, WS_VISIBLE | WS_CHILD, 0, 0, 300, 20, hwnd, 0, gInst, 0);
        return 0;
    }


    case WM_CTLCOLOREDIT:
        SelectObject((HDC)wParam, CreateFont(16, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, L"Arial"));
        //Красим и задаем стиль инпутам
        SetTextColor((HDC)wParam, RGB(255, 255, 255));
        SetBkColor((HDC)wParam, BLUE_SOLID);
        return (LRESULT)CreateSolidBrush(BLUE_SOLID);
        

    }
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}


struct Data {
    char data[8192];
};


queue<Data> datas;

void DataHandler() {

    while (true) {
        if (!datas.empty()) {

            char data[8192];
            memcpy(data, datas.front().data, 8192);
            datas.pop();
           

            int command = data[0];
            int subcommand1 = data[1];
            int subcommand2 = data[2];



            switch (command)
            {

            case 1: {
                switch (subcommand1)
                {
                case 1:
                    switch (subcommand2)
                    {
                    case 1: {//Добавляем нового юзера и создаем чат с ним в массив
                        User* user = new User();
                        user->user_id = *(long long*)&data[32];
                        memcpy(user->fName, &data[40], 32);
                        memcpy(user->lName, &data[72], 32);
                        chats.AddUser(user);
                        break;
                    }

                    case 2: {//Инициализируем инфу о себе
                        myself.user_id = *(long long*)&data[32];
                        memcpy(myself.fName, &data[40], 32);
                        memcpy(myself.lName, &data[72], 32);
                        chats.AddUser(&myself);
                        break;
                    }
                          //case 3: {//Добавляем чаты
                          //    long long chatId = *(long long*)&data[32];
                          //    long long userId = *(long long*)&data[40];

                          //    /*wchar_t msg[15];
                          //    wsprintf(msg, L"%d %d", (int)chatId, (int)userId);
                          //    MessageBox(0, msg, 0, 0);*/
                          //    chats.AddChat(userId, chatId);
                          //    if (windowId == 1)
                          //        chats.ReDrawChats(hChatsList);
                          //    break;
                          //}
                    case 4: {//Добавляем сообщения
                        Message mess;
                        mess.ownerId = *(long long*)&data[6];
                        mess.recipientId = *(long long*)&data[14];
                        mess.chat_id = *(long long*)&data[22];
                        mess.message_id = *(long long*)&data[30];
                        mess.time.wYear = data[38] * 100 + data[39];
                        mess.time.wMonth = data[40];
                        mess.time.wDay = data[41];
                        mess.time.wHour = data[42];
                        mess.time.wMinute = data[43];
                        mess.time.wSecond = data[44];

                        memcpy(mess.text, &data[92], 8100);
                        chats.AddMessage(mess);
                        SendMessage(hChatsList, WM_REDRAW_MESSAGES, 0, mess.chat_id);
                        break;
                    }
                    case 5: {//Чаты + юзеры
                        User* user = new User();
                        user->user_id = *(long long*)&data[32];
                        memcpy(user->fName, &data[40], 32);
                        memcpy(user->lName, &data[72], 32);
                        long long chatId = *(long long*)&data[24];


                        chats.AddUser(user);
                        chats.AddChat(user->user_id, chatId);
                        SendMessage(hChatsList, WM_REDRAW_CHATS, 0, 0);
                        break;
                    }
                    default:
                        break;
                    }
                    break;
                default:
                    break;
                }
                break;
            }
            case 110:
                switch (subcommand1)
                {
                case 1:
                    //DestroyWindow(hAuthwnd);
                    ShowWindow(hAuthwnd, SW_HIDE);
                    ShowWindow(hMainwnd, SW_SHOW);
                    GetStartUserData();
                    //MessageBox(NULL, L"Залогин произошел успешно!", L"Ошибка", MB_OK);
                    break;
                case 2:
                    MessageBox(NULL, L"Залогин произошел неуспешно!", L"Ошибка", MB_OK);
                    break;
                default:
                    break;
                }
                break;
            case 111:
                switch (subcommand1)
                {
                case 1:
                    MessageBox(0, L"Регистрация прошла успешно! Авторизуйтесь.", L"Ура", MB_OK);
                    SendMessage(hRegistrationDialog, WM_CLOSE, 0, 0);
                    ShowWindow(hAuthwnd, SW_SHOW);
                    break;
                case 2:
                    MessageBox(0, L"Выбранный логин уже занят.", L"Увы", MB_OK);
                    break;
                case 3:
                    MessageBox(0, L"Выбранный телефон уже используется.", L"Увы", MB_OK);
                    break;
                default:
                    break;
                }
                break;

            case 112:
                switch (subcommand1) {
                case 1:
                    MessageBox(0, L"Чат создан!", L"Успех", MB_OK);
                    SendMessage(hAddChatDialog, WM_CLOSE, 0, 0);
                    break;
                case 2:
                    MessageBox(0, L"Отсутствует пользователь с таким номером телефона!", L"Ошибка", MB_OK);
                    break;
                case 3:
                    MessageBox(0, L"У вас уже есть чат с этим пользователем!", L"Упс", MB_OK);
                    break;
                }
                break;
            }
        }
        else
            Sleep(1000);
    }

}



//MessageHandler
void MessageHandler() {
    char data[8192];
    while (true) {
        memset(data, '\0', 8192);
        if (recv(GetSocket(), data, sizeof(data), 0)) {
            if(data[0] == 0)
                break;
            else {
                Data dataS;
                memcpy(dataS.data, &data, 8192);
                datas.push(dataS);
            }
        };
    }
}









// Old main

//LRESULT CALLBACK MainWindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
//{
//    //static int cMessages, iVscrollPos, scrollInterval;
//    switch (uMsg)
//    {
//    case WM_DESTROY:
//        //send(Connection, "!closesocket", strlen("closesocket"), 0);
//        //closesocket(GetSocket());
//        PostQuitMessage(0);
//        return 0;
//
//    case WM_CREATE:
//        //hMainMesageTextBox = CreateWindowEx(0, L"EDIT", L"Введите сообщение", WS_VISIBLE | WS_CHILD | ES_AUTOHSCROLL | ES_AUTOVSCROLL | ES_MULTILINE,
//        //    25, 925, 355, 50, hwnd, (HMENU)MAIN_TEXTBOX, NULL, NULL);       //Инпут сообщения
//        //hMainMesagesBox = CreateWindowEx(0, L"EDIT", L"",
//        //    WS_VSCROLL | WS_VISIBLE | WS_CHILD | ES_MULTILINE/* | ES_READONLY*/,
//        //    25, 100, 460, 800, hwnd, (HMENU)MAIN_STATICBOX, NULL, NULL);        //Окно сообщений
//
//        //hMainMesagesBox = CreateWindowEx(0, L"static", L"",
//        //    WS_VSCROLL | WS_VISIBLE | WS_CHILD /*| ES_READONLY*/,
//        //    25, 100, 460, 800, hwnd, (HMENU)MAIN_STATICBOX, NULL, NULL);        //Окно сообщений
//
//       /* hMainSendButton = CreateWindowEx(0, L"BUTTON", L"Отправить", WS_VISIBLE | WS_CHILD | BS_FLAT, 395, 940, 75, 28, hwnd,
//            (HMENU)MAIN_SENDBUTTON, NULL, NULL); */                               //Кнопка отправить
//
//
//        Messages.push_back(CreateMessageHWND(10, 10, L"text1"));
//        Messages.push_back(CreateMessageHWND(10, 210, L"text2"));
//        Messages.push_back(CreateMessageHWND(10, 410, L"text3"));
//        Messages.push_back(CreateMessageHWND(10, 610, L"text4"));
//        Messages.push_back(CreateMessageHWND(10, 810, L"text5"));
//        Messages.push_back(CreateMessageHWND(10, 1010, L"text6"));
//        Messages.push_back(CreateMessageHWND(10, 1210, L"text7"));
//        //cMessages = 7;
//        //scrollInterval = 25;
//
//        //SetScrollRange(hwnd, SB_VERT, 0, 4*scrollInterval*cMessages, FALSE);
//        //SetScrollPos(hwnd, SB_VERT, iVscrollPos, TRUE);
//
//
//
//        //SendMessage(hMainMesagesBox, EM_REPLACESEL, TRUE, (LPARAM)L"Здесь\r\nБудут\r\nСообщения");      //Так будем заполнять сообщения
//        //SendMessage(hMainMesageTextBox, EM_SETLIMITTEXT, 256, 0);           //Ограничение на ввод символов
//
//        return 0;
//
//        //case WM_VSCROLL:
//        //    //MessageBox(NULL, L"AWESOME!", L"WOW", MB_OK);
//        //    int old;
//        //    old = iVscrollPos;
//        //    switch (LOWORD(wParam))
//        //    {
//        //    case SB_LINEUP:
//        //        iVscrollPos -= scrollInterval;
//        //        break;
//        //    case SB_LINEDOWN:
//        //        iVscrollPos += scrollInterval;
//        //        break;
//        //    case SB_PAGEUP:
//        //        iVscrollPos -= scrollInterval*4;
//        //        break;
//        //    case SB_PAGEDOWN:
//        //        iVscrollPos += scrollInterval*4;
//        //        break;
//        //    case SB_THUMBPOSITION:
//        //        iVscrollPos = HIWORD(wParam);
//        //        break;
//        //    default:
//        //        break;
//        //    }
//        //    iVscrollPos = max(0, min(iVscrollPos, 4 * scrollInterval * cMessages));
//
//        //    RECT rc;
//        //    for (int i = 0; i < cMessages; i++) {
//        //        GetClientRect(Messages[i], &rc);
//        //        MapWindowPoints(Messages[i], GetParent(Messages[i]), (LPPOINT)&rc, 2);
//        //        SetWindowPos(Messages[i], 0, 0, iVscrollPos - old + rc.top, 0, 0, SWP_NOSIZE);
//        //    }
//
//
//            //if (iVscrollPos != GetScrollPos(hMainMesagesBox, SB_VERT)) {
//            //    SetScrollPos(hwnd, SB_VERT, iVscrollPos, TRUE);
//            //    InvalidateRect(hwnd, NULL, TRUE);
//            //}
//        //    return 0;
//
//    case WM_PAINT:
//        HDC hdc;
//        HFONT hFont;
//        PAINTSTRUCT ps;
//        RECT mainRect;
//        RECT mainHeader;
//        RECT mainFooter;
//
//        hdc = BeginPaint(hwnd, &ps);
//        GetClientRect(hwnd, &mainRect);
//        //Закраска футера и хедера
//        mainHeader.left = mainFooter.left = mainRect.left;
//        mainHeader.right = mainFooter.right = mainRect.right;
//        mainHeader.top = mainRect.top;
//        mainHeader.bottom = mainRect.top + 80;
//        mainFooter.top = mainRect.bottom - 80;
//        mainFooter.bottom = mainRect.bottom;
//
//        FillRect(hdc, &mainHeader, CreateSolidBrush(RGB(23, 33, 43)));
//        FillRect(hdc, &mainFooter, CreateSolidBrush(RGB(23, 33, 43)));
//
//        //Надпись названия чата
//        hFont = CreateFont(36, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, L"Arial");
//        SelectObject(hdc, hFont);
//        SetTextColor(hdc, RGB(255, 255, 255));
//        SetBkMode(hdc, 1);
//        TextOut(hdc, 40, 18, L"Чат Калуги", strlen("Чат Калуги"));
//
//
//
//        EndPaint(hwnd, &ps);
//        return 0;
//
//    case WM_CTLCOLOREDIT:
//        SelectObject((HDC)wParam, CreateFont(20, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, L"Arial"));
//        //Красим и задаем стиль инпутам
//        if (hMainMesagesBox == (HWND)lParam)        //Окно вывода сообщений
//        {
//            SetTextColor((HDC)wParam, RGB(255, 255, 255));
//            SetBkColor((HDC)wParam, RGB(14, 22, 33));
//            return (LRESULT)CreateSolidBrush(RGB(14, 22, 33));
//        }
//        else if (hMainMesageTextBox == (HWND)lParam) {      //Инпут нового сообщения
//            SetTextColor((HDC)wParam, RGB(255, 255, 255));
//            SetBkColor((HDC)wParam, RGB(23, 33, 43));
//            return (LRESULT)CreateSolidBrush(RGB(23, 33, 43));
//        }
//        else return 0;
//
//    case WM_COMMAND:
//        switch (wParam)
//        {
//        case MAIN_SENDBUTTON:
//            LPWSTR msg = (LPWSTR)VirtualAlloc((LPVOID)NULL,
//                (DWORD)(512), MEM_COMMIT,
//                PAGE_READWRITE);
//            //char msg[256];
//            GetWindowText(hMainMesageTextBox, msg, 512);
//            // MessageBox(NULL, (LPWSTR)msg, 0, 0);
//            SetWindowText(hMainMesageTextBox, L"Введите текст");
//            ServerSendMessage(msg);
//            break;
//        }
//        return 0;
//
//    }
//    return DefWindowProc(hwnd, uMsg, wParam, lParam);
//}
//
//
//LRESULT CALLBACK ChatWindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
//{
//    switch (uMsg)
//    {
//    case WM_DESTROY:
//        PostQuitMessage(0);
//        return 0;
//
//
//    }
//    return DefWindowProc(hwnd, uMsg, wParam, lParam);
//}




//
//LRESULT CALLBACK ChatsWindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
//{
//
//    int cy = 0;
//    int cx = 0;
//    int h = 60;
//    int w = 500;
//    int iVscrollInc;
//    static int iVscrollPos, iVscrollMax, cxClient, cyClient;
//
//    switch (uMsg)
//    {
//    case WM_DESTROY:
//        PostQuitMessage(0);
//        return 0;
//
//    case WM_CREATE:
//
//        //Chats generator
//        for (int i = 0; i < 16; i++) {
//            wchar_t name[32] = { L"testName" };
//            wchar_t id[12] = { L"testId" };
//            wchar_t ownerId[12] = { L"testOwnId" };
//            wchar_t description[256] = { L"testDescription" };
//            int followers = i;
//            wsprintf(name, L"%s%d", name, i);
//            wsprintf(id, L"%s%d", id, i);
//            wsprintf(ownerId, L"%s%d", ownerId, i);
//            wsprintf(description, L"%s%d", description, i);
//
//            Chat* ch = new Chat(id, ownerId, name, description, followers);
//
//            Chats.push_back(
//                CreateWindowEx(0, CHAT_CLASS_NAME, L"Test", WS_CHILD | WS_VISIBLE | WS_BORDER, cx, cy + i * h, w, h, hwnd,
//                    (HMENU)10000 + i, NULL, static_cast<LPVOID>(ch))
//            );
//        }
//        // end generator
//        //Скролл
//        SetScrollRange(hwnd, SB_VERT, 0, Chats.size()*h, FALSE);
//        SetScrollPos(hwnd, SB_VERT, iVscrollPos, TRUE);
//        
//        return 0;
//
//    case WM_SIZE:
//        cxClient = LOWORD(lParam);
//        cyClient = HIWORD(lParam);
//        SetWindowPos(hwnd, 0, 0, 0, cx, cy, SWP_NOMOVE);
//
//        iVscrollMax = max(0, Chats.size() - cyClient / h - 1);
//        iVscrollPos = min(iVscrollPos, iVscrollMax);
//        SetScrollRange(hwnd, SB_VERT, 0, iVscrollMax, FALSE);
//        SetScrollPos(hwnd, SB_VERT, iVscrollPos, TRUE);
//
//    case WM_VSCROLL:
//        //MessageBox(NULL, L"AWESOME!", L"WOW", MB_OK);
//        int old;
//        old = iVscrollPos;
//        switch (LOWORD(wParam))
//        {
//        case SB_TOP:
//            iVscrollInc = -iVscrollPos;
//            break;
//        case SB_BOTTOM:
//            iVscrollInc = iVscrollMax - iVscrollPos;
//            break;
//        case SB_LINEUP:
//            iVscrollInc = -1;
//            break;
//        case SB_LINEDOWN:
//            iVscrollInc = 1;
//            break;
//        case SB_PAGEUP:
//            iVscrollInc = min(-1, -cyClient / h);
//            break;
//        case SB_PAGEDOWN:
//            iVscrollInc = max(1, cyClient / h);
//            break;
//        case SB_THUMBTRACK:
//            iVscrollInc = HIWORD(wParam) - iVscrollPos;
//            break;
//        default:
//            iVscrollInc = 0;
//        }
//        iVscrollInc = max(
//            -iVscrollPos,
//            min(iVscrollInc, iVscrollMax - iVscrollPos)
//        );
//        if (iVscrollInc != 0)
//        {
//            iVscrollPos += iVscrollInc;
//            ScrollWindow(hwnd, 0, -h * iVscrollInc, NULL, NULL);
//            SetScrollPos(hwnd, SB_VERT, iVscrollPos, TRUE);
//            UpdateWindow(hwnd);
//        }
//        return 0;
//
//        RECT rc;
//        for (int i = 0; i < Chats.size(); i++) {
//            GetClientRect(Chats[i], &rc);
//            MapWindowPoints(Chats[i], GetParent(Chats[i]), (LPPOINT)&rc, 2);
//            SetWindowPos(Chats[i], 0, 0, -h * iVscrollInc, 0, 0, SWP_NOSIZE);
//        }
//
//
//        if (iVscrollPos != GetScrollPos(hwnd, SB_VERT)) {
//            SetScrollPos(hwnd, SB_VERT, iVscrollPos, TRUE);
//            InvalidateRect(hwnd, NULL, TRUE);
//        }
//
//    }
//    return DefWindowProc(hwnd, uMsg, wParam, lParam);
//}
//
//
//LRESULT CALLBACK ChatWindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
//{
//    HDC hdc;
//    HFONT hFont;
//    PAINTSTRUCT ps;
//    RECT rect;
//    switch (uMsg)
//    {
//    case WM_CREATE:
//        Chat* chat;
//        HWND text;
//        text = CreateWindowEx(0, L"static", NULL, WS_CHILD | WS_VISIBLE, 30, 20, 300, 50, hwnd, 0, 0, 0);
//        chat = (Chat*)((LPCREATESTRUCT)lParam)->lpCreateParams;
//        SendMessage(text, WM_SETTEXT, TRUE, (LPARAM)chat->name);
//
//        
//        UpdateWindow(hwnd);
//        return 0;
//
//    case WM_DESTROY:
//        PostQuitMessage(0);
//        return 0;
//
//    case WM_CTLCOLORSTATIC:                         // прорисовка окон класса static
//        
//        SetBkMode((HDC)wParam, TRANSPARENT);       // наследуем цвет фона нашего ТЕКСТА от родительского (нижележащего), судя по всему, от цвета фона контекста окна класса static
//        SetTextColor((HDC)wParam, WHITE_SOLID);            // устанавливаем цвет текста (в данном случае синий) в контекст нашего окна класса static (событие которого сейчас обрабатываем)
//        hFont = CreateFont(18, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, L"Arial");
//        SelectObject((HDC)wParam, hFont);
//        return (INT_PTR)GetStockObject(NULL_BRUSH); // возвращаем окну класса static предопределенный тип (NULL_BRUSH), который соответствует прозрачному фону
//            
//    }
//    return DefWindowProc(hwnd, uMsg, wParam, lParam);
//}
//
//
//
//
//LRESULT CALLBACK MessageWindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
//{
//    switch (uMsg)
//    {
//    case WM_DESTROY:
//        PostQuitMessage(0);
//        return 0;
//
//
//    }
//    return DefWindowProc(hwnd, uMsg, wParam, lParam);
//}
//
//
//LRESULT CALLBACK MessagesWindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
//{
//    switch (uMsg)
//    {
//    case WM_DESTROY:
//        PostQuitMessage(0);
//        return 0;
//
//
//    }
//    return DefWindowProc(hwnd, uMsg, wParam, lParam);
//}
//
