#pragma once
#include "Framework/GameFrame.h"
#include <memory>
#include "NT/BaseNetwork.h"

class NTScene : public GameFrame {
public:
	NTScene() = default;
	~NTScene() override = default;

	void Initialize() override;
	void Finalize() override;
	void Update() override;


private:
	
	std::unique_ptr<BaseNetwork> network_;

};
