#pragma once
#include "Framework/GameFrame.h"
#include <memory>
#include "Framework/SceneFrame.h"

class NTScene : public GameFrame {
public:
	NTScene() = default;
	~NTScene() override = default;

	void Initialize() override;
	void Finalize() override;
	void Update() override;


private:

	std::unique_ptr<SceneFrame> gameScene_;

	

};
