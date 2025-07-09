#pragma once
#include "IStateFrame.h"
#include "StopWatch/StopWatch.h"

class Running : public IStateFrame{
public:
	Running() = default;
	Running(GameData* data) : IStateFrame(data) {}
	~Running() = default;

	void Initialize() override;
	void Finalize() override;
	void Update() override;

private:
	void ImGuiProc() override;

	std::unique_ptr<StopWatch> stopwatch_;

};