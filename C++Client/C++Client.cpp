// C++Client.cpp : 애플리케이션에 대한 진입점을 정의합니다.
//
#define _CRT_SECURE_NO_WARNINGS
#include "LibsPch.h"
#include "C++Client.h"
#include "framework.h"
#include <iostream>
#include <wchar.h>

#pragma comment (lib,"Libs.lib")

#define MAX_LOADSTRING 100

// 전역 변수:
HINSTANCE hInst;                                // 현재 인스턴스입니다.
WCHAR szTitle[MAX_LOADSTRING];                  // 제목 표시줄 텍스트입니다.
WCHAR szWindowClass[MAX_LOADSTRING];            // 기본 창 클래스 이름입니다.

// 이 코드 모듈에 포함된 함수의 선언을 전달합니다:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

std::unique_ptr<UITaskManager> uiTaskManager;
std::unique_ptr<C_Network::ChattingClient> chattingClient;

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: 여기에 코드를 입력합니다.
    AllocConsole();

    // 콘솔 입출력 스트림 연결
    freopen("CONOUT$", "w", stdout);
    freopen("CONIN$", "r", stdin);

    hInst = hInstance;

    uiTaskManager = std::make_unique<UITaskManager>(hInstance);
    //C_Network::ChattingClient chattingClient(taskManager.get(), C_Network::NetAddress(L"127.0.0.1", 6000), 1);
   
    // 전역 문자열을 초기화합니다.
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_CCLIENT, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // 애플리케이션 초기화를 수행합니다:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_CCLIENT));

    MSG msg;

    //chattingClient.Begin();

    // 기본 메시지 루프입니다:
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }
    
    //chattingClient.End();

    return (int) msg.wParam;
}



//
//  함수: MyRegisterClass()
//
//  용도: 창 클래스를 등록합니다.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_CCLIENT));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_CCLIENT);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//
//   함수: InitInstance(HINSTANCE, int)
//
//   용도: 인스턴스 핸들을 저장하고 주 창을 만듭니다.
//
//   주석:
//
//        이 함수를 통해 인스턴스 핸들을 전역 변수에 저장하고
//        주 프로그램 창을 만든 다음 표시합니다.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // 인스턴스 핸들을 전역 변수에 저장합니다.

   HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, 1200, 1100, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

//
//  함수: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  용도: 주 창의 메시지를 처리합니다.
//
//  WM_COMMAND  - 애플리케이션 메뉴를 처리합니다.
//  WM_PAINT    - 주 창을 그립니다.
//  WM_DESTROY  - 종료 메시지를 게시하고 반환합니다.
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
        // 마지막은 lparam 매개변수
    case WM_CREATE:
    {
        HWND handle = CreateWindowW(L"Button", L"연결 버튼", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 248, 184, 261, 67, hWnd, NULL, hInst, NULL); // 버튼 누르면 Connect 
        uiTaskManager->RegisterHandle(CONN_BUTTON, handle, BUTTON);

        handle = CreateWindowW(L"Edit", L"ip", WS_CHILD | WS_VISIBLE | ES_READONLY | WS_BORDER | ES_CENTER, 12, 25, 212, 24, hWnd, NULL, hInst, NULL);
        uiTaskManager->RegisterHandle(RO_IP_EDIT, handle, BUTTON);

        handle = CreateWindowW(L"Edit", L"port", WS_CHILD | WS_VISIBLE | ES_READONLY | WS_BORDER | ES_CENTER, 12, 87, 212, 24, hWnd, NULL, hInst, NULL);
        uiTaskManager->RegisterHandle(RO_PORT_EDIT, handle, RO_EDIT);

        handle = CreateWindowW(L"Edit", L"연결 상태 : 연결 안 됨", WS_CHILD | WS_VISIBLE | ES_READONLY | WS_BORDER | ES_CENTER, 12, 147, 497, 24, hWnd, NULL, hInst, NULL); // port readonly
        uiTaskManager->RegisterHandle(RO_CONN_EDIT, handle, RO_EDIT);

        handle = CreateWindowW(L"Edit", L"방 정보", WS_CHILD | WS_VISIBLE | ES_READONLY | WS_BORDER | ES_CENTER, 12, 353, 212, 24, hWnd, NULL, hInst, NULL);
        uiTaskManager->RegisterHandle(RO_ROOM_INFO_EDIT, handle, RO_EDIT);

        handle = CreateWindowW(L"Button", L"입장 버튼", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 248, 750, 261, 67, hWnd, NULL, hInst, NULL); // 마지막은 lparam 매개변수
        uiTaskManager->RegisterHandle(ENTER_ROOM_BUTTON, handle,BUTTON);

        handle = CreateWindowW(L"Edit", L"채팅창", WS_CHILD | WS_VISIBLE | ES_READONLY | WS_BORDER | ES_CENTER, 550, 25, 563, 24, hWnd, NULL, hInst, NULL);
        uiTaskManager->RegisterHandle(RO_CHATTING_EDIT, handle,RO_EDIT);

        handle = CreateWindowW(L"Edit", L"127.0.0.1", WS_CHILD | WS_VISIBLE | WS_BORDER | ES_CENTER, 248, 27, 261, 24, hWnd, NULL, hInst, NULL); // ip 입력창
        uiTaskManager->RegisterHandle(IP_EDIT, handle,EDIT);

        handle = CreateWindowW(L"Edit", L"6000", WS_CHILD | WS_VISIBLE | WS_BORDER | ES_CENTER, 248, 87, 261, 24, hWnd, NULL, hInst, NULL); // port 입력창
        uiTaskManager->RegisterHandle(PORT_EDIT, handle,EDIT);
        
        handle = CreateWindowW(L"Edit", L"ID", WS_CHILD | WS_VISIBLE | WS_BORDER | ES_CENTER, 12, 265, 212, 24, hWnd, NULL, hInst, NULL); // id 입력창
        uiTaskManager->RegisterHandle(ID_EDIT, handle, EDIT);

        handle = CreateWindowW(L"Edit", L"PW", WS_CHILD | WS_VISIBLE | WS_BORDER | ES_CENTER, 248, 265, 261, 24, hWnd, NULL, hInst, NULL); // pw 입력창
        uiTaskManager->RegisterHandle(PW_EDIT, handle, EDIT);
        
        handle = CreateWindowW(L"Edit", L"UserId : ", WS_CHILD | WS_VISIBLE | ES_READONLY | WS_BORDER | ES_CENTER | ES_MULTILINE, 12, 300, 212, 44, hWnd, NULL, hInst, NULL);
        uiTaskManager->RegisterHandle(RO_USER_ID_EDIT, handle, RO_EDIT);

        handle = CreateWindowW(L"Button", L"로그인 버튼", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 248, 300, 261, 44, hWnd, NULL, hInst, NULL); // 버튼 누르면 Connect 
        uiTaskManager->RegisterHandle(LOG_IN_BUTTON, handle, BUTTON);

        handle = CreateWindowW(L"Button", L"방 새로고침", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 12, 386, 212, 44, hWnd, NULL, hInst, NULL);
        uiTaskManager->RegisterHandle(ROOM_REFRESH_BUTTON, handle, BUTTON);

        handle = CreateWindowW(L"ListBox", L"RoomLB", WS_CHILD | WS_VISIBLE | LBS_NOTIFY | WS_BORDER, 248, 353, 261, 392, hWnd, NULL, hInst, NULL); //방 목록 출력
        uiTaskManager->RegisterHandle(ROOM_INFO_LISTBOX, handle,LISTBOX);

        handle = CreateWindowW(L"ListBox", L"ChatLB", WS_CHILD | WS_VISIBLE | LBS_NOTIFY | WS_BORDER, 550, 87, 563, 658, hWnd, NULL, hInst, NULL); // 채팅 메시지 출력
        uiTaskManager->RegisterHandle(CHATTING_LISTBOX, handle,LISTBOX);

        handle = CreateWindowW(L"Edit", L"", WS_CHILD | WS_VISIBLE | ES_MULTILINE | WS_BORDER | ES_CENTER, 550, 770, 563, 48, hWnd, NULL, hInst, NULL); // ip 입력창
        uiTaskManager->RegisterHandle(INPUT_CHATTING_EDIT, handle,EDIT);

        
        break;
    }
    
    case WM_COMMAND:
    {
        int wmId = LOWORD(wParam);
        HWND handle = (HWND)(lParam);
        // 메뉴 선택을 구문 분석합니다:
        switch (wmId)
        {
        case BN_CLICKED: // lparam -> handle, hWnd -> 어플리케이션 handle
        {
            const UITaskManager::UIInfo& handleInfo = uiTaskManager->GetHandleInfo(handle);
           
            if(handleInfo.second != UIHandleType::BUTTON)
               break;

           // 연결 버튼 클릭 시 (현재는 1회로 일단 고정.)
           if (chattingClient == nullptr && handleInfo.first == UIHandle::CONN_BUTTON)
           {
               HWND ipHandle = uiTaskManager->GetHandle(IP_EDIT);
               HWND portHandle = uiTaskManager->GetHandle(PORT_EDIT);

               int length = GetWindowTextLength(ipHandle);
               std::wstring ipText(length + 1, L'\0');
               GetWindowText(ipHandle, &ipText[0], length + 1);
               // ip, port를 이용한 chattingClient 

               length = GetWindowTextLength(portHandle);
               WCHAR* portText = new WCHAR[length + 1]{};
               GetWindowText(portHandle, portText, length + 1);

               // 짤림
               uint16 port = (uint16)(_wtoi(portText));

               std::cout << "ip, port : " << std::string(ipText.begin(), ipText.end()) << " " << port << " \n";

                C_Network::NetAddress targetEndPoint(ipText, port);
               
               chattingClient = std::make_unique<C_Network::ChattingClient>(uiTaskManager.get(), targetEndPoint, 1);
               
               // 재시도도 해야됨.
               chattingClient->Init();
               C_Network::NetworkErrorCode netErr = chattingClient->Begin();

               if (netErr == C_Network::NetworkErrorCode::NONE)
                   SetWindowText(uiTaskManager->GetHandle(RO_CONN_EDIT), L"연결됨");
               
               std::cout << "연결 시도 끝\n";
               delete[] portText;

               //chattingClient = std::make_unique<C_Network::ChattingClient>()
           }
           else if (handleInfo.first == UIHandle::LOG_IN_BUTTON)
           {
               SendLogInPacket();
           }
               // 연결 버튼 -> client.connect();
            break;
           
        }
        default:
            return DefWindowProc(hWnd, message, wParam, lParam);
        }
        break;
    }
    case WM_UPDATE_EDIT_TEXT:
    {
        switch (static_cast<LParamType>(wParam))
        {
            case LParamType::INT :
            {
                break;
            }
            case LParamType::LPCWSTR :
            {
                SetWindowText(hWnd, reinterpret_cast<LPCWSTR>(lParam));
                break;
            }
            case LParamType::USHORT :
            {
                break;
            }
            case LParamType::ULONGLONG :
            {
                std::wstring wstr = std::to_wstring(static_cast<ULONGLONG>(lParam));
                SetWindowText(hWnd, wstr.c_str());
                // ip, port를 이용한 chattingClient 
                break;
            }
        default:
            break;
        }
    }
    case WM_UPDATE_LISTBOX: break;
    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hWnd, &ps);
        // TODO: 여기에 hdc를 사용하는 그리기 코드를 추가합니다...
        EndPaint(hWnd, &ps);
        break;
    }
        
    case WM_DESTROY:
    {
        if (chattingClient)
            chattingClient->End();
     
        std::cout << "종료\n";
        PostQuitMessage(0);
        break;

    }
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// 정보 대화 상자의 메시지 처리기입니다.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}

C_Network::NetworkErrorCode SendLogInPacket()
{
    C_Network::LogInRequestPacket reqPacket;

    // TODO : 입력한 나의 ID, PW를 암호화해서 보내기.
    reqPacket.logInId = 1;
    reqPacket.logInPw = 2;

    C_Network::SharedSendBuffer sendBuffer = C_Network::ChattingServerPacketHandler::MakePacket(sizeof(C_Network::PacketHeader) + reqPacket.size, reqPacket);

    // TODO : 로그

    chattingClient->Send(sendBuffer);
    std::cout << "[LOG IN PACKET 전송]\n";

    return C_Network::NetworkErrorCode::NONE;
}