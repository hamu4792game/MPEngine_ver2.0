#pragma once
#include "BaseNetwork.h"

class Client : public BaseNetwork {
public:
	Client() = default;
	~Client() = default;

	void Initialize() override;
	void Finalize() override;
	bool Update() override;
private:


};