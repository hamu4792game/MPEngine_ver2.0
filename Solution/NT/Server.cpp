#include "Server.h"
#include "Base/WindowSupervisor/WindowSupervisor.h"
#include <cassert>

void Server::Initialize() {
	auto hwMain = WindowSupervisor::GetInstance()->GetHwnd();
	// 共有処理
	InitializeWinSock();
	CreateSocket();

	// bind関数でソケットに名前をつける
	sockaddr_in saddr{};
	ZeroMemory(&saddr, sizeof(saddr));
	saddr.sin_family = AF_INET;
	saddr.sin_port = htons(wPort);
	saddr.sin_addr.s_addr = INADDR_ANY;

	int bind_ = bind(socket_, &(sockaddr&)saddr, sizeof(saddr));
	if (bind_ == SOCKET_ERROR) {
		// エラー
		closesocket(socket_);
		SetWindowText(hwMain, L"Bindの初期化に失敗しました");
		assert(true);
	}

	// listen関数で接続の待機状態にする
	int listen_ = listen(socket_, 0);
	if (listen_ == SOCKET_ERROR) {
		// エラー
		closesocket(socket_);
		SetWindowText(hwMain, L"Listenの初期化に失敗しました");
		assert(true);
	}

	// assept関数で接続を受け入れる
	sockaddr_in sockaddr_string{}; // 接続した相手のIPアドレス等の情報を格納するポインタ
	int size = sizeof(sockaddr_in); // 上のサイズを格納するためのもの
	socket_acc = accept(socket_, &(sockaddr&)sockaddr_string, &size);
	if (socket_acc == -1) {
		// エラー
		closesocket(socket_);
		SetWindowText(hwMain, L"Acceptの初期化に失敗しました");
		assert(true);
	}
	FinalizeWinSock(socket_);
}

void Server::ThreadFunc() {
	Initialize();

	while (true) {
		Update();
	}

	Finalize();
}

void Server::Finalize() {
	shutdown(socket_acc, 0);
	closesocket(socket_acc);
}

void Server::Update() {
	int nRcv = 0;
	char szBuf[1024]{};

	// recv関数でデータを受信
	nRcv = recv(socket_acc, szBuf, sizeof(szBuf) - 1, 0);
	szBuf[nRcv] = '\0';

	std::cout << "受信 --> " << szBuf << std::endl;
	std::cout << "送信 --> " << std::endl;

	// send関数でデータを送信
	send(socket_acc, szBuf, (int)strlen(szBuf), 0);

	if (strcmp(szBuf, "end") == 0) {
		return;
	}
}