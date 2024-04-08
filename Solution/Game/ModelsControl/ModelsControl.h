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
	const WorldTransform& GetTransform() { return transform_; }
private:
	std::unique_ptr<Model> targetModel_;
	WorldTransform transform_;
	Vector4 color_ = Vector4::one;


};
