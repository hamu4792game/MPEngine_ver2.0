#pragma once
#include "Framework/SceneFrame.h"
#include "Utils/WorldTransform/WorldTransform.h"
#include "Graphics/Model/Model.h"
#include "Game/ModelsControl/ModelsControl.h"
#include "Game/Particle/DustParticle.h"

class TitleScene : public SceneFrame {
public:
	TitleScene() = default;
	~TitleScene() override = default;

	void Initialize() override;
	void Finalize() override;
	void Update() override;
	void DrawImGui();

private:
	WorldTransform cameraTransform_;
	std::unique_ptr<Model> monsterBall_;
	WorldTransform ballTrans_;
	std::unique_ptr<ModelsControl> model_;
	std::unique_ptr<DustParticle> dust_;
	Vector3 dustPosition_;

};
