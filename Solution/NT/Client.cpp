#include "Client.h"
#include "Base/WindowSupervisor/WindowSupervisor.h"
#include <cassert>
#include "Utils/GlobalVariables/GlobalVariables.h"

void Client::Initialize() {
	auto hwMain = WindowSupervisor::GetInstance()->GetHwnd();
	// 共有処理
	InitializeWinSock();
	CreateSocket();

	// ここから別処理
	// ホスト名からIPアドレスを取得
	auto gv = GlobalVariables::GetInstance();
	gv->LoadFile("ip");
	gv->CreateGroup("ip");
	std::string szServer = gv->GetStringValue("ip", "Address");

	hostent* lpHost = gethostbyname(szServer.c_str());

	// クライアントソケットをサーバーに接続
	memset(&saddr_, 0, sizeof(SOCKADDR_IN));
	saddr_.sin_family = lpHost->h_addrtype;
	saddr_.sin_port = htons(wPort);
	saddr_.sin_addr.s_addr = *((u_long*)lpHost->h_addr);

	if (connect(socket_, (SOCKADDR*)&saddr_, sizeof(saddr_)) == SOCKET_ERROR) {
		// エラー
		closesocket(socket_);
		SetWindowText(hwMain, L"サーバーと接続できませんでした");
		assert(false);
	}

}

void Client::Finalize() {
	FinalizeWinSock(socket_);
}

bool Client::Update() {
	int nRcv = 0;
	NetworkData::Data buf = datas_.data;

	sockaddr_in recv_addr{};
	int fromlen = sizeof(saddr_);


	// send関数でデータを送信 クライアント側を送信
	sendto(socket_, (const char*)&buf, sizeof(NetworkData::Data), 0, (struct sockaddr*)&saddr_, sizeof(saddr_));

	// recv関数でデータを受信 
	nRcv = recvfrom(socket_, (char*)&recvDatas_, sizeof(NetworkData::Data), 0, (struct sockaddr*)&recv_addr, &fromlen);
	
	if (nRcv == SOCKET_ERROR) { return true; }
	return false;
}
