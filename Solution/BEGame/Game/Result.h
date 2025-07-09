#pragma once
#include "IStateFrame.h"

class Result : public IStateFrame {
public:
	Result() = default;
	Result(GameData* data) : IStateFrame(data) {}
	~Result() = default;

	void Initialize() override;
	void Finalize() override {};
	void Update() override;

private:
	void ImGuiProc() override;
	int score_ = 0;
};