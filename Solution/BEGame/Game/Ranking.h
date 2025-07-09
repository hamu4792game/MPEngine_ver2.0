#pragma once
#include "IStateFrame.h"

class Ranking : public IStateFrame {
public:
	Ranking() = default;
	Ranking(GameData* data) : IStateFrame(data) {}
	~Ranking() = default;

	void Initialize() override {};
	void Finalize() override {};
	void Update() override;

private:
	void ImGuiProc() override;

};