#pragma once
#include "Framework/SceneFrame.h"
#include "Utils/WorldTransform/WorldTransform.h"
#include "Game/TitleUI/TitleUI.h"
#include "Game/Stage/Stage.h"
#include "Game/Player/Player.h"
#include "Game/Target/Target.h"

class TitleScene : public SceneFrame {
public:
	TitleScene() = default;
	~TitleScene() override = default;

	void Initialize() override;
	void Finalize() override;
	void Update() override;
	void SecondUpdate() override;
	void DrawImGui();

private:
	std::unique_ptr<TitleUI> titleUI_;
	std::unique_ptr<Stage> stage_;
	std::unique_ptr<Player> player_;
	std::shared_ptr<Sprite> tutorial_;

	bool tutorialDraw_ = false;
};
