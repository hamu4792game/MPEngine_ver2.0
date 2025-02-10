#pragma once
#include "Framework/GameFrame.h"
#include "Game/Scene/TitleScene.h"
#include "Game/Scene/BattleScene.h"
#include <memory>
#include <optional>
#include "Transition.h"
#include "Game/Spring/Spring.h"

class TestScene : public MPE::GameFrame {
public:
	TestScene() = default;
	~TestScene() override = default;

	void Initialize() override;
	void Finalize() override;
	void Update() override;

private:
	
	std::unique_ptr<Spring> spring_;

};
