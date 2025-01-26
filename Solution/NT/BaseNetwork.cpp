#include "BaseNetwork.h"
#include "Base/WindowSupervisor/WindowSupervisor.h"
#include <cassert>


void BaseNetwork::SetThread() {
	// 既にはしっていたら早期リターン
	if (datas_.isRunning.load()) {
		return;
	}

	datas_.isRunning.store(true);

	// unique_ptr に新しいスレッドを作成
	thread_ = std::make_unique<std::thread>([this]() {
		this->Initialize();

		while (datas_.isRunning.load()) {
			if (this->Update()) {
				break;
			}
		}

		this->Finalize();
		});
}

void BaseNetwork::Finalize() {
	if (datas_.isRunning.load()) {
		datas_.isRunning.store(false); // 実行フラグをリセット
		if (thread_ && thread_->joinable()) {
			thread_->join(); // スレッド終了を待機
		}
		thread_.reset(); // スレッドリソースを解放
	}
}

const NetworkData::Data& BaseNetwork::GetData() const {
	// TODO: return ステートメントをここに挿入します
	return recvDatas_;
}

void BaseNetwork::SetData(const NetworkData::Data& data) {
	datas_.data = data;
}

void BaseNetwork::InitializeWinSock() {
	auto hwMain = WindowSupervisor::GetInstance()->GetHwnd();
	WSADATA wsaData;
	// winsockの初期化
	if (WSAStartup(MAKEWORD(2, 0), &wsaData) == SOCKET_ERROR) {
		// 初期化エラー
		SetWindowText(hwMain, L"WinSockの初期化に失敗しました");
		assert(false);
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
		assert(false);
	}
}

void BaseNetwork::FinalizeWinSock(SOCKET socket) {
	closesocket(socket);
}
