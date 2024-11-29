#pragma once
#include <list>
#include <memory>
#include "Math/MathUtl.h"
#include "Graphics/Sprite/Sprite.h"
#include "Game/Stage/Ground.h"

// 注視点の探索
class PointOfGazeSearch {
public:
	PointOfGazeSearch() = default;
	~PointOfGazeSearch() = default;

	void Initialize();
	WorldTransform Update(const std::list<std::shared_ptr<Ground>>& targets);


private:
	Vector2 ChangeScreen(const Vector3& worldPos);
	// 一番近いターゲットを見つける
	void Search(const std::list<std::shared_ptr<Ground>>& targets);

	// ターゲットの移動するポイントを見つける
	void SearchPoint();

	// 距離をはかる
	bool DistanceCheck(Vector3 pos, float& distanceToCenter) const;

private:
	Vector3 minDistance_ = Vector3(-30.0f, -30.0f, 30.0f); // 感知する最大範囲
	Vector3 maxDistance_ = Vector3(30.0f, 30.0f, 300.0f); // 感知する最小範囲

	std::shared_ptr<Sprite> lockOnMark_;
	// 目標
	std::list<std::pair<float, const Ground*>> targets_;
	// ロックオン対象
	const Vector3* target_ = nullptr;

	std::unique_ptr<Collider> collision_;
	WorldTransform collTrans_;
};