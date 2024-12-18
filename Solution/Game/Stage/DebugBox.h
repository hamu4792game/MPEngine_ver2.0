#pragma once
#include "ObjectFrame.h"

/// <summary>
/// Debug用の均一オブジェクト タイリングだね
/// </summary>
class DebugBox {
public:
	DebugBox() = default;
	~DebugBox() = default;

	void Initialize(LevelData::ObjectData& objectdata);
	void DrawImGui();

	const WorldTransform& GetTrans() const { return transform_; }
	Collider* GetCollision() const { return collision_.get(); }
private:
	// 立方体なので6面
	std::array<std::shared_ptr<Model>, 6u> models_;
	WorldTransform transform_;
	WorldTransform collTransform_;
	std::unique_ptr<Collider> collision_;
};
