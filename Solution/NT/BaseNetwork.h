#pragma once
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <WinSock.h>

#pragma comment (lib, "WSock32.lib")

#include <thread>
#include "NetworkData.h"

class BaseNetwork {
public:
	BaseNetwork() = default;
	virtual ~BaseNetwork();


	void SetThread();

	virtual void Initialize() = 0;
	virtual bool Update() = 0;
	virtual void Finalize();

	// 受信用データの取得
	const NetworkData::Data& GetData() const;
	// 送信用データのセット
	void SetData(const NetworkData::Data& data);

protected:

	void InitializeWinSock();

	void CreateSocket();

	void FinalizeWinSock(SOCKET socket);

protected:
	SOCKET socket_;
	const WORD wPort = 8000;

	std::unique_ptr<std::thread> thread_;
	NetworkData datas_; // 送信データ 
	NetworkData::Data recvDatas_; // 受信データ

	sockaddr_in saddr_{};
};
