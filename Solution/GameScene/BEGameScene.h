#pragma once
#include "Framework/GameFrame.h"
#include "Framework/SceneFrame.h"
#include <memory>

class BEGameScene : public GameFrame {
public:
	BEGameScene() = default;
	~BEGameScene() override = default;

	void Initialize() override;
	void Finalize() override;
	void Update() override;
private:

	std::unique_ptr<SceneFrame> titleScene_;

};
