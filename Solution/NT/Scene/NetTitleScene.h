#pragma once
#include "MPEngine/Framework/SceneFrame.h"
#include <memory>
#include "Graphics/Sprite/Sprite.h"

class NetTitleScene : public SceneFrame {
public:
	NetTitleScene() = default;
	~NetTitleScene() override = default;

	void Initialize() override;
	void Finalize() override;
	void Update() override;

private:
	void SecondUpdate() override;

	std::unique_ptr<Sprite> text_;

};
