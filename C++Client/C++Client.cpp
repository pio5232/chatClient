// C++Client.cpp : 애플리케이션에 대한 진입점을 정의합니다.
//
#define _CRT_SECURE_NO_WARNINGS
#include "LibsPch.h"
#include "C++Client.h"
#include "framework.h"
#include <iostream>
#include <wchar.h>
#include <CommCtrl.h>
#include "UITaskManager.h"
#include "ServerPacketHandler.h"
#include <string_view>

#pragma comment(lib, "comctl32.lib") // List view 사용 위해 추가.
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
INT_PTR CALLBACK DialogProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);

std::unique_ptr<UITaskManager> uiTaskManager;
std::unique_ptr<C_Network::ChattingClient> chattingClient;
std::thread mainThread;

void AddRow(HWND listView, int row, const std::vector<std::wstring_view>& rowData) 
{
    LVITEM lvItem = {};
    lvItem.mask = LVIF_TEXT;         // 텍스트 항목 추가
    lvItem.iItem = 0;
    lvItem.iSubItem = 0; // == column 

    // 첫 번째 열 데이터 추가
    if (!rowData.empty()) {
        lvItem.pszText = (LPWSTR)rowData[0].data();
        ListView_InsertItem(listView, &lvItem);
    }

    // 나머지 열 데이터 추가
    for (size_t col = 1; col < rowData.size(); ++col) {
        ListView_SetItemText(listView, row, col, (LPWSTR)rowData[col].data());
    }
}

void AddColumn(HWND listView, const WCHAR* columnName, int width, int index) 
{
    LVCOLUMN lvColumn = {};
    lvColumn.mask = LVCF_TEXT | LVCF_WIDTH | LVCF_SUBITEM;
    lvColumn.pszText = (LPWSTR)columnName;
    lvColumn.cx = width;
    lvColumn.iSubItem = index;
    lvColumn.fmt = LVCFMT_CENTER;

    SendMessage(listView , LVM_INSERTCOLUMN, index, (LPARAM)&lvColumn);
}

void UserInit()
{
    AllocConsole();

    // 콘솔 입출력 스트림 연결
    freopen("CONOUT$", "w", stdout);
    freopen("CONIN$", "r", stdin);

    setlocale(LC_ALL, "Korean");
    
}

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    UserInit();

    hInst = hInstance;
   
    uiTaskManager = std::make_unique<UITaskManager>();

    // 전역 문자열을 초기화합니다.
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_CCLIENT, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // 애플리케이션 초기화를 수행합니다:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    //HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_CCLIENT));

    MSG msg;
    
    //chattingClient.Begin();
    // 기본 메시지 루프입니다:

    while (GetMessage(&msg, nullptr, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
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

   uiTaskManager->SetMain(hWnd);

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

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_USER_UPDATE:
    {
        TaskType taskType = static_cast<TaskType>(LOWORD(wParam));
        HWND handle = uiTaskManager->GetHandle(static_cast<UIHandle>(HIWORD(wParam)));

        if (!IsWindow(handle)) 
        {
            std::cout << "Window does not exist\n" << std::endl;
            break;
        }

        switch (taskType)
        {
        case TaskType::CLEAR:
        {
            break;
        }
        case TaskType::WRITE:
        {
            //break;
            SetWindowText(handle, reinterpret_cast<LPCWSTR>(lParam));
            
            delete[] reinterpret_cast<WCHAR*>(lParam);
            break;
        }

        default:
            return DefWindowProc(hWnd, message, wParam, lParam);
        }
        break;
    }
  
    case WM_CREATE:
    {
        INITCOMMONCONTROLSEX icex;           // Structure for control initialization.
        icex.dwICC = ICC_LISTVIEW_CLASSES;
        InitCommonControlsEx(&icex);

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

        handle = CreateWindowW(L"Button", L"방 만들기", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 12, 440, 212, 44, hWnd, NULL, hInst, NULL);
        uiTaskManager->RegisterHandle(CREATE_ROOM_BUTTON, handle, BUTTON);

        handle = CreateWindowW(WC_LISTVIEW,L"RoomLV",WS_CHILD | WS_VISIBLE | WS_BORDER | LVS_REPORT, 248, 353, 261, 392, hWnd, NULL, hInst, NULL); //방 목록 출력
        uiTaskManager->RegisterHandle(ROOM_INFO_LISTVIEW, handle,LISTVIEW);
        AddColumn(handle, L"방 번호", 60, 1);  // 2번째 위치에 추가
        AddColumn(handle, L"방 제목", 200, 2);  // 2번째 위치에 추가

        handle = CreateWindowW(WC_LISTVIEW, L"ChatLV", WS_CHILD | WS_VISIBLE | WS_BORDER | LVS_REPORT, 550, 87, 563, 658, hWnd, NULL, hInst, NULL); // 채팅 메시지 출력
        uiTaskManager->RegisterHandle(CHATTING_LISTVIEW, handle,LISTVIEW);
        AddColumn(handle, L"채팅", 563, 2);  // 2번째 위치에 추가

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
            switch (handleInfo.first)
            {
            case UIHandle::CONN_BUTTON:
            {
                if (chattingClient != nullptr)
                    break;
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
                break;
            }
            case UIHandle::LOG_IN_BUTTON:
            {
                SendLogInPacket();
                break;
            }
            case UIHandle::ROOM_REFRESH_BUTTON:
            {
                SendRefreshRoomInfoPacket();
                break;
            }
            case UIHandle::CREATE_ROOM_BUTTON:
            {
                int res = DialogBox(hInst, MAKEINTRESOURCE(IDD_MAKE_ROOM), hWnd, DialogProc);

                break;
            }
            default:
                break;
            }
            break;
        }
        default:
            return DefWindowProc(hWnd, message, wParam, lParam);
        }
        break;
    }
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

INT_PTR CALLBACK DialogProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam) {
    switch (message) {
    case WM_INITDIALOG:
        SetDlgItemText(hDlg, ID_ROOM_RO, L"만들 방 제목");
        SetDlgItemText(hDlg, ID_ROOM_EDIT, L"");

        return TRUE;

    case WM_COMMAND: // ID - LOWORD(wParam), HANDLE - lParam)
        switch (LOWORD(wParam))
        {
        case IDOK:
        {
            MessageBox(hDlg, L"OK Clicked", L"Dialog", MB_OK);
            EndDialog(hDlg, IDOK);
            return TRUE;
        }
        case IDCANCEL:
        {
            EndDialog(hDlg, IDCANCEL);
            return TRUE;
        }
        case ID_SEND_MAKE_ROOM:
        {
            HWND editHandle = GetDlgItem(hDlg, ID_ROOM_EDIT); // ID가 IDC_EDIT1인 컨트롤의 핸들 얻기
            int length = SendMessage(editHandle, WM_GETTEXTLENGTH, 0, 0);

            WCHAR* roomName = new WCHAR[ROOM_NAME_MAX_LEN]{};
            GetDlgItemText(hDlg, ID_ROOM_EDIT, roomName, length+1);

            SendMakeRoomPacket(roomName);
            EndDialog(hDlg, ID_SEND_MAKE_ROOM);
            return true;
        }
        default:break;
        }
    }
    return FALSE;
}

C_Network::NetworkErrorCode SendLogInPacket()
{
    if (!chattingClient)
    {
        C_Utility::Log(L"로그인 전송 실패");

        return C_Network::NetworkErrorCode::CLIENT_NOT_CONNECTED;
    }
    C_Network::LogInRequestPacket reqPacket;

    // TODO : 입력한 나의 ID, PW를 암호화해서 보내기.
    reqPacket.logInId = 1;
    reqPacket.logInPw = 2;

    C_Network::SharedSendBuffer sendBuffer = C_Network::ChattingServerPacketHandler::MakePacket(reqPacket);

    // TODO : 로그

    chattingClient->Send(sendBuffer);
    
    std::cout << "[LOG IN PACKET 전송]\n";

    return C_Network::NetworkErrorCode::NONE;
}

C_Network::NetworkErrorCode SendRefreshRoomInfoPacket()
{
    if (!chattingClient)
    { 
        C_Utility::Log(L"새로고침 전송 실패");
        return C_Network::NetworkErrorCode::CLIENT_NOT_CONNECTED;
    }
    C_Network::RoomListRequestPacket roomListRequestPacket;

    C_Network::SharedSendBuffer roomListRequestSendBuffer = C_Network::ChattingServerPacketHandler::MakePacket<C_Network::PacketHeader>(roomListRequestPacket);

    chattingClient->Send(roomListRequestSendBuffer);
    
    return C_Network::NetworkErrorCode::NONE;

}

C_Network::NetworkErrorCode SendMakeRoomPacket(WCHAR* roomName)
{
    if (!chattingClient)
    {
        C_Utility::Log(L"방 생성 전송 실패");
        return C_Network::NetworkErrorCode::CLIENT_NOT_CONNECTED;
    }

    C_Network::MakeRoomRequestPacket makeRoomRequestPacket;
    wcscpy_s(makeRoomRequestPacket.roomName,ROOM_NAME_MAX_LEN, roomName);

    C_Network::SharedSendBuffer sendBuffer = C_Network::ChattingServerPacketHandler::MakeSendBuffer(sizeof(makeRoomRequestPacket));

    *sendBuffer << makeRoomRequestPacket;

    chattingClient->Send(sendBuffer);

    return C_Network::NetworkErrorCode();
}
