#pragma once
#include "BaseNetwork.h"

class Client : public BaseNetwork {
public:
	Client() = default;
	~Client() = default;

	void Initialize() override;
	void ThreadFunc() override;
	void Finalize() override;

	void Update();
private:


};