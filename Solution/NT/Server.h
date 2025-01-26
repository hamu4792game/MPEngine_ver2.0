#pragma once
#include "BaseNetwork.h"

class Server : public BaseNetwork {
public:
	Server() = default;
	~Server() = default;

	void ThreadFunc() override;


	void Initialize() override;
	void Finalize() override;
	void Update() override;

private:

	// accept関数の戻り値を入れる変数
	SOCKET socket_acc;

};