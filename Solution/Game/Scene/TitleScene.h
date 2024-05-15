#pragma once
#include "Framework/SceneFrame.h"
#include "Utils/WorldTransform/WorldTransform.h"
#include "Game/TitleUI/TitleUI.h"
#include "Game/Stage/Stage.h"

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
	WorldTransform cameraTransform_;
	std::unique_ptr<TitleUI> titleUI_;
	std::unique_ptr<Stage> stage_;

};
