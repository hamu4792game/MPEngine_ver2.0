#pragma once
#include "IStateFrame.h"

class WaitStart : public IStateFrame {
public:
	WaitStart() = default;
	WaitStart(GameData* data) : IStateFrame(data) {}
	~WaitStart() = default;

	void Initialize() override {};
	void Finalize() override {};
	void Update() override;

private:
	void ImGuiProc() override;

};