#pragma once
#include "MPEngine/Framework/SceneFrame.h"

class NetTitleScene : public SceneFrame {
public:
	NetTitleScene() = default;
	~NetTitleScene() override = default;

	void Initialize() override;
	void Finalize() override;
	void Update() override;


};
