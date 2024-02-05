#pragma once
#include "Graphics/Model/Model.h"
#include "Utils/WorldTransform/WorldTransform.h"
#include <memory>

class ModelsControl {
public:
	ModelsControl();
	~ModelsControl() = default;

	void Update();
	void ImGuiProcess();

private:
	std::unique_ptr<Model> targetModel_;
	WorldTransform transform_;


};
