#include "BaseNetwork.h"
#include "Base/WindowSupervisor/WindowSupervisor.h"
#include <cassert>

DWORD WINAPI ThreadFunc(LPVOID lpParam) {
	Initialize();

	while (true) {
		Update();
	}

	Finalize();
	return 0;
}

void BaseNetwork::SetThread() {
	static DWORD dwID;
	static HANDLE hThread;
	struct Circle {
		float x;
	};
	Circle a;

	hThread = (HANDLE)CreateThread(NULL, 0, &ThreadFunc, (LPVOID)&a, 0, &dwID);
}

void BaseNetwork::InitializeWinSock() {
	auto hwMain = WindowSupervisor::GetInstance()->GetHwnd();
	WSADATA wsaData;
	// winsockの初期化
	if (WSAStartup(MAKEWORD(2, 0), &wsaData) == SOCKET_ERROR) {
		// 初期化エラー
		SetWindowText(hwMain, L"WinSockの初期化に失敗しました");
		assert(true);
	}
}

void BaseNetwork::CreateSocket() {
	auto hwMain = WindowSupervisor::GetInstance()->GetHwnd();
	// socket関数でソケットを作成する
	socket_ = socket(AF_INET, SOCK_STREAM, 0);
	if (socket_ == INVALID_SOCKET) {
		// 初期化エラー
		closesocket(socket_);
		SetWindowText(hwMain, L"Soketの作成に失敗しました");
		assert(true);
	}
}

void BaseNetwork::FinalizeWinSock(SOCKET socket) {
	closesocket(socket);
}
