#include "Client.h"
#include "Base/WindowSupervisor/WindowSupervisor.h"
#include <cassert>

void Client::Initialize() {
	auto hwMain = WindowSupervisor::GetInstance()->GetHwnd();
	// 共有処理
	InitializeWinSock();
	CreateSocket();

	// ここから別処理
	// ホスト名からIPアドレスを取得
	char szServer[256]{ "192.168.0.12" };
	hostent* lpHost = gethostbyname(szServer);

	// クライアントソケットをサーバーに接続
	sockaddr_in saddr{};
	memset(&saddr, 0, sizeof(SOCKADDR_IN));
	saddr.sin_family = lpHost->h_addrtype;
	saddr.sin_port = htons(wPort);
	saddr.sin_addr.s_addr = *((u_long*)lpHost->h_addr);

	if (connect(socket_, (SOCKADDR*)&saddr, sizeof(saddr)) == SOCKET_ERROR) {
		// エラー
		closesocket(socket_);
		SetWindowText(hwMain, L"サーバーと接続できませんでした");
		assert(true);
	}

	CreateThread();
}

void Client::ThreadFunc() {

}

void Client::Finalize() {
	FinalizeWinSock(socket_);
}

void Client::Update() {
	// accept関数の帰り値はSOCKET変数「s」に格納

	int nRcv = 0;
	char szBuf[1024]{};

	// send関数でデータを送信
	std::cout << "送信 --> " << std::endl;
	scanf_s("%s", szBuf, 1024);
	fflush(stdin);

	send(socket_, szBuf, (int)strlen(szBuf), 0);

	// recv関数でデータを受信
	nRcv = recv(socket_, szBuf, sizeof(szBuf) - 1, 0);
	szBuf[nRcv] = '\0';
	std::cout << "受信 --> " << szBuf << std::endl;

	if (strcmp(szBuf, "end") == 0) {
		return;
	}
}
