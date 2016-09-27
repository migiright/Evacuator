// TaskSch.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "Evacuator.h"

using namespace std;

#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE hInst; // current instance
TCHAR szTitle[MAX_LOADSTRING]; // The title bar text
TCHAR szWindowClass[MAX_LOADSTRING]; // the main window class name

// Forward declarations of functions included in this code module:
ATOM MyRegisterClass(HINSTANCE hInstance);
BOOL InitInstance(HINSTANCE, int);
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK About(HWND, UINT, WPARAM, LPARAM);

BOOL createTaskTrayIcon(HINSTANCE hInstance, HWND hWnd);
BOOL deleteTaskTrayIcon(HWND hWnd);

int APIENTRY _tWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow)
{
#ifdef _DEBUG
	{
		FILE *damy;
		AllocConsole(); //コンソール割り当て
		freopen_s(&damy, "CON", "r", stdin); // 標準入力の割り当て
		freopen_s(&damy, "CON", "w", stdout); // 標準出力の割り当て
	}
#endif

	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	MSG msg;
	HACCEL hAccelTable;

	// Initialize global strings
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_EVACUATOR, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// Perform application initialization:
	if (!InitInstance(hInstance, nCmdShow)) {
		return FALSE;
	}

	hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_EVACUATOR));

	// Main message loop:
	while (GetMessage(&msg, nullptr, 0, 0)) {
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return static_cast<int>(msg.wParam);
}

//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
//  COMMENTS:
//
//    This function and its usage are only necessary if you want this code
//    to be compatible with Win32 systems prior to the 'RegisterClassEx'
//    function that was added to Windows 95. It is important to call this function
//    so that the application will get 'well formed' small icons associated
//    with it.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style          = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc    = WndProc;
	wcex.cbClsExtra     = 0;
	wcex.cbWndExtra     = 0;
	wcex.hInstance      = hInstance;
	wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_EVACUATOR));
	wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground  = reinterpret_cast<HBRUSH>(COLOR_WINDOW+1);
	wcex.lpszMenuName   = MAKEINTRESOURCE(IDC_EVACUATOR);
	wcex.lpszClassName  = szWindowClass;
	wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_EVACUATOR));

	return RegisterClassEx(&wcex);
}

BOOL createTaskTrayIcon(HINSTANCE hInstance, HWND hWnd)
{
	int r;
	NOTIFYICONDATA nid;
	LPCTSTR text = szTitle;

	nid.cbSize = sizeof(nid);
	nid.hWnd = hWnd;
	nid.uID = 0;
	nid.uFlags = NIF_MESSAGE | NIF_ICON | NIF_TIP;
	nid.uCallbackMessage = WM_USER;
	nid.hIcon = static_cast<HICON>(LoadImage(hInstance, MAKEINTRESOURCE(IDI_EVACUATOR), IMAGE_ICON, 16, 16, 0));
	_tcscpy_s(nid.szTip, sizeof(nid.szTip), text);
	r = Shell_NotifyIcon(NIM_ADD, &nid);
	DestroyIcon(nid.hIcon);

	return r;
}

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int /*nCmdShow*/)
{
	HWND hWnd;

	hInst = hInstance; // Store instance handle in our global variable

	hWnd = CreateWindowEx(WS_EX_TOOLWINDOW, szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

	if (!hWnd) {
		return FALSE;
	}

	//ShowWindow(hWnd, nCmdShow);
	//UpdateWindow(hWnd);

	//タスクトレイにアイコン設置
	createTaskTrayIcon(hInstance, hWnd);

	return TRUE;
}

BOOL deleteTaskTrayIcon(HWND hWnd)
{
	NOTIFYICONDATA nid;

	nid.cbSize = sizeof(nid);
	nid.hWnd = hWnd;
	nid.uID = 0;
	return Shell_NotifyIcon(NIM_DELETE, &nid);
}

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE:  Processes messages for the main window.
//
//  WM_COMMAND - process the application menu
//  WM_PAINT - Paint the main window
//  WM_DESTROY - post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId;
	//int wmEvent;

	static HMENU hMenu, hSubMenu;
	static vector<tuple<string, string>> files;	//<src, dst>

	switch (message)
	{
	case WM_CREATE: {
		//task tray menu作る
		hMenu = LoadMenu(nullptr, MAKEINTRESOURCE(IDR_TASKTRAY_MENU));
		hSubMenu = GetSubMenu(hMenu, 0);

		//ファイル一覧をロード
		bool s = true;
		ifstream ifs("files.txt");
		string src;
		for (string buf; getline(ifs, buf);) {
			boost::trim(buf);
			if (buf.length() != 0 && buf[0] != ';') {
				auto n = ExpandEnvironmentStringsA(buf.c_str(), nullptr, 0);
				vector<char> buf2(n);
				ExpandEnvironmentStringsA(buf.c_str(), &buf2[0], n);
				if (s) {
					src = &buf2[0];
				} else {
					files.push_back(make_tuple(move(src), &buf2[0]));
				}
				s = !s;
			}
		}
		break;
	}

	case WM_COMMAND:
		wmId    = LOWORD(wParam);
		//wmEvent = HIWORD(wParam);
		// Parse the menu selections:
		switch (wmId)
		{
		case ID_TASKTRAY_ABOUT:
			DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
			break;
		case ID_TASKTRAY_EXIT:
			DestroyWindow(hWnd);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		break;

	case WM_USER: //task tray
		if(lParam == WM_RBUTTONDOWN){
			POINT pt;
			GetCursorPos(&pt);
			SetForegroundWindow(hWnd);
			SetFocus(hWnd);
			TrackPopupMenu(hSubMenu, TPM_LEFTALIGN, pt.x, pt.y, 0, hWnd, nullptr);
		}
		break;

	case WM_ENDSESSION:
		ShutdownBlockReasonCreate(hWnd, L"ファイルコピー中");

		for (auto& t : files) {
			if (!PathFileExistsA(get<0>(t).c_str())) continue;
			MakeSureDirectoryPathExists(get<1>(t).c_str());
			CopyFileA(get<0>(t).c_str(), get<1>(t).c_str(), FALSE);
		}

		ShutdownBlockReasonDestroy(hWnd);
		break;

	//case WM_PAINT:
	//	{
	//		PAINTSTRUCT ps;
	//		HDC hdc;
	//		hdc = BeginPaint(hWnd, &ps);
	//		// TODO: Add any drawing code here...
	//		EndPaint(hWnd, &ps);
	//		break;
	//	}
	case WM_DESTROY:
		DestroyMenu(hMenu);
		deleteTaskTrayIcon(hWnd);

		PostQuitMessage(0);
		break;

	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

// Message handler for about box.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message) {
	case WM_INITDIALOG:
		return static_cast<INT_PTR>(TRUE);

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL) {
			EndDialog(hDlg, LOWORD(wParam));
			return static_cast<INT_PTR>(TRUE);
		}
		break;
	}
	return static_cast<INT_PTR>(FALSE);
}
