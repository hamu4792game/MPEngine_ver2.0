#include "Server.h"
#include "Base/WindowSupervisor/WindowSupervisor.h"
#include <cassert>

void Server::Initialize() {
	auto hwMain = WindowSupervisor::GetInstance()->GetHwnd();
	// 共有処理
	InitializeWinSock();
	CreateSocket();

	// bind関数でソケットに名前をつける
	memset(&saddr_, 0, sizeof(SOCKADDR_IN));
	saddr_.sin_family = AF_INET;
	saddr_.sin_port = htons(wPort);
	saddr_.sin_addr.s_addr = INADDR_ANY;

	int bind_ = bind(socket_, (struct sockaddr*)&saddr_, sizeof(saddr_));
	if (bind_ == SOCKET_ERROR) {
		// エラー
		closesocket(socket_);
		SetWindowText(hwMain, L"Bindの初期化に失敗しました");
		assert(false);
	}
	
}

void Server::Finalize() {
	shutdown(socket_, 0);
	closesocket(socket_);
}

bool Server::Update() {
	int nRcv = 0;
	NetworkData::Data buf = datas_.data;
	int fromlen = sizeof(saddr_);
	sockaddr_in saddr{};

	// recv関数でデータを受信
	nRcv = recvfrom(socket_, (char*)&recvDatas_, sizeof(NetworkData::Data), 0, (struct sockaddr*)&saddr, &fromlen);

	if (nRcv == SOCKET_ERROR) { return true; }

	// send関数でデータを送信
	sendto(socket_, (const char*)&buf, sizeof(NetworkData::Data), 0, (struct sockaddr*)&saddr, sizeof(saddr));
	return false;
}