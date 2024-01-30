#pragma once
#include "Utils/WorldTransform/WorldTransform.h"
#include <memory>
#include <vector>

class PlayerAnimation {
public:
	PlayerAnimation() = default;
	~PlayerAnimation() = default;
public:
	void SetPartsPtr(WorldTransform* parts);

	void Request(uint32_t type); // 初期化処理やる
	
	void Update();

private:
	void Initialize();
private:
	uint32_t type_;

	// 通常アニメーション
	struct NormalParameter {
		std::vector<Vector3> offsets;
		std::vector<Vector3> prePositions;
	};
	NormalParameter normal_;
	void NormalInitialize();
	void NormalUpdate();

private:
	WorldTransform* partsTrans_;

};