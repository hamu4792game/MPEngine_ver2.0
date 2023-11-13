#include "WinApp.h"
//	imguiのinclude
#include "externals/imgui/imgui.h"
#include "externals/imgui/imgui_impl_dx12.h"
#include "externals/imgui/imgui_impl_win32.h"
//	関数の外部宣言
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
#include <string>

#include "resource.h"


WinApp* WinApp::GetInstance() {
	static WinApp instance;
	return &instance;
}

LRESULT WinApp::WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) {
#ifdef _DEBUG
	if (ImGui_ImplWin32_WndProcHandler(hwnd, msg, wparam, lparam)) {
		return true;
	}
#endif // DEBUG

	//	メッセージに応じてゲーム固有の処理を行う
	switch (msg) {
		//	ウィンドウが破棄された
	case WM_DESTROY:
		//	OSに対して、アプリの終了を伝える
		PostQuitMessage(0);
		return 0;
	}
	//	標準のメッセージ処理を行う
	return DefWindowProc(hwnd, msg, wparam, lparam);
}

bool WinApp::ProcessMessage() {
	MSG msg{};
	//	ウィンドウにメッセージが来てたら最優先で処理させる
	if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	//	終了メッセージが来たらループを抜ける
	if (msg.message == WM_QUIT)
	{
		return true;
	}
	return false;
}

void WinApp::CreateGameWindow(const wchar_t* title, int32_t clientWidth, int32_t clientHeight) {
	//	COMの初期化を行う
	CoInitializeEx(0, COINIT_MULTITHREADED);

	//	ウィンドウプロシージャ
	wc.lpfnWndProc = WindowProc;
	//	ウィンドウクラス名（なんでも良い）
	wc.lpszClassName = L"CG2WindowClass";
	//	インスタンスハンドル
	wc.hInstance = GetModuleHandle(nullptr);
	//	カーソル指定
	wc.hCursor = LoadCursor(nullptr, IDC_ARROW);

	//	アイコンの設定
	wc.hIcon = LoadIcon(wc.hInstance, MAKEINTRESOURCE(IDI_ICON1));


	//	ウィンドウクラスを登録する
	RegisterClass(&wc);

	//	ウィンドウサイズを表す構造体にクライアント領域を入れる
	//	{x座標,y座標,横幅,縦幅}
	RECT wrc = { 0,0,clientWidth,clientHeight };

	//	クライアント領域を元に実際のサイズにwrcを変更してもらう
	AdjustWindowRect(&wrc, WS_OVERLAPPEDWINDOW, false);

	hwnd = CreateWindow(
		wc.lpszClassName,		//	利用するクラス名
		title,					//	タイトルバーの文字（何でもいい）
		WS_OVERLAPPEDWINDOW,	//	よく見るウィンドウスタイル
		CW_USEDEFAULT,			//	表示X座標（Windowsに任せる）
		CW_USEDEFAULT,			//	表示Y座標（WindowsOSに任せる）
		wrc.right - wrc.left,	//	ウィンドウ横幅	
		wrc.bottom - wrc.top,	//	ウィンドウ縦幅
		nullptr,				//	親ウィンドウハンドル
		nullptr,				//	メニューハンドル
		wc.hInstance,			//	インスタンスハンドル
		nullptr					//	オプション
	);

	//	ゲームウィンドウを表示する
	ShowWindow(hwnd, SW_SHOW);
}

void WinApp::DeleteGameWindow() {
	//	登録したクラスの破棄
	UnregisterClass(wc.lpszClassName, wc.hInstance);
	//	COMの終了処理
	CoUninitialize();
}
