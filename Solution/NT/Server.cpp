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
	memset(&saddr, 0, sizeof(SOCKADDR_IN));
	saddr.sin_family = AF_INET;
	saddr.sin_port = htons(wPort);
	saddr.sin_addr.s_addr = INADDR_ANY;

	int bind_ = bind(socket_, &(sockaddr&)saddr, sizeof(saddr));
	if (bind_ == SOCKET_ERROR) {
		// エラー
		closesocket(socket_);
		SetWindowText(hwMain, L"Bindの初期化に失敗しました");
		assert(false);
	}

	// listen関数で接続の待機状態にする
	int listen_ = listen(socket_, 2);
	if (listen_ == SOCKET_ERROR) {
		// エラー
		closesocket(socket_);
		SetWindowText(hwMain, L"Listenの初期化に失敗しました");
		assert(false);
	}

	// assept関数で接続を受け入れる
	sockaddr_in sockaddr_string{}; // 接続した相手のIPアドレス等の情報を格納するポインタ
	int size = sizeof(sockaddr_in); // 上のサイズを格納するためのもの
	socket_acc = accept(socket_, &(sockaddr&)sockaddr_string, &size);
	if (socket_acc == INVALID_SOCKET) {
		// エラー
		closesocket(socket_);
		SetWindowText(hwMain, L"Acceptの初期化に失敗しました");
		assert(false);
	}
	closesocket(socket_);
}

void Server::Finalize() {
	shutdown(socket_acc, 0);
	closesocket(socket_acc);
}

bool Server::Update() {
	int nRcv = 0;
	NetworkData::Data buf = datas_.data;

	// recv関数でデータを受信
	nRcv = recv(socket_acc, (char*)&recvDatas_, sizeof(NetworkData::Data), 0);

	if (nRcv == SOCKET_ERROR) { return true; }

	// send関数でデータを送信
	send(socket_acc, (const char*)&buf, sizeof(NetworkData::Data), 0);
	return false;
}