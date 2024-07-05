#pragma once
#include "Utils/WorldTransform/WorldTransform.h"
#include "Graphics/Model/Model.h"
#include <memory>
#include <vector>
#include "Utils/Collider/Collider.h"

class Enemy {
public:
	Enemy() = default;
	~Enemy() = default;

	void Initialize();
	void Update();
	void DrawImGui();
	Collider* GetCollision() const { return collision_.get(); }
private:
	void TransformUpdate();

private:
	enum Parts {
		Body,
		Head,
		kMaxParts,
	};

	WorldTransform transform_;
	std::vector<WorldTransform> partsTrans_;
	std::vector<std::shared_ptr<Model>> models_;
	//std::shared_ptr<AABB> collision_;
	std::unique_ptr<Collider> collision_;

};
