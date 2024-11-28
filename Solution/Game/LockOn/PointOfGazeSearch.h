#pragma once
#include <list>
#include <memory>
#include "Math/MathUtl.h"
#include "Game/Stage/Ground.h"

// 注視点の探索
class PointOfGazeSearch {
public:
	PointOfGazeSearch() = default;
	~PointOfGazeSearch() = default;

	void Initialize();
	void Update(const std::list<std::shared_ptr<Ground>>& targets);


private:
	Vector2 ChangeScreen(const Vector3& worldPos);
	// 一番近いターゲットを見つける
	void Search(const std::list<std::shared_ptr<Ground>>& targets);

	// ターゲットの移動するポイントを見つける
	void SearchPoint();


private:
	Vector3 maxDistance_ = Vector3(-30.0f, -30.0f, 70.0f); // 感知する最大範囲
	Vector3 minDistance_ = Vector3(30.0f, 30.0f, 300.0f); // 感知する最小範囲

	// 目標
	std::list<std::pair<float, const Ground*>> targets_;
	// ロックオン対象
	const Ground* target_ = nullptr;
};